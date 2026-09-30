/*-------------------------------------------------------------------------*/
/*  File       : appli.c

                 18-Nov-2011
                 GSM-GPRS connection

                 19-Nov-2011
                 TCP/IP  server connection & data transmission

                 20-N0v-2011
                 Connect GPS to Port1.
                 GPRMC string extraction & data saggregation

                 22-Nov-2011 
                 ON stamp generation

                 25-Nov-2011
                 St & Tx timers
/*-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
/*								Revision History						   */
/*-------------------------------------------------------------------------*/





/*-------------------------------------------------------------------------*/
/*								Headers									   */
/*-------------------------------------------------------------------------*/

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
#include "Ql_multitask.h"
#include "Ql_tcpip.h"
#include "ql_filesystem.h"
#include "ql_trace.h"
#include "ql_error.h"
#include "ql_fcm.h"
#include "Ql_error.h"
#include "Ql_sms.h"
#include "Fun.h"
#include "GPS.h"
#include "GSM_GPRS.h"
#include "SEND_DATA.h"
#include "TCP_IP.h"
#include "MRW.h"  
#include "para_read.h" 
#include "GPRMC.h" 
#include "fota.h"
#include "sms_handle.h" 
#include "canparareadwrt.h"
/*-------------------------------------------------------------------------*/
/*								Globals									   */
/*-------------------------------------------------------------------------*/
//Variables for traces

#define DEBUG_BUFFER_SIZE   100

#define OUT_DEBUG(x,...)  \
		Ql_memset((void*)(x),0,DEBUG_BUFFER_SIZE);  \
		Ql_sprintf((char*)(x),__VA_ARGS__);   \
		Ql_SendToUart(ql_uart_port2,(u8*)(x),Ql_strlen((const char*)(x)));

char textBuf[500];
int mGSMRegister = 0;
extern unsigned char CCID;
extern unsigned char GSN;
extern u8 data_p[];
extern char UID[8];
char UIDtemp[15];
extern char UID_Fota[8];
char *pData, *p;
//Variables for Tx & St - Shweta 22-Nov-2011
extern u8 cmd_idx;
u8 CanUpdate_flag;
u16 OneMinTimer = 0, MS500timer = 0;
extern u8 mSocketClose;
//extern u8 Fota_Flag;
//Variabes for GPS data - Atul 21-Nov-2011
char buffer[250];
char uart3_buffer[100];
//char pPortEvt->data[250];
//char pPortEvt->data[];
u16 datalen=0;
extern int uCount;
extern char modem_str[];
//u16 datalen;
//u32 query_dns_number = 0;
extern u32 readedlen; 
extern bool uFlag=FALSE;  
extern int uwrt_cnt;
extern int uread_cnt;
int track_gen=0;
bool rec_flag=0;
int NGcount=0;
u8 CanUpdate_flag;
extern char STI[8];
extern char TXI[8];
//u32 fileSize;
s32 hd_file=0;
PortData_Event* pPortEvt;
extern bool SIM_Flag;
//Variables for GSM-GPRS registeration - Shweta 22-Nov-2011
//int powerStamp = 0;
//extern int modified_flag;
///////fota
extern int doing_fota;
extern u8 Fota_http_Flag;
extern bool trackfile_sent;
extern char GPRS_APN[];				// For APN
///////////////
char copy_of_buffer[30];
char gps_type[10];
extern char sms_set[25];
bool flag_set=0;
//  bool flag_creg;
bool flag_test=0;
//  bool flag_cpin;
extern bool gps_g2;
extern bool gps_g3;
extern bool gps_g4;
extern char SIMProv[20];
char temp_SI[]="00600";
char temp_TI[]="01200";

