#define afmextC
//---------------------------------------------------------------------------
#include "afm.h"
#include "afmext.h"
#include "drvUartDebug.h"
#include "ZUI_exefunc.h"
#include "MApp_GlobalSettingSt.h"
#include "MApp_SaveData.h"
#include "MApp_MultiTasks.h"
#include "MApp_GlobalFunction.h"
#include "apiXC_Sys.h"
#include "apiXC_Ace.h"
#include "apiXC.h"
#include "apiXC_Auto.h"
#include "MApp_PCMode.h"
#include "MApp_Scaler.h"
#include "MApp_XC_PQ.h"
#include "MApp_ZUI_ACTglobal.h"
#include "MApp_InputSource.h"
#include "MApp_TopStateMachine.h"
#include "MApp_ChannelChange.h"
#include "MApp_GlobalVar.h"
#include "MApp_UiMenuDef.h"
#include "msAPI_Timer.h"
#include "Utl.h"
#include "MApp_Audio.h"
//#include "tpvDefine.h"
#include "MApp_RestoreToDefault.h"
			// TPV_XM Xue 20120820: 
#include "MApp_Menu_Main.h"
#include "drvGPIO.h"
extern BOOLEAN MDrv_UsbDeviceConnect(void);


#if ENABLE_DMP
extern BOOLEAN MApp_ZUI_ACT_HandleDmpKey(VIRTUAL_KEY_CODE key);
#endif
#define DBG_AFM(x)     //x
//---------------------------------------------------------------------------
char afm_putc(char c)
{
    putcharb(c);
    return c;
}

static U16 afmGetADCValue(E_AFM_GainOffset_INDEX enGainoffset)
{
    U16 returnVal;
    E_ADC_SET_INDEX enADCSetIndex;
    if(IsVgaInUse())
    {
        enADCSetIndex = ADC_SET_VGA;
    }
    else if(IsYPbPrInUse())
    {
        if( MApi_XC_Sys_IsSrcHD(MAIN_WINDOW) )
        {
            enADCSetIndex = ADC_SET_YPBPR_HD;
        }
        else
        {
            enADCSetIndex = ADC_SET_YPBPR_SD;
        }
    }
    #if (INPUT_SCART_VIDEO_COUNT >= 1)
    else if(IsScartInUse())
    {
        enADCSetIndex = ADC_SET_SCART_RGB;
    }
    #endif
    else
    {
        DBG_AFM(printf("Get ADC value-->index error [%u]\n",(U8)enGainoffset));
        return 0x00;
    }

	switch(enGainoffset)
	{
		case AFM_GainOffset_Gain_R:
			returnVal = stGenSettingExt.g_AdcSetting[enADCSetIndex].stAdcGainOffsetSetting.u16RedGain;
			break;
		case AFM_GainOffset_Gain_G:
			returnVal = stGenSettingExt.g_AdcSetting[enADCSetIndex].stAdcGainOffsetSetting.u16GreenGain;
			break;
		case AFM_GainOffset_Gain_B:
			returnVal = stGenSettingExt.g_AdcSetting[enADCSetIndex].stAdcGainOffsetSetting.u16BlueGain;
			break;
		case AFM_GainOffset_Offset_R:
			returnVal = stGenSettingExt.g_AdcSetting[enADCSetIndex].stAdcGainOffsetSetting.u16RedOffset;
			break;
		case AFM_GainOffset_Offset_G:
			returnVal = stGenSettingExt.g_AdcSetting[enADCSetIndex].stAdcGainOffsetSetting.u16GreenOffset;
			break;
		case AFM_GainOffset_Offset_B:
			returnVal = stGenSettingExt.g_AdcSetting[enADCSetIndex].stAdcGainOffsetSetting.u16BlueOffset;
			break;
		default:
			return 0x00;
			break;
	}
	return returnVal;
}

static U16 afmGetWhiteBalanceValue(E_AFM_GainOffset_INDEX enGainoffset)
{
    U16 returnVal; //if ENABLE_PRECISE_RGBBRIGHTNESS = 1,then return U16
    switch(enGainoffset)
    {
        case AFM_GainOffset_Gain_R:
            returnVal = ST_COLOR_TEMP.cRedColor;
            break;

        case AFM_GainOffset_Gain_G:
            returnVal = ST_COLOR_TEMP.cGreenColor;
            break;

         case AFM_GainOffset_Gain_B:
            returnVal = ST_COLOR_TEMP.cBlueColor;
            break;

        case AFM_GainOffset_Offset_R:
            returnVal = ST_COLOR_TEMP.cRedOffset;
            break;

        case AFM_GainOffset_Offset_G:
            returnVal = ST_COLOR_TEMP.cGreenOffset;
            break;

        case AFM_GainOffset_Offset_B:
            returnVal = ST_COLOR_TEMP.cBlueOffset;
            break;
        default:
            returnVal = 0;
            break;
    }

    return returnVal;
}


//---------------------------------------------------------------------------
void afmEnterAFM(void)
{
    DBG_AFM(printf("Goto AFM mode\n"));/*Creass.liu at 2012-06-26*/
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_ENTER_FACTORY_MODE;
}
//---------------------------------------------------------------------------
void afmExitAFM(void)
{
   DBG_AFM(printf("Exit AFM mode\n"));/*Creass.liu at 2012-06-26*/
   //MApp_RestoreDefaultPictureSettingForRS232(DATA_INPUT_SOURCE_MIN,3,PICTURE_DYNAMIC);
   //MApp_Picture_Setting_SetColor(SYS_INPUT_SOURCE_TYPE(MAIN_WINDOW), MAIN_WINDOW);
   memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
   stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_EXIT_FACTORY_MODE;
   stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
}
//---------------------------------------------------------------------------
void afmNvmInit(void)
{
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_RESET_FACTORY_ALL;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
}
//---------------------------------------------------------------------------
void afmBurnInOnOff(U8 val)
{
    UNUSED(val);
    #if ENABLE_TPV_BURNING_MODE
     if(val)
     {
         DBG_AFM(printf("burn mode on,val=[%u]\n",val));/*Creass.liu at 2012-06-26*/
         stGenSetting.g_FactorySetting.fBuringMode = TRUE;
         memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
         stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_ENTER_BURNIN_MODE;
     }
     else
     {
         DBG_AFM(printf("burn mode off\n"));/*Creass.liu at 2012-06-26*/
         stGenSetting.g_FactorySetting.fBuringMode = FALSE;
         MApp_MultiTasks_AdjustBurningMode(DISABLE);
     }
     MApp_SaveFactorySetting();
    #endif
}
//---------------------------------------------------------------------------
void afmClrBackLightHour(void)
{
    stGenSetting.g_FactoryTime.gFactoryTotaltimeback = 0;
	stGenSetting.g_FactoryTime.gFactoryTotaltime = 0;
    MApp_SaveFactoryTime();
}

void afmSetBackLight(U8 val)
{
    DBG_AFM(printf("afmSetBackLight val= [%u],\n",val));
    if(val > 100)
    {
        val =100;
    }
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_PC_BACKLIGHT;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = val;
}

