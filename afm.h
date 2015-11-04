#ifndef afmH
#define afmH
//---------------------------------------------------------------------------
#include "datatype.h"
		// TPV_XM Xue 20120803: 
#include "MApp_GlobalSettingSt.h"
//---------------------------------------------------------------------------
#ifdef afmC
#define extern
#endif
//---------------------------------------------------------------------------
		// TPV_XM Xue 20120818: DBG
#define XUETRACE(x)    //x

//---------------------------------------------------------------------------
#define afm_cmd_header_len      3
#define afm_cmd_chksum_len      1
#define afm_pkt_len_byte        2
#define afm_max_cmd_data_len    8
#define afm_max_ack_len         8
#define afm_slave_addr          0x6E
//---------------------------------------------------------------------------
typedef struct
{
    U8	pre1;
    U8	pre2;
    U8	cmd;
    U8	ext;
    U8	data1;
    U8	data2;
    U8  data3;
    U8  data4;
} afm_cmd;
//---------------------------------------------------------------------------
typedef struct
{
    U8  revert;//for 16位数据对齐
    U8 	slave;
    U8 	source;
    U8 	length;
    afm_cmd		cmd;
    U8 	tail[afm_cmd_chksum_len + afm_max_cmd_data_len];
} afm_pkt;
//---------------------------------------------------------------------------
typedef struct
{
    U8   revert;
    U8   ack1;
    U8   ack2;
    U8   len;
    U8   ret[afm_max_ack_len];
} afm_ack;
//---------------------------------------------------------------------------
typedef struct
{
    afm_cmd     cmd;
    U8   size;
    U16  api;
} afm_item;
//---------------------------------------------------------------------------
typedef struct
{
    BOOLEAN    onoff;
    afm_pkt     pkt;
    afm_ack     ack_buf;
    U8   buf_cnt;
    U8   pkt_cnt;
    U8   pkt_len;
    U8   chksum;
} afm_machine;
//---------------------------------------------------------------------------
enum
{
    afiEnterAFM,
    afiExitAFM,
    afiNvmInit,
    afiBurnInOnOff,
    afiClrBackLightHour,
    afiSetBackLight,

    afiSetAdcRGain,
    afiGetAdcRGain,
    afiSetAdcGGain,
    afiGetAdcGGain,
    afiSetAdcBGain,
    afiGetAdcBGain,
    afiSetAdcROffset,
    afiGetAdcROffset,
    afiSetAdcGOffset,
    afiGetAdcGOffset,
    afiSetAdcBOffset,
    afiGetAdcBOffset,

    afiReadNvram,
    afiWriteNvram,

    afiChangeColorTempMode,
    afiSaveColorTemp,
    afiRecallColorTemp,
    afiChangeColorTempMode_6500K,
    afiChangeColorTempMode_9300K,
    afiChangeColorTempMode_11000K,

    afiSetRGain,
    afiGetRGain,
    afiSetGGain,
    afiGetGGain,
    afiSetBGain,
    afiGetBGain,
    afiSetROffset,
    afiGetROffset,
    afiSetGOffset,
    afiGetGOffset,
    afiSetBOffset,
    afiGetBOffset,

    afiSetContrast,
    afiGetContrast,
    afiSetBrightness,
    afiGetBrightness,

    afiAutoADC,
    afiAutoAdjust,
    afiSetFactoryDefault,
    afiSetDefaultLang,
    afiSetInputSource,
    afiGetInputSource,

    afiSetAudioVolumeGain,
    afiSetAudioVolume,
    afiGetAudioVolume,

    afiSelectChannelNum,
    afiSelectFactoryChannelNum,
    afiCheckUSBPortAttachment,
    afiSetTestPattern,
    afiSetTestPatternRGB,
    afiSetTestPatternGreyRGB,
    afiSetPictureMode,

    afiGetMCUVersion,
    afiGetNVRAMVersion,
    afiGetModelname,

    afiGetEDIDData,

