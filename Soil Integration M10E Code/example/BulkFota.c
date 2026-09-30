//#ifdef __EXAMPLE_FOTA_HTTP__
#include<stdio.h>
#include<string.h>
#include<math.h>
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
#include "fota.h"
#include "Fun.h"
#include "GPS.h"
#include "GSM_GPRS.h"
#include "SEND_DATA.h"
#include "TCP_IP.h"
#include "MRW.h"
#include "para_read.h"
#include "camera.h"
#include "ftp2.h"
#include "ftp.h"
#include "camera2.h"
#include "JRM.h"
#include "JRM_RD.h"
#include "canparareadwrt.h"
#include "SIM_lock.h"
#include "ACCSMTP.h"
#include "exce_camera_C.h"
#include "exce_camera_R.h"
#include "Cell_ID.h"
#include "Battery.h"
//#include "GSM_ON_OFF.h"
#include "Stamps.h"
#include "BulkFota.h"

extern char textBuf[1000];
extern u8 Fota_http_Flag;
char ptr_imei[50];

/************************************************************************************************************
 * Debug
 *************************************************************************************************************/
#define DEBUG_ENABLE 1
#if DEBUG_ENABLE > 0
#define OUT_DEBUG(x,...)  \
		Ql_memset((x),0,100);  \
		Ql_sprintf((x),__VA_ARGS__);   \
		Ql_SendToUart(ql_uart_port2,(u8 *)(x),Ql_strlen(x));
#else
#define OUT_DEBUG(x,...)
#endif

/*************************************************************************************************************/

#define APP_BIN_URL "http://www.myfleetview.com:8080/sample/avl_update.txt"
//#define REM_CMD_URL "http://www.myfleetview.com:8080/sample/avl_update.txt"


char temp_f_url[]="http://www.myfleetview.com:8080/sample/";
extern char URL_FOTA[150];
char bin_url[5]=".bin";
char txt_url[5]=".txt";
char REM_CMD_URL[150];
extern char UID[];
extern u8 Fota_http_Flag;
char bulk_fota_buffer[100];
u16  bulk_fota_cmd_idx=0;
int in_Bulk_fota_rou=0;
int bulk_fota_cmd_type=0;
bool bulk_fota_data=0;
bool sel_url=0;
bool Bulk_DinProg=0;
char HTTP_r_buffer[50];
char Update_filename[30];
bool bulk_fota_bDoNexAT=TRUE;
u8 URLrep=0;
extern char Code_version[];
extern u8 Fota_http_Flag;
extern bool camcaptureflag;
extern int camindicatorflag;
extern bool stop_acc_capt;
extern char APN_NAME[];
extern QlTimer GC_ftp_stuck;


