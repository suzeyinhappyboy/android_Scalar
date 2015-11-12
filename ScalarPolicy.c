/******************************************************************************
 * Implement for urat linstener, monitor the command from the scalar.
 *
 * Copyright (c) TPV IC. All rights reserved.
 *
 * author:
 * modification:
 * 
*******************************************************************************/
#include "ScalarPolicy.h"
#include "UartExecutor.h"
#include "ScalarDdcParse.h"
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/earlysuspend.h>

//bob+[
#ifdef CONFIG_HAS_WAKELOCK
#include <linux/wakelock.h>
#endif
//bob+]

#define KEYCODE_DPAD_RIGHT 		(22)
#define INP_KEYCODE_INTERVAL	(200) 		/* key receive interval (ms)        */

static unsigned char local_buf[256];    

 static int scalarpolicy_remove(struct platform_device *pdev);  //bob+
 static long ScalarPolicy_ioctl(struct file *fp, unsigned int cmd, unsigned long arg);
 static int ScalarPolicy_open(struct inode *, struct file *);
 static ssize_t ScalarPolicy_read(struct file *fp,  char __user *buf, size_t size, loff_t *ppos);
 static int ScalarPolicy_release(struct inode *, struct file *);
 static ssize_t ScalarPolicy_write(struct file *fp, const char __user *data, size_t len, loff_t *ppos);
 static int __devinit scalarpolicy_probe(struct platform_device *pdev);

static const struct file_operations scalarexector_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = ScalarPolicy_ioctl,
	.open = ScalarPolicy_open,
	.release = ScalarPolicy_release,
	.read = ScalarPolicy_read,
	.write = ScalarPolicy_write,
};

typedef struct Scalarpolicy_DrvData_T
{
	struct miscdevice       *misc_dev;   
	struct input_polled_dev *poll_input_dev; 
}Scalarpolicy_DrvData, *pScalarpolicy_DrvData;

//bob+[
struct sclar_pm_info {
struct wake_lock wake_lock;
};
static struct sclar_pm_info *spi;
//bob+]


////////////////////////////////////////////////////////////////////////////////
extern const UartExecutor g_UartExecutor;
extern const ScalarDdcParse g_ScalarDdcParse;

////////////////////////////////////////////////////////////////////////////////
extern unsigned char _SerialNumBuf[14];
extern unsigned char _MacAddBuf[6];
extern unsigned char _ScalerVerBuf[5];//robert
extern unsigned char _ModelNameBuf[14];

////////////////////////////////////////////////////////////////////////////////
static struct input_polled_dev  *ScalarPolicy_dev = NULL;

///////////////////////////////////////////////////////////////////////////////
extern unsigned char R_state,mPowerOffMode,backlight_level;
//extern unsigned char _PktNumber;
extern unsigned char scalar_feedback_state;
extern bool scalar_feedback_brightness; 
extern int set_brightness_count;
extern bool get_brightness_done;
extern bool get_scalerversion_flag;
extern bool get_model_name;
extern int get_scalerversion_count;
static unsigned char old_R_state = BOOTING;
//static unsigned char get_Mac_flag = 0; 
static unsigned char resend_flag = 0;		//used to jude resend 
//static unsigned char retry_flag = 0;
extern bool getethmac;
extern bool getsn;
extern unsigned char gLanguage;
static int recovery_mode = 0;
extern unsigned int screenon[2];
unsigned int inum=0;
extern bool scalerdebug; //add debug 

bool set_auto_poweroff=false;
static unsigned char POWEROFF_MINUTE=0xff;

Cmd_List ScalarCommand[6] = {
	 
	 {0x6E,0x51,0x86,0x03,0xF5,0xDC,0x81,0x01,0x03,0x00},
	 
	 {0x6E,0x51,0x86,0x03,0xF5,0xDC,0x82,0x01,0x01,0x00},
 
	 {0x6E,0x51,0x86,0x03,0xF5,0xDC,0x83,0x01,0x00,0x00},
	 
	 {0x6E,0x51,0x86,0x03,0xF5,0xDC,0x84,0x01,0x01,0x00},
	 
	 {0x6E,0x51,0x86,0x03,0xF5,0xDC,0x85,0x01,0x01,0x00},
	 
	 {0x6E,0x51,0x86,0x03,0xF5,0xDC,0x81,0x01,0x00,0x00}
	 
	 
	 };
	 
