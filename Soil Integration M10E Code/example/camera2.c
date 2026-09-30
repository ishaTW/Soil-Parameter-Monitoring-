/////camera code v 1.0


#include<stdio.h>
#include<string.h>
#include "ql_appinit.h"
#include "ql_interface.h"
#include "ql_type.h"
#include "ql_trace.h"
#include "ql_audio.h"
#include "ql_timer.h"
#include "ql_stdlib.h"
#include "ql_error.h"
#include "ql_fcm.h"
#include "ql_filesystem.h"
#include "ql_trace.h"
#include "ftp2.h"
#include "camera2.h"
#include "camera.h"
#include "Fun.h"
#include "MRW.h"
//#include "ftp.c"
#include "Ql_error.h"
#include "exce_camera_C.h"


#define  PATH_CAMERA2 ((u8 *)"SD:camera_C.txt")

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

//char notes[100];
extern char textBuf[1000];
char readbuffer_cam2[35000];
char sendbuffer_cam2[35000];

char Expt_tempbuffer_C[35000];
char Expt_extratempbuffer_C[35000];
char Expt_RARDbuffer_C[70000];
u32 expt_cntr_C=0;
u32 RARDflg_C=0;
u32 OSflag_C=0;

 u8 cam2_sync_cmd[]={0xAA,0x0D,0x00,0x00,0x00,0x00};
 u8 cam2_init_cmd[]={0xAA,0x01,0x00,0x07,0x03,0x05};
 u8 cam_takepic_cmd2[]={0xAA,0x04,0x01,0x00,0x00,0x00};
 u8 cam_snapshot_cmd2[]={0xAA,0x05,0x01,0x00,0x00,0x00};
 u8 cam_reset_cmd2[]={0xAA,0x08,0x00,0x00,0x00,0x00};
 u8 cam_pack_size2[]={0xAA,0x06,0x08,0x00,0x04,0x00};

//u8 cam_snapshot_cmd22[]={0xAA,0x04,0x00,0x00,0x00,0x00};
//u8 cam_takepic_cmd2[]={0xAA,0x05,0x00,0x00,0x00,0x00};

u8 cam_getdata_cmd2[]={0xAA,0x0E,0x00,0x00,0x00,0x00};
//extern u8 cam_getdata1_cmd[]={0xAA,0x0E,0x00,0x00,0x01,0x00};
u8 cam2_readbuffer[15000];
u8 cam2_readbuffer11[20000];
char cam2_final_readbuffer[35000];
char IndexFile2[25000];
bool ffFound2 = 0, d9Found2 = 0 ,d8Found2 = 0;
int fcount2=0;
u16 f_read_len2=0;
extern char bulk_n[];

u32 filecounter2=0;
u32 fileposition2=0;
u32 read_comp2=2;
//__________Ftp
extern bool ftp_data;
extern bool cam_fl_up_flag;
//extern char textBuf[100];
extern unsigned char ItoaStr[15];
extern unsigned char FtoaStr[20];
extern s32 tm_ret;						///onesec timer return value.
//____________
bool Synchronise_cmd2 = 0;
bool Initialize_cmd2 = 0;
bool takepic_cmd2 = 0;
bool snapshot_cmd2 = 0;
bool getdata_cmd2 = 0;
//bool getdata1_cmd = 0;
bool cmd_picready2 = 0;
bool snapshot_cmd2_fire =0;
bool Packetsize_cmd2 = 0;
bool reset_cam_flg2=0;

bool open_ftp_flg2=0;
bool send_ftp_flg2=0;
bool  close_ftp_flg2=0;

bool ovrwr_flg2=0;
extern bool stop_img_capt;
extern double mCurrSpeed;
int stuck_ptr2=0;

//extern bool cam_connect_success;

//bool red_fl = 0;
//bool sen_fl = 0;

u32 cam_wrt_cnt2=0;
u32 readcount22=0;
u32 dump_cam_wrt_cnt2=0;
int images2=0;

int img_taken2=0;
int x2=0;
u32 count2=0;
u32 count211=0;
s32 cam_coun1232=0;
//bool getdata1_cmd_fire;
bool Synchronise_cmd2_fire=0;
bool Initialize_cmd2_fire=0;
bool takepic_cmd2_fire=0;
bool getdata_cmd2_fire=0;
bool Packetsize_cmd2_fire=0;