void Bulk_fota_connect(void)
{
	in_Bulk_fota_rou=1;
	bulk_fota_data=1;

	OUT_DEBUG(textBuf,"\r\nInside ATCommand,,,,,bulk_fota_cmd_idx=%d\r\n",bulk_fota_cmd_idx);
	switch(bulk_fota_cmd_idx)
	{
	case 1:
		URLrep=0;
		Ql_sprintf((char *)bulk_fota_buffer,"ATE0\n");
		GC_ftp_stuck.timeoutPeriod = Ql_SecondToTicks(240);
		Ql_StopTimer(&GC_ftp_stuck);
		Ql_StartTimer(&GC_ftp_stuck);    ///timer for ftp stuck
		OUT_DEBUG(textBuf,"ATE0");
		OUT_DEBUG(textBuf,(char *)bulk_fota_buffer);
		Ql_SendToModem(ql_md_port1, (u8*)bulk_fota_buffer, Ql_strlen(bulk_fota_buffer));


		break;

	case 2:
		Ql_sprintf((char *)bulk_fota_buffer,"AT+CGATT?\n");
		bulk_fota_cmd_type=2;
		OUT_DEBUG(textBuf,"AT+CGATT?");
		bulk_fota_bDoNexAT = FALSE;
		OUT_DEBUG(textBuf,(char *)bulk_fota_buffer);
		Ql_SendToModem(ql_md_port1, (u8*)bulk_fota_buffer, Ql_strlen(bulk_fota_buffer));

		break;

	case 3:

		Ql_sprintf((char *)bulk_fota_buffer, "AT+QIFGCNT=1\n");
		//OUT_DEBUG(textBuf,"AT+QIFGCNT=1\n");
		bulk_fota_cmd_type=3;
		OUT_DEBUG(textBuf,(char *)bulk_fota_buffer);
		Ql_SendToModem(ql_md_port1, (u8*)bulk_fota_buffer, Ql_strlen(bulk_fota_buffer));

		break;

	case 4:

		Ql_sprintf((char *)bulk_fota_buffer, "AT+QICSGP=1,\"%s\"\n",APN_NAME);
		bulk_fota_cmd_type=4;
		//OUT_DEBUG(textBuf,"AT+QICSGP=1,\"WWW\"\n");
		OUT_DEBUG(textBuf,(char *)bulk_fota_buffer);
		Ql_SendToModem(ql_md_port1, (u8*)bulk_fota_buffer, Ql_strlen(bulk_fota_buffer));

		break;

	case 5:
		if(sel_url == 0)
		{
			OUT_DEBUG(textBuf,"in bulk DOTA URL_CONNECT FTP....\r\n");
			Ql_sprintf((char *)bulk_fota_buffer, "AT+QHTTPURL=%d,%d\n", Ql_strlen(APP_BIN_URL), 120);
		}
		else if(sel_url == 1)
		{
			OUT_DEBUG(textBuf,"in command data URL_CONNECT FTP....\r\n");
			Ql_sprintf((char *)bulk_fota_buffer, "AT+QHTTPURL=%d,%d\n", Ql_strlen(REM_CMD_URL), 120);
		}
		bulk_fota_cmd_type = 5;
		bulk_fota_bDoNexAT = FALSE;
		//OUT_DEBUG(textBuf,"AT+QHTTPURL=%d,%d\n", Ql_strlen(APP_BIN_URL), 120);
		OUT_DEBUG(textBuf,(char *)bulk_fota_buffer);
		Ql_SendToModem(ql_md_port1, (u8*)bulk_fota_buffer, Ql_strlen(bulk_fota_buffer));

		break;

	case 6:
		Ql_sprintf((char *)bulk_fota_buffer, "AT+QHTTPGET=%d\n", 120);
		bulk_fota_cmd_type = 6;
		//OUT_DEBUG(textBuf,"AT+QHTTPGET=%d\n", 120);
		OUT_DEBUG(textBuf,(char *)bulk_fota_buffer);
		Ql_SendToModem(ql_md_port1, (u8*)bulk_fota_buffer, Ql_strlen(bulk_fota_buffer));

		break;

	case 7:
		Ql_sprintf((char *)bulk_fota_buffer, "AT+QHTTPREAD=%d\n", 120);
		bulk_fota_cmd_type = 7;
		bulk_fota_bDoNexAT = FALSE;
		//OUT_DEBUG(textBuf,"AT+QHTTPREAD=%d\n", 120);
		OUT_DEBUG(textBuf,(char *)bulk_fota_buffer);
		Ql_SendToModem(ql_md_port1, (u8*)bulk_fota_buffer, Ql_strlen(bulk_fota_buffer));

		break;

	case 8:

		Ql_sprintf((char *)bulk_fota_buffer, "AT+QIDEACT\n");
		bulk_fota_cmd_type = 8;
		//bulk_fota_bDoNexAT=TRUE;
		//in_Bulk_fota_rou=0;
		//OUT_DEBUG(textBuf,"AT+QIDEACT\n");
		OUT_DEBUG(textBuf,(char *)bulk_fota_buffer);
		Ql_SendToModem(ql_md_port1, (u8*)bulk_fota_buffer, Ql_strlen(bulk_fota_buffer));
		break;

	default:
		bulk_fota_cmd_type = 9;
		Bulk_DinProg=0;
		OUT_DEBUG(textBuf,"bulk_dota mail sent...\r\n");
		Ql_StopTimer(&GC_ftp_stuck);
		bulk_fota_proccess();
		bulk_fota_bDoNexAT =TRUE;
		in_Bulk_fota_rou=0;
		//	in_Bulk_fota_rou=1;
		bulk_fota_data=0;

		break;
	}
}


