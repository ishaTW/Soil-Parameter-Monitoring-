/************************************************************************************************************
 * Read Stamp memory data for transmission
 ************************************************************************************************************/

#include<stdio.h>
#include<string.h>
#include "ql_trace.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "Ql_tcpip.h"
#include "ql_error.h"
#include "GPS.h"
#include "ql_fcm.h"
#include "GSM_GPRS.h"
#include "TCP_IP.h"
#include "SEND_DATA.h"
#include "Ql_filesystem.h"
#include "Fun.h"
#include "MRW.h"
#include "para_read.h"
#include "GPRMC.h"
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
extern char textBuf[1000];
/*************************************************************************************************************/

extern char pfile[10];
extern int wrt_cnt;
extern int read_cnt;
extern u8 BackUp_Flag;
extern u32 wrt_cnt_Limit;
extern u32 BackUpwrt_cnt;

s32 chkoldavailable(void)
{
	u32 wrtreddiff=0;             					// Difference between write and read count
	u32 datareadcnt=0;			  					// memory location from where to read data

	OUT_D1EBUG(textBuf,"\r\n ## In Function Check Old Data.## \r\n");
	OUT_D1EBUG(textBuf,"\r\n wrt_cnt=%d:\r\n",wrt_cnt);
	OUT_D1EBUG(textBuf,"\r\n read_cnt=%d:\r\n",read_cnt);


	if(BackUp_Flag == 0)
	{
		wrtreddiff=wrt_cnt-read_cnt;				// Difference between write and read count
		OUT_D1EBUG(textBuf,"\r\n Difference between write and read count=%d:\r\n",wrtreddiff);
		if(wrtreddiff >= 1800)
		{
			datareadcnt=wrt_cnt-200;				// memory location from where to read data
			return datareadcnt;
		}
		else
			return -1;
	}
	else if(BackUp_Flag == 1)
	{
		if(wrt_cnt != 0)
		{
			if(wrt_cnt > 200)
			{
				datareadcnt=wrt_cnt-200;				// memory location from where to read data
				return datareadcnt;
			}
			else if(wrt_cnt <= 200)
			{
				//datareadcnt=wrt_cnt-200;				// memory location from where to read data
				return 0;
			}
		}
		else if(wrt_cnt == 0)
		{
			datareadcnt=BackUpwrt_cnt-200;				// memory location from where to read data
			return datareadcnt;
		}
	}
	else
		return -1;

}

