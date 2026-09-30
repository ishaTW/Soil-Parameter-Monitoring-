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
#include "ql_fcm.h"
#include "ql_trace.h"
#include "ftp2.h"
#include "camera.h"
#include "camera2.h"
#include "ftp.h"
#include "Fun.h"
#include "MRW.h"
//#include "ftp.c"
#include "Ql_error.h"
#include "exce_camera_R.h"

#define  PATH_CAMERA ((u8 *)"SD:camera_R.txt")

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


QlEventBuffer g_event; // Keep this variable a global variable due to its big size
char notes[100];
extern char textBuf[1000];
char readbuffer_cam[35000];
char sendbuffer_cam[35000];
char Expt_tempbuffer[35000];
char Expt_extratempbuffer[35000];
char Expt_RARDbuffer[70000];
u32 expt_cntr=0;
u32 RARDflg=0;
u32 OSflag=0;


u8 cam_sync_cmd[]={0xAA,0x0D,0x00,0x00,0x00,0x00};
//u8 cam_init_cmd[]={0xAA,0x01,0x00,0x07,0x03,0x07};				//640
u8 cam_init_cmd[]={0xAA,0x01,0x00,0x07,0x03,0x05};            //320
u8 cam_takepic_cmd[]={0xAA,0x04,0x01,0x00,0x00,0x00};
u8 cam_snapshot_cmd[]={0xAA,0x05,0x01,0x00,0x00,0x00};
u8 cam_reset_cmd[]={0xAA,0x08,0x00,0x00,0x00,0x00};
u8 cam_pack_size[]={0xAA,0x06,0x08,0x00,0x04,0x00};
//u8 cam_snapshot_cmd[]={0xAA,0x04,0x00,0x00,0x00,0x00};
//u8 cam_takepic_cmd[]={0xAA,0x05,0x00,0x00,0x00,0x00};
u8 cam_getdata_cmd[]={0xAA,0x0E,0x00,0x00,0x00,0x00};
u8 cam_getdata1_cmd[]={0xAA,0x0E,0x00,0x00,0x01,0x00};
u8 cam_readbuffer[15000];
u8 cam_readbuffer11[20000];
char cam_final_readbuffer[35000];
char IndexFile[25000];
bool ffFound = 0, d9Found = 0 ,d8Found = 0;
int fcount=0;
u16 f_read_len=0;

//__________Ftp
bool ftp_data=0;
extern bool cam_fl_up_flag;
//extern char textBuf[100];
extern unsigned char ItoaStr[15];
extern unsigned char FtoaStr[20];

extern bool camdumpflag;////flag to read cmdump sms
bool Synchronise_cmd = 0;
bool Initialize_cmd = 0;
bool takepic_cmd = 0;
bool snapshot_cmd = 0;
bool getdata_cmd = 0;
bool getdata1_cmd = 0;
bool cmd_picready = 0;
bool snapshot_cmd_fire =0;
bool Packetsize_cmd = 0;
bool reset_cam_flg=0;

bool open_ftp_flg=0;
bool send_ftp_flg=0;
bool  close_ftp_flg=0;

bool ovrwr_flg=0;
bool stop_img_capt=0;
bool datareading=0;

int stuck_ptr=0;

extern bool cam_connect_success;

bool red_fl = 0;
bool sen_fl = 0;

u32 cam_wrt_cnt=0;
u32 readcount=0;
u32 dummyreadcount=0;
u32 dump_cam_wrt_cnt=0;
int images=0;

int img_taken=0;

u32 count=0;
u32 count11=0;
s32 cam_coun123=0;
bool getdata1_cmd_fire=0;
bool Synchronise_cmd_fire=0;
bool Initialize_cmd_fire=0;
bool takepic_cmd_fire=0;
bool getdata_cmd_fire=0;
bool Packetsize_cmd_fire=0;
int cam1_stk_flg=0;
extern int onestuck1;

u32 cam_length=0;
u32 data_cam_length=0;
u32 cam_datalen=0;
u32 readedlen=0;

u32 filecounter=0;
u32 fileposition=0;
u32 read_comp=2;

unsigned int nullindex[1500];
unsigned int nullarry=0;

//char readbuffer_cam[25000];
char FTPFILENAME[20];
//char pfile_FTPUID[7] ="8138_";
extern char CAMUID[10];
char pfile_FTPTXT[5] =".txt";
char bulk_n[6]="Bulk_";

char pfile_imgdata[20] ="Imagedata.txt";
char pfile_new[20]="swap_file.txt";
char cam1_mem_loc[20] ="cam1memory.txt";
u8 cam1memwcnt=0;

extern QlTimer tm;
extern QlTimer tm11,onesectimer;
extern QlTimer timer_1,timer_stuck,timer_sync;
//extern buffer[];
//extern u16 datalen;
extern u16 ucamdatalen;
extern u8 cam_cmd_type;
extern bool cam_bDoNexAT;
extern PortData_Event* pPortEvt;
int fileincr=0;
extern u16  cam_cmd_idx;
//extern char cam_pfile_FTP[20];
extern bool cam_cap_time;
extern QlTimer get_img;
extern char cam_uart_readbuffer[3000];

extern bool synctimer;
extern bool firsttimer;
extern bool timer_tm;
extern bool stucktimer;
extern bool imageget;
extern bool in_cam_rou;
extern char DATE[7];
extern char TIME[7];
extern double mCurrSpeed;
extern u32 cam_wrt_cnt2;
extern u32 readcount22;
extern u32 dump_cam_wrt_cnt2;

extern bool camcaptureflag;
extern int camindicatorflag;

extern bool cam1dump;
extern bool cam2dump;
bool timerstarted=0;
int ftpcamsendflag=0;
u32 x=0;
///////////////dump flag
//extern bool camdumpflag;
////////generate CW and CR stamps
int error_wrt=0;    ///to generate error stamp
int error_read=0;    ///to generate error stamp
extern char GPRMC[];
extern char UID[];
extern char STAT[];
extern double mCurrSpeed,mTotDist;
extern unsigned char ItoaStr[];

