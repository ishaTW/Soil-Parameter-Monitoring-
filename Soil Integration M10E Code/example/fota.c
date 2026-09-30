//#ifdef __EXAMPLE_FOTA_HTTP__
#include<string.h>
#include "ql_trace.h"
#include "ql_timer.h"
#include "ql_type.h"
#include "ql_stdlib.h"
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_audio.h"
#include "ql_pin.h"
#include "ql_fota.h"
#include "Ql_multitask.h"
#include "Ql_tcpip.h"
#include "Ql_error.h"
#include "ql_sms.h"
#include "ql_fcm.h"
#include "GPRMC.h" 
#include "Ql_filesystem.h"
#include "sms_handle.h" 
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_error.h"
#include "ql_fcm.h"
#include "ql_filesystem.h"
#include "fota.h"
int doing_fota=0;
extern char textBuf[1000];
extern u8 Fota_http_Flag;
extern QlTimer onesectimer;
extern QlTimer onesecdummy;
extern char APN_NAME[];
char ptr_imei[50];
#ifdef OUT_DEBUG(x,...)
#undef OUT_DEBUG(x,...)
#endif

/************************************************************************************************************
 * Debug
 *************************************************************************************************************/
#define DEBUG_ENABLE 1
#if DEBUG_ENABLE > 0
#define OUT_DEBUG(...)  \
		Ql_memset(textBuf, 0, sizeof(textBuf));  \
		Ql_sprintf(textBuf,__VA_ARGS__);   \
		Ql_SendToUart(ql_uart_port2,(u8*)(textBuf),Ql_strlen(textBuf));
#else
#define OUT_DEBUG(...)
#endif

/*************************************************************************************************************/

#define START_FLAG_STRING "\r\n@=-=-=-=-=-=-= HTTP2FOTA_APP_XXX =-=-=-=-=-=-=\r\n\r\n\r\n"
//#define APP_BIN_URL   "http://124.74.41.170/ftpsvr/max/"
//#define APP_BIN_URL "http://www.myfleetview.com:8080/sample/APPGS2MDM64_A01_Upgrade_Package.bin"

#define MAX_BUF_SIZE 1024
#define MAX_URL_LENGTH 1024
#define MAX_NAME_LENGTH 120
extern QlEventBuffer flSignalBuffer;
typedef enum
{
	PROCESS_STATE_TYPE_NONE,
	PROCESS_STATE_TYPE_ATE0,
	PROCESS_STATE_TYPE_CHECK_CGATT,
	PROCESS_STATE_TYPE_QIFGCNT,
	PROCESS_STATE_TYPE_QICSGP,
	PROCESS_STATE_TYPE_QHTTPURL,
	PROCESS_STATE_TYPE_INPUTURL,
	PROCESS_STATE_TYPE_QHTTGET,
	PROCESS_STATE_TYPE_QHTTPREAD,
	PROCESS_STATE_TYPE_QIDEACT,
	PROCESS_STATE_TYPE_UPDATEFLAGS
}PROCESS_STATE_TYPE;


typedef struct _TProcessState
{
	PROCESS_STATE_TYPE iState;
	u64 uiStateBeginTime;
}TProcessState;

typedef struct _TH2FData
{
	TProcessState ProcessState;
	QlTimer Timer;
	FEED_DOG    Q_t_Watch_dog;
	unsigned int uiTimeOut;
	char Buffer[MAX_BUF_SIZE];
	bool bBeginFlag;
}TH2FData;
//APPGS2MDM64A01_GCC_Upgrade_Package
//char URL_FOTA[]="http://www.myfleetview.com:8080/sample/APPGS2MDM64_A01_Upgrade_Package.bin";
char FOTA_IMEI[120];
//char URL_FOTA[150]="http://www.myfleetview.com:8080/sample/APPGS2MDM64_A01_Upgrade_Package.bin";
char URL_FOTA[150];

static TH2FData s_H2FData;
static bool s_bDataMode = FALSE;
static bool s_bDEACTReboot;
static char s_szURLBuf[MAX_URL_LENGTH];
static char APP_NAME[MAX_NAME_LENGTH];

extern bool in_cam_rou;

void H2F_SendCMDCtrl(void);

/*****************************************************************
 *
 *Check if command timeout.if time out,it means download failed!
 *the program will restart module to upgrade again
 *
 *******************************************************************/
