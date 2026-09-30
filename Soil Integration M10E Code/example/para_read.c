
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
#include "ql_fcm.h"
#include "GPS.h"
#include "GSM_GPRS.h"
#include "SEND_DATA.h"
#include "TCP_IP.h"
#include "MRW.h"  
#include "para_read.h"
#include "sms_handle.h"
#include "Ql_filesystem.h"



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

extern char pfile1[];
extern char pfile2[15];
extern u32 filehandle1;
extern u32 LatLongfilehandle;
extern u8 CanUpdate_flag;
extern double mCurrSpeed;
extern u16 Max_Count;
extern double mMultiFactorSpeed;
extern unsigned char ItoaStr[];
extern char UID[];
extern char textBuf[1000];
extern ascii uart_buffer[1000];
extern bool mVehMoving;
u8 mPrevLatLong[30]={0};
u8 mPrevGPRMC[75];
extern char checkdata[];
extern char DATE[], TIME[], nLAT[], LONG[], eLONG[], STAT[], SPEED[], COURSE[];
extern char LAT[];
extern char mSpeedAscii[];
extern unsigned char FtoaStr[];
double OSpeed=0;
extern int wrt_cnt;
extern int read_cnt;
extern u32 BackUpwrt_cnt;
extern char STI[8];
extern char TXI[8];
extern double mTotDist;
extern char GPRMC[];
extern double mTotDist;
extern int uwrt_cnt;
extern int uread_cnt;
extern int fileincr;
extern u32 cam_wrt_cnt;
extern u32 readcount;
extern u32 dump_cam_wrt_cnt;
////camera2
extern u32 cam_wrt_cnt2;
extern u32 readcount22;
extern u32 dump_cam_wrt_cnt2;
extern int fileincr2;
extern int onesectimerflag;
extern u8 BackUp_Flag;
extern char CV[15];

///camera flags 
char myprvcamwrtcnt[8];
char myprvcamreadcnt[8];
char myprvcamdumpwrtcnt[8];
char myprvcamimgcnt[8];
char myprvonetimercnt[8];
///camera 2 flags 
char myprvcam2wrtcnt[8];
char myprvcam2readcnt[8];
char myprvcam2dumpwrtcnt[8];
char myprvcam2imgcnt[8];
////aftr_reset_flags
extern int trackflsent;
char myprvtrackflsent[8];
extern bool camdumpflag;
char myprvcamdumpflag[8];
extern bool datareading;
char myprvdatareading[8];
extern int cam1dump;
char myprvcam1dump[8];
extern int cam2dump;
char myprvcam2dump[8];
char Flag_readbuffer[5];
char CanFlag_readbuffer[1];
char CanPara_readbuffer[15];
char CVread[25];
char gps_type[10];
char mprevDist[8];
char mureadcnt[8];
char CAMUID[10];
extern u8 CANDUMPflg;
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/


