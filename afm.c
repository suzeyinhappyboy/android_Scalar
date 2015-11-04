#define afmC
//---------------------------------------------------------------------------
#include "afm.h"
#include "afmext.h"
#include "msAPI_FCtrl.h"
#include "drvUartDebug.h"
#include "MApp_Exit.h"

extern void seSetUserInputControlState(U8 LockParm);
extern void seSetVolumeLimitsControl(U8 MaxVolumeValue,U8 LimitVolumeValue);

//---------------------------------------------------------------------------
static BOOLEAN fAfmMode = 0;
		// TPV_XM Xue 20120818: SET BDS mode
 #if ENABLE_TPV_EBONY_PBS_MODE
 static BOOLEAN seBDSMode = 0;
#endif
//---------------------------------------------------------------------------
void afm_control(U8 op)
{
    U16 addr;
    U8 i;
		// TPV_XM Xue 20121022:  PE require add FT FLAG enter factory cmd
		//#ifdef ENABLE_TPV_FACTORY_COMMAND_ENTER       &&  stGenSetting.g_FactorySetting.isFactoryCommandMode ==0           #endif
  
#ifdef ENABLE_TPV_FACTORY_COMMAND_ENTER   	
	if (stGenSetting.g_FactorySetting.isFactoryCommandMode ==1  )
		//fAfmMode=0;
	//else
		fAfmMode=1;
#endif
  if (fAfmMode == 0 && op != afiEnterAFM )

    {
        return;
    }

    //printf("op =[%bu]",op);/*Creass.liu at 2012-06-27*/
    switch (op)
    {
        case afiEnterAFM:
            afmEnterAFM();
            fAfmMode = 1;
        break;
        case afiExitAFM:
            afmExitAFM();
            //fAfmMode = 0;
        break;
        case afiNvmInit:
            afmNvmInit();
        break;
        case afiBurnInOnOff:
            afmBurnInOnOff(afm.pkt.cmd.data2);
        break;
        case afiClrBackLightHour:
            afmClrBackLightHour();
            break;
        case afiSetBackLight:
            afmSetBackLight(afm.pkt.cmd.data2);
            break;
        case afiSetAdcRGain:
            afmSetAdcRGain(afm.pkt.cmd.data1,afm.pkt.cmd.data2);
        break;
        case afiGetAdcRGain:
            afm.ack_buf.ret[0] = (afmGetAdcRGain()>>8) & 0xff;
            afm.ack_buf.ret[1] = afmGetAdcRGain() & 0xff;
            afm.ack_buf.len = 2;
        break;
        case afiSetAdcGGain:
            afmSetAdcGGain(afm.pkt.cmd.data1,afm.pkt.cmd.data2);
        break;
        case afiGetAdcGGain:
            afm.ack_buf.ret[0] = (afmGetAdcGGain()>>8) & 0xff;
            afm.ack_buf.ret[1] = afmGetAdcGGain() & 0xff;
            afm.ack_buf.len = 2;
        break;
        case afiSetAdcBGain:
            afmSetAdcBGain(afm.pkt.cmd.data1,afm.pkt.cmd.data2);
        break;
        case afiGetAdcBGain:
            afm.ack_buf.ret[0] = (afmGetAdcBGain()>>8) & 0xff;
            afm.ack_buf.ret[1] = afmGetAdcBGain() & 0xff;
            afm.ack_buf.len = 2;
        break;
        case afiSetAdcROffset:
            afmSetAdcROffset(afm.pkt.cmd.data1,afm.pkt.cmd.data2);
        break;
        case afiGetAdcROffset:
            afm.ack_buf.ret[0] = (afmGetAdcROffset()>>8) & 0xff;
            afm.ack_buf.ret[1] = afmGetAdcROffset() & 0xff;
            afm.ack_buf.len = 2;
        break;
        case afiSetAdcGOffset:
            afmSetAdcGOffset(afm.pkt.cmd.data1,afm.pkt.cmd.data2);
        break;
        case afiGetAdcGOffset:
            afm.ack_buf.ret[0] = (afmGetAdcGOffset()>>8) & 0xff;
            afm.ack_buf.ret[1] = afmGetAdcGOffset() & 0xff;
            afm.ack_buf.len = 2;
        break;
        case afiSetAdcBOffset:
            afmSetAdcBOffset(afm.pkt.cmd.data1,afm.pkt.cmd.data2);
        break;
        case afiGetAdcBOffset:
            afm.ack_buf.ret[0] = (afmGetAdcBOffset()>>8) & 0xff;
            afm.ack_buf.ret[1] = afmGetAdcBOffset() & 0xff;
            afm.ack_buf.len = 2;
        break;
        case afiReadNvram:
            if (afm.pkt.cmd.ext >0 && afm.pkt.cmd.ext <= 8)
            {
                afm.ack_buf.len = afm.pkt.cmd.ext;
                addr = ((U16)afm.pkt.cmd.data1 << 8) + afm.pkt.cmd.data2;
                afmReadNvram(addr, afm.ack_buf.ret, afm.ack_buf.len);
            }
        break;
        case afiWriteNvram:

            if (afm.pkt.cmd.ext >0 && afm.pkt.cmd.ext <= 8)
            {
                afm.ack_buf.len = afm.pkt.cmd.ext;
                addr = ((U16)afm.pkt.cmd.data1 << 8) + afm.pkt.cmd.data2;
                afmWriteNvram(addr, afm.pkt.tail, afm.ack_buf.len);
            }
        break;
        case afiChangeColorTempMode:
            afmChangeColorTemp(afm.pkt.cmd.ext,afm.pkt.cmd.data1,afm.pkt.cmd.data2);
            break;
        case afiSaveColorTemp:
            afmSaveColorTemp(afm.pkt.cmd.data2);
        break;
        case afiRecallColorTemp:
            afmRecallColorTemp(afm.pkt.cmd.data2);
        break;
        case afiChangeColorTempMode_6500K:
            afmChangeColorTemp_6500K(afm.pkt.cmd.data1,afm.pkt.cmd.data2);
            break;
        case afiChangeColorTempMode_9300K:
            afmChangeColorTemp_9300K(afm.pkt.cmd.data1,afm.pkt.cmd.data2);
            break;
        case afiChangeColorTempMode_11000K:
            afmChangeColorTemp_11000K(afm.pkt.cmd.data1,afm.pkt.cmd.data2);
            break;
        case afiSetRGain:
            afmSetRGain(afm.pkt.cmd.data1,afm.pkt.cmd.data2);
            break;
        case afiGetRGain:
            afm.ack_buf.ret[0] = (afmGetRGain()>>8) & 0xff;
            afm.ack_buf.ret[1] = afmGetRGain() & 0xff;
            afm.ack_buf.len = 2;
        break;
        case afiSetGGain:
            afmSetGGain(afm.pkt.cmd.data1,afm.pkt.cmd.data2);
        break;
        case afiGetGGain:
            afm.ack_buf.ret[0] = (afmGetGGain()>>8) & 0xff;
            afm.ack_buf.ret[1] = afmGetGGain() & 0xff;
            afm.ack_buf.len = 2;
        break;
        case afiSetBGain:
            afmSetBGain(afm.pkt.cmd.data1,afm.pkt.cmd.data2);
        break;
        case afiGetBGain:
            afm.ack_buf.ret[0] = (afmGetBGain()>>8) & 0xff;
            afm.ack_buf.ret[1] = afmGetBGain() & 0xff;
            afm.ack_buf.len = 2;
        break;
        case afiSetROffset:
            afmSetROffset(afm.pkt.cmd.data1,afm.pkt.cmd.data2);
        break;
        case afiGetROffset:
            afm.ack_buf.ret[0] = (afmGetROffset()>>8) & 0xff;
            afm.ack_buf.ret[1] = afmGetROffset() & 0xff;
            afm.ack_buf.len = 2;
        break;
        case afiSetGOffset:
            afmSetGOffset(afm.pkt.cmd.data1,afm.pkt.cmd.data2);
        break;
        case afiGetGOffset:
            afm.ack_buf.ret[0] = (afmGetGOffset()>>8) & 0xff;
            afm.ack_buf.ret[1] = afmGetGOffset() & 0xff;
            afm.ack_buf.len = 2;
        break;
        case afiSetBOffset:
            afmSetBOffset(afm.pkt.cmd.data1,afm.pkt.cmd.data2);
        break;
        case afiGetBOffset:
            afm.ack_buf.ret[0] = (afmGetBOffset()>>8) & 0xff;
            afm.ack_buf.ret[1] = afmGetBOffset() & 0xff;
            afm.ack_buf.len = 2;
        break;
        case afiSetContrast:
            afmSetContrast(afm.pkt.cmd.data1,afm.pkt.cmd.data2);
        break;
        case afiGetContrast:
            afm.ack_buf.ret[0] = 0;
            afm.ack_buf.ret[1] = afmGetContrast();
            afm.ack_buf.len = 2;
        break;
        case afiSetBrightness:
            afmSetBrightness(afm.pkt.cmd.data1,afm.pkt.cmd.data2);
        break;
        case afiGetBrightness:
            afm.ack_buf.ret[0] = 0;
            afm.ack_buf.ret[1] = afmGetBrightness();
            afm.ack_buf.len = 2;
        break;
        case afiAutoADC:
            afmAutoADC();
        break;
        case afiAutoAdjust:
            afmAutoAdjust();
        break;
        case afiSetFactoryDefault:
            afmSetFactoryDefault();
        break;
        case afiSetDefaultLang:
            afmSetDefaultLang();
        break;
        case afiSetInputSource:
            afmSetInputSource(afm.pkt.cmd.data1,afm.pkt.cmd.data2);
        break;
        case afiGetInputSource:
            afm.ack_buf.ret[0] = 0;
            afm.ack_buf.ret[1] = afmGetInputSource();
            afm.ack_buf.len = 2;
        break;
        case afiSetSerialNumber:
            {
                U8 SN_len;
            	for (i = 0; i < SerilNumberLength; i++)
            	{
            		if (i < afm.pkt.cmd.ext * SerilNumberPackageNum)
            			continue;
            		if (i < (afm.pkt.cmd.ext * SerilNumberPackageNum + (afm.pkt.length & 0x7F)-4))
            		{
            			if (i - afm.pkt.cmd.ext * SerilNumberPackageNum == 0)		// Data 1
            			{
            				nSerialNumber[i] = afm.pkt.cmd.data1;
            			}
            			else if(i - afm.pkt.cmd.ext * SerilNumberPackageNum == 1)	// Data 2
            			{
            				nSerialNumber[i] = afm.pkt.cmd.data2;
            			}
                        else if(i - afm.pkt.cmd.ext * SerilNumberPackageNum == 2)	// Data 2
            			{
            				nSerialNumber[i] = afm.pkt.cmd.data3;
            			}
                        else if(i - afm.pkt.cmd.ext * SerilNumberPackageNum == 3)	// Data 2
            			{
            				nSerialNumber[i] = afm.pkt.cmd.data4;
            			}
            			else
            			{
            				nSerialNumber[i] = afm.pkt.tail[i - afm.pkt.cmd.ext * SerilNumberPackageNum - 4];
            			}
            		}
            		else
            		{
            			nSerialNumber[i] = 0;
            		}
            	}
                SN_len = (afm.pkt.cmd.ext * SerilNumberPackageNum + (afm.pkt.length & 0x7F) -4);
                if (SN_len >=(SerilNumberPackageNum) && SN_len <= (SerilNumberLength-1))
                {
                    afmSetSerialNumber(nSerialNumber);
                }
            }
            break;
        case afiSetAudioVolume:
        case afiSetAudioVolumeGain:
					// TPV_XM Xue 20121122: modify volume up 50 shutdown issue
            //afmSetAudioVolume(afm.pkt.cmd.data3);
            memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
		stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_VOLUME_VALUE;
		 stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    		stRS232PostMsgSetting.ReciveData[0] = afm.pkt.cmd.data3;
            break;
        case afiGetAudioVolume:
            afm.ack_buf.ret[0] = afmGetAudioVolume();
            afm.ack_buf.len = 1;
            break;
        case afiSelectChannelNum:
            afmSelectChannelNum(afm.pkt.cmd.ext,afm.pkt.cmd.data1,afm.pkt.cmd.data2);
            break;
        case afiSelectFactoryChannelNum:
            afmSelectFactoryChannelNum(afm.pkt.cmd.data3,afm.pkt.cmd.data4,afm.pkt.tail[0],afm.pkt.tail[1],afm.pkt.tail[2],afm.pkt.tail[3]);
            break;
        case afiCheckUSBPortAttachment:
            {
                U8 u8USBCnt = 0;
                U8 u8USBPortAttachmentFlag;
                u8USBPortAttachmentFlag = afmCheckUSBPortAttachment(afm.pkt.cmd.data3);

                if(u8USBPortAttachmentFlag & BIT0)
                {
                    u8USBCnt ++;
                }
                if(u8USBPortAttachmentFlag & BIT1)
                {
                    u8USBCnt ++;
                }
                if(u8USBCnt == 0)
                {
                    afm.ack_buf.ret[0] = 0x00;
                }
                else
                {
                    afm.ack_buf.ret[0] = 0xff;
                }
                afm.ack_buf.ret[1] = u8USBCnt;
                afm.ack_buf.len = 2;
            }
            break;
        case afiSetTestPattern:
            afmSetTestPattern(afm.pkt.cmd.data1,afm.pkt.cmd.data2);

            break;
        case afiSetTestPatternRGB:
            afmSetTestPatternRGB(afm.pkt.cmd.data3,afm.pkt.tail[0],afm.pkt.tail[2]);

            break;
        case afiSetTestPatternGreyRGB:
            afmSetTestPatternRGB(afm.pkt.cmd.data3,afm.pkt.cmd.data3,afm.pkt.cmd.data3);

            break;
        case afiSetPictureMode:
            afmSetPictureMode(afm.pkt.cmd.data2);
            break;
        case afiGetHDCP_KSV:
            memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
            stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_GET_HDCP_KSV;
            stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
            break;
        case afiGetMCUVersion:
            memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
            stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_GET_MCU_VERSION;
            stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
            break;
        case afiGetNVRAMVersion:
            memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
            stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_GET_NVRAM_VERSION;
            stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
            break;
        case afiGetModelname:
            memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
            stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_GET_MODEL_NAME;
            stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
            break;
        case afiGetEDIDData:
            afmGetEdidData(afm.pkt.cmd.data3,afm.pkt.cmd.data4,afm.pkt.cmd.data2);
            break;
					// TPV_XM Xue 20121214: add new command
	  case afiSetPowerOFF:
            afmPowerOFF();
            break;
	  case afiSetExecuteFactoryDomainReset:
            afmExecuteFactoryDomainReset();
            break;	
					// TPV_XM Xue 20121214: add hash unlock
	case afiSetHASHUnlock:
		  afmHASHUnlock();
            break;
       // TPV_XM Xue 20121017: 
  #ifdef ENABLE_TPV_VCOM_ADJUST				
       case afiSetV_COM:
	    	 XUETRACE(printf("xue trace afmSetVcomValue=%d\n",afm.pkt.cmd.data2)) ;
			 if(afm.pkt.cmd.data2 <=127)
               		afmSetVcomValue(afm.pkt.cmd.data2);
       break;
       case afiGetV_COM:	   	
              afm.ack_buf.ret[0] = afmGetVcomValue();
              afm.ack_buf.len = 1;
       break;
   #endif
        default:
            break;
    }
}
// TPV_XM Xue 20120818: Ebony Command process
 #if ENABLE_TPV_EBONY_PBS_MODE
 extern  BOOLEAN MApp_IsSrcHasSignal(SCALER_WIN eWindow);