Cmd_List AndroidCommand[13] = {
	 
	 {0x6E,0x51,0x86,0x03,0xF5,0xDD,0x81,0x01,0x01,0x00},
	 
	 {0x6E,0x51,0x86,0x03,0xF5,0xDD,0x82,0x01,0x01,0x00},
 
	 {0x6E,0x51,0x86,0x03,0xF5,0xDD,0x83,0x01,0x00,0x00},
	 
	 {0x6E,0x51,0x86,0x03,0xF5,0xDD,0x84,0x01,0x00,0x00},
	 
	 {0x6E,0x51,0x86,0x03,0xF5,0xDD,0x85,0xFF,0x00,0x00},
 
	 {0x6E,0x51,0x86,0x03,0xF5,0xDD,0x87,0x01,0xFF,0x14},
	 
 
	 {0x6E,0x51,0x86,0x01,0xF5,0xDD,0x81,0x01,0x00,0x00},
	 
	 {0x6E,0x51,0x86,0x01,0xF5,0xDD,0x82,0x01,0x00,0x00},
 
	 {0x6E,0x51,0x86,0x01,0xF5,0xDD,0x83,0x01,0x00,0x12},
	 
	 {0x6E,0x51,0x86,0x01,0xF5,0xDD,0x84,0x01,0x00,0x15},
	 
	 {0x6E,0x51,0x86,0x01,0xF5,0xDD,0x85,0x01,0x00,0x00},
		 
	 {0x6E,0x51,0x86,0x01,0xF5,0xDD,0x86,0x01,0x00,0x16},
 
	 {0x6E,0x51,0x86,0x01,0xF5,0xDD,0x87,0x01,0x00,0x16}
	 };


//struct early_suspend scalar_early_suspender;

//mis device to deal with writing operation from Android Space 
static struct miscdevice misc_scalar_dev =
{  
  .minor = MISC_DYNAMIC_MINOR,
  .name  = "Misc_Scalar_Policy",  
  .fops  = &scalarexector_fops,  
};  

////////////////////////////////////////Global Platform Driver////////////////////////
static struct platform_driver scalar_plfdevice_driver = 
{   
  .probe      = scalarpolicy_probe,
  .driver     = {   
                  .name = "Scalar_Policy",   
                  .owner= THIS_MODULE,
                  .remove     = scalarpolicy_remove,  //bob+
                },   
};

#define TEGRA_GPIO_PG5 53
#define TEGRA_GPIO_PX4 188
#define TEGRA_GPIO_PX5 189

static unsigned char keycode[62] = {
  [0]	 = 0,
	[1]	 = KEY_POWER,		// ?源キー
	[2]	 = KEY_VOLUMEUP,
	[3]	 = KEY_VOLUMEDOWN,
	[4]	 = 0,
	[5]	 = KEY_F5,			// 戻る
	[6]	 = 0,
	[7]	 = KEY_F5,			// 戻る(EXIT)
	[8]	 = 0,
	[9]	 = 0,
	[10]	 = KEY_MENU,		// メニュー
	[11]	 = KEY_UP,			// 上
	[12]	 = 0,
	[13]	 = KEY_ENTER,		// 決定
	[14]	 = 0,
	[15]	 = KEY_DOWN,		// 下
	[16]	 = 0,
	[17]	 = 0,
	[18]	 = 0,
	[19]	 = 0,
	[20]	 = 0,
	[21]	 = KEY_1,			// １
	[22]	 = KEY_2,			// ２
	[23]	 = KEY_3,			// ３
	[24]	 = KEY_PAGEUP,		// チャネル 上
	[25]	 = KEY_4,			// ４
	[26]	 = KEY_5,			// ５
	[27]	 = KEY_6,			// ６
	[28]	 = KEY_PAGEDOWN,	// チャネル 下
	[29]	 = KEY_7,			// ７
	[30]	 = KEY_8,			// ８
	[31]	 = KEY_9,			// ９
  [32]   = KEYCODE_DPAD_RIGHT,
};