    afiSetSerialNumber,
    afiGetHDCP_KSV,
   // TPV_XM Xue 20121214: add new command
   afiSetPowerOFF,
   afiSetExecuteFactoryDomainReset,
   // TPV_XM Xue 20121214: HASH Unlock
   afiSetHASHUnlock,	
    		// TPV_XM Xue 20121017: 
  #ifdef ENABLE_TPV_VCOM_ADJUST	
    afiSetV_COM,
    afiGetV_COM,
    #endif
 #if ENABLE_TPV_EBONY_PBS_MODE
 	seSetPowerON,
	seSetPowerOFF,
	seSetBDSON,
	seSetBDSOFF,
	seSetBDSInstall,
	seSetMuteON,
	seSetMuteOFF,
	seSetVideoMute,
	seSetVideoUnMute,	
	seSetPixelPlusON,
	seSetPixelPlusOFF,	
	seSetSubtitleModeON,
	seSetSubtitleModeOFF,
	seSetTeletextModeON,
	seSetTeletextModeOFF,
	seSetLocalOSDSuppressON,
	seSetLocalOSDSuppressOFF,
	seSetExternalOSDSuppressON,
	seSetExternalOSDSuppressOFF,
	seSetPowerAtColdStartON,
	seSetPowerAtColdStartOFF,
	seSetAmbilightON,
	seSetAmbilightOFF,
	seSetFactoryReset,
//multi param
	seSetPictureFormat,
	seSetSource,
	seSetColorTemperature,
	seSetAudioLanguage,
	seSetOSDLanguage,
	seSetAmbiMode,
	seSetUserInputData,
//var param
	seSetPIPControl,
	seSetBrightness,
	seSetColor,
	seSetContrast,
	seSetSharpness,
	seSetHue,
	seSetVolume,
	seSetIncrementVolume,
	seSetDecrementVolume,
	seSetBalanceLeft,
	seSetBalanceMiddle,
	seSetBalanceRight,
	seSetEqualizerBand,	
	seSetClosedCaptionMode,
	seSetSubtitleLanguage,
	seSetSubtitlePage,
	seSetTeletextPage,
	seSetMapChannel,
	seSetUnmapChannel,
	seSetTunerFrequency,
	seSetFileProperties,
	seSetFileData,
	seSetTunerChannel,
	seSetOSDText,
	seSetOSDClear,
	seSetSwitchOnSource,
	seSetVolumeLimits,
	seSetWakeupTime,
	seSetClockTime,
	seSetUserInputControl,
	seSetAutoAdjust,
	seSetRS232Control,
//too long param
	seGetNextChannelName,
	seGetOperationHours,
	seGetSoftwareLabel,
	seGetPowerState,

	seReportNextChannelName,
	seReportOperationHours,
	seReportPowerState,
	seReportSoftwareLabel,
	
#endif
};
//---------------------------------------------------------------------------
#define PRE1_R                  0x01
#define PRE1_W                  0x03
#define PRE2                    0xFE    // TPV factory code
#define FCODE01                 0xE1
#define FCODE02                 0xE2
#define FCODE03                 0xE3
#define FCODE04                 0xE4
#define FCODE05                 0xE5
#define FCODE06                 0xE6
#define FCODE07                 0xE7
#define FCODE08                 0xE8
#define FCODE09                 0xE9
#define FCODE0A                 0xEA
#define FCODE0B                 0xEB
#define FCODE0C                 0xEC
#define FCODE0D                 0xED
#define FCODE0E                 0xEE
#define FCODE0F                 0xEF
#define FCODE10                 0xF0
#define FCODE11                 0xF1
#define FCODE12                 0xF2
// TPV_XM Xue 20120817: set  SerialExpressForBDS Command Type
 #if ENABLE_TPV_EBONY_PBS_MODE
 #define Set_Command                0x20