u32 cam_length2=0;
u32 data_cam_length2=0;
u32 cam_datalen2=0;
u32 readedlen2=0;
unsigned int nullindex2[1500];
unsigned int nullarry2=0;

//char readbuffer_cam2[25000];
char FTPFILENAME2[20];
//char pfile_FTPUID[7] ="8138_";
extern char CAMUID[];
char pfile_FTPTXT2[5] =".txt";

char pfile_imgdata2[20] ="Imagedata2.txt";
char pfile_new2[20]="swap_file2.txt";

extern QlTimer tm;
extern QlTimer tm2;
extern QlTimer tm11,onesectimer;
extern QlTimer get_img,get_img2,OS_ExcpTmr_C;
extern QlTimer timer_1,timer_stuck,timer_sync,timer_sync2,timer_12,timer_stuck2;
//extern buffer[];
//extern u16 datalen2;
extern u16 ucamdatalen2;
extern u8 cam_cmd_type;
extern bool cam_bDoNexAT;
extern PortData_Event* pPortEvt;
int fileincr2=0;
extern u16 cam2_cmd_idx;
//extern char cam_pfile_FTP[20];
extern bool cam_cap_time;

extern char cam_uart_readbuffer2[3000];
extern bool camdumpflag;////flag to read cmdump sms

extern bool synctimer2;
extern bool firsttimer2;
extern bool timer_tm2;
extern bool stucktimer2;
extern bool imageget2;
extern bool in_cam_rou2;

extern char DATE[7];
extern char TIME[7];

//extern bool stop_img_capt;
extern u32 readcount;
extern bool in_cam_rou;
extern bool Synchronise_cmd;


extern bool camcaptureflag;
extern int camindicatorflag;
extern bool datareading;

extern int cam1dump;
extern int cam2dump;
extern bool timerstarted;

//camera stueck flag
int cam2_stk_flg=0;
extern int onestuck2;

////////generate CW and CR stamps
int error_wrt_2=0;    ///to generate error stamp
int error_read_2=0;    ///to generate error stamp
extern char GPRMC[];
extern char UID[];
extern char STAT[];
extern double mCurrSpeed,mTotDist;
extern unsigned char ItoaStr[];
//s32 buffer11[2000];

extern QlTimer onesecdummy; 

 void cam_uartdata2(char *cam_uart_readbuffer2)
 {
     //cam_cap_time =1;
     take_pic_cmd_fun2();
     //DC//RR//OUT_D1EBUG(textBuf,"in cam uaret data2 funtion\r\n");
     //	cam_length2=cam_length2+datalen2;
    // OUT_D1EBUG(textBuf,"cam_length2=%d\r\n",cam_length2);

     if(Synchronise_cmd2_fire == 1)
     {
    	OUT_D1EBUG(textBuf,"Synchronise_cmd2 matched\r\n");
    	//DC//RR//OUT_D1EBUG(textBuf,"Synchronise_cmd2 == 1\r\n");
		Synchronise_cmd2_fire = 0;
		Synchronise_cmd2=0;
		Initialize_cmd2 =1;
     }
     else if(Initialize_cmd2_fire == 1)
     {

   		OUT_D1EBUG(textBuf,"Initialize_cmd2=1\r\n");
		takepic_cmd2 = 1;
		Initialize_cmd2_fire =0;
		Ql_StopTimer(&timer_sync2);
		// timer_sync2.timerId =0;
		
		if(timerstarted==0)
		{
			timerstarted=1;
			// onesecdummy.timerId =0;
			tm_ret= Ql_StartTimer(&onesecdummy);
			OUT_D1EBUG(textBuf,"\r\n onesectimer with timer ID in cam2 _2=%d\r\n", tm_ret);
		}

		take_pic_data2();
     }
     else if(ucamdatalen2 > 6)
     {
         stuck_ptr2=0;
         //in_cam_rou2=1;
        // OUT_D1EBUG(textBuf,"IN data storing function\r\n");
         cam_datalen2=cam_datalen2+ucamdatalen2;
         data_cam_length2=data_cam_length2+ucamdatalen2;
        // OUT_D1EBUG(textBuf,"cam_length2=%d\r\n",cam_length2);
         //cam_coun1232++;

         if(getdata_cmd2_fire == 1)
         {
        	// OUT_D1EBUG(textBuf,"getdata_cmd2 == 1\r\n");
        	 getdata_cmd2_fire=0;
         }
                    
		for(count2 =0;count2<ucamdatalen2;count2++)
		{
			stuck_ptr2=0;
                        
			if(cam_uart_readbuffer2[count2]=='\0')
            {
				cam2_readbuffer[count211]='A';
				//OUT_D1EBUG(textBuf,"%c",cam2_readbuffer[count211]);
				nullindex2[nullarry2]=count211;
				nullarry2++;
				count211++;
            }
            else
            {
            	stuck_ptr2=0;
            	cam2_readbuffer[count211]=cam_uart_readbuffer2[count2];
            	//OUT_D1EBUG(textBuf,"%c",cam2_readbuffer[count211]);
                          
            	if((cam_uart_readbuffer2[count2] - '!' - 'd' )== 'z')
                {
            		count2++;
            		count211++;
                   
            		if((cam_uart_readbuffer2[count2] - '!' - 'd' )== 'T')
            		{
            			//\\OUT_D1EBUG(textBuf,"\r\n  d9 found.\r\n");
            			cam2_readbuffer[count211]=cam_uart_readbuffer2[count2];
            			Ql_StopTimer(&get_img2);
            			spped_capt2();
            			break;
            		}
            		else
            		{
            			count2--;
            			count211--;
            		}
                }
            	count211++;
            }
		}
		return;
     }
     return;
}
            

