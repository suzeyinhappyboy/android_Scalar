 /*******************************************************************************/
#ifndef _DDC_PARSE_H_
#define _DDC_PARSE_H_

#include <linux/init.h>
#include <linux/slab.h>
//#include <linux/input.h>
#include <linux/input-polldev.h>

#include <linux/kernel.h>
#include <linux/module.h>

#define MONITOR_SLAVE_ADD 	0x6E
#define SOURCE_ADD			0x51
#define TPV_FACTORY_CODE	0xF5

#define NORM_SHUTDOWN		0x81
#define	SUSPEND			0x82
#define FORCE_SHUTDOWN		0x83
#define TURN_ON_BACKLIGHT	0x03
#define RESET_SLEEP_TIMER	0x04

#define	UART_WRITE 			0x03
#define UART_READ 			0x01

#define LEAVE_ANDROID_SRC	0x01
#define ENTER_ANDROID_SRC	0x02

#define NON_ANDROID_SRC		0x01
#define IN_ANDROID_SRC		0x02

////"Android Feedback PM State (A2S)"
typedef enum {
	BOOTING = 0x01,	
	POWER_ON_READY = 0x02,
	ANDROID_SELF_WAKEUP = 0x03,
	SHUTDOWN_IN	= 0x81,
    SHUTDOWN_COMPLETE	= 0x82,
    INTO_THE_SUSPEND_MODE = 0x83,
    SUSPEND_COMPLETE	= 0x84,
    CANCEL_SHUTDOWN	= 0x85,
    BOOT_MENU = 0x86,
    CHANGE_SOURCE_COMPLETE = 0x87
}android_pwr_stat;

//notification scalar to set language
typedef enum {
    ENGLISH = 0x00,
    FRENCH = 0x01,
    SPANISH = 0x02,
    GREEK = 0x03,
    GERMAN = 0x04,
    ITALIAN = 0x05,
    MAGYAR = 0x06,
    PORTUGUESE = 0x07,
    PORTUGUESE_BRAZIL = 0x08,    
    NEDERLANDS = 0x09,    
    SVENSKA = 0x0A,    
    SUOMI = 0x0B,    
    POLSKI = 0x0C,    
    CZECH = 0x0D,
    TURKISH = 0x0E,
    RUSSIAN = 0x0F,
    UKRAINIAN = 0x10,    
    KOREAN = 0x11,
    TRADITIONAL_CHINESE = 0x12,
    SIMPLIFIED_CHINESE = 0x13,
    JAPANESE = 0x14,
}android_language;

///////PktNbr2
typedef enum {
	BOOTING_PKTNBR = 0x01,	
	POWER_ON_READY_PKTNBR = 0x02,
	SHUTDOWN_IN_PKTNBR	= 0x81,
    SHUTDOWN_COMPLETE_PKTNBR = 0x82,
    INTO_THE_SUSPEND_MODE_PKTNBR = 0x83,
    SUSPEND_COMPLETE_PKTNBR	= 0x84,
    CANCEL_SHUTDOWN_PKTNBR	= 0x85
}android_stat_pktnbr;

////"Read EDID Serial Number   (direct mode)(Read Osd EDID Serial Number)"
#define GET_SCALAR_POWER_STATE  		0x70
#define GET_ANDROID_SERIAL_NUMBER		0x71
#define GET_ANDROID_MAC_ADDRESS			0x72
#define GET_SCALAR_BRIGHTNESS			0x73
#define SET_SCALAR_LANGUAGE				0x74
#define SET_SCALAR_LANGUAGE_SYNC		0x75    //language sync on bootup
#define SENDMSG_TO_GET_SCALERVER 	0x76 //robert
#define GET_SCALAR_VERSION 			0x77 //robert
#define SET_SCREEN_STATE                      0x78//robert 
#define SET_USB_HUB_PORT				0x79//usb  power setup
#define SENDMSG_TO_GET_MODELNAME	0x80	
#define GET_MODELNAME					0x81

#define Scalar_to_Android		0xDC
#define Android_to_Scalar		0xDD
#define CMD_TYPE_0X81		0x81
#define CMD_TYPE_0X82		0x82
#define CMD_TYPE_0X83		0x83
#define CMD_TYPE_0X84		0x84
#define CMD_TYPE_0X85		0x85
#define CMD_TYPE_0X89		0x89
#define CMD_BYTE_0XF0		0xF0
#define EXT_BYTE_0X01		0x01
#define CMD_TYPE_0X87		0x87

#define	STAT_ACK_LEN		2
#define	NORMAL_ACK_LEN		1
#define MIN_ACK_LEN			6	/*g1.5 min ack len = 3,  g2.0 =4,g3.0 = 6*/

#define MAX_PKT_LENGTH		256	
#define GEN_PKT_LENGTH		7
#define CMD_PKT_SIZE			6

#define IS_SUPPORT_BY_TV_SET	0x01
#define NOT_SUPPORT_BY_TV_SET	0x00
/*
typedef struct DDC_Command_T {
	
	
	char PktLen;		
	char Pre1Byte;		
  	char Pre2Byte;		
  	char CmdDirect;		
  	char CMD_Type;

  	char CMD_Data1;
	char CMD_Data2;
}DDC_Command,ptr_DDC_Command;
*/
typedef struct Ack_Return {
	char Ack1;
	char Ack2;

	char Data_len;
	char Data;
}Ack_RetFormat,ptr_Ack_RetFormat;

 //char Mnt_SlvAdd;	/* Monitor slave address (write) */
	//char SRC_Add;		/* Source address */
  typedef struct DDC_Command_T{
		  unsigned char Header1Byte;
		  unsigned char Header2Byte;
		  unsigned char PacketSize;	/* Packet Length (bit 7 always set to 1) */
		  unsigned char Pre1byte;	/* Write = 0x03 (Set VCP, Uart Write),Read = 0x01 (Get VCP, Uart Read). */
		  unsigned char Pre2byte; 	/* 0xF5, TPV factory code*/
		  unsigned char CmdDirection;
		  unsigned char CmdType;
		  unsigned char CmdData1;
		  unsigned char CmdData2;
		  unsigned char Checksum;
  }Cmd_List,DDC_Command,ptr_DDC_Command; 
   
  enum Scalar_to_Android{
	  Change_State,
	  Change_Source,
	  Update_Backlight_Level,
	  Update_Headphone_State,
	  Rotate_Display,
	  Get_Android_State
  };
  
  enum Android_to_Scalar{
	  Android_Feedback_State,
	  Blink_Power_LED,
	  Set_Language,
	  Set_USB_Hub_Power,
	  Set_Backlight_Level,
	  Auto_Poweroff,
  
	  Get_Scaler_Source_State,
	  Android_Get_USB_Hub_State,
	  Android_Get_Backlight_Level,
	  Read_EDID_Serial_Number,
	  Get_MAC_address,
	  Send_to_get_scalarver,
	  Send_to_get_modelname
  };


typedef struct ScalarDdcParse_T
{
	char (*Parse_DDC_Packet)(struct input_polled_dev *dev, char *ddc_buf, char dat_len);
	void (*TPVComm_TwoAckByteOkMsgGet)(unsigned char *pBuff);
	void (*TPVComm_TwoAckByteNotSupportMsgGet)(unsigned char *pBuff);
}ScalarDdcParse,ptr_ScalarDdcParse;

#endif

