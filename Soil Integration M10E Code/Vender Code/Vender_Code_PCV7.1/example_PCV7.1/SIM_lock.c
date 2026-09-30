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

#include "GSM_GPRS.h"
#include "SEND_DATA.h"
#include "TCP_IP.h"
#include "MRW.h"  
#include "para_read.h" 
#include "GPRMC.h" 
#include "sms_handle.h" 
#include "Ql_filesystem.h"
#include "fota.h"
#include "SIM_lock.h"
/*-------------------------------------------------------------------------*/
/*								Globals									   */
/*-------------------------------------------------------------------------*/
#define OUT_DEBUG(x,...)  \
		Ql_memset((x),0,100);  \
		Ql_sprintf((x),__VA_ARGS__);   \
		Ql_SendToUart(ql_uart_port2,(u8 *)(x),Ql_strlen(x));


extern char textBuf[];
extern char SIM_Lock[10];
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
extern char GPRS_APN[];				// For APN
void SIM_LockRoutines(u8 cmd_number)
{
	u8 dumcmdnum=0;
	OUT_DEBUG(textBuf,"In simlockunlock function\r\n");
	if((Ql_strstr(GPRS_APN,("www")) || (Ql_strstr(GPRS_APN,("rcomnet"))) || (Ql_strstr(GPRS_APN,("bsnlnet"))) || (Ql_strstr(GPRS_APN,("aircelgprs.po")))))
	{
		dumcmdnum=cmd_number;
		OUT_DEBUG(textBuf,"Command For %s SIM=%d\r\n",GPRS_APN,dumcmdnum);
	}
	else if((Ql_strstr(GPRS_APN,("airtelgprs.com")) ||  (Ql_strstr(GPRS_APN,("internet"))) || (Ql_strstr(GPRS_APN,("TATA.DOCOMO.INTERNET")))))
	{
		dumcmdnum=cmd_number+6;
		OUT_DEBUG(textBuf,"Command For %s SIM=%d\r\n",GPRS_APN,dumcmdnum);
	}
	switch(dumcmdnum)
	{
	case 1:
		Lock_sim=1;
		ATcommandSIM("AT+CPIN?\n");
		break;

	case 2:
		ATcommandSIM("AT+CLCK=\"SC\",2\r\n");
		//          WriteSimPin(1);

		break;

	case 3:
		ATcommandSIM("AT+CLCK=\"SC\",1,\"0000\"\r\n");                //Vodafone
		//	    	OUT_DEBUG(textBuf,"going to lock sim\r\n");
		//          ATcommand("AT+CLCK=\"SC\",1,\"1234\"\r\n");                  //Airtel
		//          WriteSimPin(1);
		CLCK_cmd=1;

		break;

	case 4:
		ATcommandSIM("AT+CPWD=\"SC\",\"0000\",\"6477\"\n");              //Vodafone
		//            ATcommand("AT+CPWD=\"SC\",\"1234\",\"6477\"\n");              //Airtel
		//          WriteSimPin(2);
		CPWD_cmd=1;
		break;

	case 5:
		ATcommandSIM("AT+CPIN=6477\n");

		CPIN_cmd=1;
		break;

	case 6:
		ATcommandSIM("AT+CPIN=0000\n");             //Vodafone
		//          ATcommand("AT+CPIN=1234\n");              //Airtel
		CPIN_cmd11=1;
		break;
	case 7:
		Lock_sim=1;
		OUT_DEBUG(textBuf,"AT+CPIN?-->\r\n");
		ATcommandSIM("AT+CPIN?\n");

		break;

	case 8:
		OUT_DEBUG(textBuf,"AT+CLCK=\"SC\",2\r\n");
		ATcommandSIM("AT+CLCK=\"SC\",2\r\n");

		break;

	case 9:
		OUT_DEBUG(textBuf,"AT+CLCK=\"SC\",1,\"1234\"\r\n");  //Airtel
		//OUT_DEBUG(textBuf,"AT+CLCK=\"SC\",1,\"0000\"\r\n");	   //Vodafone
		ATcommandSIM("AT+CLCK=\"SC\",1,\"1234\"\r\n");                  //Airtel
		//ATcommand("AT+CLCK=\"SC\",1,\"0000\"\r\n");                  //Vodafone
		CLCK_cmd=1;

		break;

	case 10:
		ATcommandSIM("AT+CPWD=\"SC\",\"1234\",\"6477\"\n");              //Airtel
		//ATcommand("AT+CPWD=\"SC\",\"0000\",\"6477\"\n");              //Vodafone
		CPWD_cmd=1;
		break;

	case 11:
		ATcommandSIM("AT+CPIN=6477\n");
		CPIN_cmd=1;
		break;

	case 12:
		OUT_DEBUG(textBuf,"AT+CPIN=1234-->\r\n");				//Airtel
		//OUT_DEBUG(textBuf,"AT+CPIN=0000-->\r\n");				//Vodafone

		ATcommandSIM("AT+CPIN=1234\n");              //Airtel
		//ATcommand("AT+CPIN=0000\n");              //Vodafone

		CPIN_cmd11=1;

		break;
	default:
		break;

	}
}