u16 OneMinTimer_Fota=0;
u16 OneMinTimer_Fota_Reset=0;
bool Fota_Reset_Flag=0;
//extern u8 Fota_Flag;
extern u8 cmd_idx;
ascii appBinFile[100];
//  tw_setunit_id(char UID[]);
extern char IMEI_Num[20];
char Test_CV[15];
extern char CV[15];
bool Fota_UnitId=0;
extern char Unit_Type[35];
extern int modified_flag;
void CallBack_NewSMS(u16 index,QlSMSStorage storage);
void CallBack_SendSMS(bool result, s16 cause, u8 msg_ref);
void CallBack_DeleteSMS(bool result, s16 cause,u16 index);
void CallBack_ReadPDUSMS(bool result, s16 cause,u16 index, u8 status, u8* data, u16 length);
void CallBack_NewFlashPDUSMS(u16 length, u8* pdu_string);
void CallBack_PDUStatusReport(u16 length, u8* pdu_string);
void CallBack_ReadTextSMS(bool result, s16 cause,u16 index, u8 status, QlSMSTextMsg* sms);
void CallBack_NewFlashTextSMS(QlSMSTextMsg* sms);
void CallBack_TextStatusReport(u8 fo,u8 msg_ref, u8* phone_num, QlSysTimer* scts, QlSysTimer* dt, u8 st);
/*-------------------------------------------------------------------------*/
/*								Main task								   */
/*-------------------------------------------------------------------------*/

QlEventBuffer flSignalBuffer; //Set flSignalBuffer to global variables  may as well, otherwise it will occupy stack space