void Bulk_Fota_modemdata(char *bulk_fota_modem_readbuffer)
{
	// TODO: receive and hanle data from CORE through virtual modem port
	//      PortData_Event* pPortEvt = (PortData_Event*)&g_event.eventData.modemdata_evt;
	//          OUT_DEBUG(textBuf,"Modem Data at vPort [%d]: %s\r\n", pPortEvt->port, bulk_fota_modem_readbuffer);
	//     OUT_DEBUG(textBuf,"Modem Data at vPort [%d]: %s \r\n", pPortEvt->port, bulk_fota_modem_readbuffer);
	// OUT_DEBUG(textBuf,(char *)bulk_fota_modem_readbuffer);
	//   OUT_DEBUG(textBuf,"in IF...bulk_fota_data==1.\r\n");
	if(bulk_fota_data==1)
	{
		//+CME ERROR: 3822

		OUT_DEBUG(textBuf,"in IF...bulk_fota_data==1.\r\n");
		OUT_DEBUG(textBuf,"\r\n*****bulk_fota_modem_readbuffer = %s,bulk_fota_cmd_type=%d*****\n",(char *)bulk_fota_modem_readbuffer,bulk_fota_cmd_type);
		//OUT_DEBUG(textBuf,(char *)bulk_fota_modem_readbuffer);
		//stop_acc_capt=1;

		if((bulk_fota_cmd_type == 6)  && (Ql_strstr((char*)bulk_fota_modem_readbuffer,"+CME ERROR: 3822") != NULL) && (URLrep < 1))
		{
			sel_url=1;
			URLrep ++;
			OUT_DEBUG(textBuf," !!!!!!!!!!!!  File not available on server. !!!!!!!!!!!!!=%d\r\n",URLrep);
			OUT_DEBUG(textBuf,"in URL_CONNECT....\r\n");
			Ql_memset((char *)REM_CMD_URL,'\0',sizeof(REM_CMD_URL));
			Ql_strcat(REM_CMD_URL,temp_f_url);
			Ql_strcat(REM_CMD_URL,UID);
			Ql_strcat(REM_CMD_URL,txt_url);
			OUT_DEBUG(textBuf,(char *)REM_CMD_URL);
			//	Ql_SendToModem(ql_md_port1, (u8*)APP_BIN_URL, Ql_strlen(APP_BIN_URL));
			bulk_fota_cmd_idx=5;
			Bulk_fota_connect();
		}
		else if((bulk_fota_cmd_type == 6)  && (Ql_strstr((char*)bulk_fota_modem_readbuffer,"+CME ERROR: 3822") != NULL) && (URLrep >= 1))

		{
			OUT_DEBUG(textBuf," !!!!!!!!!!!! In Else  File not available on server. !!!!!!!!!!!!!=%d\r\n",URLrep);
			bulk_fota_cmd_idx= 8;
			Bulk_fota_connect();
		}

		if (bulk_fota_cmd_type==8)
		{
			bulk_fota_cmd_type++;
			Ql_memset((char *)HTTP_r_buffer,'\0',sizeof(HTTP_r_buffer));
			Ql_strcpy((char *)HTTP_r_buffer,(char *)bulk_fota_modem_readbuffer);
			OUT_DEBUG(textBuf,"\r\n*****HTTP_r_buffer = %s*****\n",(char *)HTTP_r_buffer);
		}

		if (Ql_strstr((char *)bulk_fota_modem_readbuffer, "Call Ready") != NULL)
		{
			// Wait 2s for the stable signal quality
			//cm_timer.timeoutPeriod = Ql_SecondToTicks(2);
			//Ql_StartTimer(&cm_timer);
			//  bulk_fota_cmd_idx = 1;
			//  SendAtCmd();
		}
		else if ((bulk_fota_cmd_type == 2  && Ql_strstr((char*)bulk_fota_modem_readbuffer,"CGATT") != NULL)
				|| (bulk_fota_cmd_type == 5  && Ql_strstr((char*)bulk_fota_modem_readbuffer, "CONNECT")   != NULL)
				|| (bulk_fota_cmd_type == 7 && Ql_strstr((char*)bulk_fota_modem_readbuffer, "CONNECT") != NULL)
		)
		{
			OUT_DEBUG(textBuf,"in IF...bulk_fota_cmd_type.\r\n");
			OUT_DEBUG(textBuf,(char *)bulk_fota_modem_readbuffer);
			if((bulk_fota_cmd_type == 5  && Ql_strstr((char*)bulk_fota_modem_readbuffer, "CONNECT")   != NULL))
			{
				if(sel_url == 0)
				{
					OUT_DEBUG(textBuf,"in bulk DOTA URL_CONNECT....\r\n");
					OUT_DEBUG(textBuf,(char *)APP_BIN_URL);
					Ql_SendToModem(ql_md_port1, (u8*)APP_BIN_URL, Ql_strlen(APP_BIN_URL));
				}
				else if(sel_url == 1)
				{
					OUT_DEBUG(textBuf,"in remote command URL_CONNECT....\r\n");
					OUT_DEBUG(textBuf,(char *)REM_CMD_URL);
					Ql_SendToModem(ql_md_port1, (u8*)REM_CMD_URL, Ql_strlen(REM_CMD_URL));
					sel_url=0;
				}
				bulk_fota_bDoNexAT = TRUE;
			}
			else
			{
				bulk_fota_cmd_idx++;
				bulk_fota_bDoNexAT = TRUE;

				Bulk_fota_connect();
			}
		}
		else if ((  Ql_strstr((char*)bulk_fota_modem_readbuffer, "\r\nOK") != NULL
				|| Ql_strstr((char*)bulk_fota_modem_readbuffer, "OK\r\n") != NULL
				|| Ql_strstr((char*)bulk_fota_modem_readbuffer, "OK") != NULL
				|| Ql_strstr((char*)bulk_fota_modem_readbuffer, "ERROR") != NULL)
				&& bulk_fota_bDoNexAT != FALSE)
		{
			OUT_DEBUG(textBuf,"in ELSE IF....\r\n");
			OUT_DEBUG(textBuf,(char *)bulk_fota_modem_readbuffer);
			bulk_fota_cmd_idx++;
			Bulk_fota_connect();
		}
	}
	return;
}