/*******************************************************************************
 **
 **
 **  
********************************************************************************/
static void ScalarPolicy_poll(struct input_polled_dev *dev)
{
    
	int size_;
	int len = MAX_PKT_LENGTH;
	int rret,wret;
	unsigned char c,check_sum;
	unsigned char* fb_state = (unsigned char*)&AndroidCommand[Set_Backlight_Level];
	unsigned char* brightness = (unsigned char*)&AndroidCommand[Android_Get_Backlight_Level];
	unsigned char* Auto_Pwoff = (unsigned char*)&AndroidCommand[Auto_Poweroff];
	
	AndroidCommand[Auto_Poweroff].CmdData2 = POWEROFF_MINUTE;
	AndroidCommand[Android_Feedback_State].Checksum = 0x00;
	size_ = (len <= sizeof(local_buf))? len : sizeof(local_buf);
	memset(local_buf, 0 , sizeof(local_buf));
  	rret = g_UartExecutor.Read_ScalarTTyLocked (local_buf, len, NULL);
	
//*  //Eric	
	if(old_R_state != R_state){
		
		printk ("szy ScalarPolicy_poll -- old_R_state != R_state\n");
		old_R_state = R_state; 
		resend_flag = 0;

		//set pkt_nbr
		//fb_pm_stat[GEN_PKT_LENGTH] = SetPktNbr(R_state);
		AndroidCommand[Android_Feedback_State].CmdData2 = R_state;
		for(c = 0,check_sum = 0x00; c < (sizeof(AndroidCommand[Android_Feedback_State])-1); c ++){
			check_sum = check_sum^(*(fb_state+c));	//xor
		}
		AndroidCommand[Android_Feedback_State].Checksum= check_sum;
		wret = g_UartExecutor.Write_ScalarTTyLocked((unsigned char*)&AndroidCommand[Android_Feedback_State],sizeof(AndroidCommand[Android_Feedback_State]),NULL);
		if(CANCEL_SHUTDOWN == R_state){
			R_state = POWER_ON_READY;
		}
		
	}else if(old_R_state == R_state){    
		if(resend_flag < 11) {
			
			resend_flag++;	//UART_ACK_TIME = 2s,poll_interval = 200ms,so UART_ACK_TIME = poll_interval * 10;
		}
		if((scalar_feedback_state != old_R_state) && (resend_flag == 10)){		
//			fb_pm_stat[GEN_PKT_LENGTH] = SetPktNbr(old_R_state);
		
			//check sum
			printk ("szy ScalarPolicy_poll -- scalar_feedback_state != old_R_state\n");
			AndroidCommand[Android_Feedback_State].CmdData2 = R_state;
			for(c = 0,check_sum = 0x00; c < (sizeof(AndroidCommand[Android_Feedback_State])-1); c ++){
			check_sum = check_sum^(*(fb_state+c));	//xor
			}
			AndroidCommand[Android_Feedback_State].Checksum= check_sum;
			wret = g_UartExecutor.Write_ScalarTTyLocked((unsigned char*)&AndroidCommand[Android_Feedback_State],sizeof(AndroidCommand[Android_Feedback_State]),NULL);
		}
	}
//*/
	if(rret > 0){
  		g_ScalarDdcParse.Parse_DDC_Packet(dev,local_buf,rret);
	}

	if(0 == recovery_mode){
		AndroidCommand[Android_Feedback_State].CmdData2 = BOOT_MENU;
		AndroidCommand[Android_Feedback_State].Checksum = 0x94;
		g_UartExecutor.Write_ScalarTTyLocked((unsigned char*)&AndroidCommand[Android_Feedback_State],sizeof(AndroidCommand[Android_Feedback_State]),NULL);
		recovery_mode++;
	}
	else if(getsn && !getethmac)
	{
		printk("szy send command for get_serial_num \n");
		wret =g_UartExecutor.Write_ScalarTTyLocked((unsigned char*)&AndroidCommand[Read_EDID_Serial_Number],sizeof(AndroidCommand[Read_EDID_Serial_Number]),NULL);  
	}
	else if(get_model_name && !getsn)
	{
		printk("szy send command for get modelname \n");
		wret = g_UartExecutor.Write_ScalarTTyLocked((unsigned char*)&AndroidCommand[Send_to_get_modelname],sizeof(AndroidCommand[Send_to_get_modelname]),NULL);
	}
	else if(!get_brightness_done && !get_model_name){
		wret = g_UartExecutor.Write_ScalarTTyLocked((unsigned char*)&AndroidCommand[Android_Get_Backlight_Level],sizeof(AndroidCommand[Android_Get_Backlight_Level]),NULL);
		get_brightness_done = true;
		if(scalerdebug)		
		{			
			for(inum=0;inum<sizeof(AndroidCommand[Android_Get_Backlight_Level]);inum++)			
			{					
				printk("szy get_scalar_brightness data[%d]= 0x%x \n",inum,*(brightness+inum));			
			}		
		}			
	}
	
	if (set_auto_poweroff) 
	{
		AndroidCommand[Auto_Poweroff].CmdData2= POWEROFF_MINUTE;
		for(c = 0,check_sum = 0x00; c < (sizeof(AndroidCommand[Auto_Poweroff])-1); c ++){
			check_sum = check_sum^(*(Auto_Pwoff+c)); //xor
		}
		AndroidCommand[Auto_Poweroff].Checksum= check_sum;
		wret = g_UartExecutor.Write_ScalarTTyLocked((unsigned char*)&AndroidCommand[Auto_Poweroff],sizeof(AndroidCommand[Auto_Poweroff]),NULL);
		set_auto_poweroff = false;
	}


}