void se_control(U8 op,U8 item)
{
	
    //U16 addr;
    //U8 i;
   /*
   if(stGenSetting.g_PBSModeSetting.bPbsModeOn==TRUE)
   	seBDSMode=1;
   else
   	seBDSMode=0;
   */
   if ((seBDSMode == 0) && (op != seSetBDSON )&&(stGenSetting.g_PBSModeSetting.bPbsModeOn==FALSE))
    {
        return;
    }
 
  EN_RET enRetVal =EXIT_NULL;
    U8 MuteStatus=0; 
    U8 AudioOPStatus=1;    //   1 :IncrementVolume 0:Decrement
   U8 BalanceOPStatus=0;    //   1 :left balance  0:balance 2:right balance
    //PBS_CMD_SendACKMsg(0);
   switch (op)
    {

	case seSetBDSON:
		XUETRACE(printf("xue trace seSetBDSON\n"));
		PBS_CMD_SendACKMsg(0);
		seBDSMode=1;
		stRS232PostMsgSetting.enRS232PostMsgType = Ebony_RS232_POST_MSG_SET_Pbs_On;
		break;
	case seSetBDSOFF:
		XUETRACE(printf("xue trace seSetBDSOFF\n"));
		PBS_CMD_SendACKMsg(0);
		seBDSMode=0;
		stRS232PostMsgSetting.enRS232PostMsgType = Ebony_RS232_POST_MSG_SET_Pbs_Off;
		break;
	case seSetBDSInstall:
		XUETRACE(printf("xue trace seSetBDSInstall\n"));
		PBS_CMD_SendACKMsg(0);
		seSetBDSInstallModeContorl();
		break;	
	case seSetMuteON:
		XUETRACE(printf("xue trace seSetMuteON\n"));
		PBS_CMD_SendACKMsg(0);
		MuteStatus=1;	
		memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
		stRS232PostMsgSetting.enRS232PostMsgType = Ebony_RS232_POST_MSG_SET_Audio_MUTE;
		stRS232PostMsgSetting.ReciveData[0] = MuteStatus;
		//seSetMuteModeContorl(MuteStatus);
		break;
	case seSetMuteOFF:
		XUETRACE(printf("xue trace seSetMuteOFF\n"));
		PBS_CMD_SendACKMsg(0);
		MuteStatus=0;
		memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
		stRS232PostMsgSetting.enRS232PostMsgType = Ebony_RS232_POST_MSG_SET_Audio_UNMUTE;	
		stRS232PostMsgSetting.ReciveData[0] = MuteStatus;
		//seSetMuteModeContorl(MuteStatus);
		break;	
	case seSetVideoMute:		
		XUETRACE(printf("xue trace seSetVideoMute\n"));		
		PBS_CMD_SendACKMsg(0);
		memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
		stRS232PostMsgSetting.enRS232PostMsgType = Ebony_RS232_POST_MSG_SET_VIDEO_MUTE;
		break;	
	case seSetVideoUnMute:		
		XUETRACE(printf("xue trace seSetVideoUnMute\n"));
		PBS_CMD_SendACKMsg(0);
		memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
		stRS232PostMsgSetting.enRS232PostMsgType = Ebony_RS232_POST_MSG_SET_VIDEO_UNMUTE;

		break;		
	case seSetIncrementVolume:		
		PBS_CMD_SendACKMsg(0);
		XUETRACE(printf("xue trace seSetIncrementVolume\n"));		
		seSetIncrement_DecrementVolumeControl(AudioOPStatus);
		break;
	case seSetDecrementVolume:	
		PBS_CMD_SendACKMsg(0);
		AudioOPStatus=0;
		XUETRACE(printf("xue trace seSetDecrementVolume\n"));		
		seSetIncrement_DecrementVolumeControl(AudioOPStatus);
		break;
	case seSetBalanceMiddle:	
		PBS_CMD_SendACKMsg(0);
		BalanceOPStatus=0;
		XUETRACE(printf("xue trace seSetBalanceMiddle\n"));		
		seSetBalanceControl(BalanceOPStatus);
		break;
	case seSetBalanceLeft:	
		PBS_CMD_SendACKMsg(0);
		BalanceOPStatus=1;
		XUETRACE(printf("xue trace seSetBalanceLeft\n"));		
		 seSetBalanceControl(BalanceOPStatus);
		break;
	case seSetBalanceRight:	
		PBS_CMD_SendACKMsg(0);
		BalanceOPStatus=2;
		XUETRACE(printf("xue trace seSetBalanceRight\n"));		
		 seSetBalanceControl(BalanceOPStatus);
		break;	
	case seSetVolume:
		PBS_CMD_SendACKMsg(0);
		XUETRACE(printf("xue trace seSetVolume\n"));	
		 U8 VolumeTemp=g_UartCommand.Buffer[10];
		 XUETRACE(printf("xue trace VolumeTemp=%d\n",VolumeTemp));
		 if((VolumeTemp<= 100))
		{				// TPV_XM Xue 20121221: modify set volume shutdown issue
			memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
			stRS232PostMsgSetting.enRS232PostMsgType = Ebony_RS232_POST_MSG_SET_Volume_Value;				
			stRS232PostMsgSetting.ReciveData[0] = VolumeTemp;
			//seSetVolumeControl(VolumeTemp);
		}
		break;	
	case seSetBrightness:
		PBS_CMD_SendACKMsg(0);
		XUETRACE(printf("xue trace seSetBrightness\n"));	
		 U8 BrightnessTemp=g_UartCommand.Buffer[10];
		 XUETRACE(printf("xue trace seSetBrightness=%d\n",BrightnessTemp));
		   if(BrightnessTemp<= 100)
		{			
			seSetBrightnessControl(BrightnessTemp);
		}
		break;
	case seSetColor:
		PBS_CMD_SendACKMsg(0);
		XUETRACE(printf("xue trace seSetColor\n"));	
		 U8 ColorTemp=g_UartCommand.Buffer[10];
		 XUETRACE(printf("xue trace seSetColor=%d\n",ColorTemp));
		  if(ColorTemp<= 100)
		{			
			seSetColorControl(ColorTemp);
		}
		break;
	case seSetContrast:
		PBS_CMD_SendACKMsg(0);
		XUETRACE(printf("xue trace seSetContrast\n"));	
		 U8 ContrastTemp=g_UartCommand.Buffer[10];
		 XUETRACE(printf("xue trace seSetContrast=%d\n",ContrastTemp));
		 if(ContrastTemp<= 100)
		{			
			seSetContrastControl(ContrastTemp);
		}
		break;	
	case seSetSharpness:
		PBS_CMD_SendACKMsg(0);
		XUETRACE(printf("xue trace seSetSharpness\n"));	
		 U8 SharpnessTemp=g_UartCommand.Buffer[10];
		 XUETRACE(printf("xue trace seSetSharpness=%d\n",SharpnessTemp));
		 if(SharpnessTemp<= 100)
		{
			seSetSharpnessControl(SharpnessTemp);
		}
		break;	
	case seSetHue:
		PBS_CMD_SendACKMsg(0);
		XUETRACE(printf("xue trace seSetHue\n"));	
		 U8 HueTemp=g_UartCommand.Buffer[10];
		 XUETRACE(printf("xue trace seSetHue=%d\n",HueTemp));
		  if(HueTemp<= 100)
		{
			seSetHueControl(HueTemp);
		}
		break;	
	case seSetColorTemperature:
		PBS_CMD_SendACKMsg(0);
		XUETRACE(printf("xue trace seSetColorTemperature\n"));	
		 U8 ColorTemperatureTemp=g_UartCommand.Buffer[10];
		  XUETRACE(printf("xue trace seSetColorTemperature=%d\n",ColorTemperatureTemp));
		 if((ColorTemperatureTemp==0)||(ColorTemperatureTemp==1)||(ColorTemperatureTemp==2))//0:Warm 1:Normal 2:Cool
		 	{		
			 	seSetColorTemperatureControl(ColorTemperatureTemp);
		 	}
		break;

	case seSetAutoAdjust:
		PBS_CMD_SendACKMsg(0);
		XUETRACE(printf("xue trace seSetAutoAdjust\n"));	
		if((MApp_IsSrcHasSignal(MAIN_WINDOW))&&(IsVgaInUse()))
		{
			seSetAutoAdjustControl();
		}
	case seSetSource:
		PBS_CMD_SendACKMsg(0);
		XUETRACE(printf("xue trace seSetSource\n"));	
		U8 SourceTemp=g_UartCommand.Buffer[10];
		XUETRACE(printf("xue trace SourceTemp=%d\n",SourceTemp));
		// 1:TV 3:AV 5:CVI 6:VGA 7:USB 8:HDMI1 9:HDMI2
		if((SourceTemp==1)||(SourceTemp==3)||(SourceTemp==5)||(SourceTemp==6)||(SourceTemp==7)||(SourceTemp==8)||(SourceTemp==9)||(SourceTemp==14)||(SourceTemp==15))
		{
			seSetSourceSwitchControl(SourceTemp);
		}
		break;		

	case seSetPictureFormat:
		PBS_CMD_SendACKMsg(0);
		XUETRACE(printf("xue trace seSetPictureFormat\n"));	
		U8 PictureFormatTemp=g_UartCommand.Buffer[10];
		XUETRACE(printf("xue trace seSetPictureFormat=%d\n",PictureFormatTemp));
		//0-->4:3  1-->Zoom 14:9  2-->Zoom 16:9  4--> Wide Screen 5-->Super Zoom  7-->Auto
		   if((IsStorageInUse() == FALSE) && MApp_IsSrcHasSignal(MAIN_WINDOW))// && (stGenSetting.g_SysSetting.bIsBluescreenOn == ENABLE)
		 	{
			   if((PictureFormatTemp==0)||(PictureFormatTemp==1)||(PictureFormatTemp==2)||(PictureFormatTemp==4)||(PictureFormatTemp==5)||(PictureFormatTemp==6)||(PictureFormatTemp==7)) 
			 	{		
					memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
					stRS232PostMsgSetting.enRS232PostMsgType = Ebony_RS232_POST_MSG_SET_Picture_Format;				
					stRS232PostMsgSetting.ReciveData[0] = PictureFormatTemp;
				 	//seSetPictureFormatControl(PictureFormatTemp);
			 	}
		 	}
		break;		
	case seSetUserInputControl:
		PBS_CMD_SendACKMsg(0);
		XUETRACE(printf("xue trace seSetUserInputControl\n"));	
		U8 UserInputControlTemp=g_UartCommand.Buffer[10];
		XUETRACE(printf("xue trace seSetUserInputControl=%d\n",UserInputControlTemp));
		//0 ¨C Lock both IR and LKB.    1 ¨C Enable IR but lock LKB.   2 ¨C Enable LKB but lock IR  3 - Enable both LKB and IR
		 if((UserInputControlTemp==0)||(UserInputControlTemp==1)||(UserInputControlTemp==2)||(UserInputControlTemp==3))
	 	{
	 		seSetUserInputControlState(UserInputControlTemp);
	 	}
		break;	
	case seSetFactoryReset:
		PBS_CMD_SendACKMsg(0);
		XUETRACE(printf("xue trace seSetFactoryReset\n"));		
		seSetFactoryResetControl();
		break;	
	case seSetOSDLanguage:
		PBS_CMD_SendACKMsg(0);
		XUETRACE(printf("xue trace seSetOSDLanguage\n"));	
		g_u8OSDLangSelected=0;
		g_u8OSDLangSelected=g_UartCommand.Buffer[10];
		memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
		stRS232PostMsgSetting.enRS232PostMsgType = Ebony_RS232_POST_MSG_SET_OSD_Language;

		//seSetOSDLanguageControl();
		break;
	case seSetVolumeLimits:
		PBS_CMD_SendACKMsg(0);
		XUETRACE(printf("xue trace seSetVolumeLimits\n"));	
		U8 MaxVolumeTemp=g_UartCommand.Buffer[10];
		U8 LimitVolumeTemp=g_UartCommand.Buffer[11];
		XUETRACE(printf("xue trace MaxVolumeTemp=%d\n",MaxVolumeTemp));
		XUETRACE(printf("xue trace VolumeTemp=%d\n",VolumeTemp));			
		 if((LimitVolumeTemp<= 100)&&(MaxVolumeTemp<= 100) )
		 	{
		 		//seSetVolumeLimitsControl(MaxVolumeTemp,LimitVolumeTemp);
				memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
				stRS232PostMsgSetting.enRS232PostMsgType = Ebony_RS232_POST_MSG_SET_Volume_Limits;				
				stRS232PostMsgSetting.ReciveData[0] = MaxVolumeTemp;
				stRS232PostMsgSetting.ReciveData[1] = LimitVolumeTemp;
		 	}
		 break;	
	case seSetUserInputData:
		PBS_CMD_SendACKMsg(0);
		 XUETRACE(printf("xue trace seSetUserInputData\n"));		
		XUETRACE(printf("xue trace IRSystemCode=%d\n",g_UartCommand.Buffer[10]));
		XUETRACE(printf("xue trace IR_Command_Code=%d\n",g_UartCommand.Buffer[11]));	
		XUETRACE(printf("xue trace Keypress status=%d\n",g_UartCommand.Buffer[12]));
		XUETRACE(printf("xue trace Local_Key_Code=%d\n",g_UartCommand.Buffer[13]));			
		memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
		stRS232PostMsgSetting.enRS232PostMsgType= Ebony_RS232_POST_MSG_SET_UserInputData;
		stRS232PostMsgSetting.ReciveData[0] = g_UartCommand.Buffer[10];
		stRS232PostMsgSetting.ReciveData[1] = g_UartCommand.Buffer[11];
		stRS232PostMsgSetting.ReciveData[2] = g_UartCommand.Buffer[12];
		stRS232PostMsgSetting.ReciveData[3] = g_UartCommand.Buffer[13];		
		 break;
  	case seGetPowerState:
		 XUETRACE(printf("xue trace seGetPowerState\n"));	
		 if( enRetVal ==EXIT_GOTO_STANDBY)
		 	{
		 		PBS_CMD_SendACKMsg(2);
		 	}
		 else
		 	{
		 		PBS_CMD_SendACKMsg(3);
		 	}
		 break;
	case seSetPowerOFF:
		PBS_CMD_SendACKMsg(0);
		seSetPowerStateContorl(stAfmItem[item].cmd.cmd);
		break;
	case seSetPowerON:	
		XUETRACE(printf("seSetPowerON"));
		PBS_CMD_SendACKMsg(0);
		if( enRetVal ==EXIT_GOTO_STANDBY)	
		{
			seSetPowerStateContorl(1);
		}
		break;
		
	default:
		break;

    }

}		
#endif
//---------------------------------------------------------------------------
void afm_make_ack_ok_msg(void)
{
    afm.ack_buf.ack1 = tblAfmAckOkMsg[afm.pkt_cnt];
    afm.ack_buf.ack2 = ~afm.ack_buf.ack1;   // 2nd byte value = 1st byte 1's complement value
}
//---------------------------------------------------------------------------
void afm_make_ack_ng_msg(void)
{
    afm.ack_buf.ack1 = tblAfmAckNgMsg[afm.pkt_cnt];
    afm.ack_buf.ack2 = ~afm.ack_buf.ack1;   // 2nd byte value = 1st byte 1's complement value
}
//---------------------------------------------------------------------------
void afm_send_acknowledge(void)
{
    U8 i;
	U8* buf = (U8*)&afm.ack_buf;

    for (i = 1; i < 4+afm.ack_buf.len; i++)
    {
        afm_putc(*(buf+i));
    }
}
// TPV_XM Xue 20120817:  Ebony Command process
 #if ENABLE_TPV_EBONY_PBS_MODE
 void EbonyCmd_Process()	
{
	
	U8 item, i;
	for (item = 0; item < sizeof(stAfmItem) / sizeof(afm_item); item++)
	{
		for (i = 0; i < stAfmItem[item].size; i++)
		{
		//printf("xue trace g_UartCommand.Buffer= [%x]\n", g_UartCommand.Buffer[i+8]);
		//printf("xue trace stAfmItem[item].cmd= [%x]\n",*((U8*)&stAfmItem[item].cmd + i)) ;
			if ((g_UartCommand.Buffer[i+8]) != *((U8*)&stAfmItem[item].cmd + i))
			{		
				break;
			}
		}
		//printf("xue trace i=%d\n",i);
		   if (i == stAfmItem[item].size)
		   	{
		   		//printf("xue trace stAfmItem[item].api=%d\n",stAfmItem[item].api);		   		
		   		 se_control(stAfmItem[item].api,item);
				
		   	}
	}
	
}
#endif


