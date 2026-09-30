/*-------------------------------------------------------------------------*/
/*  File       : GPRMC.c

                 18-Apr-2012
                 Trackfile generation
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*								Headers									   */
/*-------------------------------------------------------------------------*/


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
//#include "GPS.h"
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
#define OUT_D1EBUG(x,...)  \
		Ql_memset((x),0,500);  \
		Ql_sprintf((x),__VA_ARGS__);   \
		Ql_SendToUart(ql_uart_port2,(u8 *)(x),Ql_strlen(x));

extern char textBuf[];
extern PortData_Event* pPortEvt;
extern  char gps_type[10];
extern u32 filehandle;
//extern u8 cmd_idx;
char pfile2[15]="canpara.txt";
char pfilecamflag[];
char CCID_val[5][40];
//extern u8 ATE_cmd;
char GSN_val[5][20];
char FW[5][20];
char modem_str[30];
extern int track_gen;
extern ascii appBinFile[100];
char trackfile[250];
//extern char pPortEvt->data[];
u8 CCID=0;
u8 GSN=0;
u8 ATI=0;
unsigned char UID[15];
extern u8 url[100];
char CV[15];
char WMSN[20];
extern char checkdata[];
extern u16 port;
char IMEI_Num[20];
char CCID_val_tf[30];
char GSN_val_tf[30];
extern char buffer[];
extern bool Fota_UnitId;
//char GPRS_APN[]="airtelgprs.com";   //  For APN of Airtel
char GPRS_APN[25];				// For APN of Vodafone
char SIM_Lock[]="0000";
extern char copy_of_buffer[];
char RMC_str[50]="\0";
extern u8 url_cmd[50];
char STI[8];
char TXI[8];
extern char sendbuffer[1000];
extern u8 DELTA_BIN_URL[50];
extern char pfile[];
extern char pfile1[];
char pfile3[15]="ND.txt";
char pfile_imgdata[20] ="Imagedata.txt";
char pfile_new[20]="swap_file.txt";

extern char readbuffer[600];
extern u8 Fota_Flag;
extern s32 hd_file;
extern bool Lock_sim;
extern bool CLCK_cmd;
extern bool CPWD_cmd;
extern bool CPIN_cmd;
bool SIM_Flag=0;
extern int mGSMRegister;
extern bool CPIN_cmd11;
extern u64 ackedNum;
extern u8 tcpsocket;
extern u32  serialnum;
extern bool flag_test;
char Unit_Type[35];

void createandwrite(void)
{
	OUT_D1EBUG(textBuf,"Write settings .....\r\n");
	u32 filehandl=-1;
	s32 ret;
	u32 writeedlen;
	char textBuf[1]="1";
	filehandle = Ql_FileOpenEx((u8*)"canpara.txt", (QL_FS_READ_WRITE|QL_FS_CREATE));
	if(filehandle > 0)
	{
		// Ql_memset(textBuf,0,100);
		ret = Ql_FileSeek(filehandle, 0 , QL_FS_FILE_BEGIN);
		//Ql_DebugTrace("\r\nQl_FileSeek write End\r\n");
		ret = Ql_FileWrite(filehandle, (u8*)textBuf, Ql_strlen(textBuf), &writeedlen);
		Ql_DebugTrace("Ql_FileWrite()=%d: writeedlen=%d\r\n",ret, writeedlen);
		//Ql_SendToUart(ql_uart_port1,(u8*)textBuf,Ql_strlen(textBuf));
		Ql_FileClose(filehandle);
		Ql_DebugTrace("\r\nFile write successfull\r\n");
	}
}

void set_info(void)
{
	s32 ret;
	u32 writeedlen;
	u16 CanCount;
	int i;
	u32 type=1;

	OUT_D1EBUG(textBuf,"IN set info....\r\n");
	ret = Ql_FileOpenEx((u8*)pfile1,QL_FS_CREATE);

	if(ret >= QL_RET_OK)
	{
		Ql_memset((char *)readbuffer,'0',sizeof(readbuffer));
		filehandle = ret;
		ret = Ql_FileSeek(filehandle,0,QL_FS_FILE_BEGIN);
		ret = Ql_FileWrite(filehandle,(u8 *)readbuffer,400,&writeedlen);
		OUT_D1EBUG(textBuf,"Pfile1_erase()=%d: writeedlen=%d\r\n",ret,writeedlen);

		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"Error in Pfile1 erasing.....\r\n");
	}

	ret = Ql_FileOpenEx((u8*)pfile2,QL_FS_CREATE);
	if(ret >= QL_RET_OK)
	{
		Ql_memset((char *)readbuffer,'0',sizeof(readbuffer));
		filehandle = ret;
		ret = Ql_FileSeek(filehandle,0,QL_FS_FILE_BEGIN);
		ret = Ql_FileWrite(filehandle,(u8 *)readbuffer,400,&writeedlen);
		OUT_D1EBUG(textBuf,"Pfile2_erase()=%d: writeedlen=%d\r\n",ret,writeedlen);

		Ql_FileClose(filehandle);
		filehandle = -1;

	}

	else
	{
		OUT_D1EBUG(textBuf,"Error in canopara file writing\r\n");
	}

	//Pfile3 erase
	ret = Ql_FileOpenEx((u8*)pfile3,QL_FS_CREATE);
	if(ret >= QL_RET_OK)
	{
		Ql_memset((char *)readbuffer,'0',sizeof(readbuffer));
		filehandle = ret;
		ret = Ql_FileSeek(filehandle,0,QL_FS_FILE_BEGIN);
		ret = Ql_FileWrite(filehandle,(u8 *)readbuffer,400,&writeedlen);
		OUT_D1EBUG(textBuf,"Pfile3_erase()=%d: writeedlen=%d\r\n",ret,writeedlen);

		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"Error in Pfile3 erasing.....\r\n");
	}

	ret = Ql_FileOpenEx((u8*)pfilecamflag,QL_FS_CREATE);
	if(ret >= QL_RET_OK)
	{
		Ql_memset((char *)readbuffer,'0',sizeof(readbuffer));

		filehandle = ret;
		ret = Ql_FileSeek(filehandle,0,QL_FS_FILE_BEGIN);
		ret = Ql_FileWrite(filehandle,(u8 *)readbuffer,400,&writeedlen);
		OUT_D1EBUG(textBuf,"pfile_imgdata_erase()=%d: writeedlen=%d\r\n",ret,writeedlen);

		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"Error in pfile_imgdata erasing.....\r\n");

	}
	//Pfile3 pfile_new
	ret = Ql_FileOpenEx((u8*)pfile_new,QL_FS_CREATE);
	if(ret >= QL_RET_OK)
	{
		Ql_memset((char *)readbuffer,'0',sizeof(readbuffer));
		filehandle = ret;
		ret = Ql_FileSeek(filehandle,0,QL_FS_FILE_BEGIN);
		ret = Ql_FileWrite(filehandle,(u8 *)readbuffer,15000,&writeedlen);
		OUT_D1EBUG(textBuf,"Pfile3pfile_new_erase()=%d: writeedlen=%d\r\n",ret,writeedlen);

		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"Error in pfile_new erasing.....\r\n");
	}
	ret = Ql_Fs_Format(1);
	OUT_D1EBUG(textBuf,"Ql_Fs_Format()=%d  type =%d\r\n",ret,type);
	return ;
}