void cam_timer2(void)
{
	if( synctimer2==1)
	{
		synctimer2=0;

		if((Synchronise_cmd2 == 1)&& (mCurrSpeed > 3.00000))
        {
			//\\OUT_D1EBUG(textBuf,"cam2_sync_cmd=%s\r\n",cam2_sync_cmd);
			Ql_SendToUart(ql_uart_port2,&cam2_sync_cmd[0],6);
			tm_ret= Ql_StartTimer(&timer_sync2);
			Synchronise_cmd2_fire=1;
        }
		if(Initialize_cmd2 == 1)
		{
			//\\OUT_D1EBUG(textBuf,"cam2_init_cmd=%s\r\n",cam2_init_cmd);
			Ql_SendToUart(ql_uart_port2,&cam2_init_cmd[0],6);
			tm_ret=Ql_StartTimer(&timer_sync2);
			Initialize_cmd2=0;
			Initialize_cmd2_fire=1;
		}
	}
    if( firsttimer2==1)
    {
    	firsttimer2=0;
    	readcount22=0;
    	readcount=0;
    	fun_cam2_read_cnt();
    	fun_cam_read_cnt();
    	dump_cam_wrt_cnt2 = cam_wrt_cnt2;
    	//\\OUT_D1EBUG(textBuf,"camera2.c file read dump_cam_wrt_cnt2=%d\r\n",dump_cam_wrt_cnt2);
    	////write dump_cam_wrt_cnt2
    	fun_dump_cam2_wrt_cnt();
    	cam_wrt_cnt2=0;
    	//fun_cam_wrt_cnt2();
    	++ovrwr_flg2;
    	//\\OUT_D1EBUG(textBuf,"\r\nThe timer_12 up.\r\n");
	}
	if( timer_tm2==1)
	{
		timer_tm2=0;
		//\\OUT_D1EBUG(textBuf,"\r\ncamera2. The timer_tm2 up.\r\n");
	}
	if( stucktimer2==1)
	{
		stucktimer2=0;
		++ stuck_ptr2;

		if(stuck_ptr2 >=3)
		{
			OUT_D1EBUG(textBuf,"\r\n Camera got stucked__C2.\r\n");
			count211=0;
			cam_length2=0;
			cam_datalen2=0;
			nullarry2=0;
			ffFound2 = 0;
			d9Found2 = 0;
			d8Found2 = 0;
			takepic_cmd2_fire = 0;
			snapshot_cmd2_fire =0;
			snapshot_cmd2=0;
			getdata_cmd2=0;
			Ql_memset((char *)cam2_readbuffer,'\0',sizeof(cam2_readbuffer));
			Ql_memset((char *)cam2_final_readbuffer,'\0',sizeof(cam2_final_readbuffer));
			Ql_memset((char *)nullindex2,0,sizeof(nullindex2));
		}
	}
	if( imageget2==1)
	{
		getdata_cmd2_fire=0;
		//	stop_img_capt=0;
		imageget2=0;
		spped_capt2();
	}
	return;
}

