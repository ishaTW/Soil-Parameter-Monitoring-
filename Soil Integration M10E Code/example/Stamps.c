/*-------------------------------------------------------------------------*/
/*  File       : Stamps.c                                                 
                 NGPRS,NGSM,NG stamp generation 
-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------*/
/*								Headers									   */
/*-------------------------------------------------------------------------*/

#include<stdio.h>
#include<string.h>
#include "ql_trace.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "Ql_filesystem.h"
#include "Ql_tcpip.h"
#include "ql_error.h"
#include "Fun.h"
#include "GPS.h"
#include "GSM_GPRS.h"
#include "TCP_IP.h"
#include "MRW.h" 
#include "ql_fcm.h"
#include "SEND_DATA.h"
#include "para_read.h"
#include "Stamps.h"


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

    
extern char textBuf[1000];
extern char GPRMC[];
extern char UID[];
extern char STAT[];
extern double mCurrSpeed,mTotDist;
extern unsigned char ItoaStr[];
extern char pfile[10];
extern int trackflsent;
extern bool camdumpflag;

/*-------------------------------------------------------------------------*/
/*  Function   : Gen_NGSM		                                           */
/*-------------------------------------------------------------------------*/
/*  Object     : NGSM Stamp Generation				                       */
/*-------------------------------------------------------------------------*/

void Gen_NGSM(void)
{
    char NGSMstring[90]; 
    Ql_memset((ascii *)NGSMstring,'\0',sizeof(NGSMstring));
	Ql_strcpy(NGSMstring,UID);
	Ql_strncat(NGSMstring,"_NGSM,",6);
	Ql_strcat(NGSMstring,GPRMC);
	OUT_D1EBUG(textBuf,"GPRMC111=%s:\r\n",GPRMC);
    ix_Itoa(mCurrSpeed);
	OUT_D1EBUG(textBuf,"FtoaStrcurre%s\r\n",ItoaStr);
	Ql_strcat(NGSMstring,(char *)ItoaStr);///mCurrSpeed speed
	Ql_strncat(NGSMstring,",",1);
	Ql_strcat(NGSMstring,(char *)(ix_Itoa(mTotDist)));
	Ql_strncat(NGSMstring,",",1);
	Ql_strncat(NGSMstring,"0.0",5);		//PDOP
    Ql_strncat(NGSMstring,",",1);
	Ql_strncat(NGSMstring,STAT,1);			//Status (A/V)
	Ql_strncat(NGSMstring,"\r\n",2);

    OUT_D1EBUG(textBuf,"NGSMstring=%s:\r\n",NGSMstring);
    trackflsent=0;
    //fun_trackflsent();
	tw_filewrite((char *)NGSMstring);	
}

void Gen_NGPRS(void)
{
    char NGPRSstring[90]; 
    Ql_memset((ascii *)NGPRSstring,'\0',sizeof(NGPRSstring));
	Ql_strcpy(NGPRSstring,UID);
	Ql_strncat(NGPRSstring,"_NGPRS,",7);
	Ql_strcat(NGPRSstring,GPRMC);
	OUT_D1EBUG(textBuf,"GPRMC111=%s:\r\n",GPRMC);
    ix_Itoa(mCurrSpeed);
	//	OUT_D1EBUG(textBuf,"FtoaStrcurre%s\r\n",ItoaStr);
	Ql_strcat(NGPRSstring,(char *)ItoaStr);///mCurrSpeed speed
	Ql_strncat(NGPRSstring,",",1);
	Ql_strcat(NGPRSstring,(char *)(ix_Itoa(mTotDist)));
	Ql_strncat(NGPRSstring,",",1);
	Ql_strncat(NGPRSstring,"0.0",5);		//PDOP
    Ql_strncat(NGPRSstring,",",1);
	Ql_strncat(NGPRSstring,STAT,1);			//Status (A/V)
	Ql_strncat(NGPRSstring,"\r\n",2);

    OUT_D1EBUG(textBuf,"NGPRSstring=%s:\r\n",NGPRSstring);
    trackflsent=0;
    //fun_trackflsent();
	tw_filewrite((char *)NGPRSstring);	
}

// read whole Stamp Data file
void read_whole_STAMP_file(void)
{
	s32 ret,size_ret;
	u32 readedlen1 = 0,i= 0,read_complete = 0,whole_no_bytes = 0;
	u32 filehandle,filesize = 0;
	char temp_read[2000];
	OUT_D1EBUG(textBuf,"IN  dump STAMP_file file \r\n");
	Ql_Sleep(500);

	size_ret = Ql_FileGetSize((u8*)pfile, &filesize);
	OUT_D1EBUG(textBuf,"GCC full size of Stamp data file =%d\r\n",filesize);
	OUT_D1EBUG(textBuf,"Stamp Data File  = %s\r\n",pfile);
	ret = Ql_FileOpenEx((u8 *)pfile,QL_FS_READ_ONLY);

	if(ret >= QL_RET_OK)
	{
		filehandle = ret;
		read_complete = 0;

		while(whole_no_bytes <=  filesize)
		{
			Ql_memset((ascii *)temp_read,0,sizeof(temp_read));
			readedlen1 = 0;
			ret = Ql_FileSeek(filehandle,read_complete, QL_FS_FILE_BEGIN);
			ret = Ql_FileRead(filehandle, (u8 *)temp_read,1500, &readedlen1);
			read_complete = read_complete +  readedlen1;
			Ql_Sleep(500);
			i= 0;

			while(i <= 1500)
			{
				OUT_D1EBUG(textBuf,"%c",temp_read[i]);
				i++;
				whole_no_bytes++;
			}
		}
		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"Error in reading whole incd file\r\n");
	}


}