void bulk_fota_proccess(void)
{
	int ix,jx,kx,lx,mx,nx,ox,px;
	char Prev_code_ver[35];
	char New_code_ver[35];
	char Update_buff[35];

	OUT_DEBUG(textBuf,"In bulk FOTA process.buff= %s\r\n",HTTP_r_buffer);

	if((Ql_strstr((char *)HTTP_r_buffer,"#UPDATE") != NULL))
	{
		for(ix=0;HTTP_r_buffer[ix]!=':';ix++)
		{
			Update_buff[ix]=HTTP_r_buffer[ix];
		}
		Update_buff[ix]='\0';
		ix++;


		for(ix,jx=0;HTTP_r_buffer[ix]!=',';ix++,jx++)
		{
			Prev_code_ver[jx]=HTTP_r_buffer[ix];
			//	OUT_DEBUG(textBuf,"\r\nPrev_code_ver[%d]= %c\n",jx,Prev_code_ver[jx]);
			//	OUT_DEBUG(textBuf,"\r\nHTTP_r_buffer[%d]= %c\n",ix,HTTP_r_buffer[ix]);
		}
		Prev_code_ver[jx]='\0';
		ix++;

		for(ix,kx=0;HTTP_r_buffer[ix]!='\0';ix++,kx++)
		{
			New_code_ver[kx]=HTTP_r_buffer[ix];
		}
		Ql_memset((char *)Update_filename,'\0',sizeof(Update_filename));
		Ql_strcat((char *)Update_filename,(char *)New_code_ver);
		Ql_strcat((char *)Update_filename,(char *)bin_url);

		New_code_ver[kx]='\0';
		//ix++;
		Ql_strcat((char *)URL_FOTA,(char *)bin_url);

		OUT_DEBUG(textBuf,"\r\nUpdate_buff= %s\n",(char *)Update_buff);
		OUT_DEBUG(textBuf,"\r\nPrev_code_ver= %s\n",(char *)Prev_code_ver);
		OUT_DEBUG(textBuf,"\r\nNew_code_ver= %s\n",(char *)New_code_ver);

		if((Ql_strstr((char *)Update_buff,"#UPDATE") != NULL)&&(Ql_strstr((char *)Prev_code_ver,(char *)Code_version) != NULL))
		{
			OUT_DEBUG(textBuf,"\r\nDO DOTA>>>>>>>>>>>>>>>.\n");
			Ql_memset((char *)URL_FOTA,'\0',sizeof(URL_FOTA));
			Ql_strcat((char *)URL_FOTA,(char *)temp_f_url);
			//Ql_strncat((char *)URL_FOTA,(char *)New_code_ver,(Ql_strlen(New_code_ver)-1));
			Ql_strncat((char *)URL_FOTA,(char *)New_code_ver,(Ql_strlen(New_code_ver)));
			Ql_strcat((char *)URL_FOTA,(char *)bin_url);
			//Ql_strcat((char *)URL_FOTA,"http://www.myfleetview.com:8080/sample/APPGS2MDM64_A01_Upgrade_Package.bin");

			OUT_DEBUG(textBuf,"\r\nURL_FOTA= %s\n",(char *)URL_FOTA);

			Fota_http_Flag=1;
			clrcam1flags();
			clrcam2flags();
			camcaptureflag=0;
			camindicatorflag=0;
			stop_acc_capt=1;
			stop_jrm_timer();
			fota_function();	//fota initial function
		}
		else
		{
			in_Bulk_fota_rou=0;
			bulk_fota_data=0;
		}
	}
	else
	{
		OUT_DEBUG(textBuf,"In bulk FOTA process.buff= %s\r\n",HTTP_r_buffer);
		read_sms(HTTP_r_buffer,Ql_strlen(HTTP_r_buffer));
	}

}