void trackfile_gen(void)
{
	unsigned char j=0,k=0;
	// ATI=1;
	s32 ret;

	///////////
	s32 ret1;
	u32 writeedlen;
	u32 imeifilehandle;

	////IMEI
	s32 TR_imeiret;
	char TR_ptr_imei[20];
	u16 TR_imei_len;

	OUT_D1EBUG(textBuf,"In TF generation function...\r\n");
	// Go for asking SIM no.
	if(CCID == 0)
	{
		OUT_D1EBUG(textBuf,"\r\n AT+QCCID command fire");
		ATcommand("AT+QCCID\r\n");
		delay();
		delay();
	}
	if(CCID == 1 && GSN == 0)
	{
		//      OUT_D1EBUG(textBuf,"\r\nCCID == 1");
		OUT_D1EBUG(textBuf,"\r\n AT+GSN command fire");
		ATcommand("AT+GSN\r\n");
	}

	if(GSN == 1)
	{

		OUT_D1EBUG(textBuf,"In tf genration function...After\r\n");

		Ql_strcpy((char *)CV,(char *)"PCV7.1");        ///with RFID HEX
		//Ql_strcpy((char *)CV,(char *)"LITE2_1.8A");
		Ql_strncat((char *)CV,(char *)"\0",1);
		//	  OUT_D1EBUG(textBuf,"\r\n pPortEvt->data for CV=%s ",CV);
		Ql_strcpy((char *)trackfile,(char *)UID);
		//	  OUT_D1EBUG(textBuf,"\r\n pPortEvt->data for CV=%s ",trackfile);
		Ql_strcat((char *)trackfile,(char *)"_TF");
		//  OUT_D1EBUG(textBuf,"\r\n pPortEvt->data for CV=%s ",trackfile);
		//        Ql_strncpy((char *)trackfile,(char *)"_TF,",4);
		//          Ql_strncat((char *)trackfile,(char *)"CV:",3); // code version
		//		  Ql_strcat((char *)trackfile,(char *)CV);
		Ql_strcat((char *)trackfile,(char *)",");
		//  OUT_D1EBUG(textBuf,"\r\n pPortEvt->data for CV=%s ",trackfile);

		Ql_strncat((char *)trackfile,(char *)"CV:",3); // code version
		//  OUT_D1EBUG(textBuf,"\r\n pPortEvt->data for CV=%s ",trackfile);
		//		  Ql_strncat((char *)trackfile,(char *)CV,12);
		Ql_strcat((char *)trackfile,(char *)CV);
		//	  OUT_D1EBUG(textBuf,"\r\n pPortEvt->data for CV=%s ",trackfile);
		Ql_strncat((char *)trackfile,(char *)",",1); // code version

		Ql_strncat((char *)trackfile,(char *)"SM:",3); // SIM NO.
		//	  OUT_D1EBUG(textBuf,"\r\n pPortEvt->data for CV=%s ",trackfile);
		// Ql_strncat((char *)trackfile,(char *)CCID_val[1],19);
		Ql_strncat((char *)trackfile,(char *)CCID_val_tf,19);
		Ql_strncat((char *)trackfile,(char *)",",1);
		//	  OUT_D1EBUG(textBuf,"\r\n pPortEvt->data for CV=%s ",trackfile);
		Ql_strncat((char *)trackfile,(char *)"IMEI:",5); // IMEI
		// Ql_strncat((char *)trackfile,(char *)GSN_val[1],15);
		Ql_strncat((char *)trackfile,(char *)GSN_val_tf,15);

		//*****
		TR_imeiret=Ql_strlen((char *)GSN_val_tf);
		OUT_D1EBUG(textBuf,"\r\n GSN_val_tf string length=%d ",TR_imeiret);

		if(TR_imeiret == 16)
		{
			ret1 = Ql_FileOpenEx((u8*)"IMEI_No.txt",QL_FS_CREATE);

			if(ret1 >= QL_RET_OK)
			{
				imeifilehandle = ret1;
				ret1 = Ql_FileSeek(imeifilehandle,0,QL_FS_FILE_BEGIN);
				ret1 = Ql_FileWrite(imeifilehandle, (u8*)GSN_val_tf,15,&writeedlen);
				//DC//OUT_D1EBUG(textBuf,"\r\n IMEI Ql_FileWritewrt_cnt()=%d: writeedlen=%d\r\n",ret1, writeedlen);
				OUT_D1EBUG(textBuf,"TR_ptr_imei_in file=%s\r\n",GSN_val_tf);
				Ql_FileClose(imeifilehandle);
				imeifilehandle = -1;
			}
			else
			{
				//DC//OUT_D1EBUG(textBuf,"error in IMEI write\r\n");
			}
		}

		Ql_strncat((char *)trackfile,(char *)",",1);
		Ql_strncat((char *)trackfile,(char *)"TT:",3);
		Ql_strcat((char *)trackfile,(char *)"IP");
		Ql_strncat((char *)trackfile,(char *)",",1);
		Ql_strncat((char *)trackfile,(char *)"IPS:",4);
		Ql_strcat((char *)trackfile,(char *)HOST_NAME);
		Ql_strncat((char *)trackfile,(char *)",",1);
		Ql_strncat((char *)trackfile,(char *)"IPP:",4);
		Ql_strncat((char *)trackfile,(char *)ix_Itoa(port),4);
		Ql_strncat((char *)trackfile,(char *)",",1);
		Ql_strncat((char *)trackfile,(char *)"Tx:",3);
		Ql_strcat((char *)trackfile,(char *)TXI);
		Ql_strncat((char *)trackfile,(char *)",",1);
		Ql_strncat((char *)trackfile,(char *)"St:",3);
		Ql_strcat((char *)trackfile,(char *)STI);
		Ql_strncat((char *)trackfile,(char *)",",1);
		//	  OUT_D1EBUG(textBuf,"\r\n pPortEvt->data for CV=%s ",trackfile);
		//		  Ql_strncat((char *)trackfile,(char *)"St:",3);
		Ql_strncat((char *)trackfile,(char *)"APN:",4);
		// Ql_strncat((char *)trackfile,(char *)APN_NAME,15);
		Ql_strncat((char *)trackfile,(char *)GPRS_APN,15);
		Ql_strncat((char *)trackfile,(char *)",",1);

		for(j=0;checkdata[j]!='\0';j++)
		{
			RMC_str[k]=checkdata[j];
			if(checkdata[j] =='*')
			{
				RMC_str[k]='\0';
				break;
			}
			k++;
		}
		Ql_strcat((char *)trackfile,(char *)"RMC STRING:");
		Ql_strcat((char *)trackfile,(char *)RMC_str);

		Ql_strncat((char *)trackfile,(char *)"\r\n",2);
		OUT_D1EBUG(textBuf,"Trackfile=%s",trackfile);
		OUT_D1EBUG(textBuf,"\r\n");

		Ql_strcpy((char *)sendbuffer,(char *)trackfile);
		transfer_tcp_data();


		//  		  tw_filewrite(trackfile);
	}
}
/*
void trackfile_gen(void)
{
      unsigned char j=0,k=0;
      s32 ret;
	  // Go for asking SIM no. 
  	  if(CCID == 0)
      {
          OUT_D1EBUG(textBuf,"\r\n AT+QCCID command fire");
 	      ATcommand("AT+QCCID\r\n");
		  delay();
		  delay();
      }
  	  if(CCID == 1 && GSN == 0)
      {
    //      OUT_D1EBUG(textBuf,"\r\nCCID == 1");
          OUT_D1EBUG(textBuf,"\r\n AT+GSN command fire");
          ATcommand("AT+GSN\r\n");
      }
      if(GSN == 1 && ATI == 0)
      {
//            pPortEvt->data[0]='\0';
          OUT_D1EBUG(textBuf,"\r\n ATI command fire");
 		    ATcommand("ATI\r\n");       

      }
      if(ATI == 1)
      {
    	  Ql_memset((ascii *)trackfile,'\0',sizeof(trackfile));
		  Ql_strcpy((char *)CV,(char *)"PCV003");		
          Ql_strncat((char *)CV,(char *)"\0",1);	
          Ql_strncpy((char *)trackfile,(char *)UID,6);
          Ql_strncat((char *)trackfile,(char *)"_TF",3);
		  Ql_strncat((char *)trackfile,(char *)",",1);

		  Ql_strncat((char *)trackfile,(char *)"CV:",3); // code version
//		  Ql_strncat((char *)trackfile,(char *)CV,12);
		  Ql_strcat((char *)trackfile,(char *)CV);
		  Ql_strncat((char *)trackfile,(char *)",",1); // code version		  

		  Ql_strncat((char *)trackfile,(char *)"SM:",3); // SIM NO.
		  Ql_strncat((char *)trackfile,(char *)CCID_val[1],19);
		  Ql_strncat((char *)trackfile,(char *)",",1); 		  

		  Ql_strncat((char *)trackfile,(char *)"IMEI:",5); // IMEI
		  Ql_strncat((char *)trackfile,(char *)GSN_val[1],15);
		  Ql_strncat((char *)trackfile,(char *)",",1); 

		  Ql_strncat((char *)trackfile,(char *)"TT:",3);
		  Ql_strcat((char *)trackfile,(char *)"IP");
		  Ql_strncat((char *)trackfile,(char *)",",1);

		  Ql_strncat((char *)trackfile,(char *)"IPS:",4);
		  Ql_strcat((char *)trackfile,(char *)HOST_NAME);
		  Ql_strncat((char *)trackfile,(char *)",",1); 

		  Ql_strncat((char *)trackfile,(char *)"IPP:",4);
		  Ql_strncat((char *)trackfile,(char *)ix_Itoa(port),4);
		  Ql_strncat((char *)trackfile,(char *)",",1); 

		  Ql_strncat((char *)trackfile,(char *)"Tx:",3);
//		  Ql_strcat((char *)trackfile,(char *)ix_Itoa(NORMAL_TIMER));
		  Ql_strcat((char *)trackfile,(char *)TXI);
		  Ql_strncat((char *)trackfile,(char *)",",1); 

		  Ql_strncat((char *)trackfile,(char *)"St:",3);
//		  Ql_strcat((char *)trackfile,(char *)ix_Itoa(SENDDATA_TIMER/1000));
		  Ql_strcat((char *)trackfile,(char *)STI);
		  Ql_strncat((char *)trackfile,(char *)",",1); 

//		  Ql_strncat((char *)trackfile,(char *)"St:",3);
		  Ql_strncat((char *)trackfile,(char *)APN_NAME,15);
		  Ql_strncat((char *)trackfile,(char *)",",1); 


		  for(j=0;checkdata[j]!='\0';j++)
		  {
		    RMC_str[k]=checkdata[j];
		    if(checkdata[j] =='*')
		    {
		      break;
		    }
		    k++;

		  }
		  OUT_D1EBUG(textBuf,"\nRMC=%s\r\n",RMC_str);
		  Ql_strncat((char *)trackfile,(char *)"RMC STRING:",11);
		  Ql_strcat((char *)trackfile,(char *)RMC_str);
		  Ql_strncat((char *)trackfile,(char *)"\r\n",2); 
		  OUT_D1EBUG(textBuf,"Trackfile=%s",trackfile);
  		  OUT_D1EBUG(textBuf,"\r\n");
  		  Ql_strcpy((char *)sendbuffer,(char *)trackfile);
          transfer_tcp_data();
      }
}



 */