/*******************************************************************************
 **
 **
 **  
********************************************************************************/
static ssize_t ScalarPolicy_write(struct file *fp, const char __user *data, size_t len, loff_t *ppos)
{	
  //if android powerstate changed 
  //the function  set_screen_state()@power.c -- android_os_power.cpp  will call ioctl to reach this function
  //this function should keep a lock until exit out, un-reentrantable
  unsigned char buf[3] = {0};
  int wret;
  unsigned char c,check_sum;
 unsigned char* Backlight = (unsigned char*)&AndroidCommand[Set_Backlight_Level];
   AndroidCommand[Set_Backlight_Level].CmdData2 = backlight_level;
  //printk ("ScalarPolicy_write CMD = %d\n", buf[0]);
  
  if (copy_from_user(&buf, data, len)) {
      return -1;
  }

  printk ("ScalarPolicy_write CMD++ = %d\n", buf[0]);

  switch(buf[0]){
	  case CMD_TYPE_0X81:
	  	  R_state = buf[1];
		  printk("CMD_TYPE_0X81   R_state =%x \n",buf[1]);
	  	  break;
	  case CMD_TYPE_0X82:
	  	  break;
	  case CMD_TYPE_0X83:
	  	  break;
	  case CMD_TYPE_0X84:
	  	  
	  	  break;
	  case CMD_TYPE_0X85:
		  set_brightness_count ++;		  	 
		  if(scalar_feedback_brightness || set_brightness_count >3)	  	  
		  	{	  	        
		  	  set_brightness_count=0;	  	  
			  backlight_level = buf[1];
			  AndroidCommand[Set_Backlight_Level].CmdData2= backlight_level;
			  AndroidCommand[Set_Backlight_Level].CmdData1= buf[2];
			  for(c = 0,check_sum = 0x00; c < (sizeof(AndroidCommand[Set_Backlight_Level])-1); c ++){
				check_sum = check_sum^(*(Backlight+c));	//xor
			  }
			  AndroidCommand[Set_Backlight_Level].Checksum= check_sum;
			  wret = g_UartExecutor.Write_ScalarTTyLocked((unsigned char*)&AndroidCommand[Set_Backlight_Level],sizeof(AndroidCommand[Set_Backlight_Level]),NULL);
			  scalar_feedback_brightness = false;
			  if(scalerdebug)			 
			  	{				  
			  	for(inum=0;inum<sizeof(AndroidCommand[Set_Backlight_Level]);inum++)				 
					{						
					printk("szy set_scalar_backlight_level data[%d]= 0x%x \n",inum,*(Backlight+inum));
					}			 
				}	  	  
			  }
	  	  //}
	  	  break;
		case CMD_TYPE_0X87:
		printk("%s : CMD_TYPE_0X87\n", __func__ );
		c = (buf[1] + (buf[2]<<8))/10; // unit: 10 minutes
		if (POWEROFF_MINUTE != c)
		{
			printk("%s : POWEROFF_MINUTE=0x%x ==>0x%x buf[1]=0x%x buf[2]=0x%x\n", __func__ ,POWEROFF_MINUTE, c, buf[1], buf[2]);
			POWEROFF_MINUTE = c;
			set_auto_poweroff = true;
		}
	  	  break;
  }
  
  //R_state = buf;
  
  return 0;
}

