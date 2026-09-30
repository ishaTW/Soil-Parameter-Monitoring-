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
#include "canparareadwrt.h"
/*-------------------------------------------------------------------------*/
/*								Globals									   */
/*-------------------------------------------------------------------------*/
#define OUT_DEBUG(x,...)  \
    Ql_memset((x),0,100);  \
    Ql_sprintf((x),__VA_ARGS__);   \
    Ql_SendToUart(ql_uart_port2,(u8 *)(x),Ql_strlen(x));

extern char pfile2[15];
extern  char gps_type[10];
char Flag_readbuffer[5];
char CanFlag_readbuffer[2];
char CanPara_readbuffer[15];
char CVread[25];
extern u8 CanUpdate_flag;
extern unsigned char ItoaStr[];
extern char UID[];
char UID_Fota[8];

extern char textBuf[];
char mPrevLatLong[30]={0};
char mPrevGPRMC[75];
extern char checkdata[];
char DATE[], TIME[], nLAT[], LONG[], eLONG[], STAT[], SPEED[], COURSE[];
extern char LAT[];
extern char mSpeedAscii[];
extern unsigned char FtoaStr[];
double OSpeed=0;
extern int wrt_cnt;
extern int read_cnt;
extern u32 BackUpwrt_cnt;
extern char STI[8];
extern char TXI[8];
extern char GPRMC[];
extern double mTotDist;
extern int uwrt_cnt;
extern int uread_cnt;
char mprevDist[8]; 
char mureadcnt[8]; 
extern u8 BackUp_Flag;
extern char CV[15];

int modified_flag=0;
extern char Unit_Type[35];

void tw_CanUpdate_flag_read(void)
{
	s32 ret;
	u32 readedlen1;
	s32 filehandle;
	ret = Ql_FileOpenEx((u8*)"canpara.txt",QL_FS_CREATE);
	OUT_DEBUG(textBuf,"Ql_FileOpenEx ret = %d\r\n",ret);

	if(ret >= QL_RET_OK)
	{
		filehandle = ret;
		Ql_memset((ascii *)CanFlag_readbuffer,0,sizeof(CanFlag_readbuffer));
		ret = Ql_FileSeek(filehandle,0, QL_FS_FILE_BEGIN);  
        ret = Ql_FileRead(filehandle, (u8 *)CanFlag_readbuffer,1, &readedlen1);
        OUT_DEBUG(textBuf,"Ql_FileRead() = %d:CanFlag_readbuffer = %s, readedlenfl = %d\r\n",ret,CanFlag_readbuffer, readedlen1);	
        CanUpdate_flag=Ql_atoi(CanFlag_readbuffer);
        OUT_DEBUG(textBuf,"after atoi CanUpdate_flag = %d\r\n",CanUpdate_flag);	        
        Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_DEBUG(textBuf,"Error in Canfile reading**1 tw_CanUpdate_flag_read\r\n");
	}
}

 void readcreated(void)
 {
	 s32 filehandle;
	 s32 ret;
	 u32 writeedlen;
	 u32 readedlen;
	 char strBuf[2];
	 filehandle = Ql_FileOpenEx((u8*)"canpara.txt", (QL_FS_READ_WRITE|QL_FS_CREATE));
	 if(filehandle > 0)
	 {
		 Ql_memset(strBuf,0,1);
		 ret = Ql_FileSeek(filehandle, 0 , QL_FS_FILE_BEGIN);
		 ret = Ql_FileRead(filehandle, strBuf,1, &readedlen);
		 OUT_DEBUG(textBuf,"Ql_FileRead()=%d: readedlen=%d, strBuf=%s\r\n",ret, readedlen, strBuf);
		 Ql_FileClose(filehandle);
		 OUT_DEBUG(textBuf,"\r\nFile read  strBuf =%s\r\n",strBuf);
		 modified_flag=Ql_atoi(strBuf);
		 OUT_DEBUG(textBuf,"\r\nFile read  flag =%d\r\n",modified_flag);
		 OUT_DEBUG(textBuf,"\r\nFile read successfull\r\n");
		 filehandle = -1;
		 mainsettings();
	 }

 }
                
 void formatmemory(void)
 {	
	 s32 ret;
	 ret = Ql_Fs_Format(Ql_FS_UFS);
	 OUT_DEBUG(textBuf,"Ql_Fs_Format()=%d\r\n",ret);
	 if(ret==0)
	 {
		 readcreated();
	 }
 }
 void mainsettings(void)
 {
 	if(modified_flag == 0)
    {
       set_info();
       createandwrite();
    }
    else
    {
      tw_updatedCanParaRead();
      remote_pararead_APN();
    }
 }