extern QlTimer onesecdummy,OS_ExcpTmr;
//u8 mailstr[]={''};
//char mailstr[]={'a','\n','0','0','0','0'};
//char buffer1[12000];
s32 buffer11[2000];
u32 tot=0;

s32 ret=0;
extern s32 tm_ret;			///onesec timer return value.
u32 cnt = 0;

 void cam_uartdata(char *cam_uart_readbuffer)
 {
	 take_pic_cmd_fun();

	 if(Synchronise_cmd_fire == 1)
	 {
		 Synchronise_cmd_fire = 0;
		 Synchronise_cmd=0;
		 Initialize_cmd =1;
	 }
	else if(Initialize_cmd_fire == 1)
	{
   		takepic_cmd = 1;
		Initialize_cmd_fire =0;
		Ql_StopTimer(&timer_sync);

		if(timerstarted == 0)
		{
			//RR//OUT_D1EBUG(textBuf,"\r\n if(timerstarted == 0) Start onesectimer.....\r\n");
			timerstarted=1;
			tm_ret = Ql_StartTimer(&onesecdummy);
			//RR//OUT_D1EBUG(textBuf,"\r\n Onesectimer with timer ID camera_1=%d\r\n", tm_ret);
		}
		take_pic_data();
	}
	else if(ucamdatalen > 6)
    {
        stuck_ptr=0;
        cam_datalen=cam_datalen+ucamdatalen;
        data_cam_length=data_cam_length+ucamdatalen;

        if(getdata_cmd_fire == 1)
        {
        	//RR//OUT_D1EBUG(textBuf,"getdata_cmd == 1\r\n");
        	getdata_cmd_fire=0;
        }
                    
        for(count =0;count<ucamdatalen;count++)
        {
        	stuck_ptr=0;

        	if(cam_uart_readbuffer[count]=='\0')
            {
        		cam_readbuffer[count11]='A';
        		//RR//OUT_D1EBUG(textBuf,"%c",cam_readbuffer[count11]);
        		nullindex[nullarry]=count11;
                nullarry++;
                count11++;
            }
            else
            {
            	stuck_ptr=0;
                cam_readbuffer[count11]=cam_uart_readbuffer[count];
               
                if((cam_uart_readbuffer[count] - '!' - 'd' )== 'z')
                {
                	count++;
                	count11++;

                	if((cam_uart_readbuffer[count] - '!' - 'd' )== 'T')
                	{
                		//OUT_D1EBUG(textBuf,"  d9 found.\r\n");
                		cam_readbuffer[count11]=cam_uart_readbuffer[count];
                		Ql_StopTimer(&get_img);
                		spped_capt();
                		break;
                	}
                	else
                	{
                		count--;
                		count11--;
                	}
                }
                count11++;
            }
        }
        return;
    }
	 return;
}
            
void cam_timer(void)
{
	if(synctimer==1)
	{
		synctimer=0;

		if((Synchronise_cmd == 1)  && (mCurrSpeed > 3.00000))
		{
			//RTR OUT_D1EBUG(textBuf,"cam_sync_cmd=%s\r\n",cam_sync_cmd);
			Ql_SendToUart(ql_uart_port3,&cam_sync_cmd[0],6);
			Ql_StopTimer(&timer_sync);
			tm_ret=Ql_StartTimer(&timer_sync);
			//RTR OUT_D1EBUG(textBuf,"started timer_sync with timer ID in cam1 _1=%d\r\n", tm_ret);
			Synchronise_cmd_fire=1;
		}
		if(Initialize_cmd == 1)
		{
			//RTR OUT_D1EBUG(textBuf,"cam_init_cmd=%s\r\n",cam_init_cmd);
			Ql_SendToUart(ql_uart_port3,&cam_init_cmd[0],6);
			Ql_StopTimer(&timer_sync);
			//timer_sync.timerId =0;
			tm_ret=Ql_StartTimer(&timer_sync);
			//RTR 	OUT_D1EBUG(textBuf,"started timer_sync with timer ID in cam1 _2=%d\r\n", tm_ret);
			Initialize_cmd=0;
			Initialize_cmd_fire=1;
		}
	}
	if( firsttimer==1)
	{
		firsttimer=0;
//R_//OUT_D1EBUG(textBuf,"\r\nTimer UP.. data overwriting..\r\n");
		dump_cam_wrt_cnt = cam_wrt_cnt;
		dump_cam_wrt_cnt2 = cam_wrt_cnt2;   //cam 2
		fun_dump_cam_wrt_cnt();
		fun_dump_cam2_wrt_cnt();
		cam_wrt_cnt=0;
		cam_wrt_cnt2=0;						//cam2
		fun_cam_wrt_cnt();
		fun_cam2_wrt_cnt();
		++ovrwr_flg;
	}
	if(timer_tm==1)
	{
		timer_tm=0;
		Ql_StopTimer(&timer_sync);
		tm_ret=Ql_StartTimer(&timer_sync);
	}
	if(stucktimer==1)
	{
		stucktimer=0;
		++ stuck_ptr;

		if(stuck_ptr >=3)
		{
			count11=0;
			cam_length=0;
			cam_datalen=0;
			nullarry=0;
			ffFound = 0;
			d9Found = 0;
			d8Found = 0;
			takepic_cmd_fire = 0;
			snapshot_cmd_fire =0;
			snapshot_cmd=0;
			getdata_cmd=0;
			Ql_memset((char *)cam_readbuffer,'\0',sizeof(cam_readbuffer));
			Ql_memset((char *)cam_final_readbuffer,'\0',sizeof(cam_final_readbuffer));
			Ql_memset((char *)nullindex,0,sizeof(nullindex));
		}
	}
	if(imageget==1)
	{
		getdata_cmd_fire=0;
		stop_img_capt=0;
		imageget=0;
		spped_capt();
	}
	return;
}