//---------------------------------------------------------------------------
void afmSetAdcRGain(U8 val_high,U8 val_low)
{
    U16 val;
    val = (((U16)val_high << 8) | (U16)(val_low));
    DBG_AFM(printf("Set ADC Rg:[0x%02bx][0x%02bx]=[0x%04x]\n",val_high,val_low,val));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_ADC_VALUE;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = AFM_GainOffset_Gain_R;
    stRS232PostMsgSetting.ReciveData[1] = val_high;
    stRS232PostMsgSetting.ReciveData[2] = val_low;
}
//---------------------------------------------------------------------------
U16 afmGetAdcRGain(void)
{
    return afmGetADCValue(AFM_GainOffset_Gain_R);
}
//---------------------------------------------------------------------------
void afmSetAdcGGain(U8 val_high,U8 val_low)
{
    U16 val;
    val = (((U16)val_high << 8) | (U16)(val_low));
    DBG_AFM(printf("Set ADC Gg:[0x%02bx][0x%02bx]=[0x%04x]\n",val_high,val_low,val));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_ADC_VALUE;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = AFM_GainOffset_Gain_G;
    stRS232PostMsgSetting.ReciveData[1] = val_high;
    stRS232PostMsgSetting.ReciveData[2] = val_low;
}

//---------------------------------------------------------------------------
U16 afmGetAdcGGain(void)
{
    return afmGetADCValue(AFM_GainOffset_Gain_G);
}
//---------------------------------------------------------------------------
void afmSetAdcBGain(U8 val_high,U8 val_low)
{
    U16 val;
    val = (((U16)val_high << 8) | (U16)(val_low));
    DBG_AFM(printf("Set ADC Bg:[0x%02bx][0x%02bx]=[0x%04x]\n",val_high,val_low,val));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_ADC_VALUE;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = AFM_GainOffset_Gain_B;
    stRS232PostMsgSetting.ReciveData[1] = val_high;
    stRS232PostMsgSetting.ReciveData[2] = val_low;
}

//---------------------------------------------------------------------------
U16 afmGetAdcBGain(void)
{
    return afmGetADCValue(AFM_GainOffset_Gain_B);
}
//---------------------------------------------------------------------------
void afmSetAdcROffset(U8 val_high,U8 val_low)
{
    U16 val;
    val = (((U16)val_high << 8) | (U16)(val_low));
    DBG_AFM(printf("Set ADC Ro:[0x%02bx][0x%02bx]=[0x%04x]\n",val_high,val_low,val));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_ADC_VALUE;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = AFM_GainOffset_Offset_R;
    stRS232PostMsgSetting.ReciveData[1] = val_high;
    stRS232PostMsgSetting.ReciveData[2] = val_low;
}

//---------------------------------------------------------------------------
U16 afmGetAdcROffset(void)
{
    return afmGetADCValue(AFM_GainOffset_Offset_R);
}
//---------------------------------------------------------------------------
void afmSetAdcGOffset(U8 val_high,U8 val_low)
{
    U16 val;
    val = (((U16)val_high << 8) | (U16)(val_low));
    DBG_AFM(printf("Set ADC Go:[0x%02bx][0x%02bx]=[0x%04x]\n",val_high,val_low,val));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_ADC_VALUE;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = AFM_GainOffset_Offset_G;
    stRS232PostMsgSetting.ReciveData[1] = val_high;
    stRS232PostMsgSetting.ReciveData[2] = val_low;
}
//---------------------------------------------------------------------------
U16 afmGetAdcGOffset(void)
{
    return afmGetADCValue(AFM_GainOffset_Offset_G);
}
//---------------------------------------------------------------------------
void afmSetAdcBOffset(U8 val_high,U8 val_low)
{
    U16 val;
    val = (((U16)val_high << 8) | (U16)(val_low));
    DBG_AFM(printf("Set ADC Bo:[0x%02bx][0x%02bx]=[0x%04x]\n",val_high,val_low,val));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_ADC_VALUE;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = AFM_GainOffset_Offset_B;
    stRS232PostMsgSetting.ReciveData[1] = val_high;
    stRS232PostMsgSetting.ReciveData[2] = val_low;
}
//---------------------------------------------------------------------------
U16 afmGetAdcBOffset(void)
{
    return afmGetADCValue(AFM_GainOffset_Offset_B);
}
//---------------------------------------------------------------------------
void afmReadNvram(U16 addr, U8 *buf, U8 len)
{
    UNUSED(addr);
    UNUSED(buf);
    UNUSED(len);
}
//---------------------------------------------------------------------------
void afmWriteNvram(U16 addr, U8 *buf, U8 len)
{
    UNUSED(addr);
    UNUSED(buf);
    UNUSED(len);
}
//---------------------------------------------------------------------------
void afmSaveColorTemp(U8 idx)
{
    // TODO:
    UNUSED(idx);
    DBG_AFM(printf("Save CT,idx=[%u]\n",idx));
    MApp_SaveWhiteBalanceSetting(DATA_INPUT_SOURCE_TYPE(MAIN_WINDOW));
}
//---------------------------------------------------------------------------
void afmRecallColorTemp(U8 idx)
{
    // TODO:
    UNUSED(idx);
    DBG_AFM(printf("Load CT\n"));
    MApp_LoadWhiteBalanceSetting(DATA_INPUT_SOURCE_TYPE(MAIN_WINDOW));
}
//---------------------------------------------------------------------------
void afmSetRGain(U8 val_high,U8 val_low)
{
    U16 val;
    val = (((U16)val_high << 8) | (U16)(val_low));
    DBG_AFM(printf("Set WB Rg:[0x%02bx][0x%02bx]=[0x%04x]\n",val_high,val_low,val));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_WB_VALUE;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = AFM_GainOffset_Gain_R;
    stRS232PostMsgSetting.ReciveData[1] = val_high;
    stRS232PostMsgSetting.ReciveData[2] = val_low;
}
//---------------------------------------------------------------------------
U16 afmGetRGain(void)
{
    return afmGetWhiteBalanceValue(AFM_GainOffset_Gain_R);
}
//---------------------------------------------------------------------------
void afmSetGGain(U8 val_high,U8 val_low)
{
    U16 val;
    val = (((U16)val_high << 8) | (U16)(val_low));
    DBG_AFM(printf("Set WB Gg:[0x%02bx][0x%02bx]=[0x%04x]\n",val_high,val_low,val));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_WB_VALUE;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = AFM_GainOffset_Gain_G;
    stRS232PostMsgSetting.ReciveData[1] = val_high;
    stRS232PostMsgSetting.ReciveData[2] = val_low;
}
//---------------------------------------------------------------------------
U16 afmGetGGain(void)
{
    return afmGetWhiteBalanceValue(AFM_GainOffset_Gain_G);
}
//---------------------------------------------------------------------------
void afmSetBGain(U8 val_high,U8 val_low)
{
    U16 val;
    val = (((U16)val_high << 8) | (U16)(val_low));
    DBG_AFM(printf("Set WB Bg:[0x%02bx][0x%02bx]=[0x%04x]\n",val_high,val_low,val));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_WB_VALUE;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = AFM_GainOffset_Gain_B;
    stRS232PostMsgSetting.ReciveData[1] = val_high;
    stRS232PostMsgSetting.ReciveData[2] = val_low;
}
//---------------------------------------------------------------------------
U16 afmGetBGain(void)
{
    return afmGetWhiteBalanceValue(AFM_GainOffset_Gain_B);
}
//---------------------------------------------------------------------------
void afmSetROffset(U8 val_high,U8 val_low)
{
    U16 val;
    val = (((U16)val_high << 8) | (U16)(val_low));
    DBG_AFM(printf("Set WB Ro:[0x%02bx][0x%02bx]=[0x%04x]\n",val_high,val_low,val));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_WB_VALUE;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = AFM_GainOffset_Offset_R;
    stRS232PostMsgSetting.ReciveData[1] = val_high;
    stRS232PostMsgSetting.ReciveData[2] = val_low;
}
//---------------------------------------------------------------------------
U16 afmGetROffset(void)
{
    return afmGetWhiteBalanceValue(AFM_GainOffset_Offset_R);
}
//---------------------------------------------------------------------------
void afmSetGOffset(U8 val_high,U8 val_low)
{
    U16 val;
    val = (((U16)val_high << 8) | (U16)(val_low));
    DBG_AFM(printf("Set WB Go:[0x%02bx][0x%02bx]=[0x%04x]\n",val_high,val_low,val));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_WB_VALUE;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = AFM_GainOffset_Offset_G;
    stRS232PostMsgSetting.ReciveData[1] = val_high;
    stRS232PostMsgSetting.ReciveData[2] = val_low;
}
//---------------------------------------------------------------------------
U16 afmGetGOffset(void)
{
    return afmGetWhiteBalanceValue(AFM_GainOffset_Offset_G);
}
//---------------------------------------------------------------------------
void afmSetBOffset(U8 val_high,U8 val_low)
{
    U16 val;
    val = (((U16)val_high << 8) | (U16)(val_low));
    DBG_AFM(printf("Set WB Bo:[0x%02bx][0x%02bx]=[0x%04x]\n",val_high,val_low,val));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_WB_VALUE;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = AFM_GainOffset_Offset_B;
    stRS232PostMsgSetting.ReciveData[1] = val_high;
    stRS232PostMsgSetting.ReciveData[2] = val_low;
}
//---------------------------------------------------------------------------
U16 afmGetBOffset(void)
{
    return afmGetWhiteBalanceValue(AFM_GainOffset_Offset_B);
}