bool H2F_CheckTimeOut(void)
{
	if(Ql_GetRelativeTime() - s_H2FData.ProcessState.uiStateBeginTime > s_H2FData.uiTimeOut)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/********************************************************************
 *
 *Init s_H2FData.s_H2FData is a very important parameter 
 *
 *********************************************************************/

void H2F_Init(void)
{
	s_H2FData.bBeginFlag = FALSE;


	/*---------------------------------------------------*/
	Ql_memset((void *)(&s_H2FData.Q_t_Watch_dog), 0, sizeof(FEED_DOG)); //Do not enable  watch_dog
	s_H2FData.Q_t_Watch_dog.Q_gpio_pin1 = Ql_GetGpioByName(QL_PINNAME_DTR);
	s_H2FData.Q_t_Watch_dog.Q_feed_interval1 = 100;
	s_H2FData.Q_t_Watch_dog.Q_gpio_pin2 = Ql_GetGpioByName(QL_PINNAME_NETLIGHT);
	s_H2FData.Q_t_Watch_dog.Q_feed_interval2 = 500;
	/*---------------------------------------------------*/
	s_H2FData.ProcessState.iState = PROCESS_STATE_TYPE_NONE;
	s_H2FData.ProcessState.uiStateBeginTime = Ql_GetRelativeTime();
	s_H2FData.uiTimeOut = 125*1000;
	Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
	s_H2FData.Timer.timeoutPeriod = Ql_MillisecondToTicks(500);
	Ql_StartTimer(&s_H2FData.Timer);
	//R_//OUT_DEBUG("\r\n Started timer s_H2FData OK!\r\n");
	Ql_memset(s_szURLBuf, 0, sizeof(s_szURLBuf));
	// Ql_strcpy(s_szURLBuf, APP_BIN_URL);
	Ql_strcpy(s_szURLBuf, URL_FOTA);
	s_bDataMode = FALSE;
	s_bDEACTReboot = FALSE;
}

/********************************************************************
 *
 *When download data and write to fota cache, you must call this 
 *functin for initialization.
 *
 *********************************************************************/

void H2F_WriteData_Init(void)
{
	int iRet = -1;

	iRet = Ql_Fota_App_Init(&s_H2FData.Q_t_Watch_dog);
	//R_//OUT_DEBUG("\r\nQl_Fota_App_Init ->iRet=%d\r\n",iRet);
	if(QL_RET_OK != iRet)
	{
		OUT_DEBUG("\r\n[max] : Ql_Fota_App_Init FAILED!\r\n");
		OUT_DEBUG("\r\n[max] : Reboot 3 seconds later ...\r\n");
		Ql_Sleep(3000);
		Ql_Reset(0);
	}
	else
	{
		OUT_DEBUG("\r\n[max] : Ql_Fota_App_Init OK!\r\n");
	}
}

/********************************************************************
 *
 *Write data to Fota cache
 *
 *********************************************************************/

void  H2F_WriteData_Write(s32 iLen, s8 *buffer)
{
	int iRet = -1;
	int i = 0;
	static int s_iSizeRem = 0;

	for(i=0; i<iLen; i++)
	{
		if(0x00 == buffer[i])
		{
			continue;
		}
		else
		{
			break;
		}
	}

	if(i == iLen)
	{
		//R_//OUT_DEBUG("[max] : ALL Data is ZERO!!!!\r\n");
	}

	iRet = Ql_Fota_App_Write_Data(iLen, buffer);
	if(QL_RET_OK != iRet)
	{
		OUT_DEBUG("\r\n[max] : Ql_Fota_App_Write_Data FAILED!\r\n");
	}
	else
	{
		s_iSizeRem += iLen;
		//R_//OUT_DEBUG("[max] : Ql_Fota_App_Write_Data -> %-5d bytes OK!(TotalWrite=%-5d)\r\n", iLen, s_iSizeRem);
	}
}

/**********************************************************************
 *
 *Finish writing data to Fota Cache.
 *
 ************************************************************************/
void  H2F_WriteData_Finish(void)
{
	int iRet = -1;
	OUT_DEBUG("\r\n[max] : H2F_WriteData_Finish is finishing...\r\n ");
	iRet = Ql_Fota_App_Finish();
	if(QL_RET_OK != iRet)
	{
		OUT_DEBUG("\r\n[max] : Ql_Fota_App_Finish FAILED!\r\n");
		OUT_DEBUG("\r\n[max] : Reboot 3 seconds later ...\r\n");
		Ql_Sleep(3000);
		Ql_Reset(0);
	}
	else
	{
		s_H2FData.ProcessState.iState++;
		H2F_SendCMDCtrl();
		// doing_fota=0;
		// in_cam_rou=1;
		OUT_DEBUG("\r\n[max] : Ql_Fota_App_Finish OK!\r\n");
	}
}

/*****************************************************************
 *
 *When get successful result,just like "OK\r\n", "CONNECT\r\n",
 *and so on,The H2F_StateMachine will jump to next state Automatically
 *
 *******************************************************************/
void H2F_StateMachine(char *pTriggerStr)
{
	int iRet = -1;
	bool bFlags = TRUE;
	if((NULL != Ql_strstr(pTriggerStr, "OK")) &&
			PROCESS_STATE_TYPE_ATE0 == s_H2FData.ProcessState.iState)
	{
		OUT_DEBUG("\r\n[max] : ATE0 OK!");
		s_H2FData.ProcessState.iState++;
		H2F_SendCMDCtrl();
	}
	else if((NULL != Ql_strstr(pTriggerStr, "+CGATT: 0")) &&
			PROCESS_STATE_TYPE_CHECK_CGATT == s_H2FData.ProcessState.iState)
	{
		Ql_Sleep(500);
		OUT_DEBUG("\r\n[max] : CGATT = 0!");
		H2F_SendCMDCtrl();
	}
	else if((NULL != Ql_strstr(pTriggerStr, "+CGATT: 1")) &&
			PROCESS_STATE_TYPE_CHECK_CGATT == s_H2FData.ProcessState.iState)
	{
		OUT_DEBUG("\r\n[max] : CGATT OK!");
		s_H2FData.ProcessState.iState++;
		H2F_SendCMDCtrl();
	}
	else if((NULL != Ql_strstr(pTriggerStr, "OK")) &&
			PROCESS_STATE_TYPE_QIFGCNT == s_H2FData.ProcessState.iState)
	{
		OUT_DEBUG("\r\n[max] : QIFGCNTA OK!");
		s_H2FData.ProcessState.iState++;
		H2F_SendCMDCtrl();
	}
	else if((NULL != Ql_strstr(pTriggerStr, "OK")) &&
			PROCESS_STATE_TYPE_QICSGP == s_H2FData.ProcessState.iState)
	{
		OUT_DEBUG("\r\n[max] : QICSGP OK!");
		s_H2FData.ProcessState.iState++;
		H2F_SendCMDCtrl();
	}
#if 0
	else if((NULL != Ql_strstr(pTriggerStr, "OK")) &&
			PROCESS_STATE_TYPE_QIREGAPP == s_H2FData.ProcessState.iState)
	{
		OUT_DEBUG("\r\n[max] : QIREGAPP OK!");
		s_H2FData.ProcessState.iState++;
		H2F_SendCMDCtrl();
	}
	else if((NULL != Ql_strstr(pTriggerStr, "OK")) &&
			PROCESS_STATE_TYPE_QIACT == s_H2FData.ProcessState.iState)
	{
		OUT_DEBUG("\r\n[max] : QIACT OK!");
		s_H2FData.ProcessState.iState++;
		H2F_SendCMDCtrl();
	}
#endif
	else if((NULL != Ql_strstr(pTriggerStr, "CONNECT")) &&
			PROCESS_STATE_TYPE_QHTTPURL == s_H2FData.ProcessState.iState)
	{
		OUT_DEBUG("\r\n[max] : QHTTPURL CONNECT...");
		s_H2FData.ProcessState.iState++;
		H2F_SendCMDCtrl();
	}
	else if((NULL != Ql_strstr(pTriggerStr, "OK")) &&
			PROCESS_STATE_TYPE_INPUTURL == s_H2FData.ProcessState.iState)
	{
		OUT_DEBUG("\r\n[max] : INPUTURL OK!");
		s_H2FData.ProcessState.iState++;
		H2F_SendCMDCtrl();
	}
	else if((NULL != Ql_strstr(pTriggerStr, "OK")) &&
			PROCESS_STATE_TYPE_QHTTGET == s_H2FData.ProcessState.iState)
	{
		OUT_DEBUG("\r\n[max] : QHTTPGET OK!");
		s_H2FData.ProcessState.iState++;
		H2F_SendCMDCtrl();
	}
	else if((NULL != Ql_strstr(pTriggerStr, "CONNECT")) &&
			PROCESS_STATE_TYPE_QHTTPREAD == s_H2FData.ProcessState.iState)
	{
		OUT_DEBUG("\r\n[max] : QHTTPREAD CONNECT...\r\n");
		H2F_WriteData_Init();
		s_bDataMode = TRUE;
	}
	else if((NULL != Ql_strstr(pTriggerStr, "OK")) &&
			PROCESS_STATE_TYPE_QHTTPREAD == s_H2FData.ProcessState.iState)
	{
		OUT_DEBUG("\r\n[max] : QHTTPREAD OK!\r\n");
		H2F_WriteData_Finish();
	}
	else if((NULL != Ql_strstr(pTriggerStr, "OK")) &&
			PROCESS_STATE_TYPE_QIDEACT == s_H2FData.ProcessState.iState)
	{
		OUT_DEBUG("\r\n[max] : QIDEACT OK!");
		if(TRUE == s_bDEACTReboot)
		{
			OUT_DEBUG("\r\n[max] : State time out! Reboot....");
			Ql_Sleep(2000);
			Ql_Reset(0);
		}
		else
		{
			s_H2FData.ProcessState.iState++;
			H2F_SendCMDCtrl();
		}
	}
	else
	{
		bFlags = FALSE;
	}

	if(TRUE == bFlags)
	{
		s_H2FData.ProcessState.uiStateBeginTime = Ql_GetRelativeTime();
	}
	return ;
}

void IMEI_read(void)
{
	s32 ret;
	u32 readedlen1;
	u32 filehandle;
	ret = Ql_FileOpenEx((u8*)"IMEI_No.txt",QL_FS_CREATE);
	if(ret >= QL_RET_OK)
	{
		filehandle = ret;
		Ql_memset((ascii *)ptr_imei,0,sizeof(ptr_imei));
		ret = Ql_FileSeek(filehandle,0, QL_FS_FILE_BEGIN);  
		ret = Ql_FileRead(filehandle, (u8 *)ptr_imei,15, &readedlen1);
		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
	}
}
void fota_Set_url()
{
	char XX_URL[100]="http://www.myfleetview.com:8080/sample/";
	char BIN[]=".bin";
	IMEI_read();						
	Ql_StopTimer(&onesecdummy);
	//onesecdummy.timerId =0;
	Ql_memset(FOTA_IMEI, 0, sizeof(FOTA_IMEI));
	Ql_strcat(FOTA_IMEI,XX_URL);
	Ql_strcat(FOTA_IMEI,ptr_imei);
	Ql_strcat(FOTA_IMEI,BIN);


	Ql_memset((char *)URL_FOTA,'\0',sizeof(URL_FOTA));
	Ql_strcat(URL_FOTA,FOTA_IMEI);

	OUT_DEBUG("\r\nSMS FOTA URL = %s\r\n",URL_FOTA);

	fota_function();
}

void fota_function(void)
{
	OUT_DEBUG("[max] : Test begin! 3 seconds later...\r\n");
	Ql_Sleep(3000);
	doing_fota=1;
	//in_cam_rou=0;
	Fota_http_Flag=0;
	s_H2FData.bBeginFlag = TRUE;
	s_H2FData.ProcessState.iState = PROCESS_STATE_TYPE_ATE0;
	H2F_SendCMDCtrl();
}

void fota_uart(void)
{

	if(TRUE == s_H2FData.bBeginFlag)
	{
		PortData_Event* pPortEvt = (PortData_Event*)&flSignalBuffer.eventData.modemdata_evt;
		////R_//OUT_DEBUG("\r\n%s\r\n", (char*)pPortEvt->data);

		if (DATA_AT == pPortEvt->type)
		{
			H2F_StateMachine((char*)pPortEvt->data);
		}
		else if (DATA_TCP_T == pPortEvt->type)
		{
			if(TRUE == s_bDataMode)
			{
				H2F_WriteData_Write(pPortEvt->len, pPortEvt->data);
			}
			else
			{
				//R_//OUT_DEBUG("[max] : Ileagle data!!!!!\r\n");
			}
		}
	}

}
/*****************************************************************
 *
 * According to current state, H2F_SendCMDCtrl will send proper
 * command to core.
 *
 *******************************************************************/

void H2F_SendCMDCtrl(void)
{
	int iRet = -1;

	switch(s_H2FData.ProcessState.iState)
	{
	case PROCESS_STATE_TYPE_ATE0:
	{
		Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
		Ql_sprintf(s_H2FData.Buffer, "ATE0\n");
		break;
	}
	case PROCESS_STATE_TYPE_CHECK_CGATT:
	{
		Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
		Ql_sprintf(s_H2FData.Buffer, "AT+CGATT?\n");
		break;
	}
	case PROCESS_STATE_TYPE_QIFGCNT:
	{
		Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
		Ql_sprintf(s_H2FData.Buffer, "AT+QIFGCNT=1\n");
		break;
	}
	case PROCESS_STATE_TYPE_QICSGP:
	{
		Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
		Ql_sprintf(s_H2FData.Buffer, "AT+QICSGP=1,\"%s\"\n",APN_NAME);
		break;
	}
#if 0
	case PROCESS_STATE_TYPE_QIREGAPP:
	{
		Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
		Ql_sprintf(s_H2FData.Buffer, "AT+QIREGAPP\n");
		break;
	}
	case PROCESS_STATE_TYPE_QIACT:
	{
		Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
		Ql_sprintf(s_H2FData.Buffer, "AT+QIACT\n");
		break;
	}
#endif
	case PROCESS_STATE_TYPE_QHTTPURL:
	{
		Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
		Ql_sprintf(s_H2FData.Buffer, "AT+QHTTPURL=%d,%d\n", Ql_strlen(URL_FOTA), 120);
		break;
	}
	case PROCESS_STATE_TYPE_INPUTURL:
	{
		Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
		Ql_sprintf(s_H2FData.Buffer, "%s", URL_FOTA);
		break;
	}
	case PROCESS_STATE_TYPE_QHTTGET:
	{
		Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
		Ql_sprintf(s_H2FData.Buffer, "AT+QHTTPGET=%d\n", 120);
		break;
	}
	case PROCESS_STATE_TYPE_QHTTPREAD:
	{
		Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
		Ql_sprintf(s_H2FData.Buffer, "AT+QHTTPREAD=%d\n", 120);
		break;
	}
	case PROCESS_STATE_TYPE_QIDEACT:
	{
		Ql_memset(s_H2FData.Buffer, 0, sizeof(s_H2FData.Buffer));
		Ql_sprintf(s_H2FData.Buffer, "AT+QIDEACT\n");
		break;
	}
	case PROCESS_STATE_TYPE_UPDATEFLAGS:
	{
		iRet = Ql_Fota_Update();
		if(QL_RET_OK != iRet)
		{
			OUT_DEBUG("\r\n[max] : Ql_Fota_Update FAILED!\r\n");
			OUT_DEBUG("\r\n[max] : Reboot 3 seconds later ...\r\n");
			Ql_Sleep(3000);
			Ql_Reset(0);
		}
		else
		{
			doing_fota=0;
			//If update OK, module will reboot automaticly
		}
	}
	default:
	{
		OUT_DEBUG("\r\n[max] : ++++++++++++++++ INVALID state, Fetal ERROR!!!\r\n");
		break;
	}
	}

	iRet = Ql_SendToModem(ql_md_port1, (u8*)s_H2FData.Buffer, Ql_strlen(s_H2FData.Buffer));
	if(iRet < 0)
	{
		OUT_DEBUG("\r\n[max] : ERROR! Failed send data to modem port![state=%d]\r\n", s_H2FData.ProcessState.iState);
		OUT_DEBUG("\r\n[max] : Reboot 3 seconds later ...\r\n");
		Ql_Sleep(3000);
		Ql_Reset(0);
		return ;
	}
	else
	{
		char sz[64];
		char *p = NULL;
		int off = 0;
		Ql_memset(sz, 0, sizeof(sz));
		Ql_memcpy(sz, s_H2FData.Buffer, sizeof(sz));
		p = Ql_strstr(sz, "\r\n");
		if(NULL != p)
		{
			Ql_strcpy(p, "\\r\\n");
		}
		OUT_DEBUG("\r\n[max] : Send CMD ->[%s]\r\n", sz);
	}
}