/*******************************************************************************
 **
 **
 **  
********************************************************************************/
static ssize_t ScalarPolicy_read(struct file *fp,  char __user *buf, size_t size, loff_t *ppos)
{
   unsigned char mode_;
   mode_ = mPowerOffMode;

   printk ("ScalarPolicy_read\n");

   if (copy_to_user(buf,&mode_,1)) {
	   return -1;
   }

   return 0;
}

/******************************************************************************* 
********************************************************************************/
static long ScalarPolicy_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
  int _get_cmd = cmd;
  int wret;
  unsigned char c,check_sum; 
  unsigned char USB_State[2];
  unsigned char* buf = (unsigned char*)&AndroidCommand[Set_USB_Hub_Power];
  unsigned char* languagebuf = (unsigned char*)&AndroidCommand[Set_Language];
  printk ("ScalarPolicy_ioctl CMD = %x\n", _get_cmd);

  switch(_get_cmd){
	 case SET_USB_HUB_PORT:	 
		 	printk("%s : SET_USB_HUB_PORT\n", __func__);
		 if(copy_from_user(&USB_State, (char *)arg, sizeof(USB_State)))
		 	{
		 	return -1;	  
			} 	  
		 for(c = 0,check_sum = 0x00; c < (sizeof(AndroidCommand[Set_USB_Hub_Power])-1); c ++)		 	
		 	{					 	
		 		check_sum = check_sum^(*(buf+c));		//xor	 		 	
		 	} 	  		 
		  AndroidCommand[Set_USB_Hub_Power].Checksum= check_sum;
		 wret = g_UartExecutor.Write_ScalarTTyLocked((unsigned char*)&AndroidCommand[Set_USB_Hub_Power],sizeof(AndroidCommand[Set_USB_Hub_Power]),NULL);
	break;

	case GET_ANDROID_SERIAL_NUMBER:
		if(true)		
			{			
			for(inum=0;inum<sizeof(_SerialNumBuf);inum++)			
				{						
				printk("GET_ANDROID_SERIAL_NUMBER serial number data[%d]= 0x%x \n",inum,_SerialNumBuf[inum]);
				}
			}
		if(copy_to_user((char *)arg, _SerialNumBuf, sizeof(_SerialNumBuf))) {
			return -1;
		}
	break;

	case GET_ANDROID_MAC_ADDRESS:
		if(copy_to_user((char *)arg, _MacAddBuf, sizeof(_MacAddBuf))) {
			return -1;
		}
	break;

	case GET_SCALAR_BRIGHTNESS:
		get_brightness_done = false;
		printk("%s : GET_SCALAR_BRIGHTNESS\n", __func__);
		if(scalerdebug)
			printk("szy GET_SCALAR_BRIGHTNESS value= %x \n", backlight_level);
		if(copy_to_user((char *)arg, &backlight_level, sizeof(backlight_level))) {
			return -1;
		}
	break;

	case SET_SCALAR_LANGUAGE:
		if(copy_from_user(&gLanguage, (char *)arg, sizeof(gLanguage))) {
			return -1;
		}
		AndroidCommand[Set_Language].CmdData2= gLanguage;
		for(c = 0,check_sum = 0x00; c < (sizeof(AndroidCommand[Set_Language])-1); c ++){
		  check_sum = check_sum^(*(languagebuf+c));		//xor
		}
		AndroidCommand[Set_Language].Checksum = check_sum;			
		wret = g_UartExecutor.Write_ScalarTTyLocked((unsigned char*)&AndroidCommand[Set_Language],sizeof(AndroidCommand[Set_Language]),NULL);
		if(scalerdebug)
			{
			for(inum=0;inum<sizeof(AndroidCommand[Set_Language]);inum++)	
				{					
				printk("set_scalar_language language data[%d]= 0x%x \n",inum,*(languagebuf+inum));
				}	
			}
	break;
    case SET_SCALAR_LANGUAGE_SYNC:
		if(copy_from_user(&gLanguage, (char *)arg, sizeof(gLanguage))){ 
			return -1;
		}

    break;
    case SENDMSG_TO_GET_SCALERVER:
		printk("%s : SENDMSG_TO_GET_SCALERVER\n", __func__);
		wret = g_UartExecutor.Write_ScalarTTyLocked((unsigned char*)&AndroidCommand[Send_to_get_scalarver],sizeof(AndroidCommand[Send_to_get_scalarver]),NULL);
		get_scalerversion_flag=true;
    break;
    case GET_SCALAR_VERSION:
		printk("%s : GET_SCALAR_VERSION\n", __func__);
		//g_UartExecutor.Write_ScalarTTyLocked(get_serial_num,sizeof(get_serial_num),NULL); 
		if(scalerdebug)
		{
			for(inum=0;inum<sizeof(_ScalerVerBuf);inum++)
			{	
					printk("GET_SCALAR_VERSION scaler version data[%d]= 0x%x \n",inum,_ScalerVerBuf[inum]);
			}
		}	
		if(copy_to_user((char *)arg, _ScalerVerBuf, sizeof(_ScalerVerBuf))) {
			return -1;
		}		
    break;
    case SET_SCREEN_STATE:
		printk("%s :SET_SCREEN_STATE\n", __func__);
		if(copy_from_user(&screenon, (char *)arg, sizeof(screenon))) {
			return -1;
		}
    break;
    case SENDMSG_TO_GET_MODELNAME:
		printk("%s : SENDMSG_TO_GET_MODELNAME\n", __func__);
		wret = g_UartExecutor.Write_ScalarTTyLocked((unsigned char*)&AndroidCommand[Send_to_get_modelname],sizeof(AndroidCommand[Send_to_get_modelname]),NULL);
		
    break;
    case GET_MODELNAME:
		printk("%s : GET_MODELNAME\n", __func__);
		if(copy_to_user((char *)arg, _ModelNameBuf, sizeof(_ModelNameBuf))) {
			return -1;
		}
    break;
	default:
	break;
  }
  
  return 0;
}