void afmChangeColorTemp_6500K(U8 InpSrc,U8 ActFlag)
{
    UNUSED(InpSrc);
    UNUSED(ActFlag);
    DBG_AFM(printf("Set CT 6500K [src=0x%x],actflag=[%u]\n",InpSrc,ActFlag));
}
void afmChangeColorTemp_9300K(U8 InpSrc,U8 ActFlag)
{
    UNUSED(InpSrc);
    UNUSED(ActFlag);
    DBG_AFM(printf("Set CT 9300K [src=0x%x],actflag=[%u]\n",InpSrc,ActFlag));
}

void afmChangeColorTemp_11000K(U8 InpSrc,U8 ActFlag)
{
    UNUSED(InpSrc);
    UNUSED(ActFlag);
    DBG_AFM(printf("Set CT 11000K [src=0x%x],actflag=[%u]\n",InpSrc,ActFlag));
}
void afmChangeColorTemp(U8 Colormode,U8 InpSrc,U8 ActFlag)
{
    DBG_AFM(printf("Set CT [mode= 0x%x], [src=0x%x],actflag=[%u]\n",Colormode,InpSrc,ActFlag));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_COLOR_TEMP_MODE;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = InpSrc;
    stRS232PostMsgSetting.ReciveData[1] = ActFlag;
    stRS232PostMsgSetting.ReciveData[2] = Colormode;
}


//---------------------------------------------------------------------------
void afmSetContrast(U8 val_high,U8 val_low)
{
     val_high = val_high;
     ST_PICTURE.u8Contrast = val_low;
     DBG_AFM(printf("Set Contrast :[%u][%u]\n",val_high,val_low));
#if 0//VGA_HDMI_YUV_POINT_TO_POINT
    if(IsHDMIInUse()&&MDrv_PQ_Get_HDMIPCMode())
    {
        MApi_XC_ACE_SetPCYUV2RGB(MAIN_WINDOW, FALSE);
        MApi_XC_ACE_PicSetContrast(MAIN_WINDOW, FALSE, MApp_Scaler_FactoryContrast(msAPI_Mode_PictureContrastN100toReallyValue(ST_PICTURE.u8Contrast),ST_SUBCOLOR.u8SubContrast));
    }
    else
#endif
    {
        MApi_XC_ACE_PicSetContrast(MAIN_WINDOW, MApi_XC_IsYUVSpace(MAIN_WINDOW), MApp_Scaler_FactoryContrast(msAPI_Mode_PictureContrastN100toReallyValue(ST_PICTURE.u8Contrast),ST_SUBCOLOR.u8SubContrast));
    }
    MApp_SaveVideoSetting(DATA_INPUT_SOURCE_TYPE(MAIN_WINDOW));

}

//---------------------------------------------------------------------------
U16 afmGetContrast(void)
{
    DBG_AFM(printf("Get Contrast :[%u]\n",ST_PICTURE.u8Contrast));
    return ST_PICTURE.u8Contrast;
}
//---------------------------------------------------------------------------
void afmSetBrightness(U8 val_high,U8 val_low)
{
    U8 u8BR, u8BG, u8BB;
    val_high = val_high;
    ST_PICTURE.u8Brightness = val_low;
    DBG_AFM(printf("Set Brightness :[%u][%u]\n",val_high,val_low));

    u8BR = MApi_XC_Sys_ACE_transfer_Bri((MApp_Scaler_FactoryAdjBrightness(msAPI_Mode_PictureBrightnessN100toReallyValue(ST_PICTURE.u8Brightness),ST_SUBCOLOR.u8SubBrightness)), BRIGHTNESS_R);
    u8BG = MApi_XC_Sys_ACE_transfer_Bri((MApp_Scaler_FactoryAdjBrightness(msAPI_Mode_PictureBrightnessN100toReallyValue(ST_PICTURE.u8Brightness),ST_SUBCOLOR.u8SubBrightness)), BRIGHTNESS_G);
    u8BB = MApi_XC_Sys_ACE_transfer_Bri((MApp_Scaler_FactoryAdjBrightness(msAPI_Mode_PictureBrightnessN100toReallyValue(ST_PICTURE.u8Brightness),ST_SUBCOLOR.u8SubBrightness)), BRIGHTNESS_B);

    MApi_XC_ACE_PicSetBrightnessInVsync(MAIN_WINDOW, u8BR, u8BG, u8BB);

    MApp_SaveVideoSetting(DATA_INPUT_SOURCE_TYPE(MAIN_WINDOW));
}