char *twoldfrmmemory(u32 memloc)
{
	static char Oldreadbuff[210]="\0";
	static char Oldsendbuff[210]="\0";
	s32 ret,filehandle;
	s32 fun_ret=0;
	//char *ptr1;
	char err[9]="FAIL\0";
	char *retbuf=NULL;
	u32 j=0,readlength=0,read_cnt1=0;
	//s32 diff_R_w=0;
	u32 readedlen=0;

	OUT_D1EBUG(textBuf,"\r\n IN function twoldfrmmemory with mem location =%d\r\n",memloc);
	ret = Ql_FileOpenEx((char*)pfile,QL_FS_CREATE);
	OUT_D1EBUG(textBuf,"\r\n Ql_FileOpenEx()=%d:\r\n",ret);

	if(ret >= QL_RET_OK)
	{
		filehandle = ret;
		Ql_memset(Oldreadbuff,'\0',sizeof(Oldreadbuff));
		Ql_memset(Oldsendbuff,'\0',sizeof(Oldsendbuff));
		ret = Ql_FileSeek(filehandle, memloc, QL_FS_FILE_BEGIN);
		ret = Ql_FileRead(filehandle, (u8 *)Oldreadbuff,100, &readedlen);
		OUT_D1EBUG(textBuf,"\r\n Ql_FileRead()=%d: readedlen=%d\r\n",ret, readedlen);
		Ql_strcpy((char *)Oldsendbuff,(char *)Oldreadbuff);
	//	readlength=readedlen;
	//	OUT_D1EBUG(textBuf,"\r\n readlength_First=%d\r\n",readlength);
		read_cnt1=memloc+readedlen;
		//read_cnt=read_cnt1;
		//Oldreadbuff[0]='\0';
		Ql_memset(Oldreadbuff,'\0',sizeof(Oldreadbuff));
		OUT_D1EBUG(textBuf,"tw_fileread____________ read_cnt1 =%d\r\n",read_cnt1);
		ret = Ql_FileSeek(filehandle, read_cnt1, QL_FS_FILE_BEGIN);
		ret = Ql_FileRead(filehandle, (u8 *)Oldreadbuff,100, &readedlen);
		OUT_D1EBUG(textBuf,"\r\n Ql_FileRead()=%d: readedlen=%d\r\n",ret, readedlen);

		Ql_FileClose(filehandle);
		filehandle = -1;

		for(j=0;Oldreadbuff[j] != '\0';j++)
		{

			if(Oldreadbuff[j] == 13 || Oldreadbuff[j] == 10)		// \r & \n values
			{
				//					OUT_D1EBUG(textBuf,"No data j=%d\r\n",j);
				Ql_strncat((char *)Oldsendbuff,(char *)"\r",1);
				Ql_strncat((char *)Oldsendbuff,(char *)"\n",1);

				j++;
				j++;
				break;
			}
			else
			{
				Ql_strncat((char *)Oldsendbuff,(char *)&Oldreadbuff[j],1);
				//			OUT_D1EBUG(textBuf,"Data j+++=%d\r\n",j);
			}
		}
		readlength=readlength+j;
		OUT_D1EBUG(textBuf,"\r\n readlength_sec=%d\r\n",readlength);
		//OUT_D1EBUG(textBuf,"\r\n read_cnt aft=%d:\r\n",read_cnt);
		OUT_D1EBUG(textBuf,"\r\n Latest data from Old Data Buffer=%s\r\n",Oldsendbuff);
		fun_ret=1;

		retbuf=(char *)Oldsendbuff;
		return retbuf;
	}
	else
	{
		OUT_D1EBUG(textBuf,"Error in file reading\r\n");
		//fun_ret=-1;
		retbuf=(char *)err;
	}

}