void cat_all(void)
{
	u16 len,fileret;
	unsigned int j=0,k, f_RE_read_len;

	stuck_ptr=0;
	cam1_stk_flg=0;
	onestuck1=0;
	images ++;
	f_RE_read_len=Ql_strlen((char *)cam_readbuffer);
	Ql_memset((char *)cam_final_readbuffer,0,sizeof(cam_final_readbuffer));
	Ql_strcat((char *)cam_final_readbuffer,(char *)cam_readbuffer);
	Ql_strcat((char *)cam_final_readbuffer,"Transworld_CameraIndex_");
	Ql_strcat((char *)cam_final_readbuffer,(char *)DATE);
	Ql_strcat((char *)cam_final_readbuffer,",");
	Ql_strncat((char *)cam_final_readbuffer,(char *)TIME,6);
	Ql_strcat((char *)cam_final_readbuffer,":");          
	f_read_len=Ql_strlen((char *)cam_final_readbuffer);
	Ql_strcat((char *)cam_final_readbuffer,(char *)IndexFile);
	Ql_strcat((char *)cam_final_readbuffer,"RBA");
	Ql_memset((char *)IndexFile,'\0',sizeof(IndexFile));
	f_read_len=Ql_strlen((char *)cam_final_readbuffer);
	Ql_memset((char *)cam_readbuffer,'\0',sizeof(cam_readbuffer));

    if(f_read_len >=100)
    {
    	Ql_memset((char *)Expt_tempbuffer,0,sizeof(Expt_tempbuffer));
    	Ql_strcat((char *)Expt_tempbuffer,(char *)cam_final_readbuffer);
    	f_read_len=Ql_strlen((char *)Expt_tempbuffer);
    	OUT_D1EBUG(textBuf,"file Expt_tempbuffer length=%d\r\n",f_read_len);

    	if((RARDflg == 1) &&(expt_cntr ==1))
    	{
    		RARDflg=0;
    		expt_cntr=0;
    		Ql_strcat((char *)Expt_RARDbuffer,(char *)Expt_tempbuffer);
    		f_read_len=Ql_strlen((char *)Expt_RARDbuffer);
    		OUT_D1EBUG(textBuf,"file Expt_RARDbuffer length 3rd =%d\r\n",f_read_len);
    		camR_Excp_filewrite((char *)Expt_RARDbuffer);

    	}
    	if((RARDflg == 1) && (expt_cntr ==0 ))
    	{
    		Ql_strcat((char *)Expt_RARDbuffer,(char *)Expt_extratempbuffer);
    		f_read_len=Ql_strlen((char *)Expt_RARDbuffer);
    		OUT_D1EBUG(textBuf,"file Expt_RARDbuffer length 1st =%d\r\n",f_read_len);
    		Ql_strcat((char *)Expt_RARDbuffer,(char *)Expt_tempbuffer);
    		f_read_len=Ql_strlen((char *)Expt_RARDbuffer);
    		OUT_D1EBUG(textBuf,"file Expt_RARDbuffer length 2nd =%d\r\n",f_read_len);
    		expt_cntr=1;
    	}
    	else if(OSflag==1)										//after timer wakes up make flag 1
    	{
    		OSflag=2;
    		OUT_D1EBUG(textBuf,"Writing OS image.\r\n");
    		OS_ExcpTmr.timeoutPeriod = Ql_SecondToTicks(10);		//Start 10 sec timer.
    		Ql_StartTimer(&OS_ExcpTmr);
    		camR_Excp_filewrite((char *)Expt_tempbuffer);
    	}
    	cam_tw_filewrite((char *)cam_final_readbuffer);

    }
    count11=0;
    cam_length=0;
    cam_datalen=0;
    nullarry=0;
    ffFound = 0;
    d9Found = 0;
    d8Found = 0;

    if(stop_img_capt == 0)
 	{
    	takepic_cmd=1;
    	take_pic_data();
 	}
}

void new_file(char *sendbuff)
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
	sen_fl=0;
 	 //RTR OUT_D1EBUG(textBuf,"@@@@@@@@@@@@@@@@@@@@@ In new_file @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@2\r\n");
 	
 	if(filecounter==0)
 	{
 		fileposition=0;
 	}
 	Ql_strcat((char *)sendbuff,"RBA");
 	sendbuff_len=Ql_strlen((char *)sendbuff);
 	retf = Ql_FileOpenEx((u8*)pfile_new,QL_FS_CREATE);

 	if(retf >= QL_RET_OK)
    {
 		filehandle_FTP = retf;
		ret2 = Ql_FileSeek(filehandle_FTP,fileposition,QL_FS_FILE_BEGIN);
		retf = Ql_FileWrite(filehandle_FTP, (u8*)sendbuff,sendbuff_len,&writeedlen);
		fileposition=fileposition+writeedlen;

	    Ql_FileClose(filehandle_FTP);
	    filehandle_FTP = -1;	
	    Ql_memset((char *)sendbuff,'\0',sizeof(sendbuff));
	    
	  	if((filecounter>=13) || (read_comp==1))
	  	{
	  		++fileincr;
	  		fun_img_cnt();
	  		//write image number
	  		x=fileposition;
	  		filecounter=0;
	  		readcount=readcount-x;
	  		fun_cam_read_cnt();
	  		Ql_memset((char *)FTPFILENAME,'\0',sizeof(FTPFILENAME));
	  		//Ql_strcat((char *)FTPFILENAME,(char *)pfile_FTPUID);
	  		Ql_strcat((char *)FTPFILENAME,(char *)CAMUID);
	  		Ql_strncat(FTPFILENAME,"_",1);
	  		Ql_strcat((char *)FTPFILENAME,(char *)bulk_n);
	  		Ql_strcat((char *)FTPFILENAME,(char *)(ix_Itoa(fileincr)));
	  		Ql_strcat((char *)FTPFILENAME,"R");
	  		Ql_strcat((char *)FTPFILENAME,(char *)pfile_FTPTXT);
	   	    OUT_D1EBUG(textBuf,"file FTPFILENAME=%s\r\n",FTPFILENAME);
	   	    cam_fl_up_flag=1;
	   	    ftp_data=1;
	   	    cam_cmd_idx = 1;
	   	    SendAtCmd();
	  	}
	  	else
	  	{
	  		filecounter++;
	  		Ql_memset(readbuffer_cam,'\0',35000);
	  		Ql_memset(sendbuffer_cam,'\0',35000);
	  		tw_fileread();
	  	}
    }
	Ql_memset((char *)sendbuff,'\0',sizeof(sendbuff));
	sendbuff_len=0;
	return;
}	