//---------------------------------------------------------------------------
U16 afmGetBrightness(void)
{
    DBG_AFM(printf("Get Brightness :[%u]\n",ST_PICTURE.u8Brightness));
    return ST_PICTURE.u8Brightness;
}
//---------------------------------------------------------------------------
void afmAutoADC(void)
{
    DBG_AFM(printf("afmAutoADC\n"));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_AUTO_ADC;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
}
//---------------------------------------------------------------------------
void afmAutoAdjust(void)
{
    DBG_AFM(printf("afmAutoAdjust\n"));
    if(IsVgaInUse())
    {
        if(IsSrcTypeVga(SYS_INPUT_SOURCE_TYPE(MAIN_WINDOW)))
        {
            if(MApp_PCMode_Enable_SelfAuto(ENABLE, MAIN_WINDOW))
            {   // to do Auto adjust
                MApp_ZUI_ACT_StartupOSD(E_OSD_MESSAGE_BOX);
                MApp_ZUI_ACT_ExecuteWndAction(EN_EXE_SHOW_AUTO_ADJUSTING_MSGBOX);
                //MApp_PCMode_RunSelfAuto(MAIN_WINDOW);
            }
        }
        else
        {
            if(MApp_PCMode_Enable_SelfAuto(ENABLE, SUB_WINDOW))
            {   // to do Auto adjust
                MApp_PCMode_RunSelfAuto(SUB_WINDOW);
            }
        }
    }
}
//---------------------------------------------------------------------------
void afmSetFactoryDefault(void)
{   
	//printf("afmSetFactoryDefault\n");
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_RESET_FACTORY_USER;

}
//---------------------------------------------------------------------------
void afmSetDefaultLang(void)
{
    DBG_AFM(printf("afmSetDefaultLang\n"));
    SET_OSD_MENU_LANGUAGE(LANGUAGE_DEFAULT); // menu language
    MApp_SaveSysSetting();
}
//---------------------------------------------------------------------------
void afmSetInputSource(U8 InpSrc,U8 u8extraAction)
{
    DBG_AFM(printf("Set input src :val=[%x],extraAct=[%u]\n",InpSrc,u8extraAction));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_CHANGE_INPUT_SOURCE;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = InpSrc;
    stRS232PostMsgSetting.ReciveData[1] = u8extraAction;
}
//---------------------------------------------------------------------------
U8 afmGetInputSource(void)
{
    if ( IsAnyTVSourceInUse() )
    {
        return 0x24;
    }
    else if ( IsVgaInUse() )
    {
        return 0x21;
    }
    else if ( UI_INPUT_SOURCE_TYPE==UI_INPUT_SOURCE_COMPONENT )
    {
        return 0x27;
    }
#if (INPUT_YPBPR_VIDEO_COUNT >= 2)
    else if ( UI_INPUT_SOURCE_TYPE==UI_INPUT_SOURCE_COMPONENT2 )
    {
        return 0x47;
    }
#endif
    else if ( UI_INPUT_SOURCE_TYPE==UI_INPUT_SOURCE_AV)
    {
        return 0x25;
    }
#if (INPUT_AV_VIDEO_COUNT >= 2)
    else if ( UI_INPUT_SOURCE_TYPE==UI_INPUT_SOURCE_AV2)
    {
        return 0x45;
    }
#endif
#if (INPUT_HDMI_VIDEO_COUNT >0)
    else if ( UI_INPUT_SOURCE_TYPE==UI_INPUT_SOURCE_HDMI)
    {
        return 0x23;
    }
  #if (INPUT_HDMI_VIDEO_COUNT >1)
    else if ( UI_INPUT_SOURCE_TYPE==UI_INPUT_SOURCE_HDMI2)
    {
        return 0x43;
    }
  #endif
  #if (INPUT_HDMI_VIDEO_COUNT >2)
    else if ( UI_INPUT_SOURCE_TYPE==UI_INPUT_SOURCE_HDMI3)
    {
        return 0x63;
    }
  #endif
#endif
    else
    {
        return 0xff;
    }
}

//---------------------------------------------------------------------------
void afmSetSerialNumber(U8 sn[SerilNumberLength])
{
    U8 i;
    i=0;
    DBG_AFM(printf("\n SN= "));
    #ifdef ENABLE_TPV_SERIAL_NUMBER
    for(i = 0; i < SerilNumberLength; i++)
    {
        DBG_AFM(printf("%02bx-",sn[i]));
        stGenSettingExt.g_astSerialNumber.u8SerialNumber[i] = sn[i];
    }
    DBG_AFM(printf("\n "));
    MApp_SaveSerialNumberSetting();
    #endif
    UNUSED(sn);

}

void afmSetAudioVolume(U8 vol)
{
    DBG_AFM(printf("afmSetAudioVolume [%u]\n",vol));
    //MApp_ZUI_ACT_ShutdownOSD();
    if(vol == 0xff)
    {
        msAPI_AUD_AdjustAudioFactor(E_ADJUST_AUDIOMUTE, E_AUDIO_BYUSER_MUTEON, E_AUDIOMUTESOURCE_ACTIVESOURCE);
    }
    else
    {
        if(vol >= 100)
        {
            vol = 100;
        }
        stGenSetting.g_SoundSetting.Volume = vol;
        msAPI_AUD_AdjustAudioFactor(E_ADJUST_VOLUME, stGenSetting.g_SoundSetting.Volume, 0);
    }
}
U8 afmGetAudioVolume(void)
{
    DBG_AFM(printf("afmGetAudioVolume [%u]\n",stGenSetting.g_SoundSetting.Volume));
    return stGenSetting.g_SoundSetting.Volume;
}
		
void afmSelectChannelNum(U8 Major_MinorHi,U8 MajorChLo,U8 MinorChLo)
{
    UNUSED(Major_MinorHi);
    UNUSED(MajorChLo);
    UNUSED(MinorChLo);
    #if ENABLE_DTV
    if(IsDTVInUse())
    {
        // TODO:
    }
    #endif
    if(IsATVInUse())
    {
        DBG_AFM(printf("afmSelectChannelNum [%u]\n",MinorChLo));
        memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
        stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_CH_NUMBER;
        stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
        stRS232PostMsgSetting.ReciveData[0] = Major_MinorHi;
        stRS232PostMsgSetting.ReciveData[1] = MajorChLo;
        stRS232PostMsgSetting.ReciveData[2] = MinorChLo;
    }
}
//1. Tuner_Attr: 0= auto(only one tuner)  1= Air tuner, 2= Cable tuner
//2. Ch_Attr: 0= auto select,1= analog channel, 2= digital channel
//3. MajorCh : 16 bit data
//4. MinorCh: 16 bit data
void afmSelectFactoryChannelNum(U8 Tuner_Attr,U8 Ch_Attr,U8 MajorChHiByte,U8 MajorChLoByte,U8 MinorChHiByte,U8 MinorChLoByte)
{
    DBG_AFM(printf("afmSelectFactoryChannelNum [%u]\n",MinorChLoByte));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_FACTORY_CH_NUMBER;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = Tuner_Attr;
    stRS232PostMsgSetting.ReciveData[1] = Ch_Attr;
    stRS232PostMsgSetting.ReciveData[2] = MajorChHiByte;
    stRS232PostMsgSetting.ReciveData[3] = MajorChLoByte;
    stRS232PostMsgSetting.ReciveData[4] = MinorChHiByte;
    stRS232PostMsgSetting.ReciveData[5] = MinorChLoByte;
}


U8 afmCheckUSBPortAttachment(U8 check)
{
   // UNUSED(check);
    // TODO:
    DBG_AFM(printf("afmCheckUSBPortAttachment [%u]\n",g_u8USBPortAttachmentFlag));
	check=MDrv_UsbDeviceConnect();
    return check;

}

///////////////////////////////////////////////////////////////////////////////
 ///  public  afmSetTestPattern
 ///DESCRIPTION: -
 ///
 ///     val1:0 = no extra action
 ///          1 : do pre-setting job-1 for next coming procedure-1,
 ///          2 : do pre-setting job-2 for next coming procedure-2,
 ///     val2:1= 0n, 0= off.
 ///  return
 ///
///   author  Creass.liu 03-07-2012 written
///////////////////////////////////////////////////////////////////////////////

void afmSetTestPattern(U8 val1,U8 val2)
{
    DBG_AFM(printf("afmSetTestPattern val1= [%u],val2=[%u]\n",val1,val2));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
		
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_TEST_PATTERN;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = val1;
    stRS232PostMsgSetting.ReciveData[1] = val2;
}

void afmSetTestPatternRGB(U8 u8R,U8 u8G,U8 u8B)
{
    DBG_AFM(printf("afmSetTestPatternRGB u8R= [%u],u8G=[%u],u8B=[%u]\n",u8R,u8G,u8B));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_TEST_PATTERN_RGB;
	
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = (U8)(((U16)u8R)*255/100);
    stRS232PostMsgSetting.ReciveData[1] = (U8)(((U16)u8G)*255/100);
    stRS232PostMsgSetting.ReciveData[2] = (U8)(((U16)u8B)*255/100);
}

void afmSetPictureMode(U8 val)
{
    DBG_AFM(printf("afmSetPictureMode val= [%u],\n",val));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_PICTURE_MODE;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = val;
}
void afmGetEdidData(U8 InputSrc,U8 offset,U8 len)
{
    DBG_AFM(printf("afmGetEdidData InputSrc= [%u],offset=[%u],len=[%u]\n",InputSrc,offset,len));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_GET_EDID_DATA;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = InputSrc;
    stRS232PostMsgSetting.ReciveData[1] = offset;
    stRS232PostMsgSetting.ReciveData[2] = len;
}