void ATcommand(char * modem_str)
{

	if(flag_test == TRUE)
	{
		if(!(Ql_strcmp((char *)modem_str,"AT+CREG?\r\n")))

		{
			//   OUT_D1EBUG(textBuf,"NOW M IN AT+CREG? CONDITION");
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
			flag_test=FALSE;
		}

		if(!(Ql_strcmp((char *)modem_str,"AT+CGATT?\r\n")))

		{
			//   OUT_D1EBUG(textBuf,"NOW M IN AT+CREG? CONDITION");
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
			flag_test=FALSE;
		}
		if(!(Ql_strcmp((char *)modem_str,"AT+CSQ\r\n")))

		{
			//   OUT_D1EBUG(textBuf,"NOW M IN AT+CREG? CONDITION");
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
			flag_test=FALSE;
		}
		if(!(Ql_strcmp((char *)modem_str,"AT+CPIN?\r\n")))

		{
			//   OUT_D1EBUG(textBuf,"NOW M IN AT+CPIN? CONDITION\r\n");
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
			flag_test=FALSE;
		}
	}

	else
	{
		if(!(Ql_strcmp((char *)modem_str,"AT+QCCID\r\n")))
		{
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));

		}
		if(!(Ql_strcmp((char *)modem_str,"AT+GSN\r\n")))
		{
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
			//    OUT_D1EBUG(textBuf,"\r\n AT+GSN fired");
		}
		if(!(Ql_strcmp((char *)modem_str,"ATI\r\n")))
		{
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
			//    OUT_D1EBUG(textBuf,"\r\n AT+GSN fired");
		}
		/*if(!(Ql_strcmp((char *)modem_str,"ATE0\n")) && Fota_Flag==1)
     {
        Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));      
          //    OUT_D1EBUG(textBuf,"\r\n AT+GSN fired");
          ATE_cmd=1;     
     }
		 *//*
     if(!(Ql_strcmp((char *)modem_str,"AT+QIFGCNT=1\n")) && Fota_Flag==1)
     {
        Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));      
//              OUT_D1EBUG(textBuf,"AT+QIFGCNT=1 fired\r\n");
     }

     if(!(Ql_strcmp((char *)modem_str,"AT+QICSGP=1,\"WWW\"\n")) && Fota_Flag==1)        // for vodafone
//     if(!(Ql_strcmp((char *)modem_str,"AT+QICSGP=1,\"airtelgprs.com\"\n")) && Fota_Flag==1)  //for airtel
     {
        Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));      
//              OUT_D1EBUG(textBuf,"AT+QICSGP fired\r\n");
     }

//     if(!(Ql_strcmp((char *)modem_str,"AT+QHTTPURL=url_len,60\n")) && Fota_Flag==1)
     if(!(Ql_strcmp((char *)modem_str,(char *)url_cmd)) && Fota_Flag==1)
     {
        Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));      
//              OUT_D1EBUG(textBuf,"AT+QHTTPURL fired\r\n");
     }

     if(!(Ql_strcmp((char *)modem_str,(char *)url)) && Fota_Flag==1)
     {
        Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));      
  //            OUT_D1EBUG(textBuf,"DELTA_BIN_URL fired\r\n");
     }

     if(!(Ql_strcmp((char *)modem_str,"AT+QHTTPGET=120\n")) && Fota_Flag==1)
     {
        Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));      
//              OUT_D1EBUG(textBuf,"AT+QHTTPGET fired\r\n");
     }

     if(!(Ql_strcmp((char *)modem_str,"AT+QHTTPREAD=120\n")) && Fota_Flag==1)
     {
        Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));      
//              OUT_D1EBUG(textBuf,"AT+QHTTPREAD fired\r\n");
     }

     if(!(Ql_strcmp((char *)modem_str,"AT+QFLST\n")) && Fota_Flag==1)
     {
        Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));      
//              OUT_D1EBUG(textBuf,"AT+QFLST fired\r\n");
     }

     if(!(Ql_strcmp((char *)modem_str,"AT+QIDEACT\n")) && Fota_Flag==1)
     {
        Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));      
              OUT_D1EBUG(textBuf,"AT+QIDEACT fired\r\n");
     }*/


		if(!(Ql_strcmp((char *)modem_str,"AT+CPIN?\n")))
		{
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
			//         OUT_D1EBUG(textBuf,"AT+CPIN?\r\n");
		}


		if(!(Ql_strcmp((char *)modem_str,"AT+CPIN=6477\n")))
		{
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
			//              OUT_D1EBUG(textBuf,"CPIN given\r\n");
		}

		if(!(Ql_strcmp((char *)modem_str,"AT+CPIN=0000\n")))   // for vodafone
			//        if(!(Ql_strcmp((char *)modem_str,"AT+CPIN=1234\n")))     //for airtel
		{
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
			//         OUT_D1EBUG(textBuf,"CPIN given\r\n");
		}

		if(!(Ql_strcmp((char *)modem_str,"AT+CLCK=\"SC\",2\r\n")))
		{
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
			OUT_D1EBUG(textBuf,"AT+CLCK=check status fired\r\n");
		}

		if(!(Ql_strcmp((char *)modem_str,"AT+CLCK=\"SC\",1,\"0000\"\r\n")))  // for vodafone
			//     if(!(Ql_strcmp((char *)modem_str,"AT+CLCK=\"SC\",1,\"1234\"\r\n")))   //for airtel
		{
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
			//              OUT_D1EBUG(textBuf,"AT+CLCK=lock fired\r\n");
		}

		if(!(Ql_strcmp((char *)modem_str,"AT+CPWD=\"SC\",\"0000\",\"6477\"\n")))       // for vodafone
			//     if(!(Ql_strcmp((char *)modem_str,"AT+CPWD=\"SC\",\"1234\",\"6477\"\n")))    //for airtel
		{
			Ql_SendToModem(ql_md_port1,(u8*)modem_str, Ql_strlen((char *)modem_str));
			//              OUT_D1EBUG(textBuf,"AT+CPWD=lock fired\r\n");
		}
	}
}