/*******************************************************************************
 **
 **
 **  
********************************************************************************/
static int ScalarPolicy_open(struct inode *node, struct file *fp)
{
  printk ("ScalarPolicy_open\n");
 
  return 0;   
}

/*******************************************************************************
 **
 **
 **  
********************************************************************************/
static int ScalarPolicy_release(struct inode *node, struct file *fp)
{
  printk ("ScalarPolicy_release\n");
  
  return 0;
}
/*
static void scalar_early_suspend(struct early_suspend *handler)
{
	R_state = SUSPEND_COMPLETE;
}

static void scalar_late_resume(struct early_suspend *handler)
{
	R_state = ANDROID_SELF_WAKEUP;
}
*/
/*******************************************************************************
 **     Implement For Scalar Platform Driver
 **
 **  
********************************************************************************/
static int __devinit scalarpolicy_probe(struct platform_device *pdev)
{
  int       ret;
  int       i;
  printk ("ScalarPolicy_init -- scalarpolicy_probe\n");
  misc_scalar_dev.parent = &pdev->dev;
  
  ret = misc_register(&misc_scalar_dev);
  if (0 != ret)
  {
    printk ("ScalarPolicy_init -- misc_register fail\n");
    return -EIO;
  }

  ScalarPolicy_dev = input_allocate_polled_device();
  if (NULL == ScalarPolicy_dev)
  {
    printk ("ScalarPolicy_init -- allocdev fail\n");
    return -ENOMEM;
  }
//bob+[
  spi = kzalloc(sizeof(struct sclar_pm_info), GFP_KERNEL);
  if (!spi)
  	return -ENOMEM;
  
  wake_lock_init(&spi->wake_lock, WAKE_LOCK_SUSPEND, "scalar_policy");
 //bob+]

  ScalarPolicy_dev->poll = ScalarPolicy_poll;
  ScalarPolicy_dev->poll_interval = INP_KEYCODE_INTERVAL;
  
  ScalarPolicy_dev->input->evbit[0] = BIT_MASK(EV_KEY);// | BIT_MASK(EV_REP);
  ScalarPolicy_dev->input->name = "ScalarPolicy";
  ScalarPolicy_dev->input->phys = "Scalar/input0";
  ScalarPolicy_dev->input->id.bustype = BUS_HOST;
  ScalarPolicy_dev->input->id.vendor  = 0xBABE;
  ScalarPolicy_dev->input->id.product = 0xBEEF;
  ScalarPolicy_dev->input->id.version = 0xDEDA;
    
  ScalarPolicy_dev->input->keycode = keycode;
  ScalarPolicy_dev->input->keycodesize = sizeof(unsigned char);
  ScalarPolicy_dev->input->keycodemax = sizeof(keycode);
  
  for (i = 0; i < sizeof(keycode); i++)
  {
    set_bit(keycode[i], ScalarPolicy_dev->input->keybit);
  }

  //printk ("ScalarPolicy_init -- input_register_polled_device\n");

  ret = input_register_polled_device(ScalarPolicy_dev);
  if (0 != ret)
  {
    printk ("ScalarPolicy_init -- input_register_device fail\n");
    wake_lock_destroy(&spi->wake_lock);  //bob+
    return -EIO;
  }

	wake_lock(&spi->wake_lock);  //bob+

  return 0;
}

//bob+[
static int scalarpolicy_remove(struct platform_device *pdev)
{
	wake_unlock(&spi->wake_lock);
	wake_lock_destroy(&spi->wake_lock);
	kfree(spi);
	return 0;
}
//bob+]

/*******************************************************************************
 **
 **
 **  
********************************************************************************/
static int __init ScalarPolicy_init(void)
{ 
  
  platform_driver_register (&scalar_plfdevice_driver);
  
  printk ("ScalarPolicy_init registered\n");

  return 0;
}

/*******************************************************************************
 **
 **
 **  
********************************************************************************/
static void __exit ScalarPolicy_exit(void)
{
  input_unregister_polled_device(ScalarPolicy_dev);
  
  misc_deregister(&misc_scalar_dev);

  platform_driver_unregister(&scalar_plfdevice_driver); 
  
  ScalarPolicy_dev = NULL;
  
  printk ("Goodbey linux_driver_module\n");
}

module_init(ScalarPolicy_init);
module_exit(ScalarPolicy_exit);

MODULE_DESCRIPTION("Virtual Device Interaction with Scalar");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("jiping.yu@tpv-tech.com");