//---------------------------------------------------------------------------
U8 afmInputSourceNormalize(U8 val)
{
    UNUSED(val);
    return 0;
}
		// TPV_XM Xue 20121214: add new command
void afmPowerOFF(void)
{
   DBG_AFM(printf("afmPowerOFF\n")); 
   memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
   stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_POWER_OFF;
   stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
}
void afmExecuteFactoryDomainReset(void)
{
   DBG_AFM(printf("afmExecuteFactoryDomainReset\n")); 
   memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
   stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_EXECUTE_FACTORY_DOMAIN_RESET;
   stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
}	
		// TPV_XM Xue 20121214: add hash unlock
void	 afmHASHUnlock(void)
{
   DBG_AFM(printf("afmHASHUnlock\n")); 
   memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
   stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_HASH_UNLOCK;
   stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
}
		// TPV_XM Xue 20121017: 
  #ifdef ENABLE_TPV_VCOM_ADJUST		
void afmSetVcomValue(U8 val)
{
    DBG_AFM(printf("afmSetVcomValue val= [%u],\n",val));
    memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
    stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_VCOM_VALUE;
    stRS232PostMsgSetting.ReciveBufCnt = afm.pkt_cnt;
    stRS232PostMsgSetting.ReciveData[0] = val;
}
U8 afmGetVcomValue(void)
{
      memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
  //  stRS232PostMsgSetting.enRS232PostMsgType = RS232_POST_MSG_SET_VCOM_VALUE;
	U8 DACValue= afm_GetVCOM_Value_DAC();
	XUETRACE( printf("xue trace afmGetVcomValue x=%x\n",DACValue));
	return DACValue;
}
#endif

//---------------------------------------------------------------------------


// TPV_XM Xue 20120818: 
 #if ENABLE_TPV_EBONY_PBS_MODE
void	seSetPowerStateContorl(U8 param)
{
	EN_RET enRetVal;
	if(param ==0)
	{
		u8KeyCode = KEY_POWER;
		enRetVal =EXIT_GOTO_STANDBY;
	}
	else if(param ==1)
	{
	
		u8KeyCode = KEY_POWER;
		
		
	}
}
 
extern void MApp_TV_SetTVKey (U8 u8Key);
extern void  MApp_ZUI_ACT_SetTargetMenuState(EN_MENU_STATE MenuState);
void seSetBDSInstallModeContorl(void)
{
	MApp_TV_SetTVKey(KEY_TV);		
	MApp_TopStateMachine_SetTopState(STATE_TOP_INSTALLGUIDE);	
	MApp_ZUI_ACT_SetTargetMenuState(STATE_MENU_INIT);
}
//extern void  MApp_UiMenu_MuteWin_Show(void);
//extern void  MApp_UiMenu_MuteWin_Hide(void);
//extern void  MApp_KeyProc_Mute(void);
extern void MApp_Ebony_PBS_CMD_Mute(U8 IsMute);
void seSetMuteModeContorl(BOOLEAN MuteStatused)
{
	if(MuteStatused ==1)
	{
		msAPI_AUD_AdjustAudioFactor(E_ADJUST_AUDIOMUTE, E_AUDIO_BYUSER_MUTEON, E_AUDIOMUTESOURCE_ACTIVESOURCE);
		//MApp_UiMenu_MuteWin_Show();
		if(IsStorageInUse() == TRUE)
			MUTE_On();
		else
			MApp_Ebony_PBS_CMD_Mute(MuteStatused);//tpv_xm gary 20130105 modify by mute behaivor
	}
	else
	{
		msAPI_AUD_AdjustAudioFactor(E_ADJUST_AUDIOMUTE, E_AUDIO_BYUSER_MUTEOFF, E_AUDIOMUTESOURCE_ACTIVESOURCE);
		//MApp_UiMenu_MuteWin_Hide();
		if(IsStorageInUse() == TRUE)
			MUTE_Off();
		else
			MApp_Ebony_PBS_CMD_Mute(MuteStatused);//tpv_xm gary 20130105 modify by mute behaivor
	}

}
void  seSetVideoMuteControl(BOOLEAN VideoMuteStatused)
{
	if(VideoMuteStatused)
	{
		//msAPI_Scaler_SetTestPattern_For_TPV_AgainMode(SYS_INPUT_SOURCE_TYPE(MAIN_WINDOW),TEST_COLOR_BLACK);
    		msAPI_Scaler_SetScreenMute(E_SCREEN_MUTE_PERMANENT, ENABLE, 0, MAIN_WINDOW);

	}
	else
	{
    		msAPI_Scaler_SetScreenMute(E_SCREEN_MUTE_PERMANENT, DISABLE, 0, MAIN_WINDOW);
		 //msAPI_Scaler_SetTestPattern_For_TPV_AgainMode(SYS_INPUT_SOURCE_TYPE(MAIN_WINDOW), TEST_COLOR_OFF);

	}
}
void seSetFactoryResetControl(void)
{
	 //printf("seSetFactoryResetControl\n");
	 if (MApp_ZUI_GetActiveOSD()!=E_OSD_EMPTY) //ZUI:
	{
		MApp_ZUI_ACT_ExecuteWndAction(EN_EXE_CLOSE_CURRENT_OSD);
	}
	memset(&stRS232PostMsgSetting, 0, sizeof(MS_RS232_POST_MSG_SETTING));
	stRS232PostMsgSetting.enRS232PostMsgType = Ebony_RS232_POST_MSG_SET_FACTORY_RESET;
	
}
void seSetIncrement_DecrementVolumeControl(U8 AudioOPStatused)
{
	if(AudioOPStatused==1)
		u8KeyCode=KEY_VOLUME_PLUS;
	else
		u8KeyCode=KEY_VOLUME_MINUS;
}
extern void Mapp_BalanceVolMiddle(void);
extern void Mapp_BalanceVolLeft(void);
extern void Mapp_BalanceVolRight(void);
extern void Mapp_AdjustVolumeParm(U8 VolumeParm);
void seSetBalanceControl(U8 BalanceOPStatused)
{
	if(BalanceOPStatused ==0)
		{
			Mapp_BalanceVolMiddle();
		}
	else if(BalanceOPStatused==1)
		{
			Mapp_BalanceVolLeft();
		}
	else  if(BalanceOPStatused==2)
		{
			Mapp_BalanceVolRight();
		}
	
}
void seSetVolumeControl(U8 VolumeParm)
{
	Mapp_AdjustVolumeParm(VolumeParm);
}

