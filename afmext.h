#ifndef afmextH
#define afmextH
//---------------------------------------------------------------------------
//#include "mcu.h"
#include "afm.h"
#include "datatype.h"
//---------------------------------------------------------------------------
#ifdef afmextC
#define extern
#endif

//---------------------------------------------------------------------------
#ifdef afmextC
#undef extern
#endif
//---------------------------------------------------------------------------
#endif
extern void seSetBDSInstallModeContorl(void);
extern void seSetMuteModeContorl(BOOLEAN MuteStatused);
extern void  seSetVideoMuteControl(BOOLEAN VideoMuteStatused);
extern void seSetFactoryResetControl(void);
extern void seSetIncrement_DecrementVolumeControl(U8 AudioOPStatused);
extern void seSetBalanceControl(U8 BalanceOPStatused) ;
extern void seSetVolumeControl(U8 VolumeParm);
extern void seSetPowerStateContorl(U8 param);
extern void seSetSourceSwitchControl(U8 SourceParm);
extern void seSetBrightnessControl(U8 BrightnessParm);
extern void seSetColorControl(U8 ColorParm);
extern void seSetContrastControl(U8 ContrastParm);
extern void seSetPictureFormatControl(U8 PictureFormatParm);
extern void seSetSharpnessControl(U8 SharpnessParm);
extern void seSetHueControl(U8 HueParm);
extern void seSetColorTemperatureControl(U8 ColorTemperatureParm);
extern void seSetAutoAdjustControl(void);
extern void seSetSourceSwitchControl(U8 SourceParm);
extern void seSetUserInputControlState(U8 LockParm);
extern void seSetVolumeLimitsControl(U8 MaxVolumeValue,U8 LimitVolumeValue);
extern void seSetOSDLanguageControl(void);
extern void seSetUserInputDataControl(U8 IRSystemCode,U8 IRCode,U8 KeyPressStatus,U8 KeyPadCode);
		// TPV_XM Xue 20121214: add new command
extern void afmPowerOFF(void);
extern void afmExecuteFactoryDomainReset(void);	
		// TPV_XM Xue 20121214: add hash unlock
extern void afmHASHUnlock(void);
// TPV_XM Xue 20121017: add adjust VCOM function
//#ifdef ENABLE_TPV_VCOM_ADJUST
extern void afm_SetVCOM_Value_OPT(U8 OPTValue);
extern void afm_SetVCOM_Value_DAC(U8 DACValue);
extern U8 afm_GetVCOM_Value_OPT(void);
extern U8 afm_GetVCOM_Value_DAC(void);

//#endif

