
/*-------------------------------------------------------------------------*/
/*								Headers									   */
/*-------------------------------------------------------------------------*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
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
#include "ql_fcm.h"
#include "GSM_GPRS.h"
#include "SEND_DATA.h"
#include "TCP_IP.h"
#include "MRW.h"  
#include "para_read.h"
#include "sms_handle.h"
#include "Ql_filesystem.h"
#include "Battery.h"


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


#define  PATH_CANPARA ((u8 *)"updatedcanpara.txt")
#define  Remote_Para ((u8 *)"remotepara.txt")
//#define  BATTRYIND ((u8 *)"BattPowerInd.txt")

extern  char gps_type[10];

extern char Flag_readbuffer[5];
extern char CanFlag_readbuffer[2];
extern char CanPara_readbuffer[50];
extern char CVread[25];
extern u8 CanUpdate_flag;
extern unsigned char ItoaStr[];
extern char UID[];
//extern char UID_Fota[8];
char BATTRYIND[30] = "BattPowerInd.txt";
extern char textBuf[1000];
//extern char mPrevLatLong[30]={0};
extern char mPrevGPRMC[75];
extern char checkdata[];
extern char DATE[], TIME[], nLAT[], LONG[], eLONG[], STAT[], SPEED[], COURSE[];
extern char LAT[];
extern char mSpeedAscii[];
extern unsigned char FtoaStr[];
extern double OSpeed;
extern int wrt_cnt;
extern int read_cnt;
extern u32 BackUpwrt_cnt;
extern char STI[8];
extern char TXI[8];
////for camera tx & rx
extern char ICacpTI[8];          	    //Image capture time interval
extern char ITxTI[8];					//Image Transmission time interval
extern int mOverSpeedLimit;
extern int mOverSpeedLimit1;
extern int mOverSpeedLimit2;
extern char GPRMC[];
extern double mTotDist;
extern int uwrt_cnt;
extern int uread_cnt;
extern char mprevDist[8]; 
extern char mureadcnt[8]; 
extern u8 BackUp_Flag;
extern char CV[15];
bool gps_g2=0;
bool gps_g3=0;
bool gps_g4=0;
bool gps_g5=0;

extern u8 CANDUMPflg;

extern int modified_flag;
extern char APN_NAME[];
extern char HOST_NAME[];
char Unit_Type[35];
extern char CAMUID[10];
extern double mAccDcLimit;
extern u16 port;
extern char HOST_NAME[30];

//SMTP deatils
extern char SMTP_SVR_ADDR[50];
extern char SMTP_USER_NAME[50];
extern char SMTP_PASSWORD[50];
extern char SMTP_ADDR[50];
extern char SMTP_DST[50];
extern char SMTP_SUB[50];

//FTPDeatils
extern char FTP_SVR_ADDR[30];
extern char FTP_SVR_PORT[10];
extern char FTP_USER_NAME[30];
extern char FTP_PASSWORD[30];
extern bool PO_STATUS;
bool POStmpGEN=0;
bool PFStmpGEN=0;

extern char Code_version[];
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/




// Added on 14 jun 2012 for Can parameters
void tw_updatedCanParaRead(void)
{
	s32 ret;
	u8 k=0;
	char *ptr;
	u32 readedlen1;
	u16 CanCount;
	s32 filehandle=-1;
	char ui[3]="UI\0";
	OUT_D1EBUG(textBuf,"\r\n###### Device Parameters  ######\r\n");

	Ql_memset((ascii *)TXI,'\0',sizeof(TXI));
	filehandle = Ql_FileOpenEx(PATH_CANPARA,QL_FS_CREATE);
	if(filehandle >= QL_RET_OK)
	{
		//--------------------Can UID-------------------------------------
		Ql_memset((ascii *)CanPara_readbuffer,'\0',sizeof(CanPara_readbuffer));
		ret = Ql_FileSeek(filehandle,1, QL_FS_FILE_BEGIN);
		ret = Ql_FileRead(filehandle, (u8 *)CanPara_readbuffer,8, &readedlen1);
		//	 OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d\r\n",ret, readedlen1);
		//OUT_D1EBUG(textBuf,"CanPara_readbuffer = %s\r\n",CanPara_readbuffer);
		Ql_strcpy(UID,(char *)CanPara_readbuffer);
		Ql_strncat(UID,"\0",1);
		//Ql_strcpy(UID_Fota,UID);

		Ql_memset((ascii *)CAMUID,'\0',sizeof(CAMUID));
		/*CAMUID[0]=UID[2];
		CAMUID[1]=UID[3];
		CAMUID[2]=UID[4];
		CAMUID[3]=UID[5];
		CAMUID[4]='_';
		CAMUID[5]='\0';*/
		ptr=Ql_strstr((char *)UID,(char *)ui);
		while(*ptr !='I') ptr++;
		ptr++;
		//k=0;
		while(*ptr !='\0')
		{

			CAMUID[k]=*ptr;
			ptr++;
			k++;
		}

		//Ql_strcpy(CAMUID,(char *)UID);
		Ql_strncat(CAMUID,"\0",1);
		Ql_memset((ascii *)SMTP_USER_NAME,'\0',sizeof(SMTP_USER_NAME));
		//Ql_memset((ascii *)SMTP_ADDR,'\0',sizeof(SMTP_ADDR));
		Ql_strcpy((char *)SMTP_USER_NAME,(char *)CAMUID);
		//Ql_strcpy((char *)SMTP_USER_NAME,(char *)"dragonfly");
		//Ql_strcpy((char *)SMTP_ADDR,(char *)CAMUID);
		OUT_D1EBUG(textBuf,"\tUID Read \t\t\t= %s\r\n",UID);
		OUT_D1EBUG(textBuf,"\tCAMUID Read \t\t\t= %s\r\n",CAMUID);

		/////////////////////////////////////////////////////////////////////////
		//--------------------Can STI-------------------------------------

		Ql_memset((ascii *)CanPara_readbuffer,'\0',sizeof(CanPara_readbuffer));
		Ql_memset((ascii *)STI,'\0',sizeof(STI));
		//ret = Ql_FileSeek(filehandle,CanCount, QL_FS_FILE_BEGIN);

		ret = Ql_FileSeek(filehandle,56, QL_FS_FILE_BEGIN);
		ret = Ql_FileRead(filehandle, (u8 *)CanPara_readbuffer,6, &readedlen1);
		//	 OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d\r\n",ret, readedlen1);

		Ql_strcpy(STI,(char *)CanPara_readbuffer);
		Ql_memset((ascii *)CanPara_readbuffer,'\0',sizeof(CanPara_readbuffer));
		Ql_strncat(STI,"\0",1);
		OUT_D1EBUG(textBuf,"\tStamping Interval \t\t= %s\r\n",STI);


		/////////////////////////////////////////////////////////////////////////
		//--------------------Can TXI-------------------------------------

		Ql_memset((ascii *)CanPara_readbuffer,'\0',sizeof(CanPara_readbuffer));
		//		Ql_memset((ascii *)TXI,'\0',sizeof(TXI));
		ret = Ql_FileSeek(filehandle,63, QL_FS_FILE_BEGIN);
		ret = Ql_FileRead(filehandle, (u8 *)CanPara_readbuffer,6, &readedlen1);
		//	 OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d\r\n",ret, readedlen1);
		Ql_strcpy(TXI,(char *)CanPara_readbuffer);
		Ql_strncat(TXI,"\0",1);
		OUT_D1EBUG(textBuf,"\tTransmission Interval \t\t= %s\r\n",TXI);

		/////////////////////////////////////////////////////////////////////////
		//--------------------Can gps_type-------------------------------------

		Ql_memset((ascii *)CanPara_readbuffer,'\0',sizeof(CanPara_readbuffer));
		ret = Ql_FileSeek(filehandle,45, QL_FS_FILE_BEGIN);
		ret = Ql_FileRead(filehandle, (u8 *)CanPara_readbuffer,10, &readedlen1);
		//	 OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d,,,,,%s\r\n",ret,readedlen1,CanPara_readbuffer);
		Ql_strcpy(gps_type,(char *)CanPara_readbuffer);
		Ql_strncat(gps_type,"\0",1);
		OUT_D1EBUG(textBuf,"\tGPS \t\t\t\t= %s\r\n",gps_type);

		if(!(Ql_strcmp((char *)gps_type,"UP501B")))
		{
			OUT_D1EBUG(textBuf,"\tGPS is UP501B\r\n");
			gps_g3=1;
		}
		else if(!(Ql_strcmp((char *)gps_type,"GR301")))
		{
			OUT_D1EBUG(textBuf,"\tGPS is GR301\r\n");
			gps_g2=1;
		}
		else if(!(Ql_strcmp((char *)gps_type,"L50")))
		{
			OUT_D1EBUG(textBuf,"\tGPS is L50\r\n");
			gps_g4=1;
		}
		else if(!(Ql_strcmp((char *)gps_type,"L80")))
		{
			OUT_D1EBUG(textBuf,"\tGPS is L80\r\n");
			gps_g5=1;
		}
		/////////////////////////////////////////////////////////////////////////
		//--------------------Camera capture interval-------------------------------------
		//extern char ICacpTI[8];          	    //Image capture time interval
		//extern char ITxTI[8];

		Ql_memset((ascii *)CanPara_readbuffer,'\0',sizeof(CanPara_readbuffer));
		Ql_memset((ascii *)ICacpTI,'\0',sizeof(ICacpTI));
		//		ret = Ql_FileSeek(filehandle,CanCount, QL_FS_FILE_BEGIN);

		ret = Ql_FileSeek(filehandle,77, QL_FS_FILE_BEGIN);
		ret = Ql_FileRead(filehandle, (u8 *)CanPara_readbuffer,6, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d\r\n",ret, readedlen1);

		if(!(Ql_strncmp((ascii *)CanPara_readbuffer,"\0",1)))
		{
			//OUT_D1EBUG(textBuf,"Camera capture interval Read CanPara_readbuffer empty\r\n");
			Ql_strcpy(CanPara_readbuffer,(char *)"00300");
		}

		Ql_strcpy(ICacpTI,(char *)CanPara_readbuffer);
		Ql_memset((ascii *)CanPara_readbuffer,'\0',sizeof(CanPara_readbuffer));
		Ql_strncat(ICacpTI,"\0",1);
		OUT_D1EBUG(textBuf,"\tCamera capture int Read \t= %s\r\n",ICacpTI);
		//        OUT_D1EBUG(textBuf,"UID Read = %s\r\n",UID);
		//        Ql_strcpy(UID,(char *)UID);
		/////////////////////////////////////////////////////////////////////////
		//--------------------Camera transmission interval-------------------------------------

		Ql_memset((ascii *)CanPara_readbuffer,'\0',sizeof(CanPara_readbuffer));
		Ql_memset((ascii *)ITxTI,'\0',sizeof(ITxTI));
		ret = Ql_FileSeek(filehandle,70, QL_FS_FILE_BEGIN);
		ret = Ql_FileRead(filehandle, (u8 *)CanPara_readbuffer,6, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d\r\n",ret, readedlen1);

		if(!(Ql_strncmp((ascii *)CanPara_readbuffer,"\0",1)))
		{
			//OUT_D1EBUG(textBuf,"\tCamera transmission interval Read CanPara_readbuffer empty\r\n");
			Ql_strcpy(CanPara_readbuffer,(char *)"01800");
		}

		Ql_strcpy(ITxTI,(char *)CanPara_readbuffer);
		Ql_strncat(ITxTI,"\0",1);
		OUT_D1EBUG(textBuf,"\tCamera transmission int Read \t= %s\r\n",ITxTI);
		//        OUT_D1EBUG(textBuf,"UID Read = %s\r\n",UID);

		/////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////
		//--------------------Can Unit_Type-------------------------------------

		Ql_memset((ascii *)CanPara_readbuffer,'\0',sizeof(CanPara_readbuffer));
		ret = Ql_FileSeek(filehandle,9, QL_FS_FILE_BEGIN);
		ret = Ql_FileRead(filehandle, (u8 *)CanPara_readbuffer,35, &readedlen1);
		//	 OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d,,,,%s\r\n",ret,readedlen1,CanPara_readbuffer);
		Ql_strcpy(Unit_Type,(char *)CanPara_readbuffer);
		Ql_strncat(Unit_Type,"\0",1);
		OUT_D1EBUG(textBuf,"\tUnit_type Read \t\t\t= %s\r\n",Unit_Type);

		//--------------------Can code version read-------------------------------------

		Ql_memset((ascii *)CanPara_readbuffer,'\0',sizeof(CanPara_readbuffer));
		ret = Ql_FileSeek(filehandle,90, QL_FS_FILE_BEGIN);
		ret = Ql_FileRead(filehandle, (u8 *)CanPara_readbuffer,25, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d,,,,%s\r\n",ret,readedlen1,CanPara_readbuffer);

		//		if((!(Ql_strncmp((ascii *)CanPara_readbuffer,"\0",1))) || (!(Ql_strcmp((ascii *)CanPara_readbuffer,"LITE"))) )

		if(!(Ql_strncmp((ascii *)CanPara_readbuffer,"\0",1)))
		{
			OUT_D1EBUG(textBuf,"\t Code version buffer is empty\r\n");
			//Ql_strcpy(CanPara_readbuffer,(char *)"01800");
		}
		else if((!(Ql_strcmp((ascii *)CanPara_readbuffer,"LITE"))))
		{
			Ql_strcpy(Code_version,(char *)CanPara_readbuffer);
			Ql_strncat(Code_version,"\0",1);
		}

		OUT_D1EBUG(textBuf,"\tCode_version Read \t\t= %s\r\n",Code_version);

		Ql_FileClose(filehandle);
		filehandle = -1;
	}

	else
	{
		OUT_D1EBUG(textBuf,"Error in CAN parameter file reading**1 tw_CanParaRead\r\n");
	}
	/////////////////////////////////////////////////////////////////////////
	//--------------------CanUpdateFlag-------------------------------------
}


void newunit_id(char * unit_id)
{
	u8 i;
	s32 ret1;
	u32 writeedlen;
	s32 filehandle=-1;
	char tempuid[8];
	ret1 = Ql_FileOpenEx(PATH_CANPARA,QL_FS_CREATE);//open the file or else create the file
	//	OUT_D1EBUG(textBuf,""ret=%d\r\n",ret1);
	if(ret1 >= QL_RET_OK)
	{
		Ql_memset((ascii *)tempuid,'\0',sizeof(tempuid));
		Ql_strncat(tempuid,"UI",2);
		Ql_strcat((char *)tempuid,(char *)unit_id);
		/*tempuid[0]='U';
		tempuid[1]='I';
		tempuid[2]=unit_id[0];
		tempuid[3]=unit_id[1];
		tempuid[4]=unit_id[2];
		tempuid[5]=unit_id[3];
		tempuid[6]='\0';*/
		filehandle = ret1;

		if(Ql_strstr((char *)tempuid,"-") == NULL)
		{
			ret1 = Ql_FileSeek(filehandle,1,QL_FS_FILE_BEGIN);
			ret1 = Ql_FileWrite(filehandle, (u8*)tempuid,8,&writeedlen);
			OUT_D1EBUG(textBuf,"Unit ID is=%s\r\n",tempuid);
		}
		Ql_FileClose(filehandle);
		filehandle = -1;
		Ql_Sleep(500);
	}
	else
	{
		OUT_D1EBUG(textBuf,"error in UID write\r\n");
	}
}	   	    


void type_gps(char * typeofgps)
{

	s32 ret1;
	u32 writeedlen;
	s32 filehandle=-1;
	ret1 = Ql_FileOpenEx(PATH_CANPARA,QL_FS_CREATE);//open the file or else create the file
	//	OUT_D1EBUG(textBuf,""ret=%d\r\n",ret1);
	if(ret1 >= QL_RET_OK)
	{
		filehandle = ret1;
		ret1 = Ql_FileSeek(filehandle,45,QL_FS_FILE_BEGIN);
		ret1 = Ql_FileWrite(filehandle, (u8*)typeofgps,10,&writeedlen);
		OUT_D1EBUG(textBuf,"in write Unit_Type=%s\r\n",typeofgps);
		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"error in gps_type write\r\n");
	}
}	   	    

