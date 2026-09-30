
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
#include "GSM_GPRS.h"
#include "SEND_DATA.h"
#include "TCP_IP.h"
#include "MRW.h"  
#include "para_read.h"
#include "sms_handle.h"
#include "Ql_filesystem.h"

/*-------------------------------------------------------------------------*/
/*								Globals									   */
/*-------------------------------------------------------------------------*/
#define OUT_D1EBUG(x,...)  \
		Ql_memset((x),0,100);  \
		Ql_sprintf((x),__VA_ARGS__);   \
		Ql_SendToUart(ql_uart_port2,(u8 *)(x),Ql_strlen(x));


#define  PATH_CANPARA ((u8 *)"updatedcanpara.txt")
#define  Remote_Para ((u8 *)"remotepara.txt")
extern  char gps_type[10];
extern char Flag_readbuffer[5];
extern char CanFlag_readbuffer[2];
extern char CanPara_readbuffer[15];
extern char CVread[25];
extern u8 CanUpdate_flag;
extern unsigned char ItoaStr[];
extern char UID[];
extern char UID_Fota[15];
extern char textBuf[];
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
extern char GPRMC[];
extern double mTotDist;
extern int uwrt_cnt;
extern int uread_cnt;
extern char mprevDist[8]; 
extern char mureadcnt[8]; 
extern u8 BackUp_Flag;
extern char CV[15];
extern int modified_flag;
extern char Unit_Type[10];
extern char GPRS_APN[];

char SIMProv[20]="\0";
char apnsim[25]="\0";
bool gps_g2=0;
bool gps_g3=0;
bool gps_g4=0;
bool gps_g5=0;