void seSetBrightnessControl(U8 BrightnessParm)
{
	if (MApp_ZUI_GetActiveOSD()!=E_OSD_EMPTY) //ZUI:
	{
		MApp_ZUI_ACT_ExecuteWndAction(EN_EXE_CLOSE_CURRENT_OSD);
	}
	U8 u8BR, u8BG, u8BB;
	ST_PICTURE.u8Brightness=BrightnessParm;	
	u8BR = MApi_XC_Sys_ACE_transfer_Bri((MApp_Scaler_FactoryAdjBrightness(msAPI_Mode_PictureBrightnessN100toReallyValue(ST_PICTURE.u8Brightness),ST_SUBCOLOR.u8SubBrightness)), BRIGHTNESS_R);
	u8BG = MApi_XC_Sys_ACE_transfer_Bri((MApp_Scaler_FactoryAdjBrightness(msAPI_Mode_PictureBrightnessN100toReallyValue(ST_PICTURE.u8Brightness),ST_SUBCOLOR.u8SubBrightness)), BRIGHTNESS_G);
	u8BB = MApi_XC_Sys_ACE_transfer_Bri((MApp_Scaler_FactoryAdjBrightness(msAPI_Mode_PictureBrightnessN100toReallyValue(ST_PICTURE.u8Brightness),ST_SUBCOLOR.u8SubBrightness)), BRIGHTNESS_B);
	MApi_XC_ACE_PicSetBrightnessInVsync(MAIN_WINDOW, u8BR, u8BG, u8BB);
	MApp_ZUI_API_InvalidateAllSuccessors(HWND_EBONY_MAIN_MENU_SETUP_PROCESSBAR_PANGE);

	 // MApi_XC_ACE_PicSetHue( MAIN_WINDOW, MApi_XC_IsYUVSpace(MAIN_WINDOW), msAPI_Mode_PictureHueN100toReallyValue(ST_PICTURE.u8Hue) );

}
void seSetColorControl(U8 ColorParm)
{
	if (MApp_ZUI_GetActiveOSD()!=E_OSD_EMPTY) //ZUI:
	{
		MApp_ZUI_ACT_ExecuteWndAction(EN_EXE_CLOSE_CURRENT_OSD);
	}
	ST_PICTURE.u8Saturation=ColorParm;
	MApi_XC_ACE_PicSetSaturation(MAIN_WINDOW, MApi_XC_IsYUVSpace(MAIN_WINDOW),  msAPI_Mode_PictureSaturationN100toReallyValue(ST_PICTURE.u8Saturation) );
	MApp_ZUI_API_InvalidateAllSuccessors(HWND_EBONY_MAIN_MENU_SETUP_PROCESSBAR_PANGE);

}
void seSetContrastControl(U8 ContrastParm)
{
	if (MApp_ZUI_GetActiveOSD()!=E_OSD_EMPTY) //ZUI:
	{
		MApp_ZUI_ACT_ExecuteWndAction(EN_EXE_CLOSE_CURRENT_OSD);
	}
	ST_PICTURE.u8Contrast=ContrastParm;
	MApi_XC_ACE_PicSetContrast(MAIN_WINDOW, MApi_XC_IsYUVSpace(MAIN_WINDOW), MApp_Scaler_FactoryContrast(msAPI_Mode_PictureContrastN100toReallyValue(ST_PICTURE.u8Contrast),ST_SUBCOLOR.u8SubContrast));
	MApp_ZUI_API_InvalidateAllSuccessors(HWND_EBONY_MAIN_MENU_SETUP_PROCESSBAR_PANGE);

}
void seSetSharpnessControl(U8 SharpnessParm)
{
	if (MApp_ZUI_GetActiveOSD()!=E_OSD_EMPTY) //ZUI:
	{
		MApp_ZUI_ACT_ExecuteWndAction(EN_EXE_CLOSE_CURRENT_OSD);
	}
	ST_PICTURE.u8Sharpness=SharpnessParm;
	MApi_XC_ACE_PicSetSharpness( MAIN_WINDOW, msAPI_Mode_PictureSharpnessN100toReallyValue(ST_PICTURE.u8Sharpness) );
	MApp_ZUI_API_InvalidateAllSuccessors(HWND_EBONY_MAIN_MENU_SETUP_PROCESSBAR_PANGE);

}
void seSetHueControl(U8 HueParm)
{
	if (MApp_ZUI_GetActiveOSD()!=E_OSD_EMPTY) //ZUI:
	{
		MApp_ZUI_ACT_ExecuteWndAction(EN_EXE_CLOSE_CURRENT_OSD);
	}
	ST_PICTURE.u8Hue=HueParm;
	MApi_XC_ACE_PicSetHue( MAIN_WINDOW, MApi_XC_IsYUVSpace(MAIN_WINDOW), msAPI_Mode_PictureHueN100toReallyValue(ST_PICTURE.u8Hue) );
	MApp_ZUI_API_InvalidateAllSuccessors(HWND_EBONY_MAIN_MENU_SETUP_PROCESSBAR_PANGE);
}
void seSetColorTemperatureControl(U8 ColorTemperatureParm)
{
	if (MApp_ZUI_GetActiveOSD()!=E_OSD_EMPTY) //ZUI:
	{
		MApp_ZUI_ACT_ExecuteWndAction(EN_EXE_CLOSE_CURRENT_OSD);
	}
	if(ColorTemperatureParm ==0)
		{
			ST_PICTURE.eColorTemp =MS_COLOR_TEMP_WARM;
		}
	else if(ColorTemperatureParm ==1)
		{
			ST_PICTURE.eColorTemp =MS_COLOR_TEMP_MEDIUM;
		}
	else
		{
			ST_PICTURE.eColorTemp =MS_COLOR_TEMP_COOL;
		}
		#if ENABLE_NEW_COLORTEMP_METHOD
			#if ENABLE_PRECISE_RGBBRIGHTNESS
				MApi_XC_ACE_PicSetColorTemp(MAIN_WINDOW, MApi_XC_IsYUVSpace(MAIN_WINDOW), (XC_ACE_color_temp_ex *) &ST_COLOR_TEMP);
			#else
				MApi_XC_ACE_PicSetColorTemp(MAIN_WINDOW, MApi_XC_IsYUVSpace(MAIN_WINDOW), (XC_ACE_color_temp *) &ST_COLOR_TEMP);
			#endif
		#else
			#if ENABLE_PRECISE_RGBBRIGHTNESS
				MApi_XC_ACE_PicSetColorTemp( MAIN_WINDOW, MApi_XC_IsYUVSpace(MAIN_WINDOW), (XC_ACE_color_temp_ex *) &ST_COLOR_TEMP );
				MApi_XC_ACE_PicSetBrightnessPreciseInVsync(MAIN_WINDOW, MApi_XC_Sys_ACE_transferRGB_Bri(ST_COLOR_TEMP.cRedOffset, BRIGHTNESS_R ), MApi_XC_Sys_ACE_transferRGB_Bri(ST_COLOR_TEMP.cGreenOffset, BRIGHTNESS_G), MApi_XC_Sys_ACE_transferRGB_Bri(ST_COLOR_TEMP.cBlueOffset, BRIGHTNESS_B));
			#else	
			//Ebony use this
				MApi_XC_ACE_PicSetColorTemp( MAIN_WINDOW, MApi_XC_IsYUVSpace(MAIN_WINDOW), (XC_ACE_color_temp *) &ST_COLOR_TEMP );
				MApi_XC_ACE_PicSetBrightnessInVsync(MAIN_WINDOW, MApi_XC_Sys_ACE_transferRGB_Bri(ST_COLOR_TEMP.cRedOffset, BRIGHTNESS_R ), MApi_XC_Sys_ACE_transferRGB_Bri(ST_COLOR_TEMP.cGreenOffset, BRIGHTNESS_G), MApi_XC_Sys_ACE_transferRGB_Bri(ST_COLOR_TEMP.cBlueOffset, BRIGHTNESS_B));
			#endif
		#endif

}
void seSetPictureFormatControl(U8 PictureFormatParm)
{
	if (MApp_ZUI_GetActiveOSD()!=E_OSD_EMPTY) //ZUI:
		{
			MApp_ZUI_ACT_ExecuteWndAction(EN_EXE_CLOSE_CURRENT_OSD);
		}
	XUETRACE(printf("xue trace PictureFormatParm=%d\n",PictureFormatParm));	
	switch(PictureFormatParm)
	{
		case 0:
			ST_VIDEO.eAspectRatio=EN_AspectRatio_4X3;			
			break;
		case 1:
				#if VGA_HDMI_YUV_POINT_TO_POINT
				    if ( IsVgaInUse()
				        #if (INPUT_HDMI_VIDEO_COUNT > 0)
				        #if (MEMORY_MAP <= MMAP_32MB)
				       ||(IsHDMIInUse() && MDrv_PQ_Check_PointToPoint_Mode())
				        #endif
					//||( IsHDMIInUse() && (!MApi_XC_IsCurrentFrameBufferLessMode()) && ( !MApi_XC_IsCurrentRequest_FrameBufferLessMode()))
					||( IsHDMIInUse()&& (!MApi_XC_IsCurrentFrameBufferLessMode()) && ( !MApi_XC_IsCurrentRequest_FrameBufferLessMode())&& !g_HdmiPollingStatus.bIsHDMIMode)
				        #if ENABLE_TPV_HDMI_MODE
				      ||(IsHDMIInUse() && (stGenSetting.g_SysSetting.bIsHDMIVideoMode == FALSE) && (MApp_Scaler_CheckHDMIModeAvailable() == TRUE))
				        #endif
				        #endif
				        )
				        	{
							break;
				        	}
						else
							
			          #endif
			          	{
					ST_VIDEO.eAspectRatio=EN_AspectRatio_Zoom1;
					break;
			          	}
			case 2:
						#if VGA_HDMI_YUV_POINT_TO_POINT
				    if ( IsVgaInUse()
				        #if (INPUT_HDMI_VIDEO_COUNT > 0)
				        #if (MEMORY_MAP <= MMAP_32MB)
				       ||(IsHDMIInUse() && MDrv_PQ_Check_PointToPoint_Mode())
				        #endif
					//||( IsHDMIInUse() && (!MApi_XC_IsCurrentFrameBufferLessMode()) && ( !MApi_XC_IsCurrentRequest_FrameBufferLessMode()))
					||( IsHDMIInUse()&& (!MApi_XC_IsCurrentFrameBufferLessMode()) && ( !MApi_XC_IsCurrentRequest_FrameBufferLessMode())&& !g_HdmiPollingStatus.bIsHDMIMode)
				        #if ENABLE_TPV_HDMI_MODE
				      ||(IsHDMIInUse() && (stGenSetting.g_SysSetting.bIsHDMIVideoMode == FALSE) && (MApp_Scaler_CheckHDMIModeAvailable() == TRUE))
				        #endif
				        #endif
				        )
				        	{
							break;
				        	}
						else		
			          #endif
			          	{
					ST_VIDEO.eAspectRatio=EN_AspectRatio_Zoom2;
					break;
			          	}
		case 4:
			ST_VIDEO.eAspectRatio=EN_AspectRatio_16X9;
			break;
		case 5:		
			break;
		case 6:
				#if VGA_HDMI_YUV_POINT_TO_POINT
				    if ( IsVgaInUse()
				        #if (INPUT_HDMI_VIDEO_COUNT > 0)
				        #if (MEMORY_MAP <= MMAP_32MB)
				       ||(IsHDMIInUse() && MDrv_PQ_Check_PointToPoint_Mode())
				        #endif
					//||( IsHDMIInUse() && (!MApi_XC_IsCurrentFrameBufferLessMode()) && ( !MApi_XC_IsCurrentRequest_FrameBufferLessMode()))
					||( IsHDMIInUse()&& (!MApi_XC_IsCurrentFrameBufferLessMode()) && ( !MApi_XC_IsCurrentRequest_FrameBufferLessMode())&& !g_HdmiPollingStatus.bIsHDMIMode)
				        #if ENABLE_TPV_HDMI_MODE
				      ||(IsHDMIInUse() && (stGenSetting.g_SysSetting.bIsHDMIVideoMode == FALSE) && (MApp_Scaler_CheckHDMIModeAvailable() == TRUE))
				        #endif
				        #endif
				        )
				ST_VIDEO.eAspectRatio=EN_AspectRatio_point_to_point;
			          #endif
			break;
		case 7:		
			break;
		default:
			break;
	}
	
	 MApp_Scaler_Setting_SetVDScale( ST_VIDEO.eAspectRatio , MAIN_WINDOW );	
}
void seSetAutoAdjustControl(void)
{
	if (MApp_ZUI_GetActiveOSD()!=E_OSD_EMPTY) //ZUI:
	{
		MApp_ZUI_ACT_ExecuteWndAction(EN_EXE_CLOSE_CURRENT_OSD);
	}
	if(IsSrcTypeVga(SYS_INPUT_SOURCE_TYPE(MAIN_WINDOW)))
	{
		if(MApp_PCMode_Enable_SelfAuto(ENABLE, MAIN_WINDOW))
		{   // to do Auto adjust
			MApp_ZUI_ACT_StartupOSD(E_OSD_MESSAGE_BOX);
			MApp_ZUI_ACT_ExecuteWndAction(EN_EXE_SHOW_AUTO_ADJUSTING_MSGBOX);
			//MApp_PCMode_RunSelfAuto(MAIN_WINDOW);
		}
	}
	else
	{
		if(MApp_PCMode_Enable_SelfAuto(ENABLE, SUB_WINDOW))
		{   // to do Auto adjust
			MApp_PCMode_RunSelfAuto(SUB_WINDOW);
		}
	}
}