void type_unit(char * typeofunit)
{
	s32 ret1;
	u32 writeedlen;
	s32 filehandle=-1;
	ret1 = Ql_FileOpenEx(PATH_CANPARA,QL_FS_CREATE);//open the file or else create the file
	//	OUT_D1EBUG(textBuf,""ret=%d\r\n",ret1);
	if(ret1 >= QL_RET_OK)
	{
		filehandle = ret1;
		if((Ql_strstr((char *)typeofunit,"-") == NULL) || (Ql_strlen((char *)typeofunit) > 2))
		{
			ret1 = Ql_FileSeek(filehandle,9,QL_FS_FILE_BEGIN);
			ret1 = Ql_FileWrite(filehandle, (u8*)typeofunit,35,&writeedlen);
			//OUT_D1EBUG(textBuf,""\r\n Ql_FileWriteUnit_Type()=%d: writeedlen=%d\r\n",ret1, writeedlen);
			OUT_D1EBUG(textBuf,"in write Unit_Type=%s\r\n",typeofunit);
		}
		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"error in Unit_type write\r\n");
	}


}

void SI_TI(char *si_int,char *ti_int)
{

	s32 ret;
	u32 writeedlen;
	u8 k;
	s32 filehandle=-1;

	ret = Ql_FileOpenEx(PATH_CANPARA,QL_FS_CREATE);

	if(ret >= QL_RET_OK)
	{
		filehandle = ret;
		OUT_D1EBUG(textBuf,"TXI=%s\r\n",ti_int);
		if(Ql_strstr((char *)ti_int,"-") == NULL)
		{
			//ret = Ql_FileSeek(filehandle,CanCount,QL_FS_FILE_BEGIN);
			ret = Ql_FileSeek(filehandle,63,QL_FS_FILE_BEGIN);
			//ret = Ql_FileWrite(filehandle,(u8 *)TXI,Ql_strlen((char *)TXI),&writeedlen);
			Ql_Sleep(200);
			ret = Ql_FileWrite(filehandle,(u8 *)ti_int,6,&writeedlen);
			OUT_D1EBUG(textBuf,"Ql_CanTXIWrite()=%d: writeedlen=%d\r\n",ret,writeedlen);
		}
		/////-----------------------Write Remote Stamping in to memory-------------------------------/////
		//OUT_D1EBUG(textBuf,"Ql_strlen(UID)=%d\r\n",Ql_strlen((char *)UID));

		Ql_Sleep(200);
		OUT_D1EBUG(textBuf,"STI=%s\r\n",si_int);
		if(Ql_strstr((char *)si_int,"-") == NULL)
		{
			ret = Ql_FileSeek(filehandle,56,QL_FS_FILE_BEGIN);
			Ql_Sleep(200);
			ret = Ql_FileWrite(filehandle,(u8 *)si_int,6,&writeedlen);
			OUT_D1EBUG(textBuf,"Ql_CanSTIWrite()=%d: writeedlen=%d\r\n",ret,writeedlen);
		}

		Ql_FileClose(filehandle);
		filehandle = -1;
	}
}
/////Store Camera tx and RX
void Cam_Tx_Rx(char *Tx_int,char *Rx_int)
{
	s32 ret;
	u32 writeedlen;
	u8 k;
	s32 filehandle=-1;

	ret = Ql_FileOpenEx(PATH_CANPARA,QL_FS_CREATE);

	if(ret >= QL_RET_OK)
	{
		filehandle = ret;
		OUT_D1EBUG(textBuf,"camera capture Rx_int=%s\r\n",Rx_int);
		if(Ql_strstr((char *)Rx_int,"-") == NULL)
		{
			ret = Ql_FileSeek(filehandle,77,QL_FS_FILE_BEGIN);
			Ql_Sleep(200);
			ret = Ql_FileWrite(filehandle,(u8 *)Rx_int,6,&writeedlen);
			OUT_D1EBUG(textBuf,"Ql_Camera capture interval()=%d: writeedlen=%d\r\n",ret,writeedlen);
		}
		/////-----------------------Write Remote Stamping in to memory-------------------------------/////
		Ql_Sleep(200);
		OUT_D1EBUG(textBuf,"camera transmission Tx_int=%s\r\n",Tx_int);
		if(Ql_strstr((char *)Tx_int,"-") == NULL)
		{
			ret = Ql_FileSeek(filehandle,70,QL_FS_FILE_BEGIN);
			Ql_Sleep(200);
			ret = Ql_FileWrite(filehandle,(u8 *)Tx_int,6,&writeedlen);
			OUT_D1EBUG(textBuf,"Ql_CanCam_TxWrite()=%d: writeedlen=%d\r\n",ret,writeedlen);
		}
		Ql_FileClose(filehandle);
		filehandle = -1;
	}
}
///New Remote Parameters
void SIM_Apn(char *Sim_apn)
{
	s32 ret1,filehandle;
	char *ptr;
	u32 writeedlen,wcnt;
	ret1 = Ql_FileOpenEx((char*)Remote_Para,QL_FS_CREATE);
	wcnt=0;                                                    // 0 to 25
	if(ret1 >= QL_RET_OK)
	{
		filehandle = ret1;
		if((Ql_strstr((char *)Sim_apn,"-") == NULL) || (Ql_strlen((char *)Sim_apn) > 2))
		{
			ret1 = Ql_FileSeek(filehandle,wcnt,QL_FS_FILE_BEGIN);
			ret1 = Ql_FileWrite(filehandle, (u8*)Sim_apn,25,&writeedlen);                                   // Memory Location = 119 to 131
			OUT_D1EBUG(textBuf,"\r\n Ql_FileWritewrt_cnt()=%d: writeedlen=%d\r\n",ret1, writeedlen);
			OUT_D1EBUG(textBuf,"SIM APN write =%s,     ret = %d\r\n",Sim_apn,ret1);
		}

		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"error in SIM APN write=%d\r\n",ret1);
	}
}