void checkATresponse(void)
{
	unsigned int j=0,k=0;
	u8 Lock_val=0;
	//     PortData_Event* pPortEvt;
	if((!(Ql_strncmp((char *)pPortEvt->data,"AT+QCCID",8))) || (!(Ql_strncmp((char *)pPortEvt->data,"OK",2))))
	{
		OUT_D1EBUG(textBuf,"\r\nAT+QCCID ok");
		CCID=1;
		Ql_memset((ascii *)CCID_val_tf,0,sizeof(CCID_val_tf));
		//     OUT_D1EBUG(textBuf,"\r\n pPortEvt->data for CCID=%s ",pPortEvt->data);
		while(pPortEvt->data[j] !='\n')
		{
			//           Ql_strcpy((char *)CCID_val[0],(char *)pPortEvt->data);
			//CCID_val[0][k]=pPortEvt->data[j];

			j++;
			k++;
		}
		j++;
		k=0;
		while(pPortEvt->data[j] !='\n')
		{
			//		 OUT_D1EBUG(textBuf,"\r\npPortEvt->data[j]=%c\n",pPortEvt->data[j]);
			//	Ql_memset((ascii *)CCID_val_tf,0,sizeof(CCID_val_tf));
			// Ql_strcpy((char *)CCID_val_tf,(char *)pPortEvt->data[j]);
			CCID_val_tf[k]=pPortEvt->data[j];
			CCID_val_tf[k+1]='\0';
			//	   OUT_D1EBUG(textBuf,"CCID_val_tf[k]=%s",CCID_val_tf);
			///CCID_val[1][k]=pPortEvt->data[j];
			//CCID_val_tf
			j++;
			k++;
		}
		j++;
		k=0;
		while(pPortEvt->data[j] !='\n')
		{
			//           Ql_strcpy((char *)CCID_val[2],(char *)pPortEvt->data);
			// CCID_val[2][k]=pPortEvt->data[j];
			j++;
			k++;
		}
		j++;
		k=0;
		while(pPortEvt->data[j] !='\n')
		{
			//           Ql_strcpy((char *)CCID_val[3],(char *)pPortEvt->data);
			//CCID_val[3][k]=pPortEvt->data[j];
			j++;
			k++;
		}
		j=0;
		k=0;



		//         Ql_strncat((char *)CCID_val,(char *)"\0",1);
		// OUT_D1EBUG(textBuf,"\r\nCCID_val[0]=%s",CCID_val[0]);
		//     OUT_D1EBUG(textBuf,"CCID_val_tf[k]=%s",CCID_val_tf);
		//  OUT_D1EBUG(textBuf,"\r\nCCID_val[2]=%s",CCID_val[2]);
		//  OUT_D1EBUG(textBuf,"\r\nCCID_val[3]=%s",CCID_val[3]);
		pPortEvt->data[0]='\0';
		if(track_gen ==0)
			trackfile_gen();


	}
	/*   else
     {
  //           OUT_D1EBUG(textBuf,"\r\nAT+QCCID  not ok");
     }*/


	if((GSN == 0) && ((!(Ql_strncmp((char *)pPortEvt->data,"AT+GSN",6))) || (!(Ql_strncmp((char *)pPortEvt->data,"OK",2)))))
	{
		OUT_D1EBUG(textBuf,"\r\nAT+GSN ok");
		GSN=1;
		Ql_memset((ascii *)GSN_val_tf,0,sizeof(GSN_val_tf));
		//         OUT_D1EBUG(textBuf,"\r\n pPortEvt->data for GSN=%s ",pPortEvt->data);
		//         Ql_strcpy((char *)GSN_val,(char *)pPortEvt->data);
		//         Ql_strncat((char *)GSN_val,(char *)"\0",1);
		//         OUT_D1EBUG(textBuf,"\r\nGSN_val[]=%s",GSN_val);
		while(pPortEvt->data[j] !='\n')
		{
			//           Ql_strcpy((char *)CCID_val[0],(char *)pPortEvt->data);
			//   GSN_val[0][k]=pPortEvt->data[j];
			j++;
			k++;
		}
		j++;
		k=0;
		while(pPortEvt->data[j] !='\n')
		{
			//		 OUT_D1EBUG(textBuf,"\r\npPortEvt->data[j]=%c",pPortEvt->data[j]);
			//	 Ql_memset((ascii *)GSN_val_tf,0,sizeof(GSN_val_tf));
			//     Ql_strcpy((char *)GSN_val_tf,(char *)pPortEvt->data[j]);
			GSN_val_tf[k]=pPortEvt->data[j];
			GSN_val_tf[k+1]='\0';
			//	   OUT_D1EBUG(textBuf,"GSN_val_tf[k]=%s",GSN_val_tf);
			//  GSN_val[1][k]=pPortEvt->data[j];

			j++;
			k++;
		}
		j++;
		k=0;
		while(pPortEvt->data[j] !='\n')
		{
			//           Ql_strcpy((char *)CCID_val[2],(char *)pPortEvt->data);
			//  GSN_val[2][k]=pPortEvt->data[j];
			j++;
			k++;
		}
		j++;
		k=0;
		while(pPortEvt->data[j] !='\n')
		{
			//           Ql_strcpy((char *)CCID_val[3],(char *)pPortEvt->data);
			//   GSN_val[3][k]=pPortEvt->data[j];
			j++;
			k++;
		}
		j=0;
		k=0;

		//         Ql_strncat((char *)CCID_val,(char *)"\0",1);
		// OUT_D1EBUG(textBuf,"\r\nGSN_val[0]=%s",GSN_val[0]);
		//     OUT_D1EBUG(textBuf,"\r\nGSN_val_tf=%s",GSN_val_tf);
		//  OUT_D1EBUG(textBuf,"\r\nGSN_val[2]=%s",GSN_val[2]);
		//  OUT_D1EBUG(textBuf,"\r\nGSN_val[3]=%s",GSN_val[3]);
		pPortEvt->data[0]='\0';

		//         pPortEvt->data[0]='\0';
		delay();
		delay();
		if(track_gen ==0)
			trackfile_gen();
		//         track_gen=1;
	}
	/*  else
     {
  //           OUT_D1EBUG(textBuf,"\r\nAT+GSN  not ok");
     }*/
	/*
   	 if((ATI == 0) && ((!(Ql_strncmp((char *)pPortEvt->data,"ATI",3))) || (!(Ql_strncmp((char *)pPortEvt->data,"OK",2)))))
	 {
         OUT_D1EBUG(textBuf,"\r\nATI ok");
         ATI=1;
		 Ql_memset((ascii *)FW_tf,0,sizeof(FW_tf));
//         OUT_D1EBUG(textBuf,"\r\n pPortEvt->data for GSN=%s ",pPortEvt->data);
//         Ql_strcpy((char *)GSN_val,(char *)pPortEvt->data);
//         Ql_strncat((char *)GSN_val,(char *)"\0",1);
//         OUT_D1EBUG(textBuf,"\r\nGSN_val[]=%s",GSN_val);
         while(pPortEvt->data[j] !='\n')
         {
//           Ql_strcpy((char *)CCID_val[0],(char *)pPortEvt->data);
         //  FW[0][k]=pPortEvt->data[j];
           j++;
           k++;
         }
         j++;
         k=0;
         while(pPortEvt->data[j] !='\n')
         {
			 OUT_D1EBUG(textBuf,"\r\npPortEvt->data[j]=%c",pPortEvt->data[j]);
		//	 Ql_memset((ascii *)FW_tf,0,sizeof(FW_tf));
       //    Ql_strcpy((char *)FW_tf,(char *)pPortEvt->data[j]);
		   FW_tf[k]=pPortEvt->data[j];
		   FW_tf[k+1]='\0';
		   OUT_D1EBUG(textBuf,"GSN_val_tf[k]=%s",FW_tf);
       //    FW[1][k]=pPortEvt->data[j];
           j++;
           k++;
         }
         j++;
         k=0;
         while(pPortEvt->data[j] !='\n')
         {
//           Ql_strcpy((char *)CCID_val[2],(char *)pPortEvt->data);
         //  FW[2][k]=pPortEvt->data[j];
           j++;
           k++;
         }
         j++;
         k=0;
         while(pPortEvt->data[j] !='\n')
         {
//           Ql_strcpy((char *)CCID_val[3],(char *)pPortEvt->data);
        //   FW[3][k]=pPortEvt->data[j];
           j++;
           k++;
         }
 		 j=0;
         k=0;



//         Ql_strncat((char *)CCID_val,(char *)"\0",1);
     //    OUT_D1EBUG(textBuf,"\r\nFW[0]=%s",FW[0]);
         OUT_D1EBUG(textBuf,"\r\nFW_tf=%s",FW_tf);
      //   OUT_D1EBUG(textBuf,"\r\nFW[2]=%s",FW[2]);
       //  OUT_D1EBUG(textBuf,"\r\nFW[3]=%s",FW[3]);
         pPortEvt->data[0]='\0';

//         pPortEvt->data[0]='\0';
         delay();
         delay();
         if(track_gen ==0)
  	     trackfile_gen();
//         track_gen=1;


//	     if(!(Ql_strcmp((char *)pPortEvt->data,(char *)"+CREG:0,2"))


	// }
	 */
	/* if(Ql_strstr((char *)pPortEvt->data,"+CMTI") != NULL)
     {

      //   sms_handle();
      OUT_D1EBUG(textBuf,"need to put sms function \r\n");

     }*/



	if(Lock_sim == 1)
	{
		if(Ql_strstr((char *)pPortEvt->data,"+CPIN:") && Ql_strstr((char *)pPortEvt->data,"READY"))
		{
			*pPortEvt->data='\0';
			OUT_D1EBUG(textBuf,"in sim lock 1\r\n");
			SIM_LockRoutines(2);

		}

		else if(Ql_strstr((char *)pPortEvt->data,"+CPIN:") && Ql_strstr((char *)pPortEvt->data,"SIM PIN"))
		{
			*pPortEvt->data='\0';
			OUT_D1EBUG(textBuf,"in sim lock 2\r\n");
			Lock_val=ReadSimPin();
			OUT_D1EBUG(textBuf,"Lock_val =%d\r\n",Lock_val);
			if(Lock_val == 1)
			{
				SIM_LockRoutines(6);
				WriteSimPin(1);
			}
			else if(Lock_val == 2)
			{
				Lock_sim=0;
				//              SIM_Flag=1;
				SIM_LockRoutines(5);
				WriteSimPin(2);
			}
			else
			{
				Lock_sim=0;
				SIM_LockRoutines(5);
				WriteSimPin(2);

			}

			//            SIM_LockRoutines(5);

		}

		else if(Ql_strstr((char *)pPortEvt->data,"+CLCK:") && Ql_strstr((char *)pPortEvt->data,"0"))
		{
			*pPortEvt->data='\0';
			OUT_D1EBUG(textBuf,"SIM unlocked\r\n");
			SIM_LockRoutines(3);



		}

		else if(Ql_strstr((char *)pPortEvt->data,"+CLCK:") && Ql_strstr((char *)pPortEvt->data,"1"))
		{
			*pPortEvt->data='\0';
			//            SIM_LockRoutines(5);
			OUT_D1EBUG(textBuf,"SIM locked\r\n");
			//            Lock_sim=0;
			Lock_val=ReadSimPin();
			if(Lock_val == 1)
			{
				SIM_LockRoutines(4);
			}
			else if(Lock_val == 2)
			{
				Lock_sim=0;
				SIM_Flag=1;
				if(mGSMRegister == 0)
				{

					OUT_D1EBUG(textBuf,"Go for GSM GPRS registeration_2\r\n");
					GSMGPRS();
				}

			}
			else
			{
			}

		}

		else if(CLCK_cmd == 1  && Ql_strstr((char *)pPortEvt->data,"\r\nOK") )
		{
			*pPortEvt->data='\0';
			OUT_D1EBUG(textBuf,"at+clck=lock ok\r\n");
			WriteSimPin(1);
			SIM_LockRoutines(4);
			CLCK_cmd = 0;
		}

		else if(CPWD_cmd == 1 && Ql_strstr((char *)pPortEvt->data,"\r\nOK"))
		{
			*pPortEvt->data='\0';
			OUT_D1EBUG(textBuf,"at+cpwd=lock ok\r\n");
			WriteSimPin(2);
			//            SIM_LockRoutines(4);
			CPWD_cmd = 0;
			Lock_sim=0;
			SIM_Flag=1;
			if(mGSMRegister == 0)
			{

				OUT_D1EBUG(textBuf,"Go for GSM GPRS registeration_1\r\n");
				GSMGPRS();
			}


		}

		else if(CPIN_cmd == 1 && Ql_strstr((char *)pPortEvt->data,"\r\nOK"))
		{
			OUT_D1EBUG(textBuf,"SIM pin accepted\r\n");
			*pPortEvt->data='\0';
			//            CPIN_cmd = 0;
			Lock_sim=0;
			SIM_Flag=1;
			//            SIM_LockRoutines(1);

		}

		else if(CPIN_cmd11 == 1 && Ql_strstr((char *)pPortEvt->data,"\r\nOK"))
		{
			OUT_D1EBUG(textBuf,"SIM pin11 accepted\r\n");
			*pPortEvt->data='\0';
			CPIN_cmd11 = 0;
			Lock_sim=0;
			SIM_Flag=1;
			SIM_LockRoutines(4);
		}

		else
		{
			//           OUT_D1EBUG(textBuf,"Error occured\r\n");
			//           SIM_Flag=1;
		}

	}

	if (CPIN_cmd == 1 && Ql_strstr((char *)pPortEvt->data, "Call Ready") != NULL)
	{
		*pPortEvt->data='\0';
		SIM_Flag=1;
		if(mGSMRegister == 0)
		{
			CPIN_cmd = 0;
			OUT_D1EBUG(textBuf,"Go for GSM GPRS registeration_3\r\n");
			GSMGPRS();
		}
	}
}