void cat_all2(void)
{
	u16 len=0,fileret=0;
	unsigned int j=0,k, f_RE_read_len=0,f_read_len=0;
	stuck_ptr2=0;
	cam2_stk_flg=0; 
	onestuck2=0;
	images2 ++;
	f_RE_read_len=Ql_strlen((char *)cam2_readbuffer);
	
	Ql_memset((char *)cam2_final_readbuffer,0,sizeof(cam2_final_readbuffer));
	Ql_strcat((char *)cam2_final_readbuffer,(char *)cam2_readbuffer);
	Ql_strcat((char *)cam2_final_readbuffer,"Transworld_CameraIndex_");
	Ql_strcat((char *)cam2_final_readbuffer,(char *)DATE);
	Ql_strcat((char *)cam2_final_readbuffer,",");
	Ql_strncat((char *)cam2_final_readbuffer,(char *)TIME,6);
	Ql_strcat((char *)cam2_final_readbuffer,":");           
	f_read_len2=Ql_strlen((char *)cam2_final_readbuffer);
	Ql_strcat((char *)cam2_final_readbuffer,(char *)IndexFile2);
	Ql_strcat((char *)cam2_final_readbuffer,"RBA");
	Ql_memset((char *)IndexFile2,'\0',sizeof(IndexFile2));
	f_read_len2=Ql_strlen((char *)cam2_final_readbuffer);
	Ql_memset((char *)cam2_readbuffer,'\0',sizeof(cam2_readbuffer));
	
	if(f_read_len2 >=100)
    {
		Ql_memset((char *)Expt_tempbuffer_C,0,sizeof(Expt_tempbuffer_C));
		Ql_strcat((char *)Expt_tempbuffer_C,(char *)cam2_final_readbuffer);
		f_read_len=Ql_strlen((char *)Expt_tempbuffer_C);
		OUT_D1EBUG(textBuf,"file Expt_tempbuffer_C length=%d\r\n",f_read_len);

		if((RARDflg_C == 1) &&(expt_cntr_C ==1))
		{
			RARDflg_C=0;
			expt_cntr_C=0;
			Ql_strcat((char *)Expt_RARDbuffer_C,(char *)Expt_tempbuffer_C);
			f_read_len=Ql_strlen((char *)Expt_RARDbuffer_C);
			OUT_D1EBUG(textBuf,"file Expt_RARDbuffer_C length 3rd =%d\r\n",f_read_len);
			cam_C_Excp_filewrite((char *)Expt_RARDbuffer_C);

		}
		if((RARDflg_C == 1) && (expt_cntr_C ==0 ))
		{
			Ql_strcat((char *)Expt_RARDbuffer_C,(char *)Expt_extratempbuffer_C);
			f_read_len=Ql_strlen((char *)Expt_RARDbuffer_C);
			OUT_D1EBUG(textBuf,"file Expt_RARDbuffer_C length 1st =%d\r\n",f_read_len);
			Ql_strcat((char *)Expt_RARDbuffer_C,(char *)Expt_tempbuffer_C);
			f_read_len=Ql_strlen((char *)Expt_RARDbuffer_C);
			OUT_D1EBUG(textBuf,"file Expt_RARDbuffer_C length 2nd =%d\r\n",f_read_len);
			expt_cntr_C=1;
		}
		else if(OSflag_C==1)										//after timer wakes up make flag 1
		{
			OSflag_C=2;
			OUT_D1EBUG(textBuf,"Writing OS_C image.\r\n");
			OS_ExcpTmr_C.timeoutPeriod = Ql_SecondToTicks(10);		//Start 10 sec timer.
			Ql_StartTimer(&OS_ExcpTmr_C);
			cam_C_Excp_filewrite((char *)Expt_tempbuffer_C);
		}

		cam_tw_filewrite2((char *)cam2_final_readbuffer);
    	//OUT_D1EBUG(textBuf,"Image sent\r\n");
  	}
 		
	count211=0;
 	cam_length2=0;
 	cam_datalen2=0;
 	nullarry2=0;
 	ffFound2 = 0;
 	d9Found2 = 0;
	d8Found2 = 0;

	if(stop_img_capt == 0)
 	{
		//OUT_D1EBUG(textBuf,"take_pic_data2() going to fire..\r\n");
		takepic_cmd2=1;
	    take_pic_data2();
    }
}