void spped_capt(void)
{
	unsigned int i=0,n_read_len;
	unsigned char Indexcam_length_buff[4];
	unsigned char NullArray_buff[8];
	cam1_stk_flg=0;
	onestuck1=0;										///cam stuck flag
	count11++;
	stuck_ptr=0;
	cam_readbuffer[count11]='\0';
	nullindex[nullarry]='\0';
	cmd_picready=0;

	while(i < nullarry)
	{
		Ql_memset((char *)NullArray_buff,0,sizeof(NullArray_buff));
		Ql_strcat((char *)NullArray_buff,(char *)(ix_Itoa(nullindex[i])));
		Ql_strcat((char *)IndexFile,(char *)NullArray_buff);
		Ql_strcat((char *)IndexFile," ");
		i++;
	}
	cat_all();
}

void take_pic_cmd_fun(void)
{
	////RR//OUT_D1EBUG(textBuf,"In take_pic_cmd_fun \r\n");
	stuck_ptr=0;

	if(takepic_cmd_fire == 1)
	{
		takepic_cmd_fire = 0;
		snapshot_cmd_fire =1;
		take_pic_data();
	}
	else if(snapshot_cmd == 1)
	{
		snapshot_cmd=0;
		getdata_cmd=1;
		take_pic_data();
	}
	return;
}



void take_pic_data(void)
{
	 stuck_ptr=0;		
	 cam1_stk_flg=0;
	 onestuck1=0;											///cam stuck flag		

	 if(takepic_cmd == 1)
	 {
		 ////RR//OUT_D1EBUG(textBuf,"cam_takepic_cmd=%s\r\n",cam_takepic_cmd);
		 Ql_SendToUart(ql_uart_port3,&cam_takepic_cmd[0],6);
		 takepic_cmd=0;
		 takepic_cmd_fire=1;
	 }
   	 if(snapshot_cmd_fire == 1)
   	 {
   		 Ql_SendToUart(ql_uart_port3,&cam_snapshot_cmd[0],6);
   		 snapshot_cmd=1;
   		 snapshot_cmd_fire=0;
   	 }

   	 if((camcaptureflag==1) && (getdata_cmd == 1) && (stop_img_capt==0))
   	 {
   		 ////RR//OUT_D1EBUG(textBuf,"cam_getdata_cmd=%s\r\n",cam_getdata_cmd);
   		 Ql_SendToUart(ql_uart_port3,&cam_getdata_cmd[0],6);
   		 getdata_cmd=0;
   		 Ql_StopTimer(&get_img);
   		 tm_ret=Ql_StartTimer(&get_img);
   		 getdata_cmd_fire=1;
   	 }
   	 return;
}


//File write in SD card
void cam_tw_filewrite(char *writebuffer)
{
	s32 wrfl_ret;
	s32 retf;
	s32 ret_flsk;
    u32 writeedlen;
    u32 readedlen;
	char *ptr;
	s32 *filesize;
	u32 writbuffer_len=0;
	s32 filehandle_imgdata;
	int j=0,k;
	cam1_stk_flg=0;
	onestuck1=0;										///cam stuck flag
	unsigned char readbuf[25000];
	stuck_ptr=0;
	writbuffer_len=Ql_strlen((char *)writebuffer);
	
	Ql_memset((char *)Expt_extratempbuffer,0,sizeof(Expt_extratempbuffer));
	Ql_strcat((char *)Expt_extratempbuffer,(char *)writebuffer);
	f_read_len=Ql_strlen((char *)Expt_extratempbuffer);
	OUT_D1EBUG(textBuf,"file Expt_extratempbuffer length=%d\r\n",f_read_len);

 	if(cam_wrt_cnt <= 104857600)
    {
	   OUT_D1EBUG(textBuf,"cam_wrt_cnt 1=%d\r\n",cam_wrt_cnt);
	    wrfl_ret = Ql_FileOpenEx(PATH_CAMERA,QL_FS_CREATE);

	    if(wrfl_ret >= QL_RET_OK)
	    {
	    	filehandle_imgdata = wrfl_ret;
	    	ret_flsk = Ql_FileSeek(filehandle_imgdata,cam_wrt_cnt,QL_FS_FILE_BEGIN);
	    	retf = Ql_FileWrite(filehandle_imgdata,(u8*)writebuffer,Ql_strlen((char*)writebuffer),&writeedlen);
	    	cam_wrt_cnt=cam_wrt_cnt+writbuffer_len;
	    	fun_cam_wrt_cnt();
	    	Ql_FileClose(filehandle_imgdata);
	    	filehandle_imgdata = -1;
	    	//OUT_D1EBUG(textBuf,"Image Stored camera 1\r\n");
   		}
		else
		{
			OUT_D1EBUG(textBuf,"ERROR in file writing\r\n");

			if(error_wrt==0)
   		    {
				Gen_CW();
   		    	OUT_D1EBUG(textBuf,"Generate CW stamp.\r\n");
   			}
			error_wrt+=1;
			if(error_wrt>=600)
   			{
   				error_wrt=0;
   			}
		}
    }
	else
	{
		////RR//OUT_D1EBUG(textBuf,"Memory Full\r\n");
		dump_cam_wrt_cnt = cam_wrt_cnt;
		fun_dump_cam_wrt_cnt();
		cam_wrt_cnt=0;
		fun_cam_wrt_cnt();
	}
	return;
}	