void code_ver(void)
{
	s32 ret1;
	u32 writeedlen;
	s32 filehandle=-1;

	ret1 = Ql_FileOpenEx(PATH_CANPARA,QL_FS_CREATE);//open the file or else create the file
	OUT_D1EBUG(textBuf,"Write code version in memory =%s\r\n",Code_version);
	if(ret1 >= QL_RET_OK)
	{
		filehandle = ret1;
		ret1 = Ql_FileSeek(filehandle,90,QL_FS_FILE_BEGIN);
		ret1 = Ql_FileWrite(filehandle, (u8*)Code_version,25,&writeedlen);
		OUT_D1EBUG(textBuf,"in write Code_version=%s\r\n",Code_version);
		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"error in Code_version write\r\n");
	}
}

void remote_pararead(void)
{
	s32 ret1,ret,filehandle;
	char *ptr;
	u32 writeedlen,wcnt,readedlen1,FirstFlag=0;
	char remPara_readbuffer[50];
	char mprevtxtime[40]="\0";
	char Os_Limit1[15];
	char Os_Limit2[15];
	char Os_Limit[15];
	char RARD_Limit[15];
	///TCPIP
	char TCP_PORT[10];

	Ql_memset((ascii *)Os_Limit,'\0',sizeof(Os_Limit));
	Ql_memset((ascii *)Os_Limit1,'\0',sizeof(Os_Limit1));
	Ql_memset((ascii *)Os_Limit2,'\0',sizeof(Os_Limit2));
	Ql_memset((ascii *)RARD_Limit,'\0',sizeof(RARD_Limit));
	Ql_memset((ascii *)TCP_PORT,'\0',sizeof(TCP_PORT));


	OUT_D1EBUG(textBuf,"\r\n###### Device Remote Parameters  ######\r\n");

	ret1 = Ql_FileOpenEx((char*)Remote_Para,QL_FS_CREATE);
	wcnt=0;
	if(ret1 >= QL_RET_OK)
	{
		filehandle=ret1;

		//Read SIM APN
		Ql_memset((ascii *)remPara_readbuffer,'\0',sizeof(remPara_readbuffer));
		ret = Ql_FileSeek(filehandle,wcnt, QL_FS_FILE_BEGIN);
		ret = Ql_FileRead(filehandle, (u8 *)remPara_readbuffer,25, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d,,,,%s\r\n",ret,readedlen1,remPara_readbuffer);
		if(!(Ql_strncmp((ascii *)remPara_readbuffer,"\0",1)))
		{
			OUT_D1EBUG(textBuf,"\tAPN buffer empty\r\n");
			FirstFlag=1;
			//Ql_strcpy(remPara_readbuffer,(char *)"WWW");
			Ql_strcpy(remPara_readbuffer,(char *)"www");
			//Ql_strcpy(remPara_readbuffer,(char *)"airtelgprs.com");
			//SIM_Apn((char *)remPara_readbuffer);
			//OS_Lmt_wrt((char *)remPara_readbuffer);
		}

		Ql_strcpy(APN_NAME,(char *)remPara_readbuffer);
		Ql_strncat(APN_NAME,"\0",1);
		OUT_D1EBUG(textBuf,"\r\n\tSIM APN \t\t\t= %s",APN_NAME);

		//Read Over Speed Limit
		Ql_memset((ascii *)remPara_readbuffer,'\0',sizeof(remPara_readbuffer));
		ret = Ql_FileSeek(filehandle,26,QL_FS_FILE_BEGIN);								///Mem loc 26 from this file  use next from 100
		ret = Ql_FileRead(filehandle, (u8 *)remPara_readbuffer,8, &readedlen1);
		//	 OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d,,,,%s\r\n",ret,readedlen1,CanPara_readbuffer);
		if(!(Ql_strncmp((ascii *)remPara_readbuffer,"\0",1)))
		{
			OUT_D1EBUG(textBuf,"\r\n\tOS speed limit buffer empty.");
			Ql_strcpy(remPara_readbuffer,(char *)"65");
			//OS_Lmt_wrt((char *)remPara_readbuffer);
		}

		Ql_strcpy(Os_Limit,(char *)remPara_readbuffer);
		mOverSpeedLimit=Ql_atoi(remPara_readbuffer);
		OUT_D1EBUG(textBuf,"\r\n\tOS speed limit Read \t\t= %d",mOverSpeedLimit);

		//Read Over Speed Limit 1
		Ql_memset((ascii *)remPara_readbuffer,'\0',sizeof(remPara_readbuffer));
		ret = Ql_FileSeek(filehandle,211,QL_FS_FILE_BEGIN);								///Mem loc 211 from this file  use next from 100
		ret = Ql_FileRead(filehandle, (u8 *)remPara_readbuffer,8, &readedlen1);
		//	 OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d,,,,%s\r\n",ret,readedlen1,CanPara_readbuffer);
		if(!(Ql_strncmp((ascii *)remPara_readbuffer,"\0",1)))
		{
			OUT_D1EBUG(textBuf,"\r\n\tOS speed limit 1 buffer empty.");
			if(((Ql_strstr((char *)Unit_Type,"JRM") != NULL)||(Ql_strstr((char *)Unit_Type,"jrm") != NULL)))
			{
				Ql_strcpy(remPara_readbuffer,(char *)"30");
			}
			else
			{
				Ql_strcpy(remPara_readbuffer,(char *)"200");
			}
			//OS_Lmt_wrt((char *)remPara_readbuffer);
		}
		Ql_strcpy(Os_Limit1,(char *)remPara_readbuffer);
		mOverSpeedLimit1=Ql_atoi(remPara_readbuffer);
		OUT_D1EBUG(textBuf,"\r\n\tOS speed limit 1 Read \t\t= %d",mOverSpeedLimit1);

		//Read Over Speed Limit 2
		Ql_memset((ascii *)remPara_readbuffer,'\0',sizeof(remPara_readbuffer));
		ret = Ql_FileSeek(filehandle,221,QL_FS_FILE_BEGIN);								///Mem loc 211 from this file  use next from 100
		ret = Ql_FileRead(filehandle, (u8 *)remPara_readbuffer,8, &readedlen1);
		//	 OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d,,,,%s\r\n",ret,readedlen1,CanPara_readbuffer);
		if(!(Ql_strncmp((ascii *)remPara_readbuffer,"\0",1)))
		{
			OUT_D1EBUG(textBuf,"\r\n\tOS speed limit 2 buffer empty.");
			if(((Ql_strstr((char *)Unit_Type,"JRM") != NULL)||(Ql_strstr((char *)Unit_Type,"jrm") != NULL)))
			{
				Ql_strcpy(remPara_readbuffer,(char *)"40");
			}
			else
			{
				Ql_strcpy(remPara_readbuffer,(char *)"200");
			}
			//OS_Lmt_wrt((char *)remPara_readbuffer);
		}
		Ql_strcpy(Os_Limit2,(char *)remPara_readbuffer);
		mOverSpeedLimit2=Ql_atoi(remPara_readbuffer);
		OUT_D1EBUG(textBuf,"\r\n\tOS speed limit 2 Read \t\t= %d",mOverSpeedLimit2);

		//Read RA RD Limit
		Ql_memset((ascii *)remPara_readbuffer,'\0',sizeof(remPara_readbuffer));
		ret = Ql_FileSeek(filehandle,35,QL_FS_FILE_BEGIN);								///Mem loc 26 from this file  use next from 100
		ret = Ql_FileRead(filehandle, (u8 *)remPara_readbuffer,8, &readedlen1);
		//	 OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d,,,,%s\r\n",ret,readedlen1,CanPara_readbuffer);
		if(!(Ql_strncmp((ascii *)remPara_readbuffer,"\0",1)))
		{
			OUT_D1EBUG(textBuf,"\r\n\tRA/RD limit buffer empty.");
			Ql_strcpy(remPara_readbuffer,(char *)"15.00");
			//OS_Lmt_wrt((char *)remPara_readbuffer);
		}

		Ql_strcpy(RARD_Limit,(char *)remPara_readbuffer);
		//	mAccDcLimit=atofd(remPara_readbuffer);
		mAccDcLimit=(double)(Ql_atoi(remPara_readbuffer));
		OUT_D1EBUG(textBuf,"\r\n\tRA/RD limit Read \t\t= %f",mAccDcLimit);


		wcnt=45;
		Ql_memset(mprevtxtime,'\0',sizeof(mprevtxtime));
		Ql_memset(HOST_NAME,'\0',sizeof(HOST_NAME));
		ret = Ql_FileSeek(filehandle,wcnt, QL_FS_FILE_BEGIN); 		                // Memory Location = 45
		ret = Ql_FileRead(filehandle, (u8 *)mprevtxtime,30, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileRead unit id() = %d: readlength = %d\r\n",ret1, readedlen1);
		if(!(Ql_strncmp(mprevtxtime,"\0",1)))
		{
			Ql_strcpy(mprevtxtime,(char *)"ip.mobileeye.in");
			Ql_strcpy(HOST_NAME,(char *)"ip.mobileeye.in");
		}
		else
			Ql_strcat(HOST_NAME,mprevtxtime);

		OUT_D1EBUG(textBuf,"\r\n\tTCP/IP server address\t\t= %s",HOST_NAME);

		wcnt=76;
		Ql_memset(mprevtxtime,'\0',sizeof(mprevtxtime));
		ret = Ql_FileSeek(filehandle,wcnt, QL_FS_FILE_BEGIN); 		                // Memory Location = 76
		ret = Ql_FileRead(filehandle, (u8 *)mprevtxtime,10, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileRead unit id() = %d: readlength = %d\r\n",ret1, readedlen1);

		if(!(Ql_strncmp(mprevtxtime,"\0",1)))
		{
			Ql_strcpy(mprevtxtime,(char *)"1529");
		}

		Ql_strcpy(TCP_PORT,(char *)mprevtxtime);
		port = Ql_atoi(mprevtxtime);
		OUT_D1EBUG(textBuf,"\r\n\tTCP/IP Port\t\t\t= %d",port);

		///SMTP server address
		wcnt=87;
		Ql_memset(mprevtxtime,'\0',sizeof(mprevtxtime));
		Ql_memset(SMTP_SVR_ADDR,'\0',sizeof(SMTP_SVR_ADDR));
		ret = Ql_FileSeek(filehandle,wcnt, QL_FS_FILE_BEGIN); 		                // Memory Location = 50
		ret = Ql_FileRead(filehandle, (u8 *)mprevtxtime,30, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileRead unit id() = %d: readlength = %d\r\n",ret1, readedlen1);
		if(!(Ql_strncmp(mprevtxtime,"\0",1)))
		{
			//a.mobileeye.in
			//Ql_strcpy(mprevtxtime,(char *)"103.8.126.138");
			//Ql_strcpy(SMTP_SVR_ADDR,(char *)"103.8.126.138");
			Ql_strcpy(mprevtxtime,(char *)"a.mobileeye.in");
			Ql_strcpy(SMTP_SVR_ADDR,(char *)"a.mobileeye.in");
		}
		else
			Ql_strcat(SMTP_SVR_ADDR,mprevtxtime);

		OUT_D1EBUG(textBuf,"\r\n\tSMTP server address\t\t= %s",SMTP_SVR_ADDR);
		OUT_D1EBUG(textBuf,"\r\n\tSMTP server username\t\t= %s",SMTP_USER_NAME);
		//	OUT_D1EBUG(textBuf,"\r\n\tSMTP user address\t\t= %s",SMTP_ADDR);
		///SMTP Destination address
		wcnt=118;
		Ql_memset(mprevtxtime,'\0',sizeof(mprevtxtime));
		Ql_memset(SMTP_DST,'\0',sizeof(SMTP_DST));
		ret = Ql_FileSeek(filehandle,wcnt, QL_FS_FILE_BEGIN); 		                // Memory Location = 81
		ret = Ql_FileRead(filehandle, (u8 *)mprevtxtime,30, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileRead unit id() = %d: readlength = %d\r\n",ret1, readedlen1);
		if(!(Ql_strncmp(mprevtxtime,"\0",1)))
		{
			Ql_strcpy(mprevtxtime,(char *)"avlincident@twphd.in");
			Ql_strcpy(SMTP_DST,(char *)"avlincident@twphd.in");
		}
		else
			Ql_strcat(SMTP_DST,mprevtxtime);

		OUT_D1EBUG(textBuf,"\r\n\tSMTP Destination address\t= %s",SMTP_DST);

		///SMTP FROM mail address
		wcnt=149;
		Ql_memset(mprevtxtime,'\0',sizeof(mprevtxtime));
		Ql_memset(SMTP_ADDR,'\0',sizeof(SMTP_ADDR));
		ret = Ql_FileSeek(filehandle,wcnt, QL_FS_FILE_BEGIN); 		                // Memory Location = 81
		ret = Ql_FileRead(filehandle, (u8 *)mprevtxtime,30, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileRead unit id() = %d: readlength = %d\r\n",ret1, readedlen1);
		if(!(Ql_strncmp(mprevtxtime,"\0",1)))
		{
			Ql_strcpy(mprevtxtime,(char *)CAMUID);
			Ql_strcpy(SMTP_ADDR,(char *)CAMUID);
		}
		else
			Ql_strcat(SMTP_ADDR,mprevtxtime);

		OUT_D1EBUG(textBuf,"\r\n\tSMTP source mail address\t= %s",SMTP_ADDR);

		///SMTP Password
		wcnt=180;
		Ql_memset(mprevtxtime,'\0',sizeof(mprevtxtime));
		Ql_memset(SMTP_PASSWORD,'\0',sizeof(SMTP_PASSWORD));
		ret = Ql_FileSeek(filehandle,wcnt, QL_FS_FILE_BEGIN); 		                // Memory Location = 81
		ret = Ql_FileRead(filehandle, (u8 *)mprevtxtime,30, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileRead unit id() = %d: readlength = %d\r\n",ret1, readedlen1);
		if(!(Ql_strncmp(mprevtxtime,"\0",1)))
		{
			//transworld
			//Ql_strcpy(mprevtxtime,(char *)"dragonfly");
			//Ql_strcpy(SMTP_PASSWORD,(char *)"dragonfly");
			Ql_strcpy(mprevtxtime,(char *)"transworld");
			Ql_strcpy(SMTP_PASSWORD,(char *)"transworld");
		}
		else
			Ql_strcat(SMTP_PASSWORD,mprevtxtime);

		if(CANDUMPflg == 1)
		{
			OUT_D1EBUG(textBuf,"\r\n\tSMTP password\t\t\t= %s",SMTP_PASSWORD);
		}


		////FTP server details
		//FTP server
		wcnt=231;
		Ql_memset(mprevtxtime,'\0',sizeof(mprevtxtime));
		Ql_memset(FTP_SVR_ADDR,'\0',sizeof(FTP_SVR_ADDR));
		ret = Ql_FileSeek(filehandle,wcnt, QL_FS_FILE_BEGIN); 		                // Memory Location = 50
		ret = Ql_FileRead(filehandle, (u8 *)mprevtxtime,30, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileRead unit id() = %d: readlength = %d\r\n",ret1, readedlen1);
		if(!(Ql_strncmp(mprevtxtime,"\0",1)))
		{
			Ql_strcpy(mprevtxtime,(char *)"images.mobileeye.in");
			Ql_strcpy(FTP_SVR_ADDR,(char *)"images.mobileeye.in");
		}
		else
			Ql_strcat(FTP_SVR_ADDR,mprevtxtime);

		OUT_D1EBUG(textBuf,"\r\n\tFTP server address\t\t= %s",FTP_SVR_ADDR);

		///FTP username
		wcnt=262;
		Ql_memset(mprevtxtime,'\0',sizeof(mprevtxtime));
		Ql_memset(FTP_USER_NAME,'\0',sizeof(FTP_USER_NAME));
		ret = Ql_FileSeek(filehandle,wcnt, QL_FS_FILE_BEGIN); 		                // Memory Location = 81
		ret = Ql_FileRead(filehandle, (u8 *)mprevtxtime,30, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileRead unit id() = %d: readlength = %d\r\n",ret1, readedlen1);
		if(!(Ql_strncmp(mprevtxtime,"\0",1)))
		{
			Ql_strcpy(mprevtxtime,(char *)"cameraimages");
			Ql_strcpy(FTP_USER_NAME,(char *)"cameraimages");
		}
		else
			Ql_strcat(FTP_USER_NAME,mprevtxtime);

		//	OUT_D1EBUG(textBuf,"\r\n\tFTP username\t\t\t= %s",FTP_USER_NAME);

		///FTP paswword
		wcnt=293;
		Ql_memset(mprevtxtime,'\0',sizeof(mprevtxtime));
		Ql_memset(FTP_PASSWORD,'\0',sizeof(FTP_PASSWORD));
		ret = Ql_FileSeek(filehandle,wcnt, QL_FS_FILE_BEGIN); 		                // Memory Location = 81
		ret = Ql_FileRead(filehandle, (u8 *)mprevtxtime,30, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileRead unit id() = %d: readlength = %d\r\n",ret1, readedlen1);
		if(!(Ql_strncmp(mprevtxtime,"\0",1)))
		{
			Ql_strcpy(mprevtxtime,(char *)"Cimages@123");
			Ql_strcpy(FTP_PASSWORD,(char *)"Cimages@123");
		}
		else
			Ql_strcat(FTP_PASSWORD,mprevtxtime);

		if(CANDUMPflg ==1)
		{
			OUT_D1EBUG(textBuf,"\r\n\tFTP password\t\t\t= %s",FTP_PASSWORD);
		}

		///FTP port
		wcnt=325;
		Ql_memset(mprevtxtime,'\0',sizeof(mprevtxtime));
		Ql_memset(FTP_SVR_PORT,'\0',sizeof(FTP_SVR_PORT));
		ret = Ql_FileSeek(filehandle,wcnt, QL_FS_FILE_BEGIN); 		                // Memory Location = 81
		ret = Ql_FileRead(filehandle, (u8 *)mprevtxtime,30, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileRead unit id() = %d: readlength = %d\r\n",ret1, readedlen1);
		if(!(Ql_strncmp(mprevtxtime,"\0",1)))
		{
			Ql_strcpy(mprevtxtime,(char *)"21");
			Ql_strcpy(FTP_SVR_PORT,(char *)"21");
		}
		else
			Ql_strcat(FTP_SVR_PORT,mprevtxtime);

		OUT_D1EBUG(textBuf,"\r\n\tFTP Port\t\t\t= %s\r\n",FTP_SVR_PORT);

		Ql_FileClose(filehandle);
		filehandle = -1;

		if(FirstFlag == 1)
		{
			SIM_Apn((char *)APN_NAME);
			Ql_Sleep(100);
			OS_Lmt_wrt((char *)Os_Limit,(char *)RARD_Limit,(char *)Os_Limit1,(char *)Os_Limit2);
			Ql_Sleep(100);
			TCP_para((char *)HOST_NAME,(char *)TCP_PORT);
			Ql_Sleep(100);
			SMTP_para((char *)SMTP_SVR_ADDR,(char *)SMTP_DST,(char *)SMTP_ADDR,(char *)SMTP_PASSWORD);
			Ql_Sleep(100);
			FTP_para((char *)FTP_SVR_ADDR,(char *)FTP_USER_NAME,(char *)FTP_PASSWORD,(char *)FTP_SVR_PORT);
		}
	}
	else
	{
		OUT_D1EBUG(textBuf,"error in SIM APN Read\r\n");
	}
}

//use memloc 26 for OS speed

void OS_Lmt_wrt(char *OS_L,char *RARD_L,char *OS_Red,char *OS_Green)
{
	s32 ret;
	u32 writeedlen;
	u8 k;
	s32 filehandle=-1;

	ret = Ql_FileOpenEx(Remote_Para,QL_FS_CREATE);

	if(ret >= QL_RET_OK)
	{
		filehandle = ret;

		if(Ql_strstr((char *)OS_L,"-") == NULL)
		{
			OUT_D1EBUG(textBuf,"OS speed Limit=%s\r\n",OS_L);
			ret = Ql_FileSeek(filehandle,26,QL_FS_FILE_BEGIN);								///Mem loc 26 from this file  use next from 100
			Ql_Sleep(200);
			ret = Ql_FileWrite(filehandle,(u8 *)OS_L,8,&writeedlen);
			OUT_D1EBUG(textBuf,"OS limit Write()=%d: writeedlen=%d\r\n",ret,writeedlen);
		}
		//RA/RD limit
		if(Ql_strstr((char *)RARD_L,"-") == NULL)
		{
			OUT_D1EBUG(textBuf,"RA RD Limit=%s\r\n",RARD_L);
			ret = Ql_FileSeek(filehandle,35,QL_FS_FILE_BEGIN);								///Mem loc 35 from this file  use next from 100
			Ql_Sleep(200);
			ret = Ql_FileWrite(filehandle,(u8 *)RARD_L,8,&writeedlen);
			OUT_D1EBUG(textBuf,"RA/RD limit Write()=%d: writeedlen=%d\r\n",ret,writeedlen);
		}
		///OS limit 1
		if(Ql_strstr((char *)OS_Red,"-") == NULL)
		{
			OUT_D1EBUG(textBuf,"OS limit 1=%s\r\n",OS_Red);
			ret = Ql_FileSeek(filehandle,211,QL_FS_FILE_BEGIN);								///Mem loc 35 from this file  use next from 100
			Ql_Sleep(200);
			ret = Ql_FileWrite(filehandle,(u8 *)OS_Red,8,&writeedlen);
			OUT_D1EBUG(textBuf,"OS limit 1 Write()=%d: writeedlen=%d\r\n",ret,writeedlen);
		}
		///OS limit 2
		if(Ql_strstr((char *)OS_Green,"-") == NULL)
		{
			OUT_D1EBUG(textBuf,"OS limit 2=%s\r\n",OS_Green);
			ret = Ql_FileSeek(filehandle,221,QL_FS_FILE_BEGIN);								///Mem loc 35 from this file  use next from 100
			Ql_Sleep(200);
			ret = Ql_FileWrite(filehandle,(u8 *)OS_Green,8,&writeedlen);
			OUT_D1EBUG(textBuf,"OS limit 2 Write()=%d: writeedlen=%d\r\n",ret,writeedlen);
		}
		Ql_FileClose(filehandle);
		filehandle = -1;
	}
}

void TCP_para(char *Serv_ip,char *Serv_port)
{
	s32 ret1,filehandle;
	char *ptr;
	u32 writeedlen,wcnt;
	ret1 = Ql_FileOpenEx((char*)Remote_Para,QL_FS_CREATE);
	wcnt=45;
	OUT_D1EBUG(textBuf,"In TCP server parameters write\r\n");
	if(ret1 >= QL_RET_OK)
	{
		filehandle = ret1;
		if((Ql_strstr((char *)Serv_ip,"-") == NULL) || (Ql_strlen((char *)Serv_ip) > 2))
		{
			ret1 = Ql_FileSeek(filehandle,wcnt,QL_FS_FILE_BEGIN);
			ret1 = Ql_FileWrite(filehandle, (u8*)Serv_ip,30,&writeedlen);                                   // Memory Location = 45 to 75
			OUT_D1EBUG(textBuf,"\r\n Ql_FileWritewrt_cnt()=%d: writeedlen=%d\r\n",ret1, writeedlen);
			OUT_D1EBUG(textBuf,"TCP_IP_Server_ip=%s ,     ret=%d\r\n",Serv_ip,ret1);
		}
		wcnt=76;
		ret1 = Ql_FileSeek(filehandle,wcnt,QL_FS_FILE_BEGIN);
		if(Ql_strstr((char *)Serv_port,"-") == NULL)
		{
			ret1 = Ql_FileWrite(filehandle, (u8*)Serv_port,10,&writeedlen);                                   // Memory Location = 76 to 86
			OUT_D1EBUG(textBuf,"\r\n Ql_FileWritewrt_cnt()=%d: writeedlen=%d\r\n",ret1, writeedlen);
			OUT_D1EBUG(textBuf,"TCP_IP_Serv_port=%s ,     ret=%d\r\n",Serv_port,ret1);
		}
		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"error in TCP server parameters write\r\n");
	}
}


void SMTP_para(char *Serv_addrs,char *SMTP_dest,char *Frm_MAilid,char *passwd)
{
	s32 ret1,filehandle;
	char *ptr;
	u32 writeedlen,wcnt;
	ret1 = Ql_FileOpenEx((char*)Remote_Para,QL_FS_CREATE);

	OUT_D1EBUG(textBuf,"In SMTP server parameters write\r\n");
	if(ret1 >= QL_RET_OK)
	{
		filehandle = ret1;
		wcnt=87;
		if((Ql_strstr((char *)Serv_addrs,"-") == NULL) || (Ql_strlen((char *)Serv_addrs) > 2))
		{
			ret1 = Ql_FileSeek(filehandle,wcnt,QL_FS_FILE_BEGIN);
			ret1 = Ql_FileWrite(filehandle, (u8*)Serv_addrs,30,&writeedlen);                                   // Memory Location = 87 to 117
			//SYS_DEBUG(DBG_Buffer,"\r\n Ql_FileWritewrt_cnt()=%d: writeedlen=%d\r\n",ret1, writeedlen);
			OUT_D1EBUG(textBuf,"SMTP server Address=%s ,     ret=%d\r\n",Serv_addrs,ret1);
		}
		wcnt=118;
		if((Ql_strstr((char *)SMTP_dest,"-") == NULL) || (Ql_strlen((char *)SMTP_dest) > 2))
		{
			ret1 = Ql_FileSeek(filehandle,wcnt,QL_FS_FILE_BEGIN);
			ret1 = Ql_FileWrite(filehandle, (u8*)SMTP_dest,30,&writeedlen);                                   // Memory Location = 118 to 148
			//SYS_DEBUG(DBG_Buffer,"\r\n Ql_FileWritewrt_cnt()=%d: writeedlen=%d\r\n",ret1, writeedlen);
			OUT_D1EBUG(textBuf,"SMTP to Mail ID=%s ,     ret=%d\r\n",SMTP_dest,ret1);
		}
		wcnt=149;
		if((Ql_strstr((char *)Frm_MAilid,"-") == NULL) || (Ql_strlen((char *)Frm_MAilid) > 2))
		{
			ret1 = Ql_FileSeek(filehandle,wcnt,QL_FS_FILE_BEGIN);
			ret1 = Ql_FileWrite(filehandle, (u8*)Frm_MAilid,30,&writeedlen);                                   // Memory Location = 149 to 179
			//SYS_DEBUG(DBG_Buffer,"\r\n Ql_FileWritewrt_cnt()=%d: writeedlen=%d\r\n",ret1, writeedlen);
			OUT_D1EBUG(textBuf,"SMTP From Mail ID=%s ,     ret=%d\r\n",Frm_MAilid,ret1);
		}
		wcnt=180;
		if((Ql_strstr((char *)passwd,"-") == NULL) || (Ql_strlen((char *)passwd) > 2))
		{
			ret1 = Ql_FileSeek(filehandle,wcnt,QL_FS_FILE_BEGIN);
			ret1 = Ql_FileWrite(filehandle, (u8*)passwd,30,&writeedlen);                                   // Memory Location = 180 to 210
			//SYS_DEBUG(DBG_Buffer,"\r\n Ql_FileWritewrt_cnt()=%d: writeedlen=%d\r\n",ret1, writeedlen);
			OUT_D1EBUG(textBuf,"SMTP Password=%s ,     ret=%d\r\n",passwd,ret1);
		}
		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"error in SMTP server parameters write\r\n");
	}
}

void FTP_para(char *Serv_addrs,char *UsrName,char *PassWrd,char *Port)
{
	s32 ret1,filehandle;
	char *ptr;
	u32 writeedlen,wcnt;
	ret1 = Ql_FileOpenEx((char*)Remote_Para,QL_FS_CREATE);

	OUT_D1EBUG(textBuf,"In FTP server parameters write\r\n");
	if(ret1 >= QL_RET_OK)
	{
		filehandle = ret1;
		wcnt=231;
		if((Ql_strstr((char *)Serv_addrs,"-") == NULL) || (Ql_strlen((char *)Serv_addrs) > 2))
		{
			ret1 = Ql_FileSeek(filehandle,wcnt,QL_FS_FILE_BEGIN);
			ret1 = Ql_FileWrite(filehandle, (u8*)Serv_addrs,30,&writeedlen);                                   // Memory Location = 231
			//SYS_DEBUG(DBG_Buffer,"\r\n Ql_FileWritewrt_cnt()=%d: writeedlen=%d\r\n",ret1, writeedlen);
			OUT_D1EBUG(textBuf,"FTP server Address=%s ,     ret=%d\r\n",Serv_addrs,ret1);
		}
		wcnt=262;
		if((Ql_strstr((char *)UsrName,"-") == NULL) || (Ql_strlen((char *)UsrName) > 2))
		{
			ret1 = Ql_FileSeek(filehandle,wcnt,QL_FS_FILE_BEGIN);
			ret1 = Ql_FileWrite(filehandle, (u8*)UsrName,30,&writeedlen);                                   // Memory Location = 262
			//SYS_DEBUG(DBG_Buffer,"\r\n Ql_FileWritewrt_cnt()=%d: writeedlen=%d\r\n",ret1, writeedlen);
			OUT_D1EBUG(textBuf,"Ftp User name=%s ,     ret=%d\r\n",UsrName,ret1);
		}
		wcnt=293;
		if((Ql_strstr((char *)PassWrd,"-") == NULL) || (Ql_strlen((char *)PassWrd) > 2))
		{
			ret1 = Ql_FileSeek(filehandle,wcnt,QL_FS_FILE_BEGIN);
			ret1 = Ql_FileWrite(filehandle, (u8*)PassWrd,30,&writeedlen);                                   // Memory Location = 293
			//SYS_DEBUG(DBG_Buffer,"\r\n Ql_FileWritewrt_cnt()=%d: writeedlen=%d\r\n",ret1, writeedlen);
			OUT_D1EBUG(textBuf,"FTP Pass=%s ,     ret=%d\r\n",PassWrd,ret1);
		}
		wcnt=325;
		if(Ql_strstr((char *)Port,"-") == NULL)
		{
			ret1 = Ql_FileSeek(filehandle,wcnt,QL_FS_FILE_BEGIN);
			ret1 = Ql_FileWrite(filehandle, (u8*)Port,10,&writeedlen);                                   // Memory Location = 325
			//SYS_DEBUG(DBG_Buffer,"\r\n Ql_FileWritewrt_cnt()=%d: writeedlen=%d\r\n",ret1, writeedlen);
			OUT_D1EBUG(textBuf,"FTP PORT=%s ,     ret=%d\r\n",Port,ret1);
		}
		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"error in SMTP server parameters write\r\n");
	}
}
/*
void OS_Limit_read(void)
{
	s32 ret;
	u32 writeedlen,readedlen1;
	u8 k;
	s32 filehandle=-1;
	char remPara_readbuffer[50];

	ret = Ql_FileOpenEx(Remote_Para,QL_FS_CREATE);

	if(ret >= QL_RET_OK)
	{
		filehandle = ret;
		Ql_memset((ascii *)remPara_readbuffer,'\0',sizeof(remPara_readbuffer));
		ret = Ql_FileSeek(filehandle,26,QL_FS_FILE_BEGIN);								///Mem loc 26 from this file  use next from 100
		ret = Ql_FileRead(filehandle, (u8 *)remPara_readbuffer,8, &readedlen1);
		//	 OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d,,,,%s\r\n",ret,readedlen1,CanPara_readbuffer);
		if(!(Ql_strncmp((ascii *)remPara_readbuffer,"\0",1)))
		{
			OUT_D1EBUG(textBuf,"OS speed limit buffer empty\r\n");
			Ql_strcpy(remPara_readbuffer,(char *)"65");
			OS_Lmt_wrt((char *)remPara_readbuffer);
		}

		mOverSpeedLimit=Ql_atoi(remPara_readbuffer);
		OUT_D1EBUG(textBuf,"OS speed limit Read = %d\r\n",mOverSpeedLimit);

		Ql_FileClose(filehandle);
		filehandle = -1;
	}
}*/


void CAN_Dump(void)
{
	JRM_DATA_READ();               ///for JRM DATA
	Ql_Sleep(500);
	tw_updatedCanParaRead();			//to read parameters
	Ql_Sleep(500);
	tw_para_read();
	Ql_Sleep(500);
	remote_pararead();
	Ql_Sleep(500);
	read_Cell_ID_flg();				////Cell ID enable Flag
	Ql_Sleep(500);
	cam_mem_loc_read();				////for camera memory locations
	Ql_Sleep(500);
	cam_mem_loc_readextra();
	Ql_Sleep(500);
	camR_EXCP_mem_loc_read();
	Ql_Sleep(500);
	cam_C_EXCP_mem_loc_read();
	Ql_Sleep(500);
	Accdata_wrtcnt_read();  //RAHMAN
	Ql_Sleep(500);
	Accdata_flags_read();
	Ql_Sleep(500);
	read_stop_acc_capt();
	Ql_Sleep(500);
	dump_cnt_read();
	Ql_Sleep(500);
	CANDUMPflg=0;
	OUT_D1EBUG(textBuf,"\r\n############## CAN DUMP completed. ##########\r\n");
}

///Power Status write
void PO_STAT_WRITE(void)
{
	s32 ret1;
	s32 battfilehandle;
	char *ptr;
	u32 writeedlen;
	ret1 = Ql_FileOpenEx((char*)BATTRYIND,QL_FS_CREATE);
	OUT_D1EBUG(textBuf,"\r\n Write power status in memory ..\r\n");
	if(ret1 >= QL_RET_OK)
	{
		battfilehandle = ret1;
		ptr=ix_Itoa(PO_STATUS);
		OUT_D1EBUG(textBuf,"Ascii PO_STATUS=%s\r\n",ptr);
		ret1 = Ql_FileSeek(battfilehandle,0,QL_FS_FILE_BEGIN);
		ret1 = Ql_FileWrite(battfilehandle, (u8*)ptr,1,&writeedlen);
		OUT_D1EBUG(textBuf,"\r\n Ql_FileWritewrt_cnt()=%d: writeedlen=%d\r\n",ret1, writeedlen);

		Ql_FileClose(battfilehandle);
		battfilehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"error in power status flags write.\r\n");
	}
}

///Power ON stamp generation flag
void PO_GEN_WRITE(void)
{
	s32 ret1;
	s32 battfilehandle;
	char *ptr;
	u32 writeedlen;
	//ret1 = Ql_FileOpenEx((u8*)"Power_state.txt",QL_FS_CREATE);

	ret1 = Ql_FileOpenEx((char*)BATTRYIND,QL_FS_CREATE);
	OUT_D1EBUG(textBuf,"\r\n Write Power ON stamp generation flag in memory  \" POStmpGEN = %d \"\r\n",POStmpGEN);
	if(ret1 >= QL_RET_OK)
	{
		battfilehandle = ret1;
		ptr=ix_Itoa(POStmpGEN);
		OUT_D1EBUG(textBuf,"Ascii POStmpGEN=%s\r\n",ptr);
		ret1 = Ql_FileSeek(battfilehandle,2,QL_FS_FILE_BEGIN);
		ret1 = Ql_FileWrite(battfilehandle, (u8*)ptr,1,&writeedlen);
		OUT_D1EBUG(textBuf,"\r\n Ql_FileWritewrt_cnt()=%d: writeedlen=%d\r\n",ret1, writeedlen);

		Ql_FileClose(battfilehandle);
		battfilehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"error in Power ON stamp generation write\r\n");
	}
}

///Power OFF stamp generation flag

void PF_GEN_WRITE(void)
{
	s32 ret1;
	s32 battfilehandle;
	char *ptr;
	u32 writeedlen;
	ret1 = Ql_FileOpenEx((char*)BATTRYIND,QL_FS_CREATE);
	OUT_D1EBUG(textBuf,"\r\n Write Power OFF stamp generation flag in memory \" PFStmpGEN = %d \"\r\n",PFStmpGEN);
	if(ret1 >= QL_RET_OK)
	{
		battfilehandle = ret1;
		ptr=ix_Itoa(PFStmpGEN);
		OUT_D1EBUG(textBuf,"Ascii PFStmpGEN=%s\r\n",ptr);
		ret1 = Ql_FileSeek(battfilehandle,4,QL_FS_FILE_BEGIN);
		ret1 = Ql_FileWrite(battfilehandle, (u8*)ptr,1,&writeedlen);
		OUT_D1EBUG(textBuf,"\r\n Ql_FileWritewrt_cnt()=%d: writeedlen=%d\r\n",ret1, writeedlen);

		Ql_FileClose(battfilehandle);
		battfilehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"error in Power OFF stamp generation write\r\n");
	}
}
void PO_STAT_READ(void)
{
	s32 ret;
	u32 readedlen1;
	u32 filehandle;
	char battflgreadbuff[9];

	//ret = Ql_FileOpenEx((u8*)"Power_state.txt",QL_FS_CREATE);
	ret = Ql_FileOpenEx((char*)BATTRYIND,QL_FS_CREATE);
	OUT_D1EBUG(textBuf,"*******************BATTERY FLAGS READING FUNCTION************************\r\n");

	if(ret >= QL_RET_OK)
	{
		filehandle = ret;
		Ql_memset((ascii *)battflgreadbuff,'\0',sizeof(battflgreadbuff));
		ret = Ql_FileSeek(filehandle,0, QL_FS_FILE_BEGIN);  
		ret = Ql_FileRead(filehandle, (u8 *)battflgreadbuff,1, &readedlen1);
		///POWER status
		PO_STATUS=Ql_atoi(battflgreadbuff);
		OUT_D1EBUG(textBuf,"READ--PO_STATUS = %d,Buffer=%s\r\n",PO_STATUS,battflgreadbuff);

		if(PO_STATUS == 0)
		{
			OUT_D1EBUG(textBuf,"Device Was On Battery\r\n");
		}
		else if(PO_STATUS == 1)
		{
			OUT_D1EBUG(textBuf,"Device Was On Mains\r\n");
		}

		//PO stamp generation Flag
		Ql_memset((ascii *)battflgreadbuff,'\0',sizeof(battflgreadbuff));
		ret = Ql_FileSeek(filehandle,2, QL_FS_FILE_BEGIN);
		ret = Ql_FileRead(filehandle, (u8 *)battflgreadbuff,1, &readedlen1);
		POStmpGEN=Ql_atoi(battflgreadbuff);
		OUT_D1EBUG(textBuf,"PO generation Flag = %d,Buffer=%s\r\n",POStmpGEN,battflgreadbuff);

		//PF stamp GENERATION FLAG
		Ql_memset((ascii *)battflgreadbuff,'\0',sizeof(battflgreadbuff));
		ret = Ql_FileSeek(filehandle,4, QL_FS_FILE_BEGIN);
		ret = Ql_FileRead(filehandle, (u8 *)battflgreadbuff,1, &readedlen1);
		PFStmpGEN=Ql_atoi(battflgreadbuff);
		OUT_D1EBUG(textBuf,"PF generation Flag = %d,Buffer=%s\r\n",PFStmpGEN,battflgreadbuff);

		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"Error in Power_state reading**1\r\n");
	}
}


void Gen_DevDetail_Stmp(void)
{
	char DevDet[90];
    Ql_memset((ascii *)DevDet,'\0',sizeof(DevDet));

	Ql_strcpy(DevDet,UID);
	Ql_strncat(DevDet,"_DEVDET,",7);
	Ql_strcat(DevDet,UID);
	Ql_strncat(DevDet,",",1);
	Ql_strcat(DevDet,Unit_Type);
	Ql_strncat(DevDet,",",1);
	Ql_strcat(DevDet,gps_type);
	Ql_strncat(DevDet,"\r\n",2);

	OUT_D1EBUG(textBuf,"Device Detail Stamp=%s:\r\n",DevDet);

	tw_filewrite((char *)DevDet);
}