void tw_para_read(void)
{
	ascii OfStampString[90];
	char mSpeed[5];
	ascii mSpeedAscii[10];
	s32 ret1;
	//u32 writeedlen1;
	u32 readedlen1;

	char *mpSpeed;
	u16 wcnt=0;
	char mprevwrtcnt[8];
	char mprevreadcnt[8];
	char muwritecnt[8];
	char Flag_BackUp[2];
	char mprevMax_count[8];
	char myUARTdata[20];      //by ambika 03/08

	OUT_D1EBUG(textBuf,"\r\n###### Device Data  ######\r\n");
	s32 *filesize;
	s32 ret;

	//OUT_D1EBUG(textBuf,"tw_para_read\r\n");
	ret=Ql_FileGetSize((u8*)pfile1,(u32*)filesize);
	//	OUT_D1EBUG(textBuf,"Ql_FileGetSize size=%d\r\n",ret);
	if(ret == QL_RET_OK)
	{
		//OUT_D1EBUG(textBuf,"pfile1 size=%d\r\n",filesize);
	}
	ret1 = Ql_FileOpenEx((u8*)pfile1,QL_FS_CREATE);
	// OUT_D1EBUG(textBuf,"Ql_FileOpenEx size=%d\r\n",ret1);
	if(ret1 >= QL_RET_OK)
	{
		filehandle1 = ret1;
		Ql_memset((ascii *)Flag_readbuffer,'\0',sizeof(Flag_readbuffer));
		//Flag_readbuffer[0]='\0';
		ret1 = Ql_FileSeek(filehandle1,0, QL_FS_FILE_BEGIN);  

		//OUT_D1EBUG(textBuf,"Ql_FileSeek size=%d\r\n",ret1);
		//ret1 = Ql_FileRead(filehandle1, (u8 *)Flag_readbuffer,Ql_strlen(Flag_readbuffer), &readedlen1);
		ret1 = Ql_FileRead(filehandle1, (u8 *)Flag_readbuffer,5, &readedlen1);

		//OUT_D1EBUG(textBuf,"Ql_FileSeek size=%d\r\n",ret1);

		//OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d\r\n",ret1, readedlen1);
		if(!(Ql_strncmp((ascii *)Flag_readbuffer,"TRUE",4)))
		{
			mVehMoving=TRUE;

		}

		else if(!(Ql_strncmp((ascii *)Flag_readbuffer,"FALSE",5)))
		{
			mVehMoving=FALSE;
		}

		OUT_D1EBUG(textBuf,"\tVehMoving Flag \t\t= %d\r\n",mVehMoving);

		//-----------------------------------------------------------------------------
		//-----------LatLong read-------------------------------------------------------
		//------------------------------------------------------------------------------
		wcnt=5+1;
		//wcnt=Ql_strlen(Flag_readbuffer)+1;
		Ql_memset((ascii *)mPrevLatLong,'\0',sizeof(mPrevLatLong));
		//mPrevLatLong[0]='\0';
		ret1 = Ql_FileSeek(filehandle1,wcnt, QL_FS_FILE_BEGIN);

		//	ret1 = Ql_FileRead(filehandle1, (u8 *)mPrevLatLong,Ql_strlen(mPrevLatLong), &readedlen1);
		ret1 = Ql_FileRead(filehandle1, (u8 *)mPrevLatLong,30, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenlatlong = %d\r\n",ret1, readedlen1);
		OUT_D1EBUG(textBuf,"\tPrev LAT-LONG  \t\t= %s\r\n",mPrevLatLong);


		//-----------------------------------------------------------------------------
		//----------- Previous GPRMC read----------------------------------------------
		//-----------------------------------------------------------------------------
		//Ql_strlen((char *)mPrevGPRMC)
		wcnt=5+30+1;
		// wcnt=wcnt+Ql_strlen(mPrevLatLong)+1;
		Ql_memset((ascii *)mPrevGPRMC,'\0',sizeof(mPrevGPRMC));
		//mPrevGPRMC[0]='\0';
		ret1 = Ql_FileSeek(filehandle1,wcnt, QL_FS_FILE_BEGIN);

		//ret1 = Ql_FileRead(filehandle1, (u8 *)mPrevGPRMC,Ql_strlen(mPrevGPRMC), &readedlen1);
		ret1 = Ql_FileRead(filehandle1, (u8 *)mPrevGPRMC,75, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileReadmPrevGPRMC() = %d: readedlenlatlong = %d\r\n",ret1, readedlen1);
		OUT_D1EBUG(textBuf,"\tPrev GPRMC \t\t\t= %s\r\n",mPrevGPRMC);


		//-----------------------------------------------------------------------------
		//------------------Distance---------------------------------------------------
		//-----------------------------------------------------------------------------

		Ql_memset(mprevDist,'\0',sizeof(mprevDist));
		wcnt=5+30+75+1;
		// wcnt=wcnt+Ql_strlen(mPrevGPRMC)+1;
		//mprevDist[0]='\0';
		ret1 = Ql_FileSeek(filehandle1,wcnt, QL_FS_FILE_BEGIN);

		//	ret1 = Ql_FileRead(filehandle1, (u8 *)mprevDist,Ql_strlen(mprevDist), &readedlen1);
		ret1 = Ql_FileRead(filehandle1, (u8 *)mprevDist,8, &readedlen1);

		mTotDist=atofd(mprevDist);
		//OUT_D1EBUG(textBuf,"Ql_FileReadmTotDist() = %d: read_dist_length = %d\r\n",ret1, readedlen1);
		OUT_D1EBUG(textBuf,"\tPrevious Distance \t\t= %s\r\n",mprevDist);

		//-----------------------------------------------------------------------------
		//------------------Write count------------------------------------------------
		//-----------------------------------------------------------------------------
		Ql_memset(mprevwrtcnt,'\0',sizeof(mprevwrtcnt));
		//mprevwrtcnt[0]='\0';
		wcnt=5+30+75+8+1;
		//wcnt=wcnt+Ql_strlen(mprevDist)+1;
		ret1 = Ql_FileSeek(filehandle1,wcnt, QL_FS_FILE_BEGIN);
		//	ret1 = Ql_FileRead(filehandle1, (u8 *)mprevwrtcnt,Ql_strlen(mprevwrtcnt), &readedlen1);
		ret1 = Ql_FileRead(filehandle1, (u8 *)mprevwrtcnt,8, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileReadwrt_cnt() = %d: mprev_wrt_len= %d\r\n",ret1, readedlen1);
		//OUT_D1EBUG(textBuf,"mprevwrtcnt \t= %s\r\n",mprevwrtcnt);
		// wrt_cnt=ix_AtoI(mprevwrtcnt,4);
		if(!(Ql_strncmp((ascii *)mprevwrtcnt,"\0",1)))
		{
			// OUT_D1EBUG(textBuf,"readcount buffer empty\r\n");
			wrt_cnt=0;
		}        
		else
		{
			//wrt_cnt=ix_AtoI(mprevwrtcnt,Ql_strlen(mprevwrtcnt));
			wrt_cnt=Ql_atoi(mprevwrtcnt);
		}

		OUT_D1EBUG(textBuf,"\tData Write count \t\t= %d\r\n",wrt_cnt);

		//-----------------------------------------------------------------------------
		//------------------camera image count------------------------------------------------
		//-----------------------------------------------------------------------------
		Ql_memset(myprvcamimgcnt,'\0',sizeof(myprvcamimgcnt));
		//mprevwrtcnt[0]='\0';
		wcnt=5+30+75+8+8+8+8+8+5+8+8+8+8+1;
		//wcnt=wcnt+Ql_strlen(mprevDist)+1;
		ret1 = Ql_FileSeek(filehandle1,wcnt, QL_FS_FILE_BEGIN);

		//	ret1 = Ql_FileRead(filehandle1, (u8 *)mprevwrtcnt,Ql_strlen(mprevwrtcnt), &readedlen1);
		ret1 = Ql_FileRead(filehandle1, (u8 *)myprvcamimgcnt,8, &readedlen1);

		//OUT_D1EBUG(textBuf,"Ql_Fileimage_cnt() = %d: mprev_wrt_len= %d\r\n",ret1, readedlen1);
		//OUT_D1EBUG(textBuf,"mprevwrtcnt = %s\r\n",mprevwrtcnt);
		// wrt_cnt=ix_AtoI(mprevwrtcnt,4);
		if(!(Ql_strncmp((ascii *)myprvcamimgcnt,"\0",1)))
		{
			OUT_D1EBUG(textBuf,"\tCamera image count NULL.\r\n");
			//wrt_cnt=0;
		}        
		else
		{
			// fileincr=ix_AtoI(myprvcamimgcnt,Ql_strlen(myprvcamimgcnt));
			fileincr=Ql_atoi(myprvcamimgcnt);
		}
		OUT_D1EBUG(textBuf,"\tCamera Image Count \t\t= %d\r\n",fileincr);
		/////////////////////////////////////////////camera 2//////////////////////////////////////////////////////
		//-----------------------------------------------------------------------------
		//------------------camera 2 Write count------------------------------------------------
		//-----------------------------------------------------------------------------
		/**/
		//-----------------------------------------------------------------------------
		//------------------camera 2 Read count------------------------------------------------
		//-----------------------------------------------------------------------------
		/**/
		//-----------------------------------------------------------------------------
		//------------------camera 2 Dump write count------------------------------------------------
		//-----------------------------------------------------------------------------
		/**/
		//-----------------------------------------------------------------------------
		//------------------camera2 image count------------------------------------------------
		//-----------------------------------------------------------------------------
		Ql_memset(myprvcam2imgcnt,'\0',sizeof(myprvcam2imgcnt));
		//mprevwrtcnt[0]='\0';
		wcnt=5+30+75+8+8+8+8+8+5+8+8+8+8+8+8+8+8+1;
		//wcnt=wcnt+Ql_strlen(mprevDist)+1;
		ret1 = Ql_FileSeek(filehandle1,wcnt, QL_FS_FILE_BEGIN);

		//	ret1 = Ql_FileRead(filehandle1, (u8 *)mprevwrtcnt,Ql_strlen(mprevwrtcnt), &readedlen1);
		ret1 = Ql_FileRead(filehandle1, (u8 *)myprvcam2imgcnt,8, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_Fileimage2_cnt() = %d: mprev_wrt_len= %d\r\n",ret1, readedlen1);
		//OUT_D1EBUG(textBuf,"mprevwrtcnt = %s\r\n",mprevwrtcnt);
		// wrt_cnt=ix_AtoI(mprevwrtcnt,4);
		if(!(Ql_strncmp((ascii *)myprvcam2imgcnt,"\0",1)))
		{
			OUT_D1EBUG(textBuf,"\tCamera 2 image count NULL.\r\n");
			//wrt_cnt=0;
		}        
		else
		{
			//fileincr2=ix_AtoI(myprvcam2imgcnt,Ql_strlen(myprvcam2imgcnt));
			fileincr2=Ql_atoi(myprvcam2imgcnt);
		}

		OUT_D1EBUG(textBuf,"\tCamera 2 image count \t\t= %d\r\n",fileincr2);


		//-----------------------------------------------------------------------------
		//------------------camera onesec timer count------------------------------------------------
		//-----------------------------------------------------------------------------
		Ql_memset(myprvonetimercnt,'\0',sizeof(myprvonetimercnt));
		//mprevwrtcnt[0]='\0';
		wcnt=5+30+75+8+8+8+8+8+5+8+8+8+8+8+8+8+8+8+1;
		//wcnt=wcnt+Ql_strlen(mprevDist)+1;
		ret1 = Ql_FileSeek(filehandle1,wcnt, QL_FS_FILE_BEGIN);

		//	ret1 = Ql_FileRead(filehandle1, (u8 *)mprevwrtcnt,Ql_strlen(mprevwrtcnt), &readedlen1);
		ret1 = Ql_FileRead(filehandle1, (u8 *)myprvonetimercnt,8, &readedlen1);

		//OUT_D1EBUG(textBuf,"one sec timer count() = %d: mprev_wrt_len= %d\r\n",ret1, readedlen1);
		//OUT_D1EBUG(textBuf,"mprevwrtcnt = %s\r\n",mprevwrtcnt);
		// wrt_cnt=ix_AtoI(mprevwrtcnt,4);
		if(!(Ql_strncmp((ascii *)myprvonetimercnt,"\0",1)))
		{
			OUT_D1EBUG(textBuf,"\tOne second timer count NULL.\r\n");
			//wrt_cnt=0;
		}        

		else
		{
			//onesectimerflag=ix_AtoI(myprvonetimercnt,Ql_strlen(myprvonetimercnt));
			onesectimerflag=Ql_atoi(myprvonetimercnt);
		}

		OUT_D1EBUG(textBuf,"\tOne second timer count \t\t= %d\r\n",onesectimerflag);
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//-----------------------------------------------------------------------------
		//------------------trackflsent flag-----------------------------------------------
		//-----------------------------------------------------------------------------
		Ql_memset(myprvtrackflsent,'\0',sizeof(myprvtrackflsent));
		//mprevwrtcnt[0]='\0';
		wcnt=5+30+75+8+8+8+8+8+5+8+8+8+8+8+8+8+8+8+8+1;
		//wcnt=wcnt+Ql_strlen(mprevDist)+1;
		ret1 = Ql_FileSeek(filehandle1,wcnt, QL_FS_FILE_BEGIN);

		//	ret1 = Ql_FileRead(filehandle1, (u8 *)mprevwrtcnt,Ql_strlen(mprevwrtcnt), &readedlen1);
		ret1 = Ql_FileRead(filehandle1, (u8 *)myprvtrackflsent,8, &readedlen1);

		//OUT_D1EBUG(textBuf,"trackflsent flag() = %d: mprev_wrt_len= %d\r\n",ret1, readedlen1);
		//OUT_D1EBUG(textBuf,"mprevwrtcnt = %s\r\n",mprevwrtcnt);
		// wrt_cnt=ix_AtoI(mprevwrtcnt,4);
		if(!(Ql_strncmp((ascii *)myprvtrackflsent,"\0",1)))
		{
			OUT_D1EBUG(textBuf,"\tTrack file sent flag NULL.\r\n");
			//wrt_cnt=0;
		}        

		else
		{
			//trackflsent=ix_AtoI(myprvtrackflsent,Ql_strlen(myprvtrackflsent));
			trackflsent=Ql_atoi(myprvtrackflsent);
		}

		OUT_D1EBUG(textBuf,"\tTrack File sent flag \t\t= %d\r\n",trackflsent);
		//-----------------------------------------------------------------------------
		//------------------camdumpflag flag-----------------------------------------------
		//-----------------------------------------------------------------------------
		/* */
		//-----------------------------------------------------------------------------
		//------------------datareading flag-----------------------------------------------
		//-----------------------------------------------------------------------------
		Ql_memset(myprvdatareading,'\0',sizeof(myprvdatareading));
		//mprevwrtcnt[0]='\0';
		wcnt=5+30+75+8+8+8+8+8+5+8+8+8+8+8+8+8+8+8+8+8+8+1;
		//wcnt=wcnt+Ql_strlen(mprevDist)+1;
		ret1 = Ql_FileSeek(filehandle1,wcnt, QL_FS_FILE_BEGIN);

		//	ret1 = Ql_FileRead(filehandle1, (u8 *)mprevwrtcnt,Ql_strlen(mprevwrtcnt), &readedlen1);
		ret1 = Ql_FileRead(filehandle1, (u8 *)myprvdatareading,8, &readedlen1);

		//OUT_D1EBUG(textBuf,"datareading flag() = %d: mprev_wrt_len= %d\r\n",ret1, readedlen1);
		//OUT_D1EBUG(textBuf,"mprevwrtcnt = %s\r\n",mprevwrtcnt);
		// wrt_cnt=ix_AtoI(mprevwrtcnt,4);
		if(!(Ql_strncmp((ascii *)myprvdatareading,"\0",1)))
		{
			OUT_D1EBUG(textBuf,"\tData reading flag NULL.\r\n");
			//wrt_cnt=0;
		}        

		else
		{
			// datareading=ix_AtoI(myprvdatareading,Ql_strlen(myprvdatareading));
			datareading=Ql_atoi(myprvdatareading);
		}

		OUT_D1EBUG(textBuf,"\tData reading flag \t\t= %d\r\n",datareading);

		//-----------------------------------------------------------------------------
		//------------------cam1dump flag-----------------------------------------------
		//-----------------------------------------------------------------------------
		Ql_memset(myprvcam1dump,'\0',sizeof(myprvcam1dump));
		//mprevwrtcnt[0]='\0';
		wcnt=5+30+75+8+8+8+8+8+5+8+8+8+8+8+8+8+8+8+8+8+8+8+1;
		//wcnt=wcnt+Ql_strlen(mprevDist)+1;
		ret1 = Ql_FileSeek(filehandle1,wcnt, QL_FS_FILE_BEGIN);

		//	ret1 = Ql_FileRead(filehandle1, (u8 *)mprevwrtcnt,Ql_strlen(mprevwrtcnt), &readedlen1);
		ret1 = Ql_FileRead(filehandle1, (u8 *)myprvcam1dump,8, &readedlen1);
		//OUT_D1EBUG(textBuf,"cam1dump flag flag() = %d: mprev_wrt_len= %d\r\n",ret1, readedlen1);
		//OUT_D1EBUG(textBuf,"mprevwrtcnt = %s\r\n",mprevwrtcnt);
		// wrt_cnt=ix_AtoI(mprevwrtcnt,4);
		if(!(Ql_strncmp((ascii *)myprvcam1dump,"\0",1)))
		{
			OUT_D1EBUG(textBuf,"\tcam1dump flag NULL.\r\n");
			//wrt_cnt=0;
		}        
		else
		{
			// cam1dump=ix_AtoI(myprvcam1dump,Ql_strlen(myprvcam1dump));
			cam1dump=Ql_atoi(myprvcam1dump);
		}

		OUT_D1EBUG(textBuf,"\tCam1dump flag  \t\t= %d\r\n",cam1dump);

		//-----------------------------------------------------------------------------
		//------------------cam2dump flag-----------------------------------------------
		//-----------------------------------------------------------------------------
		Ql_memset(myprvcam2dump,'\0',sizeof(myprvcam2dump));
		//mprevwrtcnt[0]='\0';
		//wcnt=5+30+75+8+8+8+8+8+5+8+8+8+8+8+8+8+8+8+8+8+8+8+8+1;
		wcnt=5+30+75+8+8+8+8+8+5+8+8+8+8+8+8+8+8+8+8+8+8+8+1+1;
		//wcnt=wcnt+Ql_strlen(mprevDist)+1;
		ret1 = Ql_FileSeek(filehandle1,wcnt, QL_FS_FILE_BEGIN);

		//	ret1 = Ql_FileRead(filehandle1, (u8 *)mprevwrtcnt,Ql_strlen(mprevwrtcnt), &readedlen1);
		ret1 = Ql_FileRead(filehandle1, (u8 *)myprvcam2dump,8, &readedlen1);

		//OUT_D1EBUG(textBuf,"cam2dump flag flag() = %d: mprev_wrt_len= %d\r\n",ret1, readedlen1);
		//OUT_D1EBUG(textBuf,"mprevwrtcnt = %s\r\n",mprevwrtcnt);
		// wrt_cnt=ix_AtoI(mprevwrtcnt,4);
		if(!(Ql_strncmp((ascii *)myprvcam2dump,"\0",1)))
		{
			OUT_D1EBUG(textBuf,"\tcam2dump  flag flag NULL.\r\n");
			//wrt_cnt=0;
		}        

		else
		{
			// cam2dump=ix_AtoI(myprvcam2dump,Ql_strlen(myprvcam2dump));
			cam2dump=Ql_atoi(myprvcam2dump);
		}

		OUT_D1EBUG(textBuf,"\tCam2dump flag \t\t\t= %d\r\n",cam2dump);
		//-----------------------------------------------------------------------------
		//------------------Read count------------------------------------------------
		//-----------------------------------------------------------------------------

		Ql_memset(mprevreadcnt,'\0',sizeof(mprevreadcnt));
		wcnt=5+30+75+8+8+1;
		// wcnt=wcnt+Ql_strlen(mprevwrtcnt)+1;
		//mprevreadcnt[0]='\0';
		ret1 = Ql_FileSeek(filehandle1,wcnt, QL_FS_FILE_BEGIN);

		//ret1 = Ql_FileRead(filehandle1, (u8 *)mprevreadcnt,Ql_strlen(mprevreadcnt), &readedlen1);
		ret1 = Ql_FileRead(filehandle1, (u8 *)mprevreadcnt,8, &readedlen1);

		//OUT_D1EBUG(textBuf,"Ql_FileReadread_cnt() = %d: mprev_read_len = %d\r\n",ret1, readedlen1);
		//Ql_strcpy(mprevreadcnt,(char *)"918");
		//OUT_D1EBUG(textBuf,"mprevreadcnt = %s\r\n",mprevreadcnt);
		//OUT_D1EBUG(textBuf,"Ql_strlen(mprevreadcnt) = %d\r\n",Ql_strlen(mprevreadcnt));
		if(!(Ql_strncmp((ascii *)mprevreadcnt,"\0",1)))
		{
			OUT_D1EBUG(textBuf,"Readcount NULL.\r\n");
			read_cnt=0;
		}        
		else
		{
			//read_cnt=ix_AtoI(mprevreadcnt,Ql_strlen(mprevreadcnt));
			read_cnt=Ql_atoi(mprevreadcnt);
		}
		//read_cnt=atoi(mprevreadcnt);
		// read_cnt=ix_AtoI(mprevreadcnt,);

		OUT_D1EBUG(textBuf,"\tmreadcnt \t\t\t= %d\r\n",read_cnt);

		//--------------------uWrite Count------------------------------------------------
		//-------------------------------------------------------------------------------
		/**/
		//-----------------------------------------------------------------------------
		//------------------Max_count------------------------------------------------
		//-----------------------------------------------------------------------------

		Ql_memset(mprevMax_count,'\0',sizeof(mprevMax_count));
		wcnt=5+30+75+8+8+8+8+8+1;
		//mprevMax_count[0]='\0';
		// wcnt=wcnt+Ql_strlen(mprevwrtcnt)+1;
		ret1 = Ql_FileSeek(filehandle1,wcnt, QL_FS_FILE_BEGIN);

		//ret1 = Ql_FileRead(filehandle1, (u8 *)mprevreadcnt,Ql_strlen(mprevreadcnt), &readedlen1);
		ret1 = Ql_FileRead(filehandle1, (u8 *)mprevMax_count,5, &readedlen1);

		//OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: mprev_read_len = %d\r\n",ret1, readedlen1);

		//OUT_D1EBUG(textBuf,"mpreMax_Count = %s\r\n",mprevMax_count);

		if(!(Ql_strncmp((ascii *)mprevMax_count,"\0",1)))
		{
			// OUT_D1EBUG(textBuf,"readcount buffer empty\r\n");
			Max_Count=1;
		}        

		else
		{
			//Max_Count=ix_AtoI(mprevMax_count,Ql_strlen(mprevMax_count));
			Max_Count=Ql_atoi(mprevMax_count);
			if(Max_Count == 0)
			{
				Max_Count=1;
			}
		}

		// read_cnt=0;
		OUT_D1EBUG(textBuf,"\tmMax_Count \t\t\t= %d\r\n",Max_Count);

		//-----------------------------------------------------------------------------
		//------------------BackUpwrt_cnt------------------------------------------------
		//-----------------------------------------------------------------------------

		Ql_memset(mprevreadcnt,'\0',sizeof(mprevreadcnt));
		// wcnt=5+30+75+8+8+8+8+8+5+1;
		wcnt=5+30+75+8+8+8+8+8+5+8+8+8+8+8+8+8+8+8+8+8+8+8+8+1+1;
		mprevreadcnt[0]='\0';
		// wcnt=wcnt+Ql_strlen(mprevwrtcnt)+1;
		ret1 = Ql_FileSeek(filehandle1,wcnt, QL_FS_FILE_BEGIN);

		//ret1 = Ql_FileRead(filehandle1, (u8 *)mprevreadcnt,Ql_strlen(mprevreadcnt), &readedlen1);
		ret1 = Ql_FileRead(filehandle1, (u8 *)mprevreadcnt,8, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: mprev_read_len = %d\r\n",ret1, readedlen1);
		if(!(Ql_strncmp((ascii *)mprevreadcnt,"\0",1)))
		{
			OUT_D1EBUG(textBuf,"\tBackUpwrt_cnt NULL.\r\n");
			BackUpwrt_cnt=0;
		}        
		else
		{
			// BackUpwrt_cnt=ix_AtoI(mprevreadcnt,Ql_strlen(mprevreadcnt));
			BackUpwrt_cnt=Ql_atoi(mprevreadcnt);
		}

		//OUT_D1EBUG(textBuf,"mpreBackUpwrt_cnt = %s\r\n",mprevreadcnt);
		//BackUpwrt_cnt=atoi(mprevreadcnt);
		OUT_D1EBUG(textBuf,"\tBackUpwrt_cnt \t\t\t= %d\r\n",BackUpwrt_cnt);


		//-----------------------------------------------------------------------------
		//------------------BackUp_Flag------------------------------------------------
		//-----------------------------------------------------------------------------

		Ql_memset(Flag_BackUp,'\0',sizeof(Flag_BackUp));
		//wcnt=5+30+75+8+8+8+8+8+5+8+1;
		wcnt=5+30+75+8+8+8+8+8+5+8+1+1;
		// 5+30+75+8+8+8+8+8+5+8+8+8+8+8+8+8+8+8+8+8+8+8+1+1;
		// Flag_BackUp[0]='\0';
		// wcnt=wcnt+Ql_strlen(mprevwrtcnt)+1;
		ret1 = Ql_FileSeek(filehandle1,wcnt, QL_FS_FILE_BEGIN);

		//ret1 = Ql_FileRead(filehandle1, (u8 *)mprevreadcnt,Ql_strlen(mprevreadcnt), &readedlen1);
		ret1 = Ql_FileRead(filehandle1, (u8 *)Flag_BackUp,1, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileReadFlag_BackUp() = %d: mprev_read_len = %d\r\n",ret1, readedlen1);
		//OUT_D1EBUG(textBuf,"mpreFlag_BackUp = %s\r\n",Flag_BackUp);
		BackUp_Flag=Ql_atoi(Flag_BackUp);
		OUT_D1EBUG(textBuf,"\tFlag_BackUp \t\t\t= %d\r\n",BackUp_Flag);


		Ql_FileClose(filehandle1);
		filehandle1 = -1;

	}
	else
	{
		OUT_D1EBUG(textBuf,"\tError in Parameter read\r\n");
	}

//--------------------------------------------------------------------------------------------------
//------------------------For uart data------------------------------//by ambika
	Ql_memset(uart_buffer,'\0',sizeof(uart_buffer));
	//mprevwrtcnt[0]='\0';
	wcnt=5+30+75+8+8+8+8+8+5+8+8+8+8+8+8+8+8+8+8+8+8+1+1000+1;
	//wcnt=wcnt+Ql_strlen(mprevDist)+1;
	ret1 = Ql_FileSeek(filehandle1,wcnt, QL_FS_FILE_BEGIN);

	//	ret1 = Ql_FileRead(filehandle1, (u8 *)mprevwrtcnt,Ql_strlen(mprevwrtcnt), &readedlen1);
	ret1 = Ql_FileRead(filehandle1, (u8 *)uart_buffer,1000, &readedlen1);

	//OUT_D1EBUG(textBuf,"datareading flag() = %d: mprev_wrt_len= %d\r\n",ret1, readedlen1);
	//OUT_D1EBUG(textBuf,"mprevwrtcnt = %s\r\n",mprevwrtcnt);
	// wrt_cnt=ix_AtoI(mprevwrtcnt,4);
	if(!(Ql_strncmp((ascii *)uart_buffer,"\0",1)))
	{
		OUT_D1EBUG(textBuf,"\tNo data in UART.\r\n");
		//wrt_cnt=0;
	}

	else
	{
		// datareading=ix_AtoI(myprvdatareading,Ql_strlen(myprvdatareading));
		datareading=Ql_atoi(uart_buffer);
	}

	OUT_D1EBUG(textBuf,"\tData in UART \t\t= %d\r\n",uart_buffer);

//------------------------------------------------------------------------------------------

	if(CANDUMPflg == 0 )
	{
		//-------------------------------------------------------------------------------
		//------------------Previous GPRMC-----------------------------------------------
		//-------------------------------------------------------------------------------
		//OUT_D1EBUG(textBuf,"OF stamp generation\r\n");
		Data_Seperation_func1(mPrevGPRMC);
		//OUT_D1EBUG(textBuf,"TIME=%s\r\n",TIME);
		OUT_D1EBUG(textBuf,"\tSTAT \t\t\t\t=%s\r\n",STAT);
		//OUT_D1EBUG(textBuf,"LAT=%s\r\n",LAT);
		//OUT_D1EBUG(textBuf,"nLAT=%s\r\n",nLAT);
		//OUT_D1EBUG(textBuf,"LONG=%s\r\n",LONG);
		//OUT_D1EBUG(textBuf,"eLONG=%s\r\n",eLONG);

		//mSpeedAscii[5] = 0;
		Ql_memset(mSpeedAscii,0,sizeof(mSpeedAscii));
		Ql_strncpy((ascii *)mSpeedAscii,(ascii *)SPEED,5);
		//mSpeedAscii[5] = 0;
		OSpeed=atofd(mSpeedAscii) * mMultiFactorSpeed;
		// mpSpeed=ix_Ftoa(OSpeed,2);
		// mSpeed[5]=0;
		// Ql_strncpy((ascii *)mSpeed,(ascii *)mpSpeed,5);
		//	 mSpeedAscii[5] = 0;
		Ql_strncpy((ascii *)mSpeedAscii,(ascii *)FtoaStr,5);
		// ix_Ftoa(mCurrSpeed,2);
		//OUT_D1EBUG(textBuf,"mSpeed=%s\r\n",mSpeed);
		//OUT_D1EBUG(textBuf,"COURSE=%s\r\n",COURSE);
		//OUT_D1EBUG(textBuf,"DATE=%s\r\n",DATE);

		Ql_memset((ascii *)OfStampString,0,sizeof(OfStampString));
		// Ql_strncpy(OfStampString,"UI8002_OF,",10);
		Ql_strcpy(OfStampString,UID);
		Ql_strncat(OfStampString,"_OF,",4);

		Ql_strcat(OfStampString,(ascii *)GPRMC);
		//ix_Ftoa(OSpeed,2);
		//OUT_D1EBUG(textBuf,"FtoaStrcurre%s\r\n",FtoaStr);
		ix_Itoa(OSpeed);
		Ql_strcat(OfStampString,(char *)ItoaStr);///mCurrSpeed speed
		Ql_strncat(OfStampString,",",1);
		//	 ix_Ftoa(mTotDist,2);
		//Ql_strcat(OfStampString,(ascii *)mprevDist);
		Ql_strcat(OfStampString,(ix_Itoa(mTotDist)));	//Distance
		Ql_strncat(OfStampString,",",1);
		Ql_strncat(OfStampString,"0.0",5);		//PDOP
		Ql_strncat(OfStampString,",",1);
		Ql_strcat(OfStampString,STAT);		//PDOP
		Ql_strncat(OfStampString,"\r\n",2);
		OUT_D1EBUG(textBuf,"\tOF_StampString \t\t=%s\r\n",OfStampString);
		//Ql_strncat(SIStampString,",",1);
		tw_filewrite((char *)OfStampString);
		//tw_fileread();
	}
}


void tw_LatLong_read(void)
{
	//Ql_strlen((char*)mCurrLatLong)
	s32 ret2;
	u32 LatLongreadedlen;

	OUT_D1EBUG(textBuf,"In tw_LatLong_read**pararead_1\r\n");

	ret2 = Ql_FileOpenEx((u8 *)pfile1,QL_FS_CREATE);
	//	OUT_D1EBUG(textBuf,"ret = %d\r\n",ret2);
	//	 OUT_D1EBUG(textBuf,"pfile1 = %s\r\n",pfile1);
	if(ret2 >= QL_RET_OK)
	{
		LatLongfilehandle=ret2;
		//	OUT_D1EBUG(textBuf,"LatLongread_cnt1 = %d\r\n",LatLongread_cnt);
		//	LatLongread_cnt += Ql_strlen((ascii *)mPrevLatLong);
		//	OUT_D1EBUG(textBuf,"LatLongread_cnt2  =%d: \r\n",LatLongread_cnt);
		Ql_memset((ascii *)mPrevLatLong,0,sizeof(mPrevLatLong));
		ret2 = Ql_FileSeek(LatLongfilehandle,6, QL_FS_FILE_BEGIN);
		ret2 = Ql_FileRead(LatLongfilehandle, (u8*)mPrevLatLong,30,&LatLongreadedlen);
		//OUT_D1EBUG(textBuf,"Ql_LatLong_read(() = %d: readedlenlt = %d\r\n",ret2, LatLongreadedlen);
		OUT_D1EBUG(textBuf,"mPrevLatLong = %s\r\n",mPrevLatLong);
		Ql_FileClose(LatLongfilehandle);
		LatLongfilehandle = -1;
		//OUT_D1EBUG(textBuf,"Ql_FileClose()\r\n");
		//OUT_D1EBUG(textBuf,"\r\n");
	}
	else
	{
		OUT_D1EBUG(textBuf,"Error in file reading**1\r\n");
	}

}


void tw_CanUpdate_flag_read(void)
{
	s32 ret;
	u32 readedlen1;
	u32 filehandle;
	ret = Ql_FileOpenEx((u8*)pfile2,QL_FS_CREATE);


	if(ret >= QL_RET_OK)
	{
		filehandle = ret;
		Ql_memset((ascii *)CanFlag_readbuffer,0,sizeof(CanFlag_readbuffer));
		ret = Ql_FileSeek(filehandle,0, QL_FS_FILE_BEGIN);  
		ret = Ql_FileRead(filehandle, (u8 *)CanFlag_readbuffer,1, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d\r\n",ret, readedlen1);

		CanUpdate_flag=Ql_atoi(CanFlag_readbuffer);
		OUT_D1EBUG(textBuf,"CanUpdate_flag = %d\r\n",CanUpdate_flag);

		Ql_FileClose(filehandle);
		filehandle = -1;

	}

	else
	{
		OUT_D1EBUG(textBuf,"Error in Canfile reading**1\r\n");
	}



}