//FOR SD CARD
void tw_fileread(void)
{
	int i=0,j,k,img;
    s32 ret_r;
    s32 ret_refl,fileret; 
	u32 filehandle_readfl;
    u32 size;
    u32 freespace;
 	u32 Diff_read=0;
    u32 writeedlen;
    u32 readedlen;
    datareading=1;
    fun_datareading();
	in_cam_rou=1;
	//camdumpflag=0;
	x=0;
	//RTR OUT_D1EBUG(textBuf,"**********************in tw_fileread**********************\r\n");
	//OUT_D1EBUG(textBuf,"readcount 1=%d\r\n",readcount);
	//OUT_D1EBUG(textBuf,"dump_cam_wrt_cnt 1=%d\r\n",dump_cam_wrt_cnt);
 	Ql_StopTimer(&timer_1); 
 	Ql_StopTimer(&onesecdummy); 
	
 	if(readcount < dump_cam_wrt_cnt)
 	{
 		ret_refl = Ql_FileOpenEx(PATH_CAMERA,QL_FS_CREATE);
 		read_comp=0;

 		if(ret_refl >= QL_RET_OK)
 		{
 			filehandle_readfl = ret_refl;
 			Ql_memset(readbuffer_cam,'\0',35000);
 			Ql_memset(sendbuffer_cam,'\0',35000);
 			ret_refl = Ql_FileSeek(filehandle_readfl, readcount, QL_FS_FILE_BEGIN);
 			ret_refl = Ql_FileRead(filehandle_readfl, (unsigned char *)readbuffer_cam,35000, &readedlen);
 			Ql_FileClose(filehandle_readfl);
 			filehandle_readfl = -1;

 			for(img=0;img<35000;img++)
 			{
 				if(readbuffer_cam[x] == 'R')
 				{
 					sendbuffer_cam[i]=readbuffer_cam[x];
 					i++;
 					x++;

 					if(readbuffer_cam[x] == 'B')
 					{
 						sendbuffer_cam[i]=readbuffer_cam[x];
 						i++;
 						x++;

 						if(readbuffer_cam[x] == 'A')
 						{
 							x++;
 							--i;
 							sendbuffer_cam[i]='\0';
 							--i;
 							sendbuffer_cam[i]='\0';
 							--i;
 							sendbuffer_cam[i]='\0';
 							readcount=readcount+x;
 							fun_cam_read_cnt();
 							Diff_read=dump_cam_wrt_cnt-readcount;

 							if(Diff_read<=30000)
 							{
 								read_comp=1;
 							}
 							new_file((char *)sendbuffer_cam);

 							if(readcount >= dump_cam_wrt_cnt)   // check the flg for data send thtough FTP
 							{
 								return;
 							}
 							break;
 						}
 					}
 					else
 					{
 						sendbuffer_cam[i]=readbuffer_cam[x];
 						i++;
 						x++;
 					}
 				}
 				else
 				{
 					sendbuffer_cam[i]=readbuffer_cam[x];
 					i++;
 					x++;
 				}
 			}
 		}
 		else
 		{
 			OUT_D1EBUG(textBuf,"Error in file reading\r\n");

 			if(error_read==0)
 			{
 				////CR stamp
 				Gen_CR();
 				OUT_D1EBUG(textBuf,"Generate CR stamp.\r\n");
 			}
 			error_read+=1;
 			if(error_read>=600)
 			{
 				error_read=0;
 			}
 		}
 	}
 	else
 	{
 		OUT_D1EBUG(textBuf,"file read readcount=%d\r\n",readcount);
 		OUT_D1EBUG(textBuf,"file read dump_cam_wrt_cnt=%d\r\n",dump_cam_wrt_cnt);
 		//RTR 	OUT_D1EBUG(textBuf,"@@@@@@@@@@@@@@@@@@@#################### File read complete ##################### @@@@@@@@@@@@@@@@@@@@@@r\n");
 		in_cam_rou=0;
 		Gen_TD1();
 		tw_SD_fileread2();
 		return;
 	}
}


void clrcam1flags(void)
{
	count11=0;
	cam_length=0;
	cam_datalen=0;
	nullarry=0;
	ffFound = 0;
	d9Found = 0;
	d8Found = 0;
	takepic_cmd_fire = 0;
	snapshot_cmd_fire =0;
	snapshot_cmd=0;
	getdata_cmd=0;
	Ql_StopTimer(&get_img);
	Ql_memset((char *)cam_readbuffer,'\0',sizeof(cam_readbuffer));
	Ql_memset((char *)cam_final_readbuffer,'\0',sizeof(cam_final_readbuffer));
	Ql_memset((char *)nullindex,0,sizeof(nullindex));
	return;
}

