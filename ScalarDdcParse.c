/******************************************************************************
 * Implement for urat linstener, monitor the command from the scalar.
 *
 * Copyright (c) TPV IC. All rights reserved.
 *
 * author:
 * modification:
 * 
*******************************************************************************/
#include <linux/ctype.h>
#include <linux/timer.h>
#include <linux/delay.h>

#include "ScalarDdcParse.h"
#include "UartExecutor.h"
#include "PowerStateExecutor.h"

#include "env_resolve.h"

//judge flag
//extern bool _MacSerialReed;

static unsigned char gPktNbr = 0x00;
//unsigned char _PktNumber = 0x00;
unsigned char scalar_feedback_state = 0x00;
bool scalar_feedback_brightness = true;
int set_brightness_count =0;
//"R_State: ref. Android Feedback PM State command, shutdown mode"
unsigned char R_state = BOOTING;
unsigned char scaler_key;

bool get_brightness_done = false;
unsigned char backlight_level = 0;
bool get_scalerversion_flag = false;
int get_scalerversion_count=0;
unsigned char gLanguage = 0;
unsigned int i=0;
unsigned int screenon[2]={0,0};//robert
unsigned char mPowerOffMode = NORM_SHUTDOWN;
//Scaler version store
unsigned char _ScalerVerBuf[5];//robert
//Mac Address store
unsigned char _MacAddBuf[6];
//Serial Number
unsigned char _SerialNumBuf[14];
unsigned char _ModelNameBuf[14]={0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
unsigned char _tempBuf[14];
int temp=0;

bool getethmac=true;
bool getsn=true;
bool scalerdebug=true; //add debug
bool get_model_name=true;

//use for report poweroff state 
static struct timer_list poweroff_timer;

static char Parse_DDC_Packet(struct input_polled_dev *dev, char *ddc_buf, char dat_len);
static void TPVComm_TwoAckByteOkMsgGet(unsigned char *pBuff);
static void TPVComm_TwoAckByteNotSupportMsgGet(unsigned char *pBuff);

static unsigned char TPVComm_AckMsgVerify(unsigned char *cBuff);
//static unsigned char stch_i(char *s);

static int  Send_Ack1Ack2(char len, char cmd_type, char cmd_data1,char cmd_data2, char value, char is_sptby_TV);
static void Report_PowerState2Android(struct input_polled_dev *dev,char power_state);

////////////////////////////////Global Objects//////////////////////////////////
const ScalarDdcParse g_ScalarDdcParse = {

  .Parse_DDC_Packet = Parse_DDC_Packet,
  .TPVComm_TwoAckByteOkMsgGet = TPVComm_TwoAckByteOkMsgGet,
  .TPVComm_TwoAckByteNotSupportMsgGet = TPVComm_TwoAckByteNotSupportMsgGet,
};

extern const UartExecutor g_UartExecutor;
//use to contrl power state
extern const PowerStateExecutor g_PowerStateExecutor;

//ACK1, ACK2
static void TPVComm_TwoAckByteOkMsgGet(unsigned char *pBuff)
{
	static const unsigned char UartAck_Ok_MsgTbl[16] =
		{0xc2,0xc7,0xcb,0xcc,0xd3,0xd4,0xd8,0xdd,0xe3,0xe4,0xe8,0xed,0xf0,0xf5,0xf9,0xfe};
	
	gPktNbr &= 0x0f;
	
	pBuff[0] = UartAck_Ok_MsgTbl[gPktNbr++];
	pBuff[1] = ~pBuff[0]; // 2nd byte value = 1st byte 1's complement value
}

static void TPVComm_TwoAckByteNotSupportMsgGet(unsigned char *pBuff)
{
	static const unsigned char UartAck_NotSupport_MsgTbl[16] =
		{0x81,0x86,0x8a,0x8f,0x92,0x97,0x9b,0x9c,0xa2,0xa7,0xab,0xac,0xb3,0xb4,0xb8,0xbd};
	
	gPktNbr &= 0x0f;
	
	pBuff[0] = UartAck_NotSupport_MsgTbl[gPktNbr++];
	pBuff[1] = ~pBuff[0]; // 2nd byte value = 1st byte 1's complement value
}

typedef enum { eLink_NG=0, eLink_OK, eLink_NOT_SUPPORTED } tagLINKSTAT;

// AckType check table
//
const unsigned char UartAck_Ok_ChkTbl[64] = { // for AckType3 check table
	0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, // 0xc0
	0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, // 0xd0
	0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, // 0xe0
	1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0 // 0xf0
};

const unsigned char UartAck_NotSupport_ChkTbl[64] = { // for AckType2 check table
0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, // 0x80
0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, // 0x90
0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, // 0xa0
0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0 // 0xb0
};

static unsigned char TPVComm_AckMsgVerify(unsigned char *cBuff){
	unsigned char cIndex;
	unsigned char cRetVal;
	unsigned char cLinkId; 
	
	cLinkId = cBuff[0] & 0xc0; 
	cIndex = cBuff[0] & 0x3f;
	cRetVal = (unsigned char)eLink_NG;

	if((cBuff[0] + cBuff[1]) == 0xff)
	{
		cRetVal = (unsigned char)eLink_OK;
	}
	
	return(cRetVal); // 0:NG, 1:OK, 2:Not supported,
}


//modify send ACK function @haishan.lin 2013.05.03
static int  Send_Ack1Ack2(char len, char cmd_type, char cmd_data1,char cmd_data2, char value, char is_sptby_TV){
	char ack_buffer[7] = {0};
	char tmp_len = len;
/*
	if(tmp_len < 3){
		tmp_len = 3;
	}
*/
	if(IS_SUPPORT_BY_TV_SET == is_sptby_TV){
		TPVComm_TwoAckByteOkMsgGet(ack_buffer);
	}else if(NOT_SUPPORT_BY_TV_SET == is_sptby_TV){
		TPVComm_TwoAckByteNotSupportMsgGet(ack_buffer);
	}
	ack_buffer[2] = cmd_type;
	ack_buffer[3] = cmd_data1;
	ack_buffer[4] = cmd_data2;
	if(tmp_len == 7){
		ack_buffer[5] = 0x01;
		ack_buffer[6] = value;
	}
/*
	switch(tmp_len){
		case 3:
			ack_buffer[2] = 0;
		break;

		case 4:
			ack_buffer[2] = 1;
			ack_buffer[3] = value;
		break;

		default:
			ack_buffer[2] = 0;
		break;
	}
*/
	printk("send ack\n");
	return g_UartExecutor.Write_ScalarTTyLocked(ack_buffer,(int)tmp_len,NULL);
}

//timer function
static void poweroff_timerfunc(unsigned long data){
	struct input_polled_dev *dev = (struct input_polled_dev *)data;	

	del_timer(&poweroff_timer);

	input_report_key(dev->input,KEY_POWER, 0);	
	input_sync (dev->input);
} 

void VirtualKeyClick(struct input_polled_dev *dev) {
    input_report_key(dev->input,KEY_POWER, 1);	
	input_sync (dev->input);
	printk("scalar report pwer key\n");
	//Analog Release
	input_report_key(dev->input,KEY_POWER, 0);	
	input_sync (dev->input);
}

void VirtualKeyLongPress(struct input_polled_dev *dev) {
    input_report_key(dev->input,KEY_POWER, 1);	
	input_sync (dev->input);
			
	init_timer(&poweroff_timer);
	poweroff_timer.data = (unsigned long)dev;
	poweroff_timer.expires  = jiffies + (4*HZ)/5;  
	poweroff_timer.function = poweroff_timerfunc;
	add_timer(&poweroff_timer);
}
void VirtualMenuKeyClick(struct input_polled_dev *dev, unsigned char scaler_key) {
	unsigned int key;
	if (scaler_key==0) key = KEY_POWER;
	else if (scaler_key==1) key = KEY_VOLUMEUP;
	else if (scaler_key==2) key = KEY_VOLUMEDOWN;
	else return;

	input_report_key(dev->input, key, 1);	
	input_sync (dev->input);
	printk("scalar report menu key=%d\n",key);
	//Analog Release
	input_report_key(dev->input, key, 0);	
	input_sync (dev->input);
}

void Report_PowerState2Android(struct input_polled_dev *dev,char power_state){
	char pwr_stat = power_state;
	printk("scalar power_state = %x\n",pwr_stat);
	switch(pwr_stat){
		case NORM_SHUTDOWN:
		case FORCE_SHUTDOWN:	
			VirtualKeyLongPress(dev);
 		break;

		case SUSPEND:
		case TURN_ON_BACKLIGHT:		
			VirtualKeyClick(dev);
		break;

		case RESET_SLEEP_TIMER:
			//echo on > /sys/power/state
			//ret = g_PowerStateExecutor.Set_Screen_State(_ON_STATE);
			//R_state = POWER_ON_READY;

		break;

		default:
		break;
	}
}

void Parse_Specific_Command(struct input_polled_dev *dev, ptr_DDC_Command *command_buf) {
	char reAck_len,reAck_dat;
	//initialized
	reAck_len = 0;
	reAck_dat = R_state;
	
	switch(command_buf->CmdDirect)
    {
		case CMD_BYTE_0XDC:
			if(CMD_TYPE_0X81 == command_buf->CMD_Type){	
				if(UART_WRITE == command_buf->Pre1Byte){
					reAck_len = 6;
					//"S2A: Scaler To AndroidS_State:0x03= wakeup android via scaler local keypad 
					//(turn on backlight)0x81= shutdown0x82= suspend"

					printk("Scaler To AndroidS_State=%x,R_state=%x\n",command_buf->CMD_Data2,R_state);

					switch(command_buf->CMD_Data2){
					    case NORM_SHUTDOWN:
					    //in android source and power-on-ready state, user presses power button.
					    //b. in non-android source and normal mode, user presses power button.
					    //c. in non-android source and power saving mode.
					        mPowerOffMode = NORM_SHUTDOWN; //change poweroff mode to force
										
					        if(((ANDROID_SELF_WAKEUP == R_state) || POWER_ON_READY == R_state)){
						        mPowerOffMode = NORM_SHUTDOWN; //change poweroff mode to force
						        Report_PowerState2Android(dev,NORM_SHUTDOWN);
					        }
		                break;
										
			            case SUSPEND:
				        //Current this command is not use.
				            if(/*(BOOTING == R_state) || */(POWER_ON_READY == R_state)){
					            Report_PowerState2Android(dev,SUSPEND);
				            }
				        break;

			            case TURN_ON_BACKLIGHT:
				        //1. In android source and suspend state, scaler will send wakeup command if user presses power button.
				        //2. Android should return an Ack packet if receive.
				            if((SUSPEND_COMPLETE == R_state)){
					            Report_PowerState2Android(dev,TURN_ON_BACKLIGHT);
				            }
				        break;

			            case FORCE_SHUTDOWN:
				        //add in fuqing factory
				            mPowerOffMode = FORCE_SHUTDOWN; //change poweroff mode to force

				            if(SUSPEND_COMPLETE == R_state){
				                //wake up Android
				                Report_PowerState2Android(dev,TURN_ON_BACKLIGHT);
                                msleep(500);
			                    //power off report
			                    Report_PowerState2Android(dev,FORCE_SHUTDOWN);
				
				            }else if(POWER_ON_READY == R_state || ANDROID_SELF_WAKEUP == R_state){
					            Report_PowerState2Android(dev,FORCE_SHUTDOWN);
				            }else{
					            //Another state
					            mPowerOffMode = NORM_SHUTDOWN;
				            }

				        break;

			            default:
			            break;
		            }	
								
	            }else if(UART_READ == command_buf->Pre1Byte)
	            {
					//"R_State: ref. Android Feedback PM State command"
					reAck_len = 7;
					reAck_dat = R_state;

					printk("Android Feedback PM State command = %x\n",reAck_dat);
				}
			}else if(CMD_TYPE_0X82 == command_buf->CMD_Type)
			{
				printk("Reset Android Sleep Timer = %x\n",command_buf->CMD_Type);
						
				//Reset Android Sleep Timer
				reAck_len = 6;
				//Report_PowerState2Android(dev,RESET_SLEEP_TIMER);
				
/************************************modify @haishan.lin 2013.04.26****************************************/
				//Scaler Change Source Notification (S2A)
				//"State:0x01: leave android source
				//          0x02: enter android source"
				if(LEAVE_ANDROID_SRC == command_buf->CMD_Data2){	//Source State
				//when leave android source, scaler notices android with a change-source command. 
				//Then, android side should reset and stop its sleep timer, and lock android screen. 
				//Therefore, this command is unnecessary.
					printk("LEAVE_ANDROID_SRC Scaler Change Source Notification = %x,R_state = %x\n",command_buf->CMD_Data2,R_state);
					
					if((TURN_ON_BACKLIGHT == R_state) || (POWER_ON_READY == R_state)){
						//if Android in normal mode, make android into suspend mode

						//printk(" LEAVE_ANDROID_SRC screen on is = %d bright =%d \n",screenon[0],screenon[1]);
						if(screenon[0] ==1 )
						{
							printk(" LEAVE_ANDROID_SRC screen on is = %d bright =%d \n",screenon[0],screenon[1]);
							Report_PowerState2Android(dev,SUSPEND);
							
						}
				}

				}else if(ENTER_ANDROID_SRC == command_buf->CMD_Data2){

					printk("ENTER_ANDROID_SRC Scaler Change Source Notification = %x,R_state = %x\n",command_buf->CMD_Data2,R_state);
					
					//Send factory command[6] to Android that resets sleep timer T3. 
					/*if((BOOTING == R_state) || (POWER_ON_READY == R_state)){
						//Normal mode
						//Send factory command[6] to Android that resets sleep timer T3
						//Report_PowerState2Android(dev,SUSPEND);		//??
							
						//Report_PowerState2Android(dev,RESET_SLEEP_TIMER);
					}else if((INTO_THE_SUSPEND_MODE == R_state) || (SUSPEND_COMPLETE == R_state)|| (POWER_ON_READY == R_state)){
						//ducmennt say it wake up by gpio, wait confirm 
					*/
					if(POWER_ON_READY == R_state)
					{	
						//printk("ENTER_ANDROID_SRC screen on is = %d bright =%d\n",screenon[0],screenon[1]);
						//if(!notice_android_source && notice_other_source)
						//{
						if(screenon[0] ==0 )
						{
						printk("ENTER_ANDROID_SRC screen on is = %d bright =%d\n",screenon[0],screenon[1]);
						  		Report_PowerState2Android(dev,TURN_ON_BACKLIGHT);
						   		
						}
						//}
					}
				}

				reAck_len = 6;
						
			}else if(CMD_TYPE_0X83 == command_buf->CMD_Type)
			{
				backlight_level = command_buf->CMD_Data2;

				reAck_len = 6;
			}
			else if(CMD_TYPE_0X89 == command_buf->CMD_Type)
			{
				scaler_key = command_buf->CMD_Data2;
				printk("%s scaler_key=%d\n", __func__, scaler_key);
				VirtualMenuKeyClick(dev, scaler_key);

				reAck_len = 6;
			}			
			break;

		case CMD_BYTE_0XDD:

		break;

		default:
		break;
	}

	//return Ack1 Ack2 to Scalar
	Send_Ack1Ack2(reAck_len, command_buf->CMD_Type,command_buf->CMD_Data1,command_buf->CMD_Data2,reAck_dat,IS_SUPPORT_BY_TV_SET);
}

//parse the packet
static char Parse_DDC_Packet(struct input_polled_dev *dev, char *ddc_buf, char dat_len)
{
	char data_length = dat_len;
	char CurDatPtr,Pkt_len,Ack_data_len,Command_type,Command_data2;
	char Check_sum,State,Cnt;
    unsigned char set_scalar_language[] = {MONITOR_SLAVE_ADD,SOURCE_ADD,0x86,
                          UART_WRITE,TPV_FACTORY_CODE,CMD_BYTE_0XDD,CMD_TYPE_0X83,0x01,gLanguage,0x00};
    //Read EDID Serial Number   (direct mode) (Read Osd EDID Serial Number)
    unsigned char get_serial_num[] = {MONITOR_SLAVE_ADD,SOURCE_ADD,0x86,UART_READ,
                            TPV_FACTORY_CODE,CMD_BYTE_0XDD,0x84,0x01,0x00,0x15};
	unsigned char cRetVal;
	unsigned char send_to_get_ethmac[]={MONITOR_SLAVE_ADD,SOURCE_ADD,0x86,
		UART_READ,TPV_FACTORY_CODE,CMD_BYTE_0XDD,0x85,0x01,0x00,0x14};  

	ptr_DDC_Command *ddc_cmd_buf;

	CurDatPtr = 0;		//clear
	while((CurDatPtr < data_length) && ((data_length-CurDatPtr) >= MIN_ACK_LEN)){
		//check MONITOR_SLAVE_ADD & SOURCE_ADD
		if((MONITOR_SLAVE_ADD == *(ddc_buf+CurDatPtr)) 
					&& (SOURCE_ADD == *(ddc_buf+(CurDatPtr+1)))){
			
			Check_sum = MONITOR_SLAVE_ADD^SOURCE_ADD;
			//get the wright cmd head
			CurDatPtr += 2;		//move point to the Pktlen byte NO.3

			//package length
			Pkt_len = *(ddc_buf+CurDatPtr)&0x7f;

			if((data_length-CurDatPtr) < (Pkt_len+1)){
				CurDatPtr += 1;
				continue;
			}
			
			//check sum
			for(Cnt = 0; Cnt <= Pkt_len; Cnt ++){
				Check_sum = Check_sum^ddc_buf[CurDatPtr+Cnt];
			}
			
			if(Check_sum != ddc_buf[CurDatPtr+Cnt]){
				//check sum erro
				CurDatPtr += (Pkt_len+2);	//move point to after the Check byte NO.11,next cmd
				//send erro Ack1Ack2?
				//Send_Ack1Ack2(3,0,IS_SUPPORT_BY_TV_SET);

				continue;
			}

			printk("Check_sum OK!!\n");

			//ddc_cmd_buf point to the data package
			ddc_cmd_buf = (ptr_DDC_Command *)(ddc_buf+CurDatPtr);
			CurDatPtr += (Pkt_len+2);	//move point to after the Check byte NO.11,next cmd

			if((CMD_PKT_SIZE == (ddc_cmd_buf->PktLen&0x7f)) 
					&& (TPV_FACTORY_CODE == ddc_cmd_buf->Pre2Byte)) {
                		Parse_Specific_Command(dev, ddc_cmd_buf);	    
			}
			
			}
			else{
				//maybe Ack
				//ack_buffer[0] = stch_i(ddc_buf[CurDatPtr]);
				//ack_buffer[1] = stch_i(ddc_buf[CurDatPtr+1]);

				printk("maybe Ack\n");
/****************************modify Ack packages analysis @haishan.lin 2013.05.03**************************/	
                          cRetVal = TPVComm_AckMsgVerify((ddc_buf+CurDatPtr));

				printk("TPVComm_AckMsgVerify cRetVal=%d\n",cRetVal);
				if((unsigned char)eLink_OK == cRetVal){
					CurDatPtr += 2;		//move the point to Pktlen
					Command_type = *(ddc_buf+CurDatPtr);
					CurDatPtr += 2;
					Command_data2 = *(ddc_buf+CurDatPtr);
					CurDatPtr += 1;
					Ack_data_len = *(ddc_buf+CurDatPtr);
					if(0x00 == Ack_data_len){
						switch(Command_type){
							case CMD_TYPE_0X81:
								scalar_feedback_state = Command_data2;
                                if(BOOTING == scalar_feedback_state)
                                	{
	                                	//printk("send command for get_serial_num \n");
	                                  //g_UartExecutor.Write_ScalarTTyLocked(get_serial_num,sizeof(get_serial_num),NULL);  
						printk("get Mac address \n");			  
					       g_UartExecutor.Write_ScalarTTyLocked(send_to_get_ethmac,sizeof(send_to_get_ethmac),NULL);	
	                           }
					else if(POWER_ON_READY == scalar_feedback_state)
                                    g_UartExecutor.Write_ScalarTTyLocked(set_scalar_language,sizeof(set_scalar_language),NULL);
								break;
							case CMD_TYPE_0X82:
								break;
							case CMD_TYPE_0X83:
								break;
							case CMD_TYPE_0X84:
								break;
							case CMD_TYPE_0X85:
								scalar_feedback_brightness = true;
								set_brightness_count= 0;
								printk("scalar feedback backlight level %d\n",Command_data2);
								break;
							default:
								break;
						}
					}else if(0x01 == Ack_data_len){

						switch (Command_type){
							case CMD_TYPE_0X81:
								CurDatPtr += 1; //move the point to Ack data
								State = *(ddc_buf+CurDatPtr);
								switch(State){
									case NON_ANDROID_SRC:
										printk("NON_ANDROID_SRC");
									break;
									
									case IN_ANDROID_SRC:
										printk("IN_ANDROID_SRC");
									break;
									
									default:
									break;
								}
								break;
								
							case CMD_TYPE_0X82:
								printk("USB hub on or off");
								break;
							case CMD_TYPE_0X83:
								CurDatPtr += 1;
								backlight_level = *(ddc_buf+CurDatPtr);
								get_brightness_done = true;
								printk("get scalar backlight level = %d\n",backlight_level);
								break;
							default:
								break;
						}

					}else if(0x0e== Ack_data_len){
						CurDatPtr += 1; //move the point to Ack data
						printk("Maybe SN!\n");
						getsn=false;
						memcpy(_SerialNumBuf,ddc_buf+CurDatPtr,sizeof(_SerialNumBuf));
						if(true)
						{						
							for(i=0;i<sizeof(_SerialNumBuf);i++)
							{	
								printk("serial number data[%d]= 0x%x \n",i,_SerialNumBuf[i]);
							}
						}
						CurDatPtr += Ack_data_len;	//move the point to the next cmd						
					}else if(0x06 == Ack_data_len){
						CurDatPtr += 1; //move the point to Ack data
						printk("Maybe Mac Address!\n");
						//_MacAddBuf = ddc_buf+CurDatPtr;
						getethmac=false;
						memcpy(_MacAddBuf,ddc_buf+CurDatPtr,sizeof(_MacAddBuf));
						if(scalerdebug)
						{
							for(i=0;i<sizeof(_MacAddBuf);i++)
							{	
								printk("Mac Address data[%d]= 0x%x \n",i,_MacAddBuf[i]);
							}
						}
						CurDatPtr += Ack_data_len;	//move the point to the next cmd

					}else if(0x05 == Ack_data_len){//robert
						CurDatPtr += 1; //move the point to Ack data
						printk("Get Scaler Version\n");
						get_scalerversion_flag=false;
						get_scalerversion_count=0;
						memcpy(_ScalerVerBuf,ddc_buf+CurDatPtr,sizeof(_ScalerVerBuf));
						if(scalerdebug)
						{
							for(i=0;i<sizeof(_ScalerVerBuf);i++)
							{	
								printk("scaler version data[%d]= 0x%x \n",i,_ScalerVerBuf[i]);
							}
						}
						CurDatPtr += Ack_data_len;	//move the point to the next cmd
					}else if(0x08 <= Ack_data_len && get_model_name && !getsn){
						temp=0;
						get_model_name=false;
						printk("Get Model Name\n");
						memcpy(_tempBuf,ddc_buf+CurDatPtr,sizeof(_tempBuf));
						for(i=0;i<sizeof(_tempBuf);i++)
						{
							if(_tempBuf[i]>=0x30 && _tempBuf[i]<=0x39)
							{
								_ModelNameBuf[i-temp]=_tempBuf[i];
							}
							else if(_tempBuf[i]>=0x41 && _tempBuf[i]<=0x5A)
							{
								_ModelNameBuf[i-temp]=_tempBuf[i];
							}
							else
							{
								temp++;
							}
						
						}
						if(true)
						{
							for(i=0;i<sizeof(_ModelNameBuf);i++)
							{	
								printk("Model name data[%d]= 0x%x \n",i,_ModelNameBuf[i]);
							}
						}
						CurDatPtr += Ack_data_len;	//move the point to the next cmd
					}
				
				}else if(cRetVal == (unsigned char)eLink_NOT_SUPPORTED){

				CurDatPtr += 1;
			}
			else{
				CurDatPtr += 1;
			}		
		}
	}

	return 0;
}