void tw_updatedCanParaRead(void)
{
	s32 ret;
	u32 readedlen1;
	u16 CanCount;
	s32 filehandle=-1;

	Ql_memset((ascii *)TXI,'\0',sizeof(TXI));
	filehandle = Ql_FileOpenEx(PATH_CANPARA,QL_FS_CREATE);
	OUT_D1EBUG(textBuf,"tw_CanParaRead Ql_FileOpenEx ret = %d\r\n",ret);
	if(filehandle >= QL_RET_OK)
	{
		//--------------------Can UID-------------------------------------
		Ql_memset((ascii *)CanPara_readbuffer,'\0',sizeof(CanPara_readbuffer));
		ret = Ql_FileSeek(filehandle,1, QL_FS_FILE_BEGIN);  
		ret = Ql_FileRead(filehandle, (u8 *)CanPara_readbuffer,8, &readedlen1);
		OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d\r\n",ret, readedlen1);
		Ql_strcpy(UID,(char *)CanPara_readbuffer);
		Ql_strncat(UID,"\0",1);    
		Ql_strcpy(UID_Fota,UID);
		OUT_D1EBUG(textBuf,"UID Read = %s\r\n",UID);

		//--------------------Can STI-------------------------------------
		Ql_memset((ascii *)CanPara_readbuffer,'\0',sizeof(CanPara_readbuffer));
		Ql_memset((ascii *)STI,'\0',sizeof(STI));		
		ret = Ql_FileSeek(filehandle,56, QL_FS_FILE_BEGIN); 
		ret = Ql_FileRead(filehandle, (u8 *)CanPara_readbuffer,6, &readedlen1);
		OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d\r\n",ret, readedlen1);
		Ql_strcpy(STI,(char *)CanPara_readbuffer);
		Ql_memset((ascii *)CanPara_readbuffer,'\0',sizeof(CanPara_readbuffer));
		Ql_strncat(STI,"\0",1);
		OUT_D1EBUG(textBuf,"STI Read = %s\r\n",STI);

		//--------------------Can TXI-------------------------------------
		Ql_memset((ascii *)CanPara_readbuffer,'\0',sizeof(CanPara_readbuffer));
		ret = Ql_FileSeek(filehandle,63, QL_FS_FILE_BEGIN);  
		ret = Ql_FileRead(filehandle, (u8 *)CanPara_readbuffer,6, &readedlen1);
		OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d\r\n",ret, readedlen1);
		Ql_strcpy(TXI,(char *)CanPara_readbuffer);
		Ql_strncat(TXI,"\0",1);
		OUT_D1EBUG(textBuf,"TXI Read = %s\r\n",TXI);

		//--------------------Can gps_type-------------------------------------
		Ql_memset((ascii *)CanPara_readbuffer,'\0',sizeof(CanPara_readbuffer));
		ret = Ql_FileSeek(filehandle,45, QL_FS_FILE_BEGIN);  
		ret = Ql_FileRead(filehandle, (u8 *)CanPara_readbuffer,10, &readedlen1);
		OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d,,,,,%s\r\n",ret,readedlen1,CanPara_readbuffer);
		Ql_strcpy(gps_type,(char *)CanPara_readbuffer);
		Ql_strncat(gps_type,"\0",1);
		OUT_D1EBUG(textBuf,"gps_type Read = %s\r\n",gps_type);
		if(!(Ql_strcmp((char *)gps_type,"UP501B")))
		{
			OUT_D1EBUG(textBuf,"GPS Type is UP501B\r\n");
			gps_g3=1;
		}
		else if(!(Ql_strcmp((char *)gps_type,"GR301")))
		{
			OUT_D1EBUG(textBuf,"GPS Type is GR301\r\n");
			gps_g2=1;
		}
		else if(!(Ql_strcmp((char *)gps_type,"L50")))
		{
			OUT_D1EBUG(textBuf,"GPS Type is L50\r\n");
			gps_g4=1;
		}
		else if(!(Ql_strcmp((char *)gps_type,"L80")))
		{
			OUT_D1EBUG(textBuf,"GPS Type is L80\r\n");
			gps_g5=1;
		}

		//--------------------Can Unit_Type-------------------------------------
		Ql_memset((ascii *)CanPara_readbuffer,'\0',sizeof(CanPara_readbuffer));
		ret = Ql_FileSeek(filehandle,9, QL_FS_FILE_BEGIN);  
		ret = Ql_FileRead(filehandle, (u8 *)CanPara_readbuffer,35, &readedlen1);
		OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d,,,,%s\r\n",ret,readedlen1,CanPara_readbuffer);
		Ql_strcpy(Unit_Type,(char *)CanPara_readbuffer);
		Ql_strncat(Unit_Type,"\0",1);
		OUT_D1EBUG(textBuf,"Unit_type Read = %s\r\n",Unit_Type);
		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"Error in Canfile reading**1 tw_CanParaRead\r\n");
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
		if(Ql_strstr((char *)Sim_apn,"-") == NULL)
		{
			ret1 = Ql_FileSeek(filehandle,wcnt,QL_FS_FILE_BEGIN);
			ret1 = Ql_FileWrite(filehandle, (u8*)Sim_apn,25,&writeedlen);                                   // Memory Location = 119 to 131
			OUT_D1EBUG(textBuf,"\r\n Ql_FileWritewrt_cnt()=%d: writeedlen=%d\r\n",ret1, writeedlen);
			OUT_D1EBUG(textBuf,"SIM APN write =%s,     ret = %d\r\n",Sim_apn,ret1);
		}

		Ql_FileClose(filehandle);
		filehandle = -1;

		Ql_strcpy(GPRS_APN,(char *)Sim_apn);
		Ql_strncat(GPRS_APN,"\0",1);
		OUT_D1EBUG(textBuf,"SIM APN = %s\r\n\r\n",GPRS_APN);
	}
	else
	{
		OUT_D1EBUG(textBuf,"error in SIM APN write=%d\r\n",ret1);
	}
}
void remote_pararead_APN(void)
{
	s32 ret1,ret,filehandle;
	char *ptr;
	u32 writeedlen,wcnt,readedlen1,FirstFlag=0;
	char remPara_readbuffer[50];
	char mprevtxtime[40]="\0";

	//	OUT_D1EBUG(textBuf,"\r\n###### Device Remote Parameters  ######\r\n");

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
		/*if(!(Ql_strncmp((ascii *)remPara_readbuffer,"\0",1)))
		{
			OUT_D1EBUG(textBuf,"\tAPN buffer empty\r\n");
			FirstFlag=1;
			//Ql_strcpy(remPara_readbuffer,(char *)"WWW");
			Ql_strcpy(remPara_readbuffer,(char *)"WWW");
			//Ql_strcpy(remPara_readbuffer,(char *)"airtelgprs.com");
			//SIM_Apn((char *)remPara_readbuffer);
			//OS_Lmt_wrt((char *)remPara_readbuffer);
		}*/

		Ql_FileClose(filehandle);
		filehandle = -1;

		Ql_strcpy(GPRS_APN,(char *)remPara_readbuffer);
		Ql_strncat(GPRS_APN,"\0",1);
		OUT_D1EBUG(textBuf,"SIM APN = %s\r\n\r\n",GPRS_APN);
	}
}
void newunit_id(char * unit_id)
{
	u8 i;
	s32 ret1;
	u32 writeedlen;
	s32 filehandle=-1;
	char tempuid[8];
	ret1 = Ql_FileOpenEx(PATH_CANPARA,QL_FS_CREATE);//open the file or else create the file
	if(ret1 >= QL_RET_OK)
	{
		/*  tempuid[0]='U';
	    tempuid[1]='I';
		tempuid[2]=unit_id[0];
		tempuid[3]=unit_id[1];
		tempuid[4]=unit_id[2];
		tempuid[5]=unit_id[3];
		tempuid[6]='\0';*/
		filehandle = ret1;
		ret1 = Ql_FileSeek(filehandle,1,QL_FS_FILE_BEGIN);
		ret1 = Ql_FileWrite(filehandle, (u8*)unit_id,8,&writeedlen);
		OUT_D1EBUG(textBuf,"Unit ID is=%s\r\n",unit_id);
		Ql_FileClose(filehandle);
		filehandle = -1;
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

void FunSIMProvider(char *SIMp)
{
	s32 ret1,filehandle;
	u32 writeedlen,wcnt;


	Ql_memset(apnsim,'\0',sizeof(apnsim));
	/*	ret1 = Ql_FileOpenEx(PATH_CANPARA,QL_FS_CREATE);//open the file or else create the file
	wcnt=380;                                                    // 97 to 133

	if(ret1 >= QL_RET_OK)
	{
		filehandle = ret1;
		ret1 = Ql_FS_Seek(filehandle,wcnt,QL_FS_FILE_BEGIN);
		ret1 = Ql_FS_Write(filehandle, (u8*)SIMp,20,&writeedlen);                                   // Memory Location = 226 to 246
		//	SYS_DEBUG(DBG_Buffer,"\r\n Ql_FileWritewrt_cnt()=%d: writeedlen=%d\r\n",ret1, writeedlen);
		OUT_D1EBUG(textBuf,"SIM Provider write =%s,     ret = %d\r\n",SIMp,ret1);
		//Ql_memset(SIM_No,'\0',sizeof(n));

		Ql_FS_Close(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"error in SIM Provider write\r\n");
	}*/

	if((Ql_strstr(SIMProv,("airtel")) || Ql_strstr(SIMProv,("AIRTEL"))))
	{
		Ql_strcat(apnsim,(char *)"airtelgprs.com");
		SIM_Apn((char *)apnsim);
	}
	else if((Ql_strstr(SIMProv,("vodafone")) || Ql_strstr(SIMProv,("VODAFONE"))))
	{
		Ql_strcat(apnsim,(char *)"www");
		SIM_Apn((char *)apnsim);
	}
	else if((Ql_strstr(SIMProv,("reliance")) || Ql_strstr(SIMProv,("RELIANCE"))))
	{
		Ql_strcat(apnsim,(char *)"rcomnet");
		SIM_Apn((char *)apnsim);
	}
	else if((Ql_strstr(SIMProv,("idea")) || Ql_strstr(SIMProv,("IDEA"))))
	{
		Ql_strcat(apnsim,(char *)"internet");
		SIM_Apn((char *)apnsim);
	}
	else if((Ql_strstr(SIMProv,("BSNL")) || Ql_strstr(SIMProv,("bsnl"))))
	{
		Ql_strcat(apnsim,(char *)"bsnlnet");
		SIM_Apn((char *)apnsim);
	}
	else if((Ql_strstr(SIMProv,("TATADOCOMO")) || Ql_strstr(SIMProv,("tatadocomo"))))
	{
		Ql_strcat(apnsim,(char *)"TATA.DOCOMO.INTERNET");
		SIM_Apn((char *)apnsim);
	}
	else if((Ql_strstr(SIMProv,("aircel")) || Ql_strstr(SIMProv,("AIRTEL"))))
	{
		Ql_strcat(apnsim,(char *)"aircelgprs.po");
		SIM_Apn((char *)apnsim);
	}
}
void type_unit(char * typeofunit)
{
	s32 ret1;
	u32 writeedlen;
	s32 filehandle=-1;
	ret1 = Ql_FileOpenEx(PATH_CANPARA,QL_FS_CREATE);//open the file or else create the file
	if(ret1 >= QL_RET_OK)
	{
		filehandle = ret1;
		ret1 = Ql_FileSeek(filehandle,9,QL_FS_FILE_BEGIN);
		ret1 = Ql_FileWrite(filehandle, (u8*)typeofunit,35,&writeedlen);
		OUT_D1EBUG(textBuf,"in write Unit_Type=%s\r\n",typeofunit);
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
		ret = Ql_FileSeek(filehandle,63,QL_FS_FILE_BEGIN);
		ret = Ql_FileWrite(filehandle,(u8 *)ti_int,6,&writeedlen);
		OUT_D1EBUG(textBuf,"Ql_CanTXIWrite()=%d: writeedlen=%d\r\n",ret,writeedlen);

		/////-----------------------Write Remote Stamping in to memory-------------------------------/////
		OUT_D1EBUG(textBuf,"STI=%s\r\n",si_int);
		ret = Ql_FileSeek(filehandle,56,QL_FS_FILE_BEGIN);
		ret = Ql_FileWrite(filehandle,(u8 *)si_int,6,&writeedlen);
		OUT_D1EBUG(textBuf,"Ql_CanSTIWrite()=%d: writeedlen=%d\r\n",ret,writeedlen);
		Ql_FileClose(filehandle);
		filehandle = -1;
	}
}

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
			//OUT_D1EBUG(textBuf,"OS speed Limit=%s\r\n",OS_L);
			OUT_D1EBUG(textBuf,"O =%s\r\n",OS_L);
			ret = Ql_FileSeek(filehandle,26,QL_FS_FILE_BEGIN);								///Mem loc 26 from this file  use next from 100
			Ql_Sleep(200);
			ret = Ql_FileWrite(filehandle,(u8 *)OS_L,8,&writeedlen);
			OUT_D1EBUG(textBuf,"Write()=%d: writeedlen=%d\r\n",ret,writeedlen);
		}
		//RA/RD limit
		if(Ql_strstr((char *)RARD_L,"-") == NULL)
		{
			OUT_D1EBUG(textBuf,"%s\r\n",RARD_L);
			ret = Ql_FileSeek(filehandle,35,QL_FS_FILE_BEGIN);								///Mem loc 35 from this file  use next from 100
			Ql_Sleep(200);
			ret = Ql_FileWrite(filehandle,(u8 *)RARD_L,8,&writeedlen);
			OUT_D1EBUG(textBuf,"Write()=%d: writeedlen=%d\r\n",ret,writeedlen);
		}
		///OS limit 1
		if(Ql_strstr((char *)OS_Red,"-") == NULL)
		{
			OUT_D1EBUG(textBuf,"O1=%s\r\n",OS_Red);
			ret = Ql_FileSeek(filehandle,211,QL_FS_FILE_BEGIN);								///Mem loc 35 from this file  use next from 100
			Ql_Sleep(200);
			ret = Ql_FileWrite(filehandle,(u8 *)OS_Red,8,&writeedlen);
			OUT_D1EBUG(textBuf," Write()=%d: writeedlen=%d\r\n",ret,writeedlen);
		}
		///OS limit 2
		if(Ql_strstr((char *)OS_Green,"-") == NULL)
		{
			OUT_D1EBUG(textBuf,"O2=%s\r\n",OS_Green);
			ret = Ql_FileSeek(filehandle,221,QL_FS_FILE_BEGIN);								///Mem loc 35 from this file  use next from 100
			Ql_Sleep(200);
			ret = Ql_FileWrite(filehandle,(u8 *)OS_Green,8,&writeedlen);
			OUT_D1EBUG(textBuf,"Write()=%d: writeedlen=%d\r\n",ret,writeedlen);
		}
		Ql_FileClose(filehandle);
		filehandle = -1;
	}
}