void fun_cam_wrt_cnt(void)
{
	s32 ret1;
	char *ptr;
	u32 writeedlen;
	//    u32 filehandle;
	u32 filehandle;
	
	ret1 = Ql_FileOpenEx((u8*)cam1_mem_loc,QL_FS_CREATE);
    cam1memwcnt=1;
    if(ret1 >= QL_RET_OK)
    {
        filehandle = ret1;
        ptr=ix_Itoa(cam_wrt_cnt);
        ret1 = Ql_FileSeek(filehandle,cam1memwcnt,QL_FS_FILE_BEGIN);
        ret1 = Ql_FileWrite(filehandle, (u8*)ptr,14,&writeedlen);
       // OUT_D1EBUG(textBuf,"cam_wrt_cnt=%s\r\n",ptr);
        Ql_FileClose(filehandle);
	    filehandle = -1;
    }	
    else
    {
       OUT_D1EBUG(textBuf,"error in cam_wrt_cnt write\r\n");
    }
}
void cam_mem_loc_read(void)
{
     s32 ret;
     u32 readedlen1;     
     u32 filehandle;
     char cam_temp_read_buff[50];
     char cam_temp_dumpflg_buff[3];

     OUT_D1EBUG(textBuf,"\r\n###### Camera Memory Read  ######\r\n");

     ret = Ql_FileOpenEx((u8*)cam1_mem_loc,QL_FS_CREATE);
     
     if(ret >= QL_RET_OK)
     {
		filehandle = ret;
		cam1memwcnt=1;
		///// Camera wrt count read
		Ql_memset((ascii *)cam_temp_read_buff,0,sizeof(cam_temp_read_buff));
		ret = Ql_FileSeek(filehandle,cam1memwcnt, QL_FS_FILE_BEGIN);  
        ret = Ql_FileRead(filehandle, (u8 *)cam_temp_read_buff,14, &readedlen1);
        cam_wrt_cnt=Ql_atoi(cam_temp_read_buff);
        OUT_D1EBUG(textBuf,"\tcam_wrt_cnt \t\t\t= %d\r\n",cam_wrt_cnt);
		cam1memwcnt=80;
		Ql_memset((ascii *)cam_temp_dumpflg_buff,0,sizeof(cam_temp_dumpflg_buff));
		ret = Ql_FileSeek(filehandle,cam1memwcnt, QL_FS_FILE_BEGIN);  
        ret = Ql_FileRead(filehandle, (u8 *)cam_temp_dumpflg_buff,1, &readedlen1);
        camdumpflag=Ql_atoi(cam_temp_dumpflg_buff);
        OUT_D1EBUG(textBuf,"\tcamdumpflag \t\t\t= %d\r\n",camdumpflag);
        Ql_FileClose(filehandle);
		filehandle = -1;
      }
	 else
	 {
    	 OUT_D1EBUG(textBuf,"Error in cam_wrt_cnt reading**1\r\n");
	 }
}
void fun_camdumpflag(void)
{
    s32 ret1;
    char *ptr;
    u32 writeedlen;
	u8 camdump_wcnt=0;        ///file memory location
    u32 filehandle;

	ret1 = Ql_FileOpenEx((u8*)cam1_mem_loc,QL_FS_CREATE);
	camdump_wcnt=80;
    if(ret1 >= QL_RET_OK)
    {
        filehandle = ret1;
        ptr=ix_Itoa(camdumpflag);
        ret1 = Ql_FileSeek(filehandle,camdump_wcnt,QL_FS_FILE_BEGIN);
        ret1 = Ql_FileWrite(filehandle, (u8*)ptr,1,&writeedlen);
       // OUT_D1EBUG(textBuf,"camdumpflag in camera file=%s\r\n",ptr);
        Ql_FileClose(filehandle);
	    filehandle = -1;
    }	
    else
    {
       OUT_D1EBUG(textBuf,"error camdumpflag write\r\n");
    }
}
void Gen_CW(void)
{
    char CWstring[90]; 
    Ql_memset((ascii *)CWstring,'\0',sizeof(CWstring));

	Ql_strcpy(CWstring,UID);
	Ql_strncat(CWstring,"_CW,",6);
	Ql_strcat(CWstring,GPRMC);
    ix_Itoa(mCurrSpeed);
	Ql_strcat(CWstring,(char *)ItoaStr);///mCurrSpeed speed
	Ql_strncat(CWstring,",",1);
	Ql_strcat(CWstring,(char *)(ix_Itoa(mTotDist)));
	Ql_strncat(CWstring,",",1);
	Ql_strncat(CWstring,"0.0",5);		//PDOP
    Ql_strncat(CWstring,",",1);
	Ql_strncat(CWstring,STAT,1);			//Status (A/V)
	Ql_strncat(CWstring,"\r\n",2);
    OUT_D1EBUG(textBuf,"CWstring=%s:\r\n",CWstring);
	tw_filewrite((char *)CWstring);	
}
void Gen_CR(void)
{
    char CRstring[90]; 
    Ql_memset((ascii *)CRstring,'\0',sizeof(CRstring));


	Ql_strcpy(CRstring,UID);
	Ql_strncat(CRstring,"_CR,",6);
	Ql_strcat(CRstring,GPRMC);
    ix_Itoa(mCurrSpeed);
	Ql_strcat(CRstring,(char *)ItoaStr);///mCurrSpeed speed
	Ql_strncat(CRstring,",",1);
	Ql_strcat(CRstring,(char *)(ix_Itoa(mTotDist)));
	Ql_strncat(CRstring,",",1);
	Ql_strncat(CRstring,"0.0",5);		//PDOP
    Ql_strncat(CRstring,",",1);
	Ql_strncat(CRstring,STAT,1);			//Status (A/V)
	Ql_strncat(CRstring,"\r\n",2);
    OUT_D1EBUG(textBuf,"CRstring=%s:\r\n",CRstring);
	tw_filewrite((char *)CRstring);	  
}

///////  Stamp for Camera message DUMP  //////
void Gen_CA(void)
{
    char CAstring[110]; 
    Ql_memset((ascii *)CAstring,'\0',sizeof(CAstring));

	Ql_strcpy(CAstring,UID);
	Ql_strncat(CAstring,"_CA,",6);
	Ql_strcat(CAstring,GPRMC);
	Ql_strcat(CAstring,(char *)(ix_Itoa(readcount)));			//cam 1 read count
	Ql_strncat(CAstring,",",1);
	Ql_strcat(CAstring,(char *)(ix_Itoa(dump_cam_wrt_cnt)));			//cam 1 dump count
	Ql_strncat(CAstring,",",1);
	Ql_strcat(CAstring,(char *)(ix_Itoa(readcount22)));			//cam 2 read count
	Ql_strncat(CAstring,",",1);
	Ql_strcat(CAstring,(char *)(ix_Itoa(dump_cam_wrt_cnt2)));			//cam 1 dump count
	Ql_strncat(CAstring,",",1);
	Ql_strncat(CAstring,STAT,1);			//Status (A/V)
	Ql_strncat(CAstring,"\r\n",2);
    OUT_D1EBUG(textBuf,"CAstring=%s:\r\n",CAstring);
	tw_filewrite((char *)CAstring);	  
}

void Gen_TC(void)
{
	char TCstring[110];
    Ql_memset((ascii *)TCstring,'\0',sizeof(TCstring));

	Ql_strcpy(TCstring,UID);
	Ql_strncat(TCstring,"_TC,",6);
	Ql_strcat(TCstring,GPRMC);
	Ql_strcat(TCstring,(char *)(ix_Itoa(readcount)));			//cam 1 read count
	Ql_strncat(TCstring,",",1);
	Ql_strcat(TCstring,(char *)(ix_Itoa(dump_cam_wrt_cnt)));			//cam 1 dump count
	Ql_strncat(TCstring,",",1);
	Ql_strcat(TCstring,(char *)(ix_Itoa(readcount22)));			//cam 2 read count
	Ql_strncat(TCstring,",",1);
	Ql_strcat(TCstring,(char *)(ix_Itoa(dump_cam_wrt_cnt2)));			//cam 1 dump count
	Ql_strncat(TCstring,",",1);
	Ql_strncat(TCstring,STAT,1);			//Status (A/V)
	Ql_strncat(TCstring,"\r\n",2);
    OUT_D1EBUG(textBuf,"TCstring=%s:\r\n",TCstring);
	tw_filewrite((char *)TCstring);	  
}