#define Get_Command                0x21
#define Get_Response                0x22
#endif
//---------------------------------------------------------------------------
#define SerilNumberLength       19//TPV_CSM_SN_SIZE
#define SerilNumberPackageNum   8
//---------------------------------------------------------------------------
extern U8 nSerialNumber[SerilNumberLength]
#ifdef afmC
= {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
}
#endif
;
//---------------------------------------------------------------------------
extern afm_machine afm
#ifdef afmC
= {
    0,
    {   0x00, //revert
        0x00,
        0x00,
        0x00,
        {
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
        },

        {
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00
        }
    },
    {
        0x00, //revert
        0x00,
        0x00,
        0x00,
        {
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00
        }
    },
    0,
    0,
    0,
    0,
}
#endif
;
//---------------------------------------------------------------------------
extern code U8 tblAfmAckOkMsg[16]
#ifdef afmC
= { 0xc2,0xc7,0xcb,0xcc,0xd3,0xd4,0xd8,0xdd,0xe3,0xe4,0xe8,0xed,0xf0,0xf5,0xf9,0xfe }
#endif
;
//---------------------------------------------------------------------------
extern code U8 tblAfmAckNgMsg[16]
#ifdef afmC
= { 0x81,0x86,0x8a,0x8f,0x92,0x97,0x9b,0x9c,0xa2,0xa7,0xab,0xac,0xb3,0xb4,0xb8,0xbd }
#endif
;
//---------------------------------------------------------------------------
extern code afm_item stAfmItem[]
#ifdef afmC
= {
    {{PRE1_W, PRE2, FCODE01,    0xA0, 0x00, 0x01,0xFF,0xFF}, 6, afiEnterAFM,             }, // enter automatic factory mode
    {{PRE1_W, PRE2, FCODE01,    0xA0, 0x00, 0x00,0xFF,0xFF}, 6, afiExitAFM,              }, // exit automatic factory mode
    {{PRE1_W, PRE2, FCODE01,    0xA1, 0x00, 0x00,0xFF,0xFF}, 6, afiNvmInit,              }, // Non-volatile memory init
    {{PRE1_W, PRE2, FCODE01,    0xA2, 0x00, 0xFF,0xFF,0xFF}, 5, afiBurnInOnOff,          }, // Turn On/Off burning mode
    {{PRE1_W, PRE2, FCODE01,    0xA3, 0x00, 0x00,0xFF,0xFF}, 6, afiClrBackLightHour,     }, // Clear backlihg hour
    {{PRE1_W, PRE2, FCODE01,    0xA8, 0x00, 0x00,0xFF,0xFF}, 4, afiSetTestPattern,       }, // set test pattern
    {{PRE1_W, PRE2, FCODE01,    0xA7, 0x05, 0x00,0xFF,0xFF}, 5, afiSetPictureMode,       }, // set test pattern
    {{PRE1_R, PRE2, FCODE01,    0xE3, 0xFF, 0xFF,0xFF,0xFF}, 4, afiGetHDCP_KSV,      }, // get HDCP KSV

    {{PRE1_W, PRE2, FCODE02,    0x01, 0xFF, 0xFF,0xFF,0xFF}, 4, afiSetAdcRGain,          }, // Set ADC R-Gain
    {{PRE1_R, PRE2, FCODE02,    0x01, 0xFF, 0xFF,0xFF,0xFF}, 4, afiGetAdcRGain,          }, // Get ADC R-Gain
    {{PRE1_W, PRE2, FCODE02,    0x02, 0xFF, 0xFF,0xFF,0xFF}, 4, afiSetAdcGGain,          }, // Set ADC G-Gain
    {{PRE1_R, PRE2, FCODE02,    0x02, 0xFF, 0xFF,0xFF,0xFF}, 4, afiGetAdcGGain,          }, // Get ADC G-Gain
    {{PRE1_W, PRE2, FCODE02,    0x03, 0xFF, 0xFF,0xFF,0xFF}, 4, afiSetAdcBGain,          }, // Set ADC B-Gain
    {{PRE1_R, PRE2, FCODE02,    0x03, 0xFF, 0xFF,0xFF,0xFF}, 4, afiGetAdcBGain,          }, // Get ADC B-Gain
    {{PRE1_W, PRE2, FCODE02,    0x04, 0xFF, 0xFF,0xFF,0xFF}, 4, afiSetAdcROffset,        }, // Set ADC R-Offset
    {{PRE1_R, PRE2, FCODE02,    0x04, 0xFF, 0xFF,0xFF,0xFF}, 4, afiGetAdcROffset,        }, // Get ADC R-Offset
    {{PRE1_W, PRE2, FCODE02,    0x05, 0xFF, 0xFF,0xFF,0xFF}, 4, afiSetAdcGOffset,        }, // Set ADC G-Offset
    {{PRE1_R, PRE2, FCODE02,    0x05, 0xFF, 0xFF,0xFF,0xFF}, 4, afiGetAdcGOffset,        }, // Get ADC G-Offset
    {{PRE1_W, PRE2, FCODE02,    0x06, 0xFF, 0xFF,0xFF,0xFF}, 4, afiSetAdcBOffset,        }, // Set ADC B-Offset
    {{PRE1_R, PRE2, FCODE02,    0x06, 0xFF, 0xFF,0xFF,0xFF}, 4, afiGetAdcBOffset,        }, // Get ADC B-Offset

    {{PRE1_R, PRE2, FCODE03,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, afiReadNvram,            }, // Read NV-Ram
    {{PRE1_W, PRE2, FCODE03,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, afiWriteNvram,           }, // Write NV-Ram

    {{PRE1_R, PRE2, FCODE04,    0x13, 0x00, 0x00,0xFF,0xFF}, 4, afiGetMCUVersion,        }, // afiGetMCUVersion

    {{PRE1_W, PRE2, FCODE05,    0x00, 0x00, 0x00,0xFF,0xFF}, 3, afiSelectChannelNum,     }, // Select channel num

    {{PRE1_W, PRE2, FCODE08,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, afiSetSerialNumber,      }, // Set Seril Number

    {{PRE1_W, PRE2, FCODE10,    0x11, 0x02, 0x01,0xFF,0xFF}, 6, afiSetAudioVolumeGain,      }, // Set audio volume gain
    {{PRE1_W, PRE2, FCODE10,    0x11, 0x03, 0x01,0xFF,0xFF}, 6, afiSetAudioVolume,       }, // Set audio volume
    {{PRE1_R, PRE2, FCODE10,    0x11, 0x03, 0x00,0xFF,0xFF}, 6, afiGetAudioVolume,       }, // Get audio volume
    {{PRE1_W, PRE2, FCODE10,    0x16, 0x02, 0x06,0xFF,0xFF}, 3, afiSelectFactoryChannelNum,       }, // Select channel num
    {{PRE1_R, PRE2, FCODE10,    0x10, 0x03, 0x01,0xFF,0xFF}, 6, afiCheckUSBPortAttachment,      }, // check USB number
    {{PRE1_R, PRE2, FCODE10,    0x18, 0x03, 0x00,0xFF,0xFF}, 5, afiGetNVRAMVersion,      }, // afiGetNVRAMVersion
    {{PRE1_R, PRE2, FCODE10,    0x18, 0x04, 0x00,0xFF,0xFF}, 5, afiGetModelname,         }, // afiGetModelname
    {{PRE1_R, PRE2, FCODE10,    0x17, 0x03, 0x00,0xFF,0xFF}, 5, afiGetEDIDData,          }, // afiGetEDIDData

    {{PRE1_W, PRE2, 0x13,       0x00, 0x00, 0x00,0xFF,0xFF}, 3, afiSetBackLight,         }, // afiSetBackLight
    {{PRE1_W, PRE2, 0x14,    0xFF, 0xFF, 0x00,0xFF,0xFF}, 3, afiChangeColorTempMode,    }, // change CT mode and recall or save
    {{PRE1_W, PRE2, 0x14,    0xFF, 0xFF, 0x00,0xFF,0xFF}, 6, afiSaveColorTemp,        }, // Save RGB setting for target color temperature
    {{PRE1_W, PRE2, 0x14,    0xFF, 0xFF, 0x01,0xFF,0xFF}, 6, afiRecallColorTemp,      }, // Recall RGB setting for target color temperature
    {{PRE1_W, PRE2, 0x14,    0x0A, 0xFF, 0xFF,0xFF,0xFF}, 4, afiChangeColorTempMode_6500K,   }, // set color temp mode 6500K
    {{PRE1_W, PRE2, 0x14,    0x06, 0xFF, 0xFF,0xFF,0xFF}, 4, afiChangeColorTempMode_9300K,   }, // set color temp mode 9300K
    {{PRE1_W, PRE2, 0x14,    0x05, 0xFF, 0xFF,0xFF,0xFF}, 4, afiChangeColorTempMode_11000K,   }, // set color temp mode 11000K

    {{PRE1_W, PRE2, 0x16,    0x00, 0xFF, 0xFF,0xFF,0xFF}, 4, afiSetRGain,             }, // Set R-Gain
    {{PRE1_R, PRE2, 0x16,    0x00, 0xFF, 0xFF,0xFF,0xFF}, 4, afiGetRGain,             }, // Get R-Gain
    {{PRE1_W, PRE2, 0x18,    0x00, 0xFF, 0xFF,0xFF,0xFF}, 4, afiSetGGain,             }, // Set G-Gain
    {{PRE1_R, PRE2, 0x18,    0x00, 0xFF, 0xFF,0xFF,0xFF}, 4, afiGetGGain,             }, // Get G-Gain
    {{PRE1_W, PRE2, 0x1A,    0x00, 0xFF, 0xFF,0xFF,0xFF}, 4, afiSetBGain,             }, // Set B-Gain
    {{PRE1_R, PRE2, 0x1A,    0x00, 0xFF, 0xFF,0xFF,0xFF}, 4, afiGetBGain,             }, // Get B-Gain
    {{PRE1_W, PRE2, 0x6C,    0x00, 0xFF, 0xFF,0xFF,0xFF}, 4, afiSetROffset,           }, // Set R-Offset
    {{PRE1_R, PRE2, 0x6C,    0x00, 0xFF, 0xFF,0xFF,0xFF}, 4, afiGetROffset,           }, // Get R-Offset
    {{PRE1_W, PRE2, 0x6E,    0x00, 0xFF, 0xFF,0xFF,0xFF}, 4, afiSetGOffset,           }, // Set G-Offset
    {{PRE1_R, PRE2, 0x6E,    0x00, 0xFF, 0xFF,0xFF,0xFF}, 4, afiGetGOffset,           }, // Get G-Offset
    {{PRE1_W, PRE2, 0x70,    0x00, 0xFF, 0xFF,0xFF,0xFF}, 4, afiSetBOffset,           }, // Set B-Offset
    {{PRE1_R, PRE2, 0x70,    0x00, 0xFF, 0xFF,0xFF,0xFF}, 4, afiGetBOffset,           }, // Get B-Offset

    {{PRE1_W, PRE2, 0x12,    0x00, 0xFF, 0xFF,0xFF,0xFF}, 4, afiSetContrast,          }, // Set Contrast
    {{PRE1_R, PRE2, 0x12,    0x00, 0xFF, 0xFF,0xFF,0xFF}, 4, afiGetContrast,          }, // Get Contrast
    {{PRE1_W, PRE2, 0x10,    0x00, 0xFF, 0xFF,0xFF,0xFF}, 4, afiSetBrightness,        }, // Set Brightness
    {{PRE1_R, PRE2, 0x10,    0x00, 0xFF, 0xFF,0xFF,0xFF}, 4, afiGetBrightness,        }, // Get Brightness

    {{PRE1_W, PRE2, 0x1F,    0x00, 0x00, 0x00,0xFF,0xFF}, 6, afiAutoADC,            }, // Do auto white
    {{PRE1_W, PRE2, 0x1E,    0x00, 0x00, 0x00,0xFF,0xFF}, 6, afiAutoAdjust,           }, // Do auto adjust
    {{PRE1_W, PRE2, 0x04,    0x00, 0x00, 0x00,0xFF,0xFF}, 6, afiSetFactoryDefault,    }, // Reset factor to default setting
    {{PRE1_W, PRE2, 0xCC,    0x00, 0x00, 0xFF,0xFF,0xFF}, 3, afiSetDefaultLang,       }, // Set default language
    {{PRE1_W, PRE2, 0x60,    0x00, 0xFF, 0x00,0xFF,0xFF}, 4, afiSetInputSource,       }, // Set input source
    {{PRE1_R, PRE2, 0x60,    0x00, 0xEE, 0x00,0xFF,0xFF}, 4, afiGetInputSource,       }, // Get input source
    {{PRE1_W, PRE2, 0x74,    0x01, 0x00, 0x00,0xFF,0xFF}, 4, afiSetTestPatternRGB,    }, // set test pattern RGB[x:y:z]
    {{PRE1_W, PRE2, 0x74,    0x05, 0x00, 0x00,0xFF,0xFF}, 4, afiSetTestPatternGreyRGB,   }, // set test pattern RGB[x:x:x]
    // TPV_XM Xue 20121214: add new command
    {{PRE1_W, PRE2, 0xD6,    0x00, 0x00, 0x05,0xFF,0xFF}, 4, afiSetPowerOFF,   }, // Set Power OFF
    {{PRE1_W, PRE2, 0x04,    0x02, 0x00, 0x00,0xFF,0xFF}, 4, afiSetExecuteFactoryDomainReset,   }, // Execute Factory Domain Reset
    		// TPV_XM Xue 20121214: Add HASH Unlock
     {{PRE1_W, PRE2, 0xE1,    0xA7, 0x09, 0x00,0xFF,0xFF}, 4, afiSetHASHUnlock,   }, // Execute Factory Domain Reset
		
    // TPV_XM Xue 20121017: xue add complier VCOM adjust condition
    #ifdef ENABLE_TPV_VCOM_ADJUST
    {{PRE1_W, PRE2, FCODE01, 0xA9, 0x00, 0x00,0xFF,0xFF}, 5, afiSetV_COM,	  }, // set V-COM
    {{PRE1_R, PRE2, FCODE01, 0xA9, 0x00, 0x00,0xFF,0xFF}, 6, afiGetV_COM,	 }, // get V-COM
    #endif
    	// TPV_XM Xue 20120817: Configuration PBS Command  information
 #if ENABLE_TPV_EBONY_PBS_MODE	
  	{{Set_Command, 0x18, 0x00,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetPowerOFF,   }, // set command power off
  	{{Set_Command, 0xDF, 0x01,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetBDSON,   }, // set command BDS on
  	{{Set_Command, 0xDF, 0x00,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetBDSOFF,   }, // set command BDS off
  	{{Set_Command, 0xDF, 0x02,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetBDSInstall,   }, // set command
  	{{Set_Command, 0x46, 0x01,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetMuteON,   }, // set command 
  	{{Set_Command, 0x46, 0x00,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetMuteOFF,   }, // set command 
  	{{Set_Command, 0x34, 0x01,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetVideoMute,   }, // set command 
  	{{Set_Command, 0x34, 0x00,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetVideoUnMute,   }, // set command 
  	{{Set_Command, 0x45, 0x01,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetIncrementVolume,   }, // set command 
	{{Set_Command, 0x45, 0x00,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetDecrementVolume,   }, // set command 
	{{Set_Command, 0x4C, 0x00,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetBalanceMiddle,   }, // set command 
	{{Set_Command, 0x4C, 0x32,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetBalanceLeft,   }, // set command 
	{{Set_Command, 0x4C, 0x64,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetBalanceRight}, // set command 
	{{Set_Command, 0x44, 0xFF,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 2, seSetVolume}, // set command 
	{{Set_Command, 0x30, 0xFF,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 2, seSetBrightness,   }, // set command 
	{{Set_Command, 0x31, 0xFF,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 2, seSetColor,   }, // set command 
	{{Set_Command, 0x32, 0xFF,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 2, seSetContrast,   }, // set command 
	{{Set_Command, 0x33, 0xFF,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 2, seSetSharpness,   }, // set command 
	{{Set_Command, 0x35, 0xFF,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 2, seSetHue,   }, // set command 	
	{{Set_Command, 0x3D, 0xFF,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 2, seSetColorTemperature,   }, // set comman 
	{{Set_Command, 0x3F, 0x40,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 4, seSetAutoAdjust}, // set command 
	{{Set_Command, 0xAC, 0xFF,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 2, seSetSource,   }, // set command 	
	{{Set_Command, 0x1C, 0xFF,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 2, seSetUserInputControl,   }, // set command  
	{{Set_Command, 0x3A, 0xFF,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 2, seSetPictureFormat,   }, // set command  
	{{Set_Command, 0x1D, 0xFF,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 2, seSetFactoryReset,   }, // set command 
	{{Set_Command, 0x1E, 0xFF,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 2, seSetOSDLanguage,   }, // set command 
	 {{Set_Command, 0xD8, 0xFF,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 2, seSetVolumeLimits,   }, // set command 	 
	 {{Set_Command, 0x1B, 0xFF,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 2, seSetUserInputData,   }, // set command 
	 
 {{Get_Command, 0x18, 0xFF,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 2, seGetPowerState,   }, // set command 
 
	 {{Set_Command, 0x18, 0x01,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetPowerON,   }, // set command power on
	  {{Set_Command, 0x3E, 0x01,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetPixelPlusON,   }, // set command 
	  {{Set_Command, 0x3E, 0x00,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetPixelPlusOFF,   }, // set command 
	  {{Set_Command, 0x69, 0x00,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetSubtitleModeOFF,   }, // set command 
	  {{Set_Command, 0x69, 0x01,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetSubtitleModeON,   }, // set command 
	  {{Set_Command, 0x60, 0x01,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetTeletextModeON,   }, // set command 
	  {{Set_Command, 0x60, 0x00,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetTeletextModeOFF,   }, // set command 
	  {{Set_Command, 0x1F, 0x01,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetLocalOSDSuppressON,   }, // set command 
	  {{Set_Command, 0x1F, 0x00,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetLocalOSDSuppressOFF,   }, // set command 
	  {{Set_Command, 0x2D, 0x01,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetExternalOSDSuppressON,   }, // set command 
	  {{Set_Command, 0x2D, 0x00,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetExternalOSDSuppressOFF,   }, // set command 
	  {{Set_Command, 0xA2, 0x00,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetPowerAtColdStartOFF,   }, // set command 
	  {{Set_Command, 0xA2, 0x01,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetPowerAtColdStartON,   }, // set command 
	  {{Set_Command, 0x90, 0x00,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetAmbilightOFF,   }, // set command 
	  {{Set_Command, 0x90, 0x01,    0xFF, 0xFF, 0xFF,0xFF,0xFF}, 3, seSetAmbilightON,   }, // set command 
	  
	 
///





#endif
}
#endif
;




//---------------------------------------------------------------------------
extern void afm_parse(U8 c);	
		// TPV_XM Xue 20120817: PBS Command Control
 #if ENABLE_TPV_EBONY_PBS_MODE
 extern void EbonyCmd_Process(void);	
#endif
extern BOOLEAN IsAfmMode(void);
extern void SetAfmMode(U8 val);
extern char afm_putc(char c);
extern U8 afmInputSourceNormalize(U8 val);
extern void afmEnterAFM(void);
extern void afmExitAFM(void);
extern void afmNvmInit(void);
extern void afmBurnInOnOff(U8 val);
extern void afmClrBackLightHour(void);
extern void afmSetBackLight(U8 val);
extern void afmSetAdcRGain(U8 val_high,U8 val_low);
extern U16 afmGetAdcRGain(void);
extern void afmSetAdcGGain(U8 val_high,U8 val_low);
extern U16 afmGetAdcGGain(void);
extern void afmSetAdcBGain(U8 val_high,U8 val_low);
extern U16 afmGetAdcBGain(void);
extern void afmSetAdcROffset(U8 val_high,U8 val_low);
extern U16 afmGetAdcROffset(void);
extern void afmSetAdcGOffset(U8 val_high,U8 val_low);
extern U16 afmGetAdcGOffset(void);
extern void afmSetAdcBOffset(U8 val_high,U8 val_low);
extern U16 afmGetAdcBOffset(void);
extern void afmReadNvram(U16 addr, U8 *buf, U8 len);
extern void afmWriteNvram(U16 addr, U8 *buf, U8 len);
extern void afmSaveColorTemp(U8 idx);
extern void afmRecallColorTemp(U8 idx);
extern void afmChangeColorTemp_6500K(U8 InpSrc,U8 ActFlag);
extern void afmChangeColorTemp_9300K(U8 InpSrc,U8 ActFlag);
extern void afmChangeColorTemp_11000K(U8 InpSrc,U8 ActFlag);
extern void afmSetRGain(U8 val_high,U8 val_low);
extern U16 afmGetRGain(void);
extern void afmSetGGain(U8 val_high,U8 val_low);
extern U16 afmGetGGain(void);
extern void afmSetBGain(U8 val_high,U8 val_low);
extern U16 afmGetBGain(void);
extern void afmSetROffset(U8 val_high,U8 val_low);
extern U16 afmGetROffset(void);
extern void afmSetGOffset(U8 val_high,U8 val_low);
extern U16 afmGetGOffset(void);
extern void afmSetBOffset(U8 val_high,U8 val_low);
extern U16 afmGetBOffset(void);
extern void afmSetContrast(U8 val_high,U8 val_low);
extern U16 afmGetContrast(void);
extern void afmSetBrightness(U8 val_high,U8 val_low);
extern U16 afmGetBrightness(void);
extern void afmAutoADC(void);
extern void afmAutoAdjust(void);
extern void afmSetFactoryDefault(void);
extern void afmSetDefaultLang(void);
extern void afmSetInputSource(U8 InpSrc,U8 u8extraAction);
extern U8 afmGetInputSource(void);
extern void afmSetSerialNumber(U8 sn[SerilNumberLength]);
extern void afmSetAudioVolume(U8 vol);
extern U8 afmGetAudioVolume(void);
extern void afmSelectChannelNum(U8 Major_MinorHi,U8 MajorChLo,U8 MinorChLo);
extern void afmSelectFactoryChannelNum(U8 Tuner_Attr,U8 Ch_Attr,U8 MajorChHiByte,U8 MajorChLoByte,U8 MinorChHiByte,U8 MinorChLoByte);
extern U8 afmCheckUSBPortAttachment(U8 check);
extern void afmSetTestPattern(U8 val1,U8 val2);
extern void afmSetTestPatternRGB(U8 u8R,U8 u8G,U8 u8B);
extern void afmSetPictureMode(U8 val);
extern void afmChangeColorTemp(U8 Colormode,U8 InpSrc,U8 ActFlag);
extern void afmGetEdidData(U8 InputSrc,U8 offset,U8 len);
extern void afmSetVcomValue(U8 val);
extern U8 afmGetVcomValue(void);

//---------------------------------------------------------------------------
#ifdef afmC
#undef extern
#endif
//---------------------------------------------------------------------------
#endif