/*
void checkATresponse(void)
{
     unsigned int j=0,k=0;
     u8 Lock_val=0;
//     PortData_Event* pPortEvt;
	 if((!(Ql_strncmp((char *)pPortEvt->data,"AT+QCCID",8))) || (!(Ql_strncmp((char *)pPortEvt->data,"OK",2))))
	 {
         OUT_D1EBUG(textBuf,"AT+QCCID ok\r\n");
         CCID=1;
         OUT_D1EBUG(textBuf,"\r\n pPortEvt->data for CCID=%s ",pPortEvt->data);
         while(pPortEvt->data[j] !='\n')
         {
//           Ql_strcpy((char *)CCID_val[0],(char *)pPortEvt->data);
           CCID_val[0][k]=pPortEvt->data[j];
           j++;
           k++;
         }	 
         j++;
         k=0;
         while(pPortEvt->data[j] !='\n')
         {
//           Ql_strcpy((char *)CCID_val[1],(char *)pPortEvt->data);
           CCID_val[1][k]=pPortEvt->data[j];
           j++;
           k++;
         }	 
         j++;
         k=0;
         while(pPortEvt->data[j] !='\n')
         {
//           Ql_strcpy((char *)CCID_val[2],(char *)pPortEvt->data);
           CCID_val[2][k]=pPortEvt->data[j];
           j++;
           k++;
         }	 
         j++;
         k=0;
         while(pPortEvt->data[j] !='\n')
         {
//           Ql_strcpy((char *)CCID_val[3],(char *)pPortEvt->data);
           CCID_val[3][k]=pPortEvt->data[j];
           j++;
           k++;
         }	 
 		 j=0;
         k=0;



//         Ql_strncat((char *)CCID_val,(char *)"\0",1);
         OUT_D1EBUG(textBuf,"\r\nCCID_val[0]=%s",CCID_val[0]);
         OUT_D1EBUG(textBuf,"\r\nCCID_val[1]=%s",CCID_val[1]);
         OUT_D1EBUG(textBuf,"\r\nCCID_val[2]=%s",CCID_val[2]);
         OUT_D1EBUG(textBuf,"\r\nCCID_val[3]=%s",CCID_val[3]);
         pPortEvt->data[0]='\0';
         if(track_gen ==0)
  		 trackfile_gen();


	 }


	 if((GSN == 0) && (!(Ql_strncmp((char *)pPortEvt->data,"AT+GSN",6))) || (!(Ql_strncmp((char *)pPortEvt->data,"OK",2))))
	 {
         OUT_D1EBUG(textBuf,"\r\nAT+GSN ok");
         GSN=1;

         while(pPortEvt->data[j] !='\n')
         {
//           Ql_strcpy((char *)CCID_val[0],(char *)pPortEvt->data);
           GSN_val[0][k]=pPortEvt->data[j];
           j++;
           k++;
         }	 
         j++;
         k=0;
         while(pPortEvt->data[j] !='\n')
         {
//           Ql_strcpy((char *)CCID_val[1],(char *)pPortEvt->data);
           GSN_val[1][k]=pPortEvt->data[j];
           j++;
           k++;
         }	 
         j++;
         k=0;
         Ql_strcpy((char *)IMEI_Num,(char *)GSN_val[1]);
         while(pPortEvt->data[j] !='\n')
         {
//           Ql_strcpy((char *)CCID_val[2],(char *)pPortEvt->data);
           GSN_val[2][k]=pPortEvt->data[j];
           j++;
           k++;
         }	 
         j++;
         k=0;
         while(pPortEvt->data[j] !='\n')
         {
//           Ql_strcpy((char *)CCID_val[3],(char *)pPortEvt->data);
           GSN_val[3][k]=pPortEvt->data[j];
           j++;
           k++;
         }	 
 		 j=0;
         k=0;



//         Ql_strncat((char *)CCID_val,(char *)"\0",1);
         OUT_D1EBUG(textBuf,"\r\nGSN_val[0]=%s",GSN_val[0]);
         OUT_D1EBUG(textBuf,"\r\nGSN_val[1]=%s",GSN_val[1]);
         OUT_D1EBUG(textBuf,"\r\nGSN_val[2]=%s",GSN_val[2]);
         OUT_D1EBUG(textBuf,"\r\nGSN_val[3]=%s",GSN_val[3]);
         pPortEvt->data[0]='\0';

//         pPortEvt->data[0]='\0';
         delay();
         delay();
         if(track_gen ==0)
  	     trackfile_gen();
//         track_gen=1;     
	 }
   	 if((ATI == 0) && (!(Ql_strncmp((char *)pPortEvt->data,"ATI",3))) || (!(Ql_strncmp((char *)pPortEvt->data,"OK",2))))
	 {
         OUT_D1EBUG(textBuf,"\r\nATI ok");
         ATI=1;

         while(pPortEvt->data[j] !='\n')
         {
//           Ql_strcpy((char *)CCID_val[0],(char *)pPortEvt->data);
           FW[0][k]=pPortEvt->data[j];
           j++;
           k++;
         }	 
         j++;
         k=0;
         while(pPortEvt->data[j] !='\n')
         {
//           Ql_strcpy((char *)CCID_val[1],(char *)pPortEvt->data);
           FW[1][k]=pPortEvt->data[j];
           j++;
           k++;
         }	 
         j++;
         k=0;
         while(pPortEvt->data[j] !='\n')
         {
//           Ql_strcpy((char *)CCID_val[2],(char *)pPortEvt->data);
           FW[2][k]=pPortEvt->data[j];
           j++;
           k++;
         }	 
         j++;
         k=0;
         while(pPortEvt->data[j] !='\n')
         {
//           Ql_strcpy((char *)CCID_val[3],(char *)pPortEvt->data);
           FW[3][k]=pPortEvt->data[j];
           j++;
           k++;
         }	 
 		 j=0;
         k=0;



//         Ql_strncat((char *)CCID_val,(char *)"\0",1);
         OUT_D1EBUG(textBuf,"\r\nFW[0]=%s",FW[0]);
         OUT_D1EBUG(textBuf,"\r\nFW[1]=%s",FW[1]);
         OUT_D1EBUG(textBuf,"\r\nFW[2]=%s",FW[2]);
         OUT_D1EBUG(textBuf,"\r\nFW[3]=%s",FW[3]);
         pPortEvt->data[0]='\0';

//         pPortEvt->data[0]='\0';
         delay();
         delay();
         if(track_gen ==0)
  	     trackfile_gen();

	 }

     if(Ql_strstr((char *)pPortEvt->data,"+CMTI") != NULL)
     {

       //  sms_handle();  

     }
     if(Ql_strstr((char *)pPortEvt->data,"+CMGR") != NULL)
     {

     }


     if(Lock_sim == 1)
     {
         if(Ql_strstr((char *)pPortEvt->data,"+CPIN:") && Ql_strstr((char *)pPortEvt->data,"READY"))
         {
 *pPortEvt->data='\0';
            SIM_LockRoutines(2);

         }

         else if(Ql_strstr((char *)pPortEvt->data,"+CPIN:") && Ql_strstr((char *)pPortEvt->data,"SIM PIN"))
         {
 *pPortEvt->data='\0';
            Lock_val=ReadSimPin();
            OUT_D1EBUG(textBuf,"Lock_val =%d\r\n",Lock_val);
            if(Lock_val == 1)
            {
              SIM_LockRoutines(6);
              WriteSimPin(1);
            }
            else if(Lock_val == 2)
            {
                Lock_sim=0;  
//              SIM_Flag=1;
                SIM_LockRoutines(5);
                WriteSimPin(2);
            }
            else
            {
               Lock_sim=0; 
               SIM_LockRoutines(5);
               WriteSimPin(2);

            }


         }

         else if(Ql_strstr((char *)pPortEvt->data,"+CLCK:") && Ql_strstr((char *)pPortEvt->data,"0"))
         {
 *pPortEvt->data='\0';
            OUT_D1EBUG(textBuf,"SIM unlocked\r\n");
            SIM_LockRoutines(3);



         }

         else if(Ql_strstr((char *)pPortEvt->data,"+CLCK:") && Ql_strstr((char *)pPortEvt->data,"1"))
         {
 *pPortEvt->data='\0';
//            SIM_LockRoutines(5);
            OUT_D1EBUG(textBuf,"SIM locked\r\n");
//            Lock_sim=0;
            Lock_val=ReadSimPin();
            if(Lock_val == 1)
            {
              SIM_LockRoutines(4);
            }
            else if(Lock_val == 2)
            {
              Lock_sim=0;  
              SIM_Flag=1;
              if(mGSMRegister == 0)
  			  {

				OUT_D1EBUG(textBuf,"Go for GSM GPRS registeration\r\n");
				GSMGPRS();
 			  }

            }
            else
            {
            }

         }

         else if(CLCK_cmd == 1  && Ql_strstr((char *)pPortEvt->data,"\r\nOK") )
         {
 *pPortEvt->data='\0';
            OUT_D1EBUG(textBuf,"at+clck=lock ok\r\n");
            WriteSimPin(1);
            SIM_LockRoutines(4);
            CLCK_cmd = 0;
         }

         else if(CPWD_cmd == 1 && Ql_strstr((char *)pPortEvt->data,"\r\nOK"))
         {
 *pPortEvt->data='\0';
            OUT_D1EBUG(textBuf,"at+cpwd=lock ok\r\n");
            WriteSimPin(2);         
//            SIM_LockRoutines(4);
            CPWD_cmd = 0;
            Lock_sim=0;
            SIM_Flag=1;
            if(mGSMRegister == 0)
			{

					OUT_D1EBUG(textBuf,"Go for GSM GPRS registeration\r\n");
					GSMGPRS();
			}


         }

         else if(CPIN_cmd == 1 && Ql_strstr((char *)pPortEvt->data,"\r\nOK"))
         {
          OUT_D1EBUG(textBuf,"SIM pin accepted\r\n");
 *pPortEvt->data='\0';
//            CPIN_cmd = 0;
            Lock_sim=0;
            SIM_Flag=1;
//            SIM_LockRoutines(1);

         }

         else if(CPIN_cmd11 == 1 && Ql_strstr((char *)pPortEvt->data,"\r\nOK"))
         {
          OUT_D1EBUG(textBuf,"SIM pin11 accepted\r\n");
 *pPortEvt->data='\0';
            CPIN_cmd11 = 0;
            Lock_sim=0;
            SIM_Flag=1;
            SIM_LockRoutines(4);
         }

         else
         {
//           OUT_D1EBUG(textBuf,"Error occured\r\n");
//           SIM_Flag=1;
         }

     }

     if (CPIN_cmd == 1 && Ql_strstr((char *)pPortEvt->data, "Call Ready") != NULL)
     {
 *pPortEvt->data='\0';
          SIM_Flag=1;
          if(mGSMRegister == 0)
		  {
                CPIN_cmd = 0;
				OUT_D1EBUG(textBuf,"Go for GSM GPRS registeration\r\n");
				GSMGPRS();
		  }
     }
}

 */

void delay(void)
{
	unsigned long int i;
	//          OUT_D1EBUG(textBuf,"\r\n in delay");
	for(i=0;i<2500000;i++);

}