//---------------------------------------------------------------------------
void afm_parse(U8 c)
{
    U8 item, i;
    U8 *pkt = (U8 *)&afm.pkt;

    if (afm.buf_cnt == 0 && c != afm_slave_addr) return;
    *(pkt + afm.buf_cnt+1) = c;
    if (afm.buf_cnt == afm_pkt_len_byte)
        afm.pkt_len = c & 0x7F;
    afm.buf_cnt ++;
    afm.chksum ^= c;
    if (afm.buf_cnt == sizeof(afm_pkt) || afm.buf_cnt==(afm.pkt_len+afm_cmd_header_len+afm_cmd_chksum_len))
    {
        #if 0
        for(i=0;i<sizeof(afm_pkt);i++)
        {
            printf("\n afm_pkt [%bu]=[%02bx]",i,*(pkt + i) );/*Creass.liu at 2012-07-04*/
        }

        printf("\n1,afm.pkt.slave =[%02bx] \n",afm.pkt.slave);
        printf("2,afm.pkt.source =[%02bx] \n",afm.pkt.source);
        printf("3,afm.pkt.length =[%02bx] \n",afm.pkt.length);
        printf("4,afm.pkt.cmd.pre1 =[%02bx] \n",afm.pkt.cmd.pre1);
        printf("5,afm.pkt.cmd.pre2 =[%02bx] \n",afm.pkt.cmd.pre2);
        printf("6,afm.pkt.cmd.cmd =[%02bx] \n",afm.pkt.cmd.cmd);
        printf("7,afm.pkt.cmd.ext =[%02bx] \n",afm.pkt.cmd.ext);
        printf("8,afm.pkt.cmd.data1 =[%02bx] \n",afm.pkt.cmd.data1);
        printf("9,afm.pkt.cmd.data2 =[%02bx] \n",afm.pkt.cmd.data2);
        printf("10,afm.pkt.cmd.data3 =[%02bx] \n",afm.pkt.cmd.data3);
        printf("11,afm.pkt.cmd.data4 =[%02bx] \n",afm.pkt.cmd.data4);
        printf("12,afm.pkt.tail[0] =[%02bx] \n",afm.pkt.tail[0]);
        printf("13,afm.pkt.tail[1] =[%02bx] \n",afm.pkt.tail[1]);
        printf("14,afm.pkt.tail[2] =[%02bx] \n",afm.pkt.tail[2]);
        printf("15,afm.pkt.tail[3] =[%02bx] \n",afm.pkt.tail[3]);
        printf("16,afm.pkt.tail[4] =[%02bx] \n",afm.pkt.tail[4]);
        printf("17,afm.pkt.tail[5] =[%02bx] \n",afm.pkt.tail[5]);
        printf("18,afm.pkt.tail[6] =[%02bx] \n",afm.pkt.tail[6]);
        printf("19,afm.pkt.tail[7] =[%02bx] \n",afm.pkt.tail[7]);
        printf("20,afm.pkt.tail[8] =[%02bx] \n",afm.pkt.tail[8]);
        #endif

        for (item = 0; item < sizeof(stAfmItem) / sizeof(afm_item); item++)
        {
            for (i = 0; i < stAfmItem[item].size; i++)
            {
                if (pkt[i+afm_cmd_header_len+1] != *((U8*)&stAfmItem[item].cmd + i))
                {
                    break;
                }
            }
            //printf("item[%bu].size=[%bu], i=[%bu], checksum=[%bu] \n",item,stAfmItem[item].size,i,afm.chksum);/*Creass.liu at 2012-06-27*/
            if (i == stAfmItem[item].size && afm.chksum == 0)   // yes. this item
            {
                afm_control(stAfmItem[item].api);

                if(stRS232PostMsgSetting.enRS232PostMsgType == RS232_POST_MSG_NONE)
                {
                    afm_make_ack_ok_msg();
                }

                if (stAfmItem[item].api == afiExitAFM)
                {
                    afm_send_acknowledge();
                }
                break;
            }
        }
        if (item == sizeof(stAfmItem) / sizeof(afm_item))   // not support command
        {
            afm_make_ack_ng_msg();
            if (IsAfmMode())
            {
                afm_send_acknowledge();
            }
        }
        else
        {
            if (IsAfmMode())
            {
                if(stRS232PostMsgSetting.enRS232PostMsgType == RS232_POST_MSG_NONE)
                {
                    afm_send_acknowledge();
                }
            }
        }
        afm.buf_cnt = 0;
        afm.pkt_cnt ++;
        afm.pkt_cnt &= 0x0f;
        afm.ack_buf.len = 0;
        afm.chksum = 0;
    }
}
//---------------------------------------------------------------------------
BOOLEAN IsAfmMode(void)
{
    return fAfmMode;
}
//---------------------------------------------------------------------------
void SetAfmMode(U8 val)
{
	fAfmMode = val;

}
//---------------------------------------------------------------------------