void ATcommandSIM(char * modem_str)
{
	OUT_DEBUG(textBuf,"IN ATcommand-->\r\n");
	if(!(Ql_strcmp((char *)modem_str,"AT+CPIN?\n")))
	{
		Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
		//         OUT_D1EBUG(textBuf,"AT+CPIN?\r\n");

	}

	if(!(Ql_strcmp((char *)modem_str,"AT+CPIN=6477\n")))
	{
		Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
	}

	if((Ql_strstr(GPRS_APN,("www")) || (Ql_strstr(GPRS_APN,("rcomnet"))) || (Ql_strstr(GPRS_APN,("bsnlnet"))) || (Ql_strstr(GPRS_APN,("aircelgprs.po")))))
	{
		if(!(Ql_strcmp((char *)modem_str,"AT+CPIN=0000\n")))     //for Vodafone
		{
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
		}

		if(!(Ql_strcmp((char *)modem_str,"AT+CLCK=\"SC\",2\r\n")))
		{
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
			OUT_DEBUG(textBuf,"AT+CLCK=check status fired\r\n");
		}

		if(!(Ql_strcmp((char *)modem_str,"AT+CLCK=\"SC\",1,\"0000\"\r\n")))   //for Vodafone
		{
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
		}

		if(!(Ql_strcmp((char *)modem_str,"AT+CPWD=\"SC\",\"0000\",\"6477\"\n")))	// for Vodafone
		{
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
		}
	}

	else if((Ql_strstr(GPRS_APN,("airtelgprs.com")) ||  (Ql_strstr(GPRS_APN,("internet"))) || (Ql_strstr(GPRS_APN,("TATA.DOCOMO.INTERNET")))))
	{
		if(!(Ql_strcmp((char *)modem_str,"AT+CPIN=1234\n")))     //for airtel
		{
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
		}

		if(!(Ql_strcmp((char *)modem_str,"AT+CLCK=\"SC\",2\r\n")))
		{
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
			OUT_DEBUG(textBuf,"AT+CLCK=check status fired\r\n");
		}

		if(!(Ql_strcmp((char *)modem_str,"AT+CLCK=\"SC\",1,\"1234\"\r\n")))  // for airtel
		{
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
		}

		if(!(Ql_strcmp((char *)modem_str,"AT+CPWD=\"SC\",\"1234\",\"6477\"\n")))		// for airtel
		{
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
		}
	}

	Ql_memset((char *)modem_str,0,sizeof(modem_str));
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
		OUT_DEBUG(textBuf,"Ql_SIMLOCKPINVALUEWrite()=%d: writeedlen=%d\r\n",ret,writeedlen);

		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_DEBUG(textBuf,"Error in Ql_SIMLOCKPINVALUE write.....\r\n");

	}

}



u8 ReadSimPin()
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
		OUT_DEBUG(textBuf,"Ql_SIMLOCKPINVALUERead()=%d: readedlen=%d\r\n",ret,readedlen);
		OUT_DEBUG(textBuf,"Ql_SIMLOCKPINVALUERead()=%s\r\n",ptr);
		//val=Ql_atoi(ptr);
		OUT_DEBUG(textBuf,"Ql_SIMLOCKPINVALUE()=%d\r\n",ptr);
		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_DEBUG(textBuf,"Error in Ql_SIMLOCKPINVALUE Read.....\r\n");

	}

	return val;



}