/*
char *tw_fileread_mrw(void)
{
	static char Oldreadbuff[1001]="\0";
	static char Oldsendbuff[1001]="\0";
	s32 ret,filehandle;
	s32 fun_ret=0;
	char *ptr1;
	char err[9]="FAIL\0";
	char *retbuf=NULL;
	u32 j=0,readlength=0,read_cnt1=0;
	s32 diff_R_w=0;
	u32 readedlen=0;


	ret = Ql_FileOpenEx((char*)pfile,Ql_FileRead_ONLY);
	OUT_D1EBUG(textBuf,"\r\n Ql_FileOpenEx()=%d:\r\n",ret);
	OUT_D1EBUG(textBuf,"\r\n wrt_cnt=%d:\r\n",wrt_cnt);
	OUT_D1EBUG(textBuf,"\r\n read_cnt=%d:\r\n",read_cnt);
	diff_R_w=wrt_cnt-read_cnt;
	OUT_D1EBUG(textBuf,"\r\n diff_R_w=%d:\r\n",diff_R_w);

	if(ret >= QL_RET_OK)
	{
		filehandle = ret;
		if(BackUp_Flag == 1)
		{
			OUT_D1EBUG(textBuf,"\r\n in if(BackUp_Flag == 1)\r\n");
			filehandle = ret;
			Ql_memset(Oldreadbuff,'\0',sizeof(Oldreadbuff));
			Ql_memset(Oldsendbuff,'\0',sizeof(Oldsendbuff));
			ret = Ql_FileSeek(filehandle, read_cnt, QL_FS_FILE_BEGIN);
			ret = Ql_FileRead(filehandle, (u8 *)Oldreadbuff,900, &readedlen);
			OUT_D1EBUG(textBuf,"\r\n Ql_FileRead()=%d: readedlen=%d\r\n",ret, readedlen);
			Ql_strcpy((char *)Oldsendbuff,(char *)Oldreadbuff);
			readlength=readedlen;
			OUT_D1EBUG(textBuf,"\r\n readlength_First=%d\r\n",readlength);
			read_cnt1=read_cnt+readlength;
			//read_cnt=read_cnt1;
			//Oldreadbuff[0]='\0';
			Ql_memset(Oldreadbuff,'\0',sizeof(Oldreadbuff));
			OUT_D1EBUG(textBuf,"tw_fileread____________ read_cnt1 =%d\r\n",read_cnt1);
			ret = Ql_FileSeek(filehandle, read_cnt1, QL_FS_FILE_BEGIN);
			ret = Ql_FileRead(filehandle, (u8 *)Oldreadbuff,100, &readedlen);
			OUT_D1EBUG(textBuf,"\r\n Ql_FileRead()=%d: readedlen=%d\r\n",ret, readedlen);

			Ql_FileClose(filehandle);
			filehandle = -1;

			for(j=0;Oldreadbuff[j] != '\0';j++)
			{

				if(Oldreadbuff[j] == 13 || Oldreadbuff[j] == 10)		// \r & \n values
				{
					//					OUT_D1EBUG(textBuf,"No data j=%d\r\n",j);
					Ql_strncat((char *)Oldsendbuff,(char *)"\r",1);
					Ql_strncat((char *)Oldsendbuff,(char *)"\n",1);

					j++;
					j++;
					break;
				}
				else
				{
					Ql_strncat((char *)Oldsendbuff,(char *)&Oldreadbuff[j],1);
					//			OUT_D1EBUG(textBuf,"Data j+++=%d\r\n",j);
				}
			}
			readlength=readlength+j;
			OUT_D1EBUG(textBuf,"\r\n readlength_sec=%d\r\n",readlength);
			OUT_D1EBUG(textBuf,"\r\n read_cnt aft=%d:\r\n",read_cnt);
			fun_ret=1;
			//	OUT_D1EBUG(textBuf,"Oldsendbuff= %s\r\n",Oldsendbuff);
			//transfer_tcp_data();
			//smtpconnection();			////SMTP tx routine

			//TCP_TX(Oldsendbuff);

			retbuf=(char *)Oldsendbuff;
			return retbuf;

		}
		else if(BackUp_Flag == 0)
		{
			OUT_D1EBUG(textBuf,"\r\n in if(BackUp_Flag == 0)\r\n");
			if(diff_R_w >= 900)
			{
				OUT_D1EBUG(textBuf,"\r\n in if(BackUp_Flag == 0) && if(diff_R_w >= 900)\r\n");
				filehandle = ret;
				Ql_memset(Oldreadbuff,'\0',sizeof(Oldreadbuff));
				Ql_memset(Oldsendbuff,'\0',sizeof(Oldsendbuff));
				ret = Ql_FileSeek(filehandle, read_cnt, QL_FS_FILE_BEGIN);
				ret = Ql_FileRead(filehandle, (u8 *)Oldreadbuff,900, &readedlen);
				OUT_D1EBUG(textBuf,"\r\n Ql_FileRead()=%d: readedlen=%d\r\n",ret, readedlen);
				Ql_strcpy((char *)Oldsendbuff,(char *)Oldreadbuff);
				readlength=readedlen;
				read_cnt1=read_cnt+readlength;
				//read_cnt=read_cnt1;
				Ql_memset(Oldreadbuff,'\0',sizeof(Oldreadbuff));
				OUT_D1EBUG(textBuf,"tw_fileread____________ read_cnt1 =%d\r\n",read_cnt1);
				ret = Ql_FileSeek(filehandle, read_cnt1, QL_FS_FILE_BEGIN);
				ret = Ql_FileRead(filehandle, (u8 *)Oldreadbuff,100, &readedlen);
				OUT_D1EBUG(textBuf,"\r\n Ql_FileRead()=%d: readedlen=%d\r\n",ret, readedlen);

				Ql_FileClose(filehandle);
				filehandle = -1;

				//	OUT_D1EBUG(textBuf,"\r\n Oldreadbuff=%s\r\n",Oldreadbuff);

				for(j=0;Oldreadbuff[j] != '\0';j++)
				{
					if(Oldreadbuff[j] == 13 || Oldreadbuff[j] == 10)		// \r & \n values
					{
						//						OUT_D1EBUG(textBuf,"No data j=%d\r\n",j);
						Ql_strncat((char *)Oldsendbuff,(char *)"\r",1);
						Ql_strncat((char *)Oldsendbuff,(char *)"\n",1);

						j++;
						j++;
						break;
					}
					else
					{
						Ql_strncat((char *)Oldsendbuff,(char *)&Oldreadbuff[j],1);
						//			OUT_D1EBUG(textBuf,"Data j+++=%d\r\n",j);
					}
				}

				readlength=readlength+j;

				OUT_D1EBUG(textBuf,"\r\n readlength_sec=%d\r\n",readlength);
				OUT_D1EBUG(textBuf,"\r\n read_cnt aft=%d:\r\n",read_cnt);
				fun_ret=1;
				//			OUT_D1EBUG(textBuf,"Oldsendbuff= %s\r\n",Oldsendbuff);
				//transfer_tcp_data();
				//smtpconnection();     ////SMTP tx routine
				//	TCP_TX(Oldsendbuff);

				retbuf=(char *)Oldsendbuff;
				return retbuf;
			}
			else if((diff_R_w < 900) && (diff_R_w >= 0))
			{
				OUT_D1EBUG(textBuf,"\r\n in if(BackUp_Flag == 0) && else if((diff_R_w < 900) && (diff_R_w >= 0)),,,diff_R_w=%d\r\n",diff_R_w);
				filehandle = ret;
				Ql_memset(Oldreadbuff,'\0',sizeof(Oldreadbuff));
				Ql_memset(Oldsendbuff,'\0',sizeof(Oldsendbuff));
				ret = Ql_FileSeek(filehandle, read_cnt, QL_FS_FILE_BEGIN);
				ret = Ql_FileRead(filehandle, (u8 *)Oldreadbuff,diff_R_w, &readedlen);
				OUT_D1EBUG(textBuf,"\r\n Ql_FileRead()=%d: readedlen=%d\r\n",ret, readedlen);
				//OUT_D1EBUG(textBuf,"\r\n Oldreadbuff=%s\r\n",Oldreadbuff);
				Ql_strcpy((char *)Oldsendbuff,(char *)Oldreadbuff);
				readlength=readedlen;
				//read_cnt=read_cnt+readedlen;			///////remove
				//	OUT_D1EBUG(textBuf,"\r\n readlength_First=%d\r\n",readlength);
				//OUT_D1EBUG(textBuf,"\r\n Oldsendbuff_First=%s\r\n",Oldsendbuff);
				Ql_FileClose(filehandle);
				filehandle = -1;

				readlength=readlength+j;

				OUT_D1EBUG(textBuf,"\r\n readlength_sec=%d,\r\n",readlength);
				OUT_D1EBUG(textBuf,"\r\n read_cnt aft=%d:\r\n",read_cnt);
				fun_ret=1;
				//	OUT_D1EBUG(textBuf,"Oldsendbuff= %s\r\n",Oldsendbuff);
				//transfer_tcp_data();
				//smtpconnection();      ////SMTP tx routine
				//TCP_TX(Oldsendbuff);

				retbuf=(char *)Oldsendbuff;
				return retbuf;
			}
			else
			{
				Ql_FileClose(filehandle);
				filehandle = -1;
			}
		}

	}
	else
	{
		OUT_D1EBUG(textBuf,"Error in file reading\r\n");
		//fun_ret=-1;
		retbuf=(char *)err;
	}

	return err;
}*/