void seSetSourceSwitchControl(U8 SourceParm)
{
	if (MApp_ZUI_GetActiveOSD()!=E_OSD_EMPTY) //ZUI:
	{
		MApp_ZUI_ACT_ExecuteWndAction(EN_EXE_CLOSE_CURRENT_OSD);
	}
	
	if(IsStorageInUse())
	{
		switch(SourceParm)
		{
			case 1:
				MApp_ZUI_ACT_HandleDmpKey(VK_TV);
				break;
			case 3:
				MApp_ZUI_ACT_HandleDmpKey(VK_AV);
				break;
			case 5:
				MApp_ZUI_ACT_HandleDmpKey(VK_COMPONENT);				
				break;
			case 6:
				MApp_ZUI_ACT_HandleDmpKey(VK_PC);				
				break;
			case 7:
				MApp_ZUI_ACT_HandleDmpKey(VK_DMP);					
				break;
			case 8:
			case 14:
				MApp_ZUI_ACT_HandleDmpKey(VK_HDMI);					
				break;
			case 9:	
			case 15:
				SwitchHDMI2Source=TRUE;
				MApp_ZUI_ACT_HandleDmpKey(VK_HDMI);				  
				break;	
			default:
			break;
		}
	}
	else
	{
		switch(SourceParm)
		{
			case 1:
				MApp_TV_SetTVKey(KEY_TV);	
				break;
			case 3:
				MApp_TV_SetTVKey(KEY_AV);	
				break;
			case 5:
				MApp_TV_SetTVKey(KEY_COMPONENT);	
				break;
			case 6:
				MApp_TV_SetTVKey(KEY_PC);	
				break;
			case 7:
				MApp_TV_SetTVKey(KEY_DMP);	
				break;
			case 8:
			case 14:
				MApp_TV_SetTVKey(KEY_HDMI1);	
				break;
			case 9:
			case 15:
				MApp_TV_SetTVKey(KEY_HDMI2);	
				break;	
			default:
			break;
		}
 	}

	
}