void Gen_TD1(void)
{
    char TD1string[110]; 
    Ql_memset((ascii *)TD1string,'\0',sizeof(TD1string));

	Ql_strcpy(TD1string,UID);
	Ql_strncat(TD1string,"_TD1,",6);
	Ql_strcat(TD1string,GPRMC);
	Ql_strcat(TD1string,(char *)(ix_Itoa(readcount)));			//cam 1 read count
	Ql_strncat(TD1string,",",1);
	Ql_strcat(TD1string,(char *)(ix_Itoa(dump_cam_wrt_cnt)));			//cam 1 dump count
	Ql_strncat(TD1string,",",1);
	Ql_strcat(TD1string,(char *)(ix_Itoa(readcount22)));			//cam 2 read count
	Ql_strncat(TD1string,",",1);
	Ql_strcat(TD1string,(char *)(ix_Itoa(dump_cam_wrt_cnt2)));			//cam 1 dump count
	Ql_strncat(TD1string,",",1);
	Ql_strncat(TD1string,STAT,1);			//Status (A/V)
	Ql_strncat(TD1string,"\r\n",2);
    OUT_D1EBUG(textBuf,"TD1string=%s:\r\n",TD1string);
	tw_filewrite((char *)TD1string);	  
}

void Gen_TD2(void)
{
	char TD2string[110];
    Ql_memset((ascii *)TD2string,'\0',sizeof(TD2string));

	Ql_strcpy(TD2string,UID);
	Ql_strncat(TD2string,"_TD2,",6);
	Ql_strcat(TD2string,GPRMC);
	Ql_strcat(TD2string,(char *)(ix_Itoa(readcount)));			//cam 1 read count
	Ql_strncat(TD2string,",",1);
	Ql_strcat(TD2string,(char *)(ix_Itoa(dump_cam_wrt_cnt)));			//cam 1 dump count
	Ql_strncat(TD2string,",",1);
	Ql_strcat(TD2string,(char *)(ix_Itoa(readcount22)));			//cam 2 read count
	Ql_strncat(TD2string,",",1);
	Ql_strcat(TD2string,(char *)(ix_Itoa(dump_cam_wrt_cnt2)));			//cam 1 dump count
	Ql_strncat(TD2string,",",1);
	Ql_strncat(TD2string,STAT,1);			//Status (A/V)
	Ql_strncat(TD2string,"\r\n",2);
    OUT_D1EBUG(textBuf,"TD1string=%s:\r\n",TD2string);
	tw_filewrite((char *)TD2string);	  
}



/*
void TEST_CAM_fun(void)
{
	u32 XT=50000;
	u32 XL=100000;

	OUT_D1EBUG(textBuf,"Write All Values.\r\n");
	readcount=readcount+XT;
	OUT_D1EBUG(textBuf,"readcount=%d\r\n",readcount);
	fun_cam_read_cnt();
	readcount22=readcount22+XT;
	OUT_D1EBUG(textBuf,"readcount22=%d\r\n",readcount22);
	fun_cam2_read_cnt();
	dump_cam_wrt_cnt=dump_cam_wrt_cnt+XL;
	OUT_D1EBUG(textBuf,"dump_cam_wrt_cnt=%d\r\n",dump_cam_wrt_cnt);
	fun_dump_cam_wrt_cnt();
	dump_cam_wrt_cnt2=dump_cam_wrt_cnt2+XL;
	OUT_D1EBUG(textBuf,"dump_cam_wrt_cnt2=%d\r\n",dump_cam_wrt_cnt2);
	fun_dump_cam2_wrt_cnt();
	OUT_D1EBUG(textBuf,"Read All Values.\r\n");

	cam_mem_loc_readextra();
	//cam_mem_loc_read();

}
*/
void fun_cam_read_cnt(void)
{
	s32 ret1;
	char *ptr;
	u32 writeedlen;
	//    u32 filehandle;
	u32 filehandle;

	ret1 = Ql_FileOpenEx((u8*)cam1_mem_loc,QL_FS_CREATE);
    //cam1memwcnt=1;
    if(ret1 >= QL_RET_OK)
    {
        filehandle = ret1;
        ptr=ix_Itoa(readcount);
        ret1 = Ql_FileSeek(filehandle,15,QL_FS_FILE_BEGIN);                 // readcount 15
        ret1 = Ql_FileWrite(filehandle, (u8*)ptr,14,&writeedlen);
        //OUT_D1EBUG(textBuf,"cam_wrt_cnt=%s\r\n",ptr);
        Ql_FileClose(filehandle);
	    filehandle = -1;
    }
    else
    {
       OUT_D1EBUG(textBuf,"error in cam_wrt_cnt write\r\n");
    }
}
void fun_cam2_read_cnt(void)
{
	s32 ret1;
	char *ptr;
	u32 writeedlen;
	//    u32 filehandle;
	u32 filehandle;

	ret1 = Ql_FileOpenEx((u8*)cam1_mem_loc,QL_FS_CREATE);
    //cam1memwcnt=1;
    if(ret1 >= QL_RET_OK)
    {
        filehandle = ret1;
        ptr=ix_Itoa(readcount22);
        ret1 = Ql_FileSeek(filehandle,30,QL_FS_FILE_BEGIN);                 // readcount22 30
        ret1 = Ql_FileWrite(filehandle, (u8*)ptr,14,&writeedlen);
        //OUT_D1EBUG(textBuf,"cam_wrt_cnt=%s\r\n",ptr);
        Ql_FileClose(filehandle);
	    filehandle = -1;
    }
    else
    {
       OUT_D1EBUG(textBuf,"error in cam_wrt_cnt write\r\n");
    }
}
void fun_dump_cam_wrt_cnt(void)
{
	s32 ret1;
	char *ptr;
	u32 writeedlen;
	//    u32 filehandle;
	u32 filehandle;

	ret1 = Ql_FileOpenEx((u8*)cam1_mem_loc,QL_FS_CREATE);
    //cam1memwcnt=1;
    if(ret1 >= QL_RET_OK)
    {
        filehandle = ret1;
        ptr=ix_Itoa(dump_cam_wrt_cnt);
        ret1 = Ql_FileSeek(filehandle,45,QL_FS_FILE_BEGIN);                 // dump_cam_wrt_cnt 45
        ret1 = Ql_FileWrite(filehandle, (u8*)ptr,14,&writeedlen);
        //OUT_D1EBUG(textBuf,"cam_wrt_cnt=%s\r\n",ptr);
        Ql_FileClose(filehandle);
	    filehandle = -1;
    }
    else
    {
       OUT_D1EBUG(textBuf,"error in cam_wrt_cnt write\r\n");
    }
}
void fun_dump_cam2_wrt_cnt(void)
{
	s32 ret1;
	char *ptr;
	u32 writeedlen;
	//    u32 filehandle;
	u32 filehandle;

	ret1 = Ql_FileOpenEx((u8*)cam1_mem_loc,QL_FS_CREATE);
    //cam1memwcnt=1;
    if(ret1 >= QL_RET_OK)
    {
        filehandle = ret1;
        ptr=ix_Itoa(dump_cam_wrt_cnt2);
        ret1 = Ql_FileSeek(filehandle,60,QL_FS_FILE_BEGIN);                 // dump_cam_wrt_cnt2 60
        ret1 = Ql_FileWrite(filehandle, (u8*)ptr,14,&writeedlen);
       //OUT_D1EBUG(textBuf,"cam_wrt_cnt=%s\r\n",ptr);
        Ql_FileClose(filehandle);
	    filehandle = -1;
    }
    else
    {
       OUT_D1EBUG(textBuf,"error in cam_wrt_cnt write\r\n");
    }
}