void ql_entry()
{   
	u8 i,k;
	u8 j=2;
	char *p = &copy_of_buffer[0];
	s32 ret;
	char *ptr = NULL;
	s32 iret;
	QlTimer timer={0,0},timer_Fota={0,0};
	u32 fileSize=0;
	u8 pwd_set11=0;
	QlPinParameter pinparameter;
	QlPinLevel pinlevel;

	Ql_SMS_Callback cb_func={CallBack_NewSMS,CallBack_SendSMS,CallBack_DeleteSMS,                 //public
			CallBack_ReadPDUSMS,CallBack_NewFlashPDUSMS,CallBack_PDUStatusReport,               //pdu
			CallBack_ReadTextSMS,CallBack_NewFlashTextSMS,CallBack_TextStatusReport};           //text
	ret=Ql_SMSInitialize(&cb_func);

	bool keepGoing = TRUE;
	Ql_SetDebugMode(BASIC_MODE);
	Ql_UartClrRxBuffer(ql_uart_port1);
	OUT_DEBUG(textBuf,"Dragon Setting All SIM\r\n");
	//Open UART1
	Ql_OpenModemPort(ql_md_port1);
	//Set baud rate of Port1 to 4800
	// Ql_SetUartBaudRate(ql_uart_port1,4800);
	//  1 min delay for GSM registeration
	readcreated();
	if(modified_flag==1)
	{
		readcreated();
	}

	if(Ql_strstr((char *)gps_type,"L80") != NULL)
	{
		Ql_SetUartBaudRate(ql_uart_port1,9600);
	}
	else
	{ 
		Ql_SetUartBaudRate(ql_uart_port1,4800);
	}

	H2F_Init();
	/////fotatimer
	timer_Fota.timeoutPeriod = Ql_SecondToTicks(900);    ///Dota after 15 min
	Ql_StartTimer(&timer_Fota);

	OUT_DEBUG(textBuf,"\r\nWant to format memory ? if Yes send YES,,,if no send NO\r\n");

	if(gps_g2 == 1)
	{
		Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF103,00,00,00,01*24\r\n",25);
		Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF103,01,00,00,01*25\r\n",25);
		Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF103,03,00,00,01*27\r\n",25);
		Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF103,05,00,00,01*21\r\n",25);
		Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF103,06,00,00,01*22\r\n",25);
		Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF103,08,00,00,01*2C\r\n",25);
	}

	else if(gps_g3 == 1)
	{
		Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF103,00,00,00,01*24\r\n",25);
		Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF103,01,00,00,01*25\r\n",25);
		Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF103,03,00,00,01*27\r\n",25);
		Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF103,05,00,00,01*21\r\n",25);
		Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF103,06,00,00,01*22\r\n",25);
		Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF103,08,00,00,01*2C\r\n",25);
		Ql_SendToUart(ql_uart_port1,(u8 *)"$PMTK397,0.7*3A\r\n",17);
	}
	else if(gps_g4 == 1)
	{
		pinparameter.pinconfigversion = QL_PIN_VERSION;
		pinparameter.pinparameterunion.gpioparameter.pinpullenable = QL_PINPULLENABLE_ENABLE;
		pinparameter.pinparameterunion.gpioparameter.pindirection = QL_PINDIRECTION_OUT;
		pinparameter.pinparameterunion.gpioparameter.pinlevel = QL_PINLEVEL_HIGH;
		iret = Ql_pinSubscribe(QL_PINNAME_KBR0,QL_PINMODE_2,&pinparameter);
		//     OUT_DEBUG(textbuf,"mode2 gpio\r\n");
		OUT_DEBUG(textBuf,"Subscribe(%d),pin=%d,mod=%d,pul=%d,dir=%d,lev=%d\r\n",iret,QL_PINNAME_KBR0,QL_PINMODE_2,QL_PINPULLENABLE_ENABLE,QL_PINDIRECTION_OUT,QL_PINLEVEL_HIGH);
		//     OUT_DEBUG(textbuf,"making low\r\n");
		iret = Ql_pinWrite(QL_PINNAME_KBR0, QL_PINLEVEL_LOW);
		OUT_DEBUG(textBuf, "WriteLow(%d),pin=%d,lev=%d\r\n",iret,QL_PINNAME_KBR0,QL_PINLEVEL_LOW);
		iret = Ql_pinWrite(QL_PINNAME_KBR0, QL_PINLEVEL_HIGH);
		OUT_DEBUG(textBuf, "WriteLow(%d),pin=%d,lev=%d\r\n",iret,QL_PINNAME_KBR0,QL_PINLEVEL_HIGH);
		delay();
		delay();
		delay();

		iret = Ql_pinWrite(QL_PINNAME_KBR0, QL_PINLEVEL_LOW);
		OUT_DEBUG(textBuf, "\r\nWriteLow(%d),pin=%d,lev=%d\r\n",iret, QL_PINNAME_KBR0,QL_PINLEVEL_LOW);
		delay();
		//     Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF100,1,9600,8,1,0*0D\r\n",26);
		Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF103,00,00,00,01*24\r\n",25);    // Disable GPGGA string
		Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF103,02,00,00,01*26\r\n",25);


	}
	timer.timeoutPeriod = Ql_SecondToTicks(60);
	Ql_StartTimer(&timer);
	while(keepGoing)
	{
		Ql_GetEvent(&flSignalBuffer);
		switch(flSignalBuffer.eventType)
		{
		case EVENT_TIMER:
		{
			if( timer.timerId  == flSignalBuffer.eventData.timer_evt.timer_id)
			{

				if((Ql_strstr((char *)UID,"\0") != NULL) && ((Ql_strlen(UID)) <= 2))
					//if((Ql_strstr((char *)UID,"\0") != NULL))
				{
					//OUT_DEBUG(textBuf,"Do settings First... =%d\r\n",(Ql_strlen(UID)))
					OUT_DEBUG(textBuf,"Do settings First...\r\n")
				}
				//	if(SIM_Flag == 0)
				if(((Ql_strncmp((ascii *)GPRS_APN,"\0",1)) && (Ql_strlen(GPRS_APN) >= 2)) && (SIM_Flag == 0))
				{
					OUT_DEBUG(textBuf,"Go for SIM Lock \r\n");
					SIM_LockRoutines(1);
				}
				if(mGSMRegister == 0 && SIM_Flag == 1)
				{
					OUT_DEBUG(textBuf,"Go for GSM GPRS registeration\r\n");
					GSMGPRS();
				}
				Ql_StartTimer(&timer);
			}
			if( timer_Fota.timerId  == flSignalBuffer.eventData.timer_evt.timer_id)
			{
				OUT_DEBUG(textBuf,"timer_Fota timer up\r\n");

				if((trackfile_sent==1) && (Ql_strlen(UID)) >= 2)
				{
					Fota_http_Flag=1;
					fota_function();
				}
				Ql_StartTimer(&timer_Fota);
			}
			break;
		}
		case EVENT_MODEMDATA:
		{
			pPortEvt = (PortData_Event*)&flSignalBuffer.eventData.modemdata_evt;
			if(doing_fota==1)
			{
				fota_uart();
			}
			else if(DATA_TCP_T == pPortEvt->type)
			{
				u32 writtenlen;
				ret = Ql_FileWrite(hd_file,(u8*)pPortEvt->data, pPortEvt->len, &writtenlen);
				OUT_DEBUG(textBuf,"DATA_TCP_T, len:%d\r\n", pPortEvt->len);
				OUT_DEBUG(textBuf,"DATA_TCP_T, data:%s\r\n", pPortEvt->data);
			}
			else
			{
				if(Ql_strstr(pPortEvt->data,("AT+CPIN=")))
				{
					OUT_DEBUG(textBuf,"SIM PIN Given...\r\n");
				}
				else
				{
					OUT_DEBUG(textBuf,"\r\nbuffer = %s\n",pPortEvt->data);
				}
				checkATresponse();
			}
			break;
		}
		case EVENT_UARTDATA:
		{
			//AT command send to BB, goto run

			if(flSignalBuffer.eventData.modemdata_evt.port == 2)
			{
				Ql_strcpy(buffer,((char *)flSignalBuffer.eventData.uartdata_evt.data));
				//  OUT_DEBUG(textBuf,"PORT=%d  PORT_1=%s\r\n",flSignalBuffer.eventData.uartdata_evt.port,flSignalBuffer.eventData.uartdata_evt.data);
				datalen = Ql_strlen(buffer);
				gprsformat1(buffer,datalen);
			}
			else if(flSignalBuffer.eventData.modemdata_evt.port == 3)
			{
				Ql_strcpy(uart3_buffer,((char *)flSignalBuffer.eventData.uartdata_evt.data));

				if(Ql_strstr(uart3_buffer,("YES")) || Ql_strstr(uart3_buffer,("yes")))
				{
					formatmemory();
				}
				else if(Ql_strstr(uart3_buffer,("NO")) || Ql_strstr(uart3_buffer,("no")))
				{
					readcreated();

					if(Ql_strstr((char *)gps_type,"L80") != NULL)
					{
						Ql_SetUartBaudRate(ql_uart_port1,9600);
					}
					else
					{
						Ql_SetUartBaudRate(ql_uart_port1,4800);
					}
				}
				else if(Ql_strstr(uart3_buffer,("AT+P=")) || Ql_strstr(uart3_buffer,("at+p=")))
				{
					if(Ql_strstr(uart3_buffer,("ku45ma23")))
					{
						//OUT_DEBUG(textBuf,"inside uart pwd_set=%d\r\n",pwd_set11);
						pwd_set11=1;
						//OUT_DEBUG(textBuf,"%s\r\n",buffer);
						OUT_DEBUG(textBuf,"password accepted Enter Unit ID\r\n");
					}
				}
				else if(Ql_strstr(uart3_buffer,("AT+U=")) || Ql_strstr(uart3_buffer,("at+u=")))
				{
					// SYS_DEBUG( DBG_Buffer,"pwd_set after=%d\r\n",pwd_set11);
					if (pwd_set11 == 1)
					{
						OUT_DEBUG(textBuf,"%s\r\n",uart3_buffer);
						Ql_memset(UIDtemp,'\0',sizeof(UIDtemp));
						Ql_memset(copy_of_buffer,'\0',sizeof(copy_of_buffer));
						//Ql_strcpy(copy_of_buffer,(m_RxBuf_Uart1));
						ptr=(char *)uart3_buffer;
						Ql_strcpy(UIDtemp,(char *)"UI");
						//Ql_strcat(UIDtemp,);

						if((!(Ql_strncmp((char *)ptr,(char *)"AT+U=",4))) || (!(Ql_strncmp((char *)ptr,(char *)"at+u=",4))))
						{
							k=0;
							OUT_DEBUG(textBuf,"routine for changing Unit id\r\n");
							ptr=ptr+6;
							OUT_DEBUG(textBuf,"ptr Unit id:%s\r\n",ptr);
							while((*ptr !='\0') && (*ptr !='"'))
							{
								copy_of_buffer[k]=*ptr;
								ptr++;
								k++;
							}
							copy_of_buffer[k]='\0';
						}
						Ql_strcat(UIDtemp,copy_of_buffer);

						OUT_DEBUG(textBuf,"unit id from main=%s\r\n",UIDtemp);
						newunit_id((char *)UIDtemp);

						OUT_DEBUG(textBuf,"password accepted Enter SIM Provider..\r\n");

					}
					else
					{
						OUT_DEBUG(textBuf,"set password first for Unit id \r\n");
					}
				}
				/////Write SIM Provider
				else if(Ql_strstr(uart3_buffer,("AT+S=")) || Ql_strstr(uart3_buffer,("at+s=")))
				{
					OUT_DEBUG(textBuf," Set SIM Provider=%s\r\n",uart3_buffer);
					k=0;
					if (pwd_set11 == 1)
					{
						OUT_DEBUG(textBuf,"%s\r\n",uart3_buffer);
						Ql_strcpy(copy_of_buffer,(uart3_buffer));

						for(i=6;copy_of_buffer[i]!='"';i++)
						{
							SIMProv[k]=copy_of_buffer[i];
							k++;
						}
						SIMProv[k]='\0';
						OUT_DEBUG(textBuf,"SIM Provider=%s\r\n",SIMProv);
						copy_of_buffer[0]='\0';
						//tw_setunit_Type();
						FunSIMProvider((char *)SIMProv);
						OUT_DEBUG(textBuf,"Reset the device. \r\n");
					}
					else
					{
						OUT_DEBUG(textBuf,"set password first for Unit id \r\n");
					}
				}
				/////////////Added On 13 Jan 2013////////////////////////////////
				else if(Ql_strstr(uart3_buffer,("AT+I=")) || Ql_strstr(uart3_buffer,("at+i=")))
				{
					// OUT_DEBUG(textBuf,"pwd_set after=%d\r\n",pwd_set11);
					k=0;
					if (pwd_set11 == 1)
					{
						OUT_DEBUG(textBuf,"%s\r\n",uart3_buffer);
						Ql_strcpy(copy_of_buffer,(uart3_buffer));

						for(i=6;copy_of_buffer[i]!='"';i++)
						{
							Unit_Type[k]=copy_of_buffer[i];
							k++;
						}
						Unit_Type[k]='\0';
						OUT_DEBUG(textBuf,"Unit_Type=%s\r\n",Unit_Type);
						copy_of_buffer[0]='\0';
						//tw_setunit_Type();
						type_unit((char *)Unit_Type);
						if(Ql_strstr(Unit_Type,("JRM")) || Ql_strstr(Unit_Type,("jrm")))
						{
							OS_Lmt_wrt((char *)"65",(char *)"15",(char *)"30",(char *)"40");
						}
						else
						{
							OS_Lmt_wrt((char *)"65",(char *)"15",(char *)"200",(char *)"200");
						}
					}
					else
					{
						OUT_DEBUG(textBuf,"set password first for Unit id \r\n");
					}
				}

				///////////////////////////////////////////////////////////////////////////////////////////////
				else if(Ql_strstr(uart3_buffer,("AT+SET")) || Ql_strstr(uart3_buffer,("at+set")))
				{
					OUT_DEBUG(textBuf,"%s\r\n",uart3_buffer);
					flag_set=TRUE;
					Ql_strcpy(sms_set,"UIXXXX#SET0003");
					//Change_RemoteParameter(":0000,240,-,120,-,-");
					SI_TI((char *)temp_SI,(char *)temp_TI);

				}
				else if(Ql_strstr(uart3_buffer,("AT+CFUN=")) || Ql_strstr(uart3_buffer,("at+cfun=")))
				{
					//OUT_DEBUG(textBuf,"Resettttt\r\n");
					OUT_DEBUG(textBuf,"%s\r\n",uart3_buffer);
					delay();
					Ql_Reset(0);
				}
				else if(Ql_strstr(uart3_buffer,("AT+CREG?")) || Ql_strstr(uart3_buffer,("at+creg?")))
				{
					OUT_DEBUG(textBuf,"%s\r\n",uart3_buffer);
					flag_test=TRUE;
					ATcommand("AT+CREG?\r\n");
				}
				else if(Ql_strstr(uart3_buffer,("AT+CGATT?")) || Ql_strstr(uart3_buffer,("at+cgatt?")))
				{
					OUT_DEBUG(textBuf,"%s\r\n",uart3_buffer);
					flag_test=TRUE;
					ATcommand("AT+CGATT?\r\n");
				}
				else if(Ql_strstr(uart3_buffer,("AT+CSQ")) || Ql_strstr(uart3_buffer,("at+csq")))
				{
					OUT_DEBUG(textBuf,"%s\r\n",uart3_buffer);
					flag_test=TRUE;
					ATcommand("AT+CSQ\r\n");
				}
				else if(Ql_strstr(uart3_buffer,("AT+CPIN?")) || Ql_strstr(uart3_buffer,("at+cpin?")))
				{
					OUT_DEBUG(textBuf,"%s\r\n",uart3_buffer);
					flag_test=TRUE;
					ATcommand("AT+CPIN?\r\n");
				}
				else if(Ql_strstr(uart3_buffer,("AT+G=")) || Ql_strstr(uart3_buffer,("at+g=")))
				{
					OUT_DEBUG(textBuf,"%s\r\n",uart3_buffer);
					if (pwd_set11 == 1)
					{
						if(Ql_strstr(uart3_buffer,("g1")) || Ql_strstr(uart3_buffer,("G1")))
						{
							OUT_DEBUG(textBuf,"gps is GR1100\r\n");
							Ql_strcpy(gps_type,"GR1100");
							Ql_strncat(gps_type,"\0",1);
							type_gps((char *)gps_type);
						}

						else if(Ql_strstr(uart3_buffer,("g2")) || Ql_strstr(uart3_buffer,("G2")))
						{
							OUT_DEBUG(textBuf,"gps is GR301\r\n");
							Ql_strcpy(gps_type,"GR301");
							Ql_strncat(gps_type,"\0",1);
							type_gps((char *)gps_type);
							//  tw_writegps();

						}
						else if(Ql_strstr(uart3_buffer,("g3")) || Ql_strstr(uart3_buffer,("G3")))
						{
							OUT_DEBUG(textBuf,"gps is UP501B\r\n");
							Ql_strcpy(gps_type,"UP501B");
							Ql_strncat(gps_type,"\0",1);
							type_gps((char *)gps_type);
							// tw_writegps();
						}

						else if(Ql_strstr(uart3_buffer,("g4")) || Ql_strstr(uart3_buffer,("G4")))
						{
							OUT_DEBUG(textBuf,"gps is L50\r\n");
							Ql_strcpy(gps_type,"L50");
							Ql_strncat(gps_type,"\0",1);
							type_gps((char *)gps_type);
							// tw_writegps();

						}
						else if(Ql_strstr(uart3_buffer,("g5")) || Ql_strstr(uart3_buffer,("G5")))
						{
							OUT_DEBUG(textBuf,"gps is L80\r\n");
							Ql_strcpy(gps_type,"L80");
							Ql_strncat(gps_type,"\0",1);
							type_gps((char *)gps_type);
							// tw_writegps();
						}
					}
					else
					{
						OUT_DEBUG(textBuf,"set password first \r\n");
					}
					//pwd_set11=0;
				}

			}
			break;
		}
		}
	}
}