void seSetUserInputControlState(U8 LockParm)
//0 ¨C Lock both IR and LKB.    1 ¨C Enable IR but lock LKB.   2 ¨C Enable LKB but lock IR  3 - Enable both LKB and IR
{
	switch(LockParm)
	{
		case 0:
			//stKeyStatus.keydata == IRKEY_BUTTON_LOCK;
			//u8KeyCode=KEY_BUTTON_RC_LOCK;
			u8KeyCode=KEY_BUTTON_LOCK_ALL;
			stGenSetting.g_PBSModeSetting.bPbsCtlSetRCL =ENABLE;
			stGenSetting.g_SysSetting.g_enKeyPadLock = EN_E5_KeyLock_All;			 
			break;
		case 1:
			u8KeyCode=KEY_BUTTON_LOCK_ALL;
			stGenSetting.g_PBSModeSetting.bPbsCtlSetRCL =DISABLE;
			stGenSetting.g_SysSetting.g_enKeyPadLock = EN_E5_KeyLock_All;	
			break;
		case 2:
			u8KeyCode=KEY_BUTTON_RC_LOCK;
			stGenSetting.g_PBSModeSetting.bPbsCtlSetRCL =ENABLE;
			stGenSetting.g_SysSetting.g_enKeyPadLock = EN_E5_KeyLock_Off;	
			break;
		case 3:
			//stKeyStatus.keydata == IRKEY_BUTTON_
			stGenSetting.g_PBSModeSetting.bPbsCtlSetRCL =DISABLE;
			stGenSetting.g_SysSetting.g_enKeyPadLock = EN_E5_KeyLock_Off;	
			break;
		default:
			break;
	}
}
//volume limit have issue ??????		// TPV_XM Xue 20120927: 
void seSetVolumeLimitsControl(U8 MaxVolumeValue,U8 LimitVolumeValue)
{
	U8 LimitVolumeValueTemp=0;
	stGenSetting.g_PBSModeSetting.u8PbsMaxVolume=MaxVolumeValue;
	//sPbsSettingBak.u8PbsMaxVolume=MaxVolumeValue;
	stGenSetting.g_PBSModeSetting.u8PbsSwitchOnVolUD=LimitVolumeValue;
	//sPbsSettingBak.u8PbsSwitchOnVolUD=LimitVolumeValue;
	
	LimitVolumeValueTemp=(LimitVolumeValue*stGenSetting.g_PBSModeSetting.u8PbsMaxVolume)/100;
	XUETRACE(printf("xue trace LimitVolumeValueTemp=%d\n",LimitVolumeValueTemp));		
	//stGenSetting.g_SoundSetting.Volume*MAX_NUM_OF_VOL_LEVEL)/stGenSetting.g_PBSModeSetting.u8PbsMaxVolume
	Mapp_AdjustVolumeParm(LimitVolumeValueTemp);	
}
extern void SetKeydata (U8 keydata);
void seSetUserInputDataControl(U8 IRSystemCode,U8 IRCode,U8 KeyPressStatus,U8 KeyPadCode)
{

	
	if((IRSystemCode!=0xff)&&(IRSystemCode==0)&&(KeyPressStatus==0xff))
	{
	     SetKeydata(IRCode);
		
	}
	if((IRSystemCode!=0xff) &&(KeyPressStatus==0xff)&&(KeyPadCode==0Xff))
	{
		switch(IRCode)
			{
      			case 0x0C:
      				u8KeyCode=KEY_POWER;
      				break;
      			case 0x4C:
      				u8KeyCode=KEY_CHANNEL_PLUS;
      				break;
      			case 0x4D:
      				u8KeyCode=KEY_CHANNEL_MINUS;
      				break;
      		 	case 0x10:
      				u8KeyCode=KEY_VOLUME_PLUS;
      				break;
      			 case 0x11:
      				u8KeyCode=KEY_VOLUME_MINUS;
      				break;
      			case 0x54:
      				u8KeyCode=KEY_MENU;
      				break;
      			default:
      					break;
			}
	}
				
}
void seSetOSDLanguageControl()
{
   XUETRACE(printf("xue trace seSetOSDLanguage  LanguageIndex=%d\n",g_u8OSDLangSelected));
 /* 
 0 English 14 Danish 28 Arabic
1 German 15 Dutch 29 Hebrew
2 Swedish 16 Finnish 30 Hungarian
3 Italian 17 Gallic 31 Hungal
4 French 18 Galligan 32 Malay
5 Spanish 19 Norwegian 33 Persian
6 Czech 20 Portuguese 34 Simplified
Chinese
7 Polish 21 Serbian 35 Taiwanese
8 Turkish 22 Slovak
9 Russian 23 Slovenian 254 Reserved
10 Greek 24 Welsh 255 Reserved
11 Basque 25 Romanian
12 Catalan 26 Estonian
13 Croatian 27 Ukrainian
*/   
if (MApp_ZUI_GetActiveOSD()!=E_OSD_EMPTY) //ZUI:
	{
		MApp_ZUI_ACT_ExecuteWndAction(EN_EXE_CLOSE_CURRENT_OSD);
	}
   switch(g_u8OSDLangSelected)
   	{
   	case 0:
		SET_OSD_MENU_LANGUAGE(LANGUAGE_ENGLISH);
		break;
	case 34:
		SET_OSD_MENU_LANGUAGE(LANGUAGE_CHINESE);
		break;
	default:
		 SET_OSD_MENU_LANGUAGE(LANGUAGE_DEFAULT); // menu language
		break;
   	}      
    MApp_SaveSysSetting();
}
#endif
		// TPV_XM Xue 20121017: add adjust VCOM function
#ifdef ENABLE_TPV_VCOM_ADJUST
extern BOOLEAN MDrv_IIC_WriteByte(U16 u16BusNumSlaveID, U8 u8RegAddr, U8 u8Data);
extern BOOLEAN MDrv_IIC_ReadByte(U16 u16BusNumSlaveID, U8 u8RegAddr, U8 *pu8Data);
extern U16 MApp_CalCheckSum( BYTE *pBuf, U16 ucBufLen );
extern void MApp_WriteDatabase(U32 dstIndex, U8* srcAddr, U16 size);

void afm_SetVCOM_Value_OPT(U8 OPTValue)
{
	if( MDrv_IIC_WriteByte( 0x50, 0x02, 0x00))
		MDrv_IIC_WriteByte( 0x50, 0x00, OPTValue);
		
}
void afm_SetVCOM_Value_DAC(U8 DACValue)
{
	MDrv_IIC_WriteByte( 0x50, 0x02, 0x80);
		MDrv_IIC_WriteByte( 0x50, 0x00, DACValue);
			{			
				stGenSettingExt.VcomDACValueSave.u8VCOMValue=DACValue;
				stGenSettingExt.VcomDACValueSave.u16VCOMValueCS= MApp_CalCheckSum((BYTE *)&(stGenSettingExt.VcomDACValueSave), RM_SIZE_VCOM_DATA );
				   MApp_WriteDatabase(RM_VCOM_DATA_START_ADR,
                             (BYTE *)&(stGenSettingExt.VcomDACValueSave), RM_SIZE_VCOM_DATA);
			}
				
}
U8 afm_GetVCOM_Value_OPT(void)
{
	U8 ReadOPTvalue=0;
	if( MDrv_IIC_WriteByte( 0x50, 0x02, 0x00))
		MDrv_IIC_ReadByte( 0x50, 0x00 , &ReadOPTvalue );
	return ReadOPTvalue;
	
}
U8 afm_GetVCOM_Value_DAC(void)
{
	U8 ReadDACvalue=0;
	//MDrv_IIC_WriteByte( 0x50, 0x02, 0x80);
		//MDrv_IIC_ReadByte( 0x50, 0x00, &ReadDACvalue );
		 MApp_ReadDatabase(RM_VCOM_DATA_START_ADR, (BYTE *)&(stGenSettingExt.VcomDACValueSave), RM_SIZE_VCOM_DATA);
			 if(stGenSettingExt.VcomDACValueSave.u16VCOMValueCS==MApp_CalCheckSum((BYTE *)&(stGenSettingExt.VcomDACValueSave), RM_SIZE_VCOM_DATA ))
			 	ReadDACvalue=stGenSettingExt.VcomDACValueSave.u8VCOMValue;
	return ReadDACvalue;
	
}
#endif