void fun_cam2_wrt_cnt(void)
{
    s32 ret1;
    char *ptr;
    u32 writeedlen;
    u32 filehandle;

	ret1 = Ql_FileOpenEx((u8*)cam1_mem_loc,QL_FS_CREATE);
    if(ret1 >= QL_RET_OK)
    {
        filehandle = ret1;
        ptr=ix_Itoa(cam_wrt_cnt2);
        ret1 = Ql_FileSeek(filehandle,85,QL_FS_FILE_BEGIN);
        ret1 = Ql_FileWrite(filehandle, (u8*)ptr,14,&writeedlen);
       //OUT_D1EBUG(textBuf,"cam_wrt_cnt2=%s\r\n",ptr);
        Ql_FileClose(filehandle);
	    filehandle = -1;
    }
    else
    {
       //_R//OUT_D1EBUG(textBuf,"error in cam_wrt_cnt write\r\n");
    }
}

void cam_mem_loc_readextra(void)
{
     s32 ret;
     u32 readedlen1;
     u32 filehandle;
     char cam_temp_read_buff[50];
     char cam_temp_dumpflg_buff[3];
     ret = Ql_FileOpenEx((u8*)cam1_mem_loc,QL_FS_CREATE);

     if(ret >= QL_RET_OK)
     {
		filehandle = ret;
		cam1memwcnt=15;
		///// Camera readcount count read
		Ql_memset((ascii *)cam_temp_read_buff,0,sizeof(cam_temp_read_buff));
		ret = Ql_FileSeek(filehandle,cam1memwcnt, QL_FS_FILE_BEGIN);
        ret = Ql_FileRead(filehandle, (u8 *)cam_temp_read_buff,14, &readedlen1);
        readcount=Ql_atoi(cam_temp_read_buff);
        OUT_D1EBUG(textBuf,"\tCAM readcount \t\t\t= %d\r\n",readcount);
		cam1memwcnt=30;
		///readcount22
		Ql_memset((ascii *)cam_temp_read_buff,0,sizeof(cam_temp_read_buff));
		ret = Ql_FileSeek(filehandle,cam1memwcnt, QL_FS_FILE_BEGIN);
        ret = Ql_FileRead(filehandle, (u8 *)cam_temp_read_buff,14, &readedlen1);
        readcount22=Ql_atoi(cam_temp_read_buff);
        OUT_D1EBUG(textBuf,"\tCAM readcount22 \t\t= %d\r\n",readcount22);
        cam1memwcnt=45;
        ///// Camera dump_cam_wrt_cnt count read
        Ql_memset((ascii *)cam_temp_read_buff,0,sizeof(cam_temp_read_buff));
        ret = Ql_FileSeek(filehandle,cam1memwcnt, QL_FS_FILE_BEGIN);
        ret = Ql_FileRead(filehandle, (u8 *)cam_temp_read_buff,14, &readedlen1);
        dump_cam_wrt_cnt=Ql_atoi(cam_temp_read_buff);
        OUT_D1EBUG(textBuf,"\tCAM dump_cam_wrt_cnt \t= %d\r\n",dump_cam_wrt_cnt);
        cam1memwcnt=60;
        ///// Camera dump_cam_wrt_cnt2 count read
        Ql_memset((ascii *)cam_temp_read_buff,0,sizeof(cam_temp_read_buff));
        ret = Ql_FileSeek(filehandle,cam1memwcnt, QL_FS_FILE_BEGIN);
        ret = Ql_FileRead(filehandle, (u8 *)cam_temp_read_buff,14, &readedlen1);
        dump_cam_wrt_cnt2=Ql_atoi(cam_temp_read_buff);
        OUT_D1EBUG(textBuf,"\tCAM dump_cam_wrt_cnt2 \t= %d\r\n",dump_cam_wrt_cnt2);
        ///// Camera cam_wrt_cnt2 count read
        cam1memwcnt=85;
        Ql_memset((ascii *)cam_temp_read_buff,0,sizeof(cam_temp_read_buff));
        ret = Ql_FileSeek(filehandle,cam1memwcnt, QL_FS_FILE_BEGIN);
        ret = Ql_FileRead(filehandle, (u8 *)cam_temp_read_buff,14, &readedlen1);
        cam_wrt_cnt2=Ql_atoi(cam_temp_read_buff);
        OUT_D1EBUG(textBuf,"\tCAM cam_wrt_cnt2 \t\t= %d\r\n",cam_wrt_cnt2);

        Ql_FileClose(filehandle);
		filehandle = -1;
      }
	 else
	 {
    	 OUT_D1EBUG(textBuf,"Error in cam_wrt_cnt reading**1\r\n");
	 }
}
