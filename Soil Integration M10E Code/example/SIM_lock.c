#include<stdio.h>
#include<string.h>
#include "ql_trace.h"
#include "ql_timer.h"
#include "ql_type.h"
#include "ql_stdlib.h"
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_audio.h"
#include "ql_pin.h"
#include "Ql_multitask.h"
#include "Ql_tcpip.h"
#include "Ql_error.h"
#include "Fun.h"
#include "GPS.h"
#include "GSM_GPRS.h"
#include "SEND_DATA.h"
#include "ql_fcm.h"
#include "TCP_IP.h"
#include "MRW.h"  
#include "para_read.h" 
#include "GPRMC.h" 
#include "sms_handle.h" 
#include "Ql_filesystem.h"
#include "fota.h"
#include "SIM_lock.h"

/************************************************************************************************************
 * Debug
 *************************************************************************************************************/
#define DEBUG_ENABLE 1
#if DEBUG_ENABLE > 0
#define OUT_D1EBUG(x,...)  \
		Ql_memset((x),0,100);  \
		Ql_sprintf((x),__VA_ARGS__);   \
		Ql_SendToUart(ql_uart_port2,(u8 *)(x),Ql_strlen(x));
#else
#define OUT_D1EBUG(x,...)
#endif

/*************************************************************************************************************/

/*-------------------------------------------------------------------------*/
/*								Globals									   */
/*-------------------------------------------------------------------------*/

extern char textBuf[1000];
bool Lock_sim=0;
bool CLCK_cmd=0;
bool CPWD_cmd=0;
bool CPIN_cmd=0;
bool CPIN_cmd11=0;
extern char pfile2[15];
extern char STI[8];
extern char TXI[8];
extern char CV[15];
extern unsigned char UID[8];

void SIM_LockRoutines(u8 cmd_number)
{
	switch(cmd_number)
	{
	case 1:
		Lock_sim=1;
		ATcommand("AT+CPIN?\n");
		break;

	case 2:
		ATcommand("AT+CLCK=\"SC\",2\r\n");
		//          WriteSimPin(1);

		break;

	case 3:
		ATcommand("AT+CLCK=\"SC\",1,\"0000\"\r\n");                //Vodafone
		//ATcommand("AT+CLCK=\"SC\",1,\"1234\"\r\n");                  //Airtel
		//          WriteSimPin(1);
		CLCK_cmd=1;

		break;

	case 4:
		 ATcommand("AT+CPWD=\"SC\",\"0000\",\"6477\"\n");              //Vodafone
		//ATcommand("AT+CPWD=\"SC\",\"1234\",\"6477\"\n");              //Airtel
		//          WriteSimPin(2);
		CPWD_cmd=1;
		break;

	case 5:
		ATcommand("AT+CPIN=6477\n");

		CPIN_cmd=1;
		break;

	case 6:
		ATcommand("AT+CPIN=0000\n");             //Vodafone
		//ATcommand("AT+CPIN=1234\n");              //Airtel
		CPIN_cmd11=1;
		break;


	default:
		break;

	}
}



void WriteSimPin(u8 val)
{
	char *ptr;
	u32 filehandle;
	s32 ret;
	u32 writeedlen;
	u16 CanCount;


	//     ptr[0]='\0';
	//     ptr[1]='\0';

	//     ptr=ix_Itoa();
	ret = Ql_FileOpenEx((u8*)pfile2,QL_FS_CREATE);
	if(ret >= QL_RET_OK)
	{


		filehandle = ret;
		ptr=ix_Itoa(val);
		/////////////////////////////////////////////////////////////////////////
		//--------------------SIM LOCK PIN VALUE-------------------------------------

		//			CanCount=1+Ql_strlen((char *)UID)+Ql_strlen((char *)STI)+Ql_strlen((char *)TXI)+Ql_strlen((char *)CV)+1;
		ret = Ql_FileSeek(filehandle,50,QL_FS_FILE_BEGIN);
		ret = Ql_FileWrite(filehandle,(u8 *)ptr,1,&writeedlen);
		OUT_D1EBUG(textBuf,"Ql_SIMLOCKPINVALUEWrite()=%d: writeedlen=%d\r\n",ret,writeedlen);

		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"Error in Ql_SIMLOCKPINVALUE write.....\r\n");

	}

}



u8 ReadSimPin(void)
{
	char ptr[2];
	u32 filehandle;
	s32 ret;
	u32 readedlen;
	u8 val;
	u16 CanCount;
	ptr[0]='\0';
	ptr[1]='\0';
	//     ptr=Ql_atoi(val);

	ret = Ql_FileOpenEx((u8*)pfile2,QL_FS_CREATE);
	if(ret >= QL_RET_OK)
	{


		filehandle = ret;

		/////////////////////////////////////////////////////////////////////////
		//--------------------SIM LOCK PIN VALUE-------------------------------------

		//			CanCount=1+Ql_strlen((char *)UID)+Ql_strlen((char *)STI)+Ql_strlen((char *)TXI)+Ql_strlen((char *)CV)+1;
		ret = Ql_FileSeek(filehandle,50,QL_FS_FILE_BEGIN);
		ret = Ql_FileRead(filehandle,(u8 *)ptr,1,&readedlen);
		OUT_D1EBUG(textBuf,"Ql_SIMLOCKPINVALUERead()=%d: readedlen=%d\r\n",ret,readedlen);
		OUT_D1EBUG(textBuf,"Ql_SIMLOCKPINVALUERead()=%s\r\n",ptr);
		val=Ql_atoi(ptr);
		OUT_D1EBUG(textBuf,"Ql_SIMLOCKPINVALUE()=%d\r\n",ptr);
		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"Error in Ql_SIMLOCKPINVALUE Read.....\r\n");

	}

	return val;
}