void new_file2(char *sendbuff)
{
	s32 retf;
	s32 ret1;
	s32 ret2;
 	u32 writeedlen;
 	s32 filehandle_FTP;
 	u32 sendbuff_len=0;
 	u16 len,fileret;
 	
    char modem_str[30];
	unsigned int j=0,k,tp;
	
	if(filecounter2==0)
 	{
 		fileposition2=0;
 		//OUT_D1EBUG(textBuf,"file read dummyreadcount=%lu\r\n",dummyreadcount);
 	}
 	Ql_strcat((char *)sendbuff,"RBA");
 	sendbuff_len=Ql_strlen((char *)sendbuff);
 	retf = Ql_FileOpenEx((u8*)pfile_new2,QL_FS_CREATE);

    if(retf >= QL_RET_OK)
    {
    		
	    filehandle_FTP = retf;	
		ret2 = Ql_FileSeek(filehandle_FTP,fileposition2,QL_FS_FILE_BEGIN);
		//	OUT_D1EBUG(textBuf,"twBuffer=%s\r\n",tw);
	    retf = Ql_FileWrite(filehandle_FTP, (u8*)sendbuff,sendbuff_len,&writeedlen);
	    OUT_D1EBUG(textBuf,"file Write retf=%d\r\n",retf);
		fileposition2=fileposition2+writeedlen;
	    Ql_FileClose(filehandle_FTP);
	    filehandle_FTP = -1;	
	    Ql_memset((char *)sendbuff,'\0',sizeof(sendbuff));
	    
	    if((filecounter2>=13) || (read_comp2==1))
		{    
	    	++fileincr2;
	    	fun_img_2cnt();
	    	//write image number
	    	x2=fileposition2;
	    	filecounter2=0;
	    	readcount22=readcount22-x2;
	    	fun_cam2_read_cnt();
	    	OUT_D1EBUG(textBuf,"file read readcount=%lu\r\n",readcount);
	    	OUT_D1EBUG(textBuf,"file read xxxxxxxxxx=%lu\r\n",x2);
	    	Ql_memset((char *)FTPFILENAME2,'\0',sizeof(FTPFILENAME2));
	    	//Ql_strcat((char *)FTPFILENAME2,(char *)pfile_FTPUID);
	    	Ql_strcat((char *)FTPFILENAME2,(char *)CAMUID);
	    	Ql_strncat(FTPFILENAME2,"_",1);
	    	Ql_strcat((char *)FTPFILENAME2,(char *)bulk_n);
	    	Ql_strcat((char *)FTPFILENAME2,(char *)(ix_Itoa(fileincr2)));
	    	Ql_strcat((char *)FTPFILENAME2,"C");
	    	Ql_strcat((char *)FTPFILENAME2,(char *)pfile_FTPTXT2);
	    	OUT_D1EBUG(textBuf,"file write successful\r\n");
	   		cam_fl_up_flag=1;
			ftp_data=1;
	        cam2_cmd_idx = 1;
            SendAtCmd2();
            OUT_D1EBUG(textBuf,"file sending successful\r\n");
       	}
     	else
     	{
     		filecounter2++;
     		Ql_memset(readbuffer_cam2,'\0',35000);
     		Ql_memset(sendbuffer_cam2,'\0',35000);
     		tw_SD_fileread2();
     	}
	
	}
	Ql_memset((char *)sendbuff,'\0',sizeof(sendbuff));
	sendbuff_len=0;
	return;
}

void spped_capt2(void)
{
	unsigned int i=0,n_read_len=0;
	char xxxxxx;
	unsigned char NullArray_buff[8];
	cam2_stk_flg=0;         ///camera stuck flag
	onestuck2=0;
	count211++;
    //nullarry2++;
	stuck_ptr2=0;
	cam2_readbuffer[count211]='\0';
	nullindex2[nullarry2]='\0';
	cmd_picready2=0;

   while(i < nullarry2)
   {
		Ql_memset((char *)NullArray_buff,0,sizeof(NullArray_buff));
		Ql_strcat((char *)NullArray_buff,(char *)(ix_Itoa(nullindex2[i])));
		Ql_strcat((char *)IndexFile2,(char *)NullArray_buff);
		Ql_strcat((char *)IndexFile2," ");
		i++;
	}
   cat_all2();
}

void take_pic_cmd_fun2(void)
{
	//OUT_D1EBUG(textBuf,"In take_pic_cmd_fun2 \r\n");
 	stuck_ptr2=0;
 	cam2_stk_flg=0;
 	onestuck2=0;

 	if(takepic_cmd2_fire == 1)
	{
   		takepic_cmd2_fire = 0;
		snapshot_cmd2_fire =1;
		take_pic_data2();
	}
	else if(snapshot_cmd2 == 1)
    {
		snapshot_cmd2=0;
		getdata_cmd2=1;
		take_pic_data2();
    }
 	return;
}



void take_pic_data2(void)
{
	//DC//RR//OUT_D1EBUG(textBuf,"In take_pic_data2 \r\n");
	stuck_ptr2=0;

	if(takepic_cmd2 == 1)
    {
		//DC//RR//OUT_D1EBUG(textBuf,"cam_takepic_cmd2=%s\r\n",cam_takepic_cmd2);
		Ql_SendToUart(ql_uart_port2,&cam_takepic_cmd2[0],6);
		takepic_cmd2=0;
		takepic_cmd2_fire=1;
		//DC//RR//OUT_D1EBUG(textBuf,"cam_takepic_cmd2 fired ___=%s\r\n",cam_takepic_cmd2);
    }
    if(snapshot_cmd2_fire == 1)
    {
    	//DC//RR//OUT_D1EBUG(textBuf,"cam_snapshot_cmd22=%s\r\n",cam_snapshot_cmd2);
    	Ql_SendToUart(ql_uart_port2,&cam_snapshot_cmd2[0],6);
    	snapshot_cmd2=1;
    	snapshot_cmd2_fire=0;
    }
    if((camcaptureflag==1) && (stop_img_capt == 0) && (getdata_cmd2 == 1))
    {
    	//DC//RR//OUT_D1EBUG(textBuf,"cam_getdata_cmd2=%s\r\n",cam_getdata_cmd2);
    	Ql_SendToUart(ql_uart_port2,&cam_getdata_cmd2[0],6);
    	getdata_cmd2=0;
        tm_ret=Ql_StartTimer(&get_img2);
        //OUT_D1EBUG(textBuf,"\r\n get_img2 with timer ID in cam2 _2_1=%d\r\n", tm_ret);
        getdata_cmd2_fire=1;
    }
    return;
}

void cam_tw_filewrite2(char *writebuffer)
{
	s32 wrfl_ret;
	s32 retf;
	s32 ret_flsk;
    u32 writeedlen;
    u32 readedlen2;
	char *ptr;
	s32 *filesize;
	u32 f_read_len=0;
	u32 writbuffer_len=0;
	s32 filehandle_imgdata;
	int j=0,k;
	cam2_stk_flg=0;
	onestuck2=0;					 ///camera stuck flag
	unsigned char readbuf[25000];
	stuck_ptr2=0;
	writbuffer_len=Ql_strlen((char *)writebuffer);
	
	Ql_memset((char *)Expt_extratempbuffer_C,0,sizeof(Expt_extratempbuffer_C));
	Ql_strcat((char *)Expt_extratempbuffer_C,(char *)writebuffer);
	f_read_len=Ql_strlen((char *)Expt_extratempbuffer_C);
	OUT_D1EBUG(textBuf,"file Expt_extratempbuffer_C length=%d\r\n",f_read_len);

	if(cam_wrt_cnt2 <= 104857600)
    {
	    OUT_D1EBUG(textBuf,"cam_wrt_cnt2=%d: \r\n",cam_wrt_cnt2);
		wrfl_ret = Ql_FileOpenEx(PATH_CAMERA2,QL_FS_CREATE);
	    
		if(wrfl_ret >= QL_RET_OK)
	    {
			filehandle_imgdata = wrfl_ret;
			ret_flsk = Ql_FileSeek(filehandle_imgdata,cam_wrt_cnt2,QL_FS_FILE_BEGIN);
			//OUT_D1EBUG(textBuf,"twBuffer=%s\r\n",tw);
			retf = Ql_FileWrite(filehandle_imgdata,(u8*)writebuffer,Ql_strlen((char*)writebuffer),&writeedlen);
			cam_wrt_cnt2=cam_wrt_cnt2+writbuffer_len;
 	 		fun_cam2_wrt_cnt();
 	 		// need to write save data fun
 	 	    Ql_FileClose(filehandle_imgdata);
 	 	    filehandle_imgdata = -1;
 	 	    //OUT_D1EBUG(textBuf,"Image Stored camera 2\r\n");
   		}
		else
		{
   		    OUT_D1EBUG(textBuf,"ERROR in cam2 file writing\r\n");
   		    if(error_wrt_2==0)
   		    {
   		    	Gen_CW();
   		    	OUT_D1EBUG(textBuf,"Generate CW stamp cam2.\r\n");
   			}
   		    error_wrt_2+=1;
   			if(error_wrt_2>=600)
   			{
   				error_wrt_2=0;
   			}
		}
	}
	
	else
	{
		//\\OUT_D1EBUG(textBuf,"Memory Full\r\n");
		//	cam_wrt_cnt2=0;
		dump_cam_wrt_cnt2 = cam_wrt_cnt2;
		//\\OUT_D1EBUG(textBuf,"filewrigt camera2.c file read dump_cam_wrt_cnt2=%d\r\n",dump_cam_wrt_cnt2);
		////write dump_cam_wrt_cnt2
		fun_dump_cam2_wrt_cnt();
		cam_wrt_cnt2=0;
		fun_cam2_wrt_cnt();
	}
	return;
}
	
//void tw_fileread2(void)
void tw_SD_fileread2(void)
{
    int i=0,j,k,img;
    s32 ret_r;
    s32 ret_refl,fileret; 
    //	char *ptr1;
	u32 filehandle_readfl;
    u32 size;
    u32 freespace;
    u32 writeedlen;
    u32 readedlen2;
	in_cam_rou2=1;
	datareading=1;
	u32 Diff_read2=0;
	fun_datareading();
	cam2dump=1;
	fun_cam2dump();
	x2=0;
	OUT_D1EBUG(textBuf,"**********************in tw_fileread2**********************\r\n");
 	Ql_StopTimer(&timer_1); 
 	Ql_StopTimer(&onesecdummy); 
 	//OUT_D1EBUG(textBuf,"file read readcount22=%d\r\n",readcount22);
 	//OUT_D1EBUG(textBuf,"fileread2 camera2.c file read dump_cam_wrt_cnt2=%d\r\n",dump_cam_wrt_cnt2);

 	if(readcount22 < dump_cam_wrt_cnt2)
 	{
 		ret_refl = Ql_FileOpenEx(PATH_CAMERA2,QL_FS_CREATE);
 		OUT_D1EBUG(textBuf,"Readfile open ret=%d: \r\n",ret_refl);
 		read_comp2=0;

 		if(ret_refl >= QL_RET_OK)
 		{
 			filehandle_readfl = ret_refl;
 			Ql_memset(readbuffer_cam2,'\0',35000);
      		Ql_memset(sendbuffer_cam2,'\0',35000);
      		ret_refl = Ql_FileSeek(filehandle_readfl, readcount22, QL_FS_FILE_BEGIN);
	        ret_refl = Ql_FileRead(filehandle_readfl, (unsigned char *)readbuffer_cam2,35000, &readedlen2);
	       // OUT_D1EBUG(textBuf,"readbuffer_cam2= %s\r\n",readbuffer_cam2);
	        Ql_FileClose(filehandle_readfl);
	        filehandle_readfl = -1;

	        for(img=0;img<35000;img++)
	        {
	        	if(readbuffer_cam2[x2] == 'R')
	        	{
	        		sendbuffer_cam2[i]=readbuffer_cam2[x2];
	        		//OUT_D1EBUG(textBuf,"%c",sendbuffer_cam2[i]);
	        		i++;
	        		x2++;
			
	        		if(readbuffer_cam2[x2] == 'B')
	        		{
	        			//OUT_D1EBUG(textBuf,"%d\r\n",x2);
	        			//OUT_D1EBUG(textBuf,"*****%%%%I found\r\n");
	        			sendbuffer_cam2[i]=readbuffer_cam2[x2];
	        			//OUT_D1EBUG(textBuf,"%c",sendbuffer_cam2[i]);
	        			i++;
	        			x2++;

	        			if(readbuffer_cam2[x2] == 'A')
	        			{
	        				x2++;
			  				//OUT_D1EBUG(textBuf,"%d\r\n",x2);
	        				--i;
	        				sendbuffer_cam2[i]='\0';
	        				--i;
	        				sendbuffer_cam2[i]='\0';
	        				--i;
	        				sendbuffer_cam2[i]='\0';
	        				readcount22=readcount22+x2;
	        				fun_cam2_read_cnt();
	        				Diff_read2=dump_cam_wrt_cnt2-readcount22;
	        				//DC//OUT_D1EBUG(textBuf,"__++++Diff_read2___+++====%d",Diff_read2);

	        				if(Diff_read2<=30000)
	        				{
	        					read_comp2=1;
	        				}
	        				new_file2((char *)sendbuffer_cam2);
	        				//////////read count2 save
	        				OUT_D1EBUG(textBuf,"readcount22====%d\r\n",x2);

	        				if(readcount22 >= dump_cam_wrt_cnt2)   // check the flg for data send thtough FTP
	        				{
	        					//\\OUT_D1EBUG(textBuf,"fileread2 else camera.2 file read dump_cam_wrt_cnt2=%d\r\n",dump_cam_wrt_cnt2);
	        					return;
	        				}
							break;
	        			}
	        		}
	        		else
	        		{
	        			sendbuffer_cam2[i]=readbuffer_cam2[x2];
	        			i++;
	        			x2++;
	        			//OUT_D1EBUG(textBuf,"%c",readcount22);
				    }
	        	}
	        	else
	        	{
	        		sendbuffer_cam2[i]=readbuffer_cam2[x2];
	        		i++;
	        		x2++;
	        		//OUT_D1EBUG(textBuf,"%c",readcount22);
		        }
	        }
		}
 		else
 		{
 			//	OUT_D1EBUG(textBuf,"Error in file reading\r\n");
 			if(error_read_2==0)
   		    {
 				Gen_CR();
 				//	OUT_D1EBUG(textBuf,"Generate CR stamp.\r\n");
   			}
   			error_read_2+=1;
   			if(error_read_2>=600)
   			{
   				error_read_2=0;
   			}
 		}
 	}
 	else
 	{
 		OUT_D1EBUG(textBuf,"file read readcount22=%d\r\n",readcount22);
 		OUT_D1EBUG(textBuf,"file read dump_cam_wrt_cnt2=%d\r\n",dump_cam_wrt_cnt2);
 		Gen_TD2();
 		//\\OUT_D1EBUG(textBuf,"@@@@@@@@@@@@@@@@@@@#################### File read complited##################### @@@@@@@@@@@@@@@@@@@@@@r\n");
 		datareading=0;
 		fun_datareading();
 		stop_img_capt=0;
 		in_cam_rou2=0;
 		readcount22=0;
 		fun_cam2_read_cnt();
 		cam2dump=0;
 		fun_cam2dump();
 		///cam 1
 		in_cam_rou=0;
 		camdumpflag=0;
 		fun_camdumpflag();
 		cam1dump=0;
 		readcount=0;
 		fun_cam_read_cnt();
 		fun_cam1dump();
 		fun_cam_read_cnt();
 		in_cam_rou=0;
 		camindicatorflag=0;
 		return;
   }
}


void clrcam2flags(void)
{
	//OUT_D1EBUG(textBuf,"\r\nCamera got stucked_C22.\r\n");
	count211=0;
 	cam_length2=0;
 	cam_datalen2=0;
 	nullarry2=0;
 	ffFound2 = 0;
 	d9Found2 = 0;
 	d8Found2 = 0;
 	takepic_cmd2_fire = 0;
 	snapshot_cmd2_fire =0;
 	snapshot_cmd2=0;
 	getdata_cmd2=0;
 	Ql_StopTimer(&get_img2);
 	Ql_memset((char *)cam2_readbuffer,'\0',sizeof(cam2_readbuffer));
 	Ql_memset((char *)cam2_final_readbuffer,'\0',sizeof(cam2_final_readbuffer));
 	Ql_memset((char *)nullindex2,0,sizeof(nullindex2));
  	//return;
}
