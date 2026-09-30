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

#define  PATH_EXCP_CAMERA_R ((u8 *)"SD:EXCP_camera_R.txt")

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

typedef enum tagATCmdType {
    AT_General,
    AT_QISTAT,
    AT_QFTPOPEN,
    AT_QFTPCFG,
    AT_QFTPPATH,
    AT_QFTPPUT,
    AT_QFTPCLOSE,
    AT_QIDEACT
}ATCmdType;

#define FTP_SVR_ADDR   "images.mobileeye.in"
#define FTP_SVR_PORT   "21"
//#define FTP_SVR_PATH   "/GOLD_C/"
#define FTP_SVR_PATH   "/FTPTESTFOLDER/"
//#define FTP_SVR_PATH   "/TestBulk/"
//#define FTP_SVR_PATH   "/ImageTrack/"
#define FTP_USER_NAME  "cameraimages"
#define FTP_PASSWORD   "Cimages@123"


char camEXCP_mem_loc[20] ="camEXCEp_mem.txt";
char ExR_swap_fl_[20]="ExR_swap_fl.txt";
extern char textBuf[1000];
extern char Expt_RARDbuffer[70000];
char ExR_cam_buffer[200];
extern u32 expt_cntr;
extern u32 RARDflg;
extern int error_read;    ///to generate error stamp
extern int fileincr;
extern char FTPFILENAME[20];
extern char CAMUID[6];
extern char pfile_FTPTXT[5];
extern char bulk_n[6];
extern bool cam_bDoNexAT;
char Ex_R_Stam_Buffer[200];

char ExR_cam_buffer[200];
u8 ExR_cam_cmd_type=0;
u32 Excp_cam_wrt_cntR=0;
u32 EXCP_R_limit=26214400;    ///Max limit of storage
u32 EX_R_CAM_bkp_flg=0;
u32 EXCP_R_readcount=0;
//u32 EX_R_EX_R_read_comp=0;
u32 in_EXCp_cam_rou=0;
char EXR_readbuffer_cam[35000];
char EXR_sendbuffer_cam[35000];
u32 EX_FTP_TX_Flag=0;
u32 EX_R_read_comp=0;
u32 ERx=0;
u32 ExR_readedlen=0;
u16 ExR_cam_cmd_idx=0;
u32 ExR_filecounter=0;
u32 ExR_fileposition=0;
int ExR_chkftpcloseflg=0;
extern char APN_NAME[];
extern bool cam_fl_up_flag;


// Exception image File write in SD card
void camR_Excp_filewrite(char *Excp_writebuffer)
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
	unsigned char readbuf[25000];


	writbuffer_len=Ql_strlen((char *)Excp_writebuffer);
	OUT_D1EBUG(textBuf,"file Excp_writebuffer length=%d\r\n",writbuffer_len);

 	if(Excp_cam_wrt_cntR <= EXCP_R_limit)
    {
	   OUT_D1EBUG(textBuf,"Excp_cam_wrt_cntR 1=%d\r\n",Excp_cam_wrt_cntR);
	    wrfl_ret = Ql_FileOpenEx(PATH_EXCP_CAMERA_R,QL_FS_CREATE);

	    if(wrfl_ret >= QL_RET_OK)
	    {
	    	filehandle_imgdata = wrfl_ret;
	    	ret_flsk = Ql_FileSeek(filehandle_imgdata,Excp_cam_wrt_cntR,QL_FS_FILE_BEGIN);
	    	retf = Ql_FileWrite(filehandle_imgdata,(u8*)Excp_writebuffer,Ql_strlen((char*)Excp_writebuffer),&writeedlen);
	    	Excp_cam_wrt_cntR=Excp_cam_wrt_cntR+writbuffer_len;
	    	fun_Excp_cam_wrt_cntR();   //Write in to memory
	    	Ql_FileClose(filehandle_imgdata);
	    	filehandle_imgdata = -1;
	    	OUT_D1EBUG(textBuf,"Exception Image Stored camera 1\r\n");
	    	Ql_memset((char *)Expt_RARDbuffer,0,sizeof(Expt_RARDbuffer));
   		}
		else
		{
			OUT_D1EBUG(textBuf,"ERROR in file writing\r\n");
			/*
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
			*/
		}
    }
	else
	{
		OUT_D1EBUG(textBuf,"Memory Full\r\n");
		EX_R_CAM_bkp_flg=1;
		/*
		dump_cam_wrt_cnt = cam_wrt_cnt;
		fun_dump_cam_wrt_cnt();
		cam_wrt_cnt=0;
		fun_cam_wrt_cnt();
		*/
	}
	return;
}


///Write pointers into memory
//A.Write pointer              mem loc 1

void fun_Excp_cam_wrt_cntR(void)
{
	s32 ret1;
	char *ptr;
	u32 writeedlen;
	//    u32 filehandle;
	u32 filehandle;

	ret1 = Ql_FileOpenEx((u8*)camEXCP_mem_loc,QL_FS_CREATE);

    if(ret1 >= QL_RET_OK)
    {
        filehandle = ret1;
        ptr=ix_Itoa(Excp_cam_wrt_cntR);
        ret1 = Ql_FileSeek(filehandle,1,QL_FS_FILE_BEGIN);
        ret1 = Ql_FileWrite(filehandle, (u8*)ptr,14,&writeedlen);       ///  mem loc 1 to 15
        OUT_D1EBUG(textBuf,"Excp_cam_wrt_cntR in memory=%s\r\n",ptr);
        Ql_FileClose(filehandle);
	    filehandle = -1;
    }
    else
    {
       OUT_D1EBUG(textBuf,"error in Excp_cam_wrt_cntR write\r\n");
    }
}


//B.read pointer              mem loc 15

void fun_Excp_cam_read_cntR(void)
{
	s32 ret1;
	char *ptr;
	u32 writeedlen;
	//    u32 filehandle;
	u32 filehandle;

	ret1 = Ql_FileOpenEx((u8*)camEXCP_mem_loc,QL_FS_CREATE);

    if(ret1 >= QL_RET_OK)
    {
        filehandle = ret1;
        ptr=ix_Itoa(EXCP_R_readcount);
        ret1 = Ql_FileSeek(filehandle,15,QL_FS_FILE_BEGIN);
        ret1 = Ql_FileWrite(filehandle, (u8*)ptr,14,&writeedlen);       ///  mem loc 1 to 15
        OUT_D1EBUG(textBuf,"Excp_cam_wrt_cntR in memory=%s\r\n",ptr);
        Ql_FileClose(filehandle);
	    filehandle = -1;
    }
    else
    {
       OUT_D1EBUG(textBuf,"error in Excp_cam_wrt_cntR write\r\n");
    }
}

//read Pointer values
//Road camera

void camR_EXCP_mem_loc_read(void)
{
     s32 ret;
     u32 readedlen1;
     u32 filehandle;
     char cam_temp_read_buff[50];
     char cam_temp_dumpflg_buff[3];
     ret = Ql_FileOpenEx((u8*)camEXCP_mem_loc,QL_FS_CREATE);

     if(ret >= QL_RET_OK)
     {
		filehandle = ret;
		///// Camera writr count read
		Ql_memset((ascii *)cam_temp_read_buff,0,sizeof(cam_temp_read_buff));
		ret = Ql_FileSeek(filehandle,1, QL_FS_FILE_BEGIN);
        ret = Ql_FileRead(filehandle, (u8 *)cam_temp_read_buff,14, &readedlen1);  //1 to 14
        Excp_cam_wrt_cntR=Ql_atoi(cam_temp_read_buff);
        OUT_D1EBUG(textBuf,"\tExcp_cam_wrt_cntR \t\t= %d\r\n",Excp_cam_wrt_cntR);

        Ql_memset((ascii *)Ex_R_Stam_Buffer,'\0',sizeof(Ex_R_Stam_Buffer));   //for stamp generation
        Ql_strcat((char *)Ex_R_Stam_Buffer,(char *)cam_temp_read_buff);
        Ql_strcat((char *)Ex_R_Stam_Buffer,",");
        ///// Camera readcount count read
        Ql_memset((ascii *)cam_temp_read_buff,0,sizeof(cam_temp_read_buff));
        ret = Ql_FileSeek(filehandle,15, QL_FS_FILE_BEGIN);
        ret = Ql_FileRead(filehandle, (u8 *)cam_temp_read_buff,14, &readedlen1);	//15 to 30
        EXCP_R_readcount=Ql_atoi(cam_temp_read_buff);
        Ql_strcat((char *)Ex_R_Stam_Buffer,(char *)cam_temp_read_buff);
        OUT_D1EBUG(textBuf,"\tEXCP_R_readcount \t\t= %d\r\n",EXCP_R_readcount);

        Ql_FileClose(filehandle);
		filehandle = -1;
      }
	 else
	 {
    	 OUT_D1EBUG(textBuf,"Error in Excp_cam_wrt_cntR reading**1\r\n");
	 }
}

void tw_EX_C_fileread(void)
{
	//int i=0,x=0,j,k,img;
    int i=0,j,k,img;
    s32 ret_r;
    s32 ret_refl,fileret;
    //char *ptr1;
	u32 filehandle_readfl;
    u32 size;
    u32 freespace;
 	u32 Diff_read=0;
    u32 writeedlen;
    u32 readedlen;

	in_EXCp_cam_rou=1;
	ERx=0;
	OUT_D1EBUG(textBuf,"in tw_EX_C_fileread\r\n");

	OUT_D1EBUG(textBuf,"FR EX_R_CAM_bkp_flg=%d\r\n",EX_R_CAM_bkp_flg);
	OUT_D1EBUG(textBuf,"FR EXCP_R_readcount=%d\r\n",EXCP_R_readcount);
	OUT_D1EBUG(textBuf,"FR Excp_cam_wrt_cntR=%d\r\n",Excp_cam_wrt_cntR);
	OUT_D1EBUG(textBuf,"FR EXCP_R_limit=%d\r\n",EXCP_R_limit);

	if(EX_R_CAM_bkp_flg ==1)
	{
		if(EXCP_R_readcount < EXCP_R_limit)
		{
			ret_refl = Ql_FileOpenEx(PATH_EXCP_CAMERA_R,QL_FS_CREATE);
			//ret_refl = Ql_FileOpenEx((u8*)pfile_imgdata,QL_FS_CREATE);
			OUT_D1EBUG(textBuf,"EX_R_CAM_bkp_flg =1 Readfile open ret=%d: \r\n",ret_refl);
			EX_R_read_comp=0;

			if(ret_refl >= QL_RET_OK)
			{
				filehandle_readfl = ret_refl;
				Ql_memset(EXR_readbuffer_cam,'\0',35000);
				Ql_memset(EXR_sendbuffer_cam,'\0',35000);
				ret_refl = Ql_FileSeek(filehandle_readfl, EXCP_R_readcount, QL_FS_FILE_BEGIN);
				OUT_D1EBUG(textBuf,"Readfile_seek_ret=%d: \r\n",ret_refl);
				ret_refl = Ql_FileRead(filehandle_readfl, (unsigned char *)EXR_readbuffer_cam,35000, &readedlen);
				OUT_D1EBUG(textBuf,"Ql_FileRead()=%d: readedlen=%d\r\n",ret_refl, readedlen);
				//	OUT_D1EBUG(textBuf,"EXR_readbuffer_cam= %s\r\n",EXR_readbuffer_cam);
				Ql_FileClose(filehandle_readfl);
				filehandle_readfl = -1;

				for(img=0;img<35000;img++)
				{
					if(EXR_readbuffer_cam[ERx] == 'R')
					{
						//  OUT_D1EBUG(textBuf,"%d\r\n",x);
						//  OUT_D1EBUG(textBuf,"*****%%%%I found\r\n");
						EXR_sendbuffer_cam[i]=EXR_readbuffer_cam[ERx];
						//	OUT_D1EBUG(textBuf,"%c",EXR_sendbuffer_cam[i]);
						i++;
						ERx++;

						if(EXR_readbuffer_cam[ERx] == 'B')
						{
							//  OUT_D1EBUG(textBuf,"%d\r\n",x);
							//  OUT_D1EBUG(textBuf,"*****%%%%I found\r\n");
							EXR_sendbuffer_cam[i]=EXR_readbuffer_cam[ERx];
							//	OUT_D1EBUG(textBuf,"%c",EXR_sendbuffer_cam[i]);
							i++;
							ERx++;

							if(EXR_readbuffer_cam[ERx] == 'A')
							{
								ERx++;
								//OUT_D1EBUG(textBuf,"%d\r\n",x);
								--i;
								EXR_sendbuffer_cam[i]='\0';
								--i;
								EXR_sendbuffer_cam[i]='\0';
								--i;
								EXR_sendbuffer_cam[i]='\0';
								//readcount++;
								//OUT_D1EBUG(textBuf,"%c",EXR_sendbuffer_cam[i]);
								////Send image
								//break;
								OUT_D1EBUG(textBuf,"RF EXR_sendbuffer_cam cam_length=%d\r\n",Ql_strlen((char *)EXR_sendbuffer_cam));
								//OUT_D1EBUG(textBuf,"%d\r\n",x);
								/*	for(j=0;j<i;++j)
	      		     	 	 	 {
	  	  	          	  	  	  	  for(k=0;k<100;++k);
		   	          	  	  	  	  OUT_D1EBUG(textBuf,"%c",EXR_sendbuffer_cam[j]);
 		    	     	 	 	 }
								 */
								EXCP_R_readcount=EXCP_R_readcount+ERx;
								//fun_Excp_cam_read_cntR();							//Write readcount value in memory

								Diff_read=EXCP_R_limit-EXCP_R_readcount;
								OUT_D1EBUG(textBuf,"EX_R Diff_read=%d",Diff_read);

								//if(Diff_read<=30000)
								if(EXCP_R_readcount >= EXCP_R_limit)
								{
									EX_R_read_comp=1;
								}
								OUT_D1EBUG(textBuf,"calling NF.\r\n");
								//Ql_Sleep(1000);
								EXR_new_file((char *)EXR_sendbuffer_cam);

								if(EXCP_R_readcount >= EXCP_R_limit)   // check the flg for data send thtough FTP
								{
									return;
								}
								break;
							}
						}
						else
						{
							//OUT_D1EBUG(textBuf,"%d\r\n",x);
							EXR_sendbuffer_cam[i]=EXR_readbuffer_cam[ERx];
							//OUT_D1EBUG(textBuf,"%c",EXR_sendbuffer_cam[i]);
							i++;
							ERx++;
							//OUT_D1EBUG(textBuf,"%c",readcount);
						}
					}
					else
					{
						//OUT_D1EBUG(textBuf,"%d\r\n",x);
						EXR_sendbuffer_cam[i]=EXR_readbuffer_cam[ERx];
						//OUT_D1EBUG(textBuf,"%c",EXR_sendbuffer_cam[i]);
						i++;
						ERx++;
						//OUT_D1EBUG(textBuf,"%c",readcount);
					}
					//  break;
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
			OUT_D1EBUG(textBuf,"RF EXCP_R_readcount=%d\r\n",EXCP_R_readcount);
			OUT_D1EBUG(textBuf,"RF EXCP_R_limit=%d\r\n",EXCP_R_limit);
			OUT_D1EBUG(textBuf,"RF complited With backup flg as 1 \r\n");
			EX_R_CAM_bkp_flg=0;
			in_EXCp_cam_rou=0;
		//	fun_cam_Backup_flg();				//Write backup flag
			EXCP_R_readcount=0;
			fun_Excp_cam_read_cntR();					///Write read count
			//Ql_StopTimer(&cam_tx);
			//cam_tx.timerId =0;
			EX_FTP_TX_Flag=0;
			//Ql_StartTimer(&cam_tx);

		}
	}
	else if(EX_R_CAM_bkp_flg ==0)
	{
		if(EXCP_R_readcount < Excp_cam_wrt_cntR)
		{
			ret_refl = Ql_FileOpenEx(PATH_EXCP_CAMERA_R,QL_FS_CREATE);
			//ret_refl = Ql_FileOpenEx((u8*)pfile_imgdata,QL_FS_CREATE);
			//OUT_D1EBUG(textBuf,"Readfile open ret=%d: \r\n",ret_refl);
			EX_R_read_comp=0;

			if(ret_refl >= QL_RET_OK)
			{
				filehandle_readfl = ret_refl;
	       		Ql_memset(EXR_readbuffer_cam,'\0',35000);
	       		Ql_memset(EXR_sendbuffer_cam,'\0',35000);
	       		ret_refl = Ql_FileSeek(filehandle_readfl, EXCP_R_readcount, QL_FS_FILE_BEGIN);
				OUT_D1EBUG(textBuf,"CAM_backup_flg =0 Readfile_seek_ret=%d: \r\n",ret_refl);
		        ret_refl = Ql_FileRead(filehandle_readfl, (unsigned char *)EXR_readbuffer_cam,35000, &readedlen);
				OUT_D1EBUG(textBuf,"Ql_FileRead()=%d: readedlen=%d\r\n",ret_refl, readedlen);
				//Ql_strlen(AccBuffer);
				OUT_D1EBUG(textBuf,"EXR_readbuffer_cam length=%d\r\n",Ql_strlen(EXR_readbuffer_cam));
				//	OUT_D1EBUG(textBuf,"EXR_readbuffer_cam= %s\r\n",EXR_readbuffer_cam);
				Ql_FileClose(filehandle_readfl);
				filehandle_readfl = -1;

				for(img=0;img<35000;img++)
				{
					if(EXR_readbuffer_cam[ERx] == 'R')
					{
						//  OUT_D1EBUG(textBuf,"%d\r\n",x);
						//  OUT_D1EBUG(textBuf,"*****%%%%I found\r\n");
						EXR_sendbuffer_cam[i]=EXR_readbuffer_cam[ERx];
						//	OUT_D1EBUG(textBuf,"%c",EXR_sendbuffer_cam[i]);
						i++;
						ERx++;

						if(EXR_readbuffer_cam[ERx] == 'B')
						{
							//  OUT_D1EBUG(textBuf,"%d\r\n",x);
							//  OUT_D1EBUG(textBuf,"*****%%%%I found\r\n");
							EXR_sendbuffer_cam[i]=EXR_readbuffer_cam[ERx];
							//	OUT_D1EBUG(textBuf,"%c",EXR_sendbuffer_cam[i]);
							i++;
							ERx++;

							if(EXR_readbuffer_cam[ERx] == 'A')
							{
								ERx++;
								//OUT_D1EBUG(textBuf,"%d\r\n",x);
								--i;
								EXR_sendbuffer_cam[i]='\0';
								--i;
								EXR_sendbuffer_cam[i]='\0';
								--i;
								EXR_sendbuffer_cam[i]='\0';
								//readcount++;
								//OUT_D1EBUG(textBuf,"%c",EXR_sendbuffer_cam[i]);
								////Send image
								//break;
								OUT_D1EBUG(textBuf,"RF2 EXR_sendbuffer_cam cam_length=%d\r\n",Ql_strlen((char *)EXR_sendbuffer_cam));
								//OUT_D1EBUG(textBuf,"%d\r\n",x);
								/*	for(j=0;j<i;++j)
		      		     	 	 {
		  	  	          	  	  	  for(k=0;k<100;++k);
			   	          	  	  	  OUT_D1EBUG(textBuf,"%c",EXR_sendbuffer_cam[j]);
	 		    	     	 	 }
								 */
								EXCP_R_readcount=EXCP_R_readcount+ERx;
								//fun_Excp_cam_read_cntR();								//Write read count

								Diff_read=Excp_cam_wrt_cntR-EXCP_R_readcount;
								OUT_D1EBUG(textBuf,"RF2 Diff_read=%d\r\n",Diff_read);

								//if(Diff_read<=30000)
								if(EXCP_R_readcount >= Excp_cam_wrt_cntR)
								{
									EX_R_read_comp=1;
								}
								OUT_D1EBUG(textBuf,"RF2 calling new_file.\r\n");
								//Ql_Sleep(1000);
								EXR_new_file((char *)EXR_sendbuffer_cam);
								//	OUT_D1EBUG(textBuf,"again in twfileread function.\r\n");
								//	Ql_memset(EXR_readbuffer_cam,'\0',35000);
								//Ql_memset(EXR_sendbuffer_cam,'\0',35000);
								//  readcount=readcount+x;
								// fun_cam_read_cnt();
								/*   if(ftpcamsendflag==1)
								{
									ftpcamsendflag=0;
									tw_fileread();
								}
								*/
								//////////read count save
								//OUT_D1EBUG(textBuf,"readcount====%lu",x);
								if(EXCP_R_readcount >= Excp_cam_wrt_cntR)   // check the flg for data send thtough FTP
								{
									return;
								}
								break;
							}
						}
						else
						{
							//OUT_D1EBUG(textBuf,"%d\r\n",x);
							EXR_sendbuffer_cam[i]=EXR_readbuffer_cam[ERx];
							//OUT_D1EBUG(textBuf,"%c",EXR_sendbuffer_cam[i]);
							i++;
							ERx++;
							//OUT_D1EBUG(textBuf,"%c",readcount);
						}
					}
					else
					{
						//OUT_D1EBUG(textBuf,"%d\r\n",x);
						EXR_sendbuffer_cam[i]=EXR_readbuffer_cam[ERx];
						//OUT_D1EBUG(textBuf,"%c",EXR_sendbuffer_cam[i]);
						i++;
						ERx++;
						//OUT_D1EBUG(textBuf,"%c",readcount);
					}
					//  break;
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
			OUT_D1EBUG(textBuf,"file read EXCP_R_readcount=%d\r\n",EXCP_R_readcount);
			OUT_D1EBUG(textBuf,"file read Excp_cam_wrt_cntR=%d\r\n",Excp_cam_wrt_cntR);
			OUT_D1EBUG(textBuf,"RF2 File read done, with back up flag as 0 \n");
			EX_R_CAM_bkp_flg=0;
			in_EXCp_cam_rou=0;
			tw_EX_CC_fileread();							////Read Cabin File
		//	fun_cam_Backup_flg();	//Write backup flag
	//r		EX_FTP_TX_Flag=0;
			//Ql_StopTimer(&cam_tx);
			//cam_tx.timerId =0;
			//Ql_StartTimer(&cam_tx);
			return;
		}
	}
}


void EXR_new_file(char *sendbuff)
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

 	OUT_D1EBUG(textBuf,"In EXR_new_file \r\n");

 	if(ExR_filecounter==0)
 	{
 		ExR_fileposition=0;

 	}

 	Ql_strcat((char *)sendbuff,"RBA");
 	sendbuff_len=Ql_strlen((char *)sendbuff);
 	//OUT_D1EBUG(textBuf,"NF sendbuff_len=%d\r\n",sendbuff_len);

	//OUT_D1EBUG(textBuf,"NF filecounter=%d\r\n",filecounter);
	//OUT_D1EBUG(textBuf,"NF fileposition=%d\r\n",fileposition);
    retf = Ql_FileOpenEx((u8*)ExR_swap_fl_,QL_FS_CREATE);
	//OUT_D1EBUG(textBuf,"file open retf=%d\r\n",retf);
	//OUT_D1EBUG(textBuf,"file QL_RET_OK=%d\r\n",QL_RET_OK);

	if(retf >= QL_RET_OK)
    {
   	    filehandle_FTP = retf;
   	    //ret = Ql_FileSeek(filehandle_FTP,ACCcam_wrt_cnt,QL_FS_FILE_BEGIN);
		ret2 = Ql_FileSeek(filehandle_FTP,ExR_fileposition,QL_FS_FILE_BEGIN);
		//OUT_D1EBUG(textBuf,"twBuffer=%s\r\n",tw);
	    retf = Ql_FileWrite(filehandle_FTP, (u8*)sendbuff,sendbuff_len,&writeedlen);
	    //OUT_D1EBUG(textBuf,"file Write retf=%d\r\n",retf);
	    //OUT_D1EBUG(textBuf,"file Write writeedlen***=%d\r\n",writeedlen);
		//writeedlen +=1;
	    ExR_fileposition=ExR_fileposition+writeedlen;
		//OUT_D1EBUG(textBuf,"file Write fileposition after+1 ***=%d\r\n",fileposition);
	    Ql_FileClose(filehandle_FTP);
	    filehandle_FTP = -1;
	    Ql_memset((char *)sendbuff,'\0',sizeof(sendbuff));

	    if((ExR_filecounter>=13) || (EX_R_read_comp==1))
	    {
	    	++fileincr;
	    	fun_img_cnt();
	    	//write image number
	    	ERx=ExR_fileposition;
	    	ExR_filecounter=0;
	    	EXCP_R_readcount=EXCP_R_readcount-ERx;
	    	fun_Excp_cam_read_cntR();
	    	OUT_D1EBUG(textBuf,"file read EXCP_R_readcount=%d\r\n",EXCP_R_readcount);
	    	OUT_D1EBUG(textBuf,"file read xxxxxxxxxx=%d\r\n",ERx);
	   	    Ql_memset((char *)FTPFILENAME,'\0',sizeof(FTPFILENAME));
	   	    //Ql_strcat((char *)FTPFILENAME,(char *)pfile_FTPUID);
	   	    Ql_strcat((char *)FTPFILENAME,(char *)CAMUID);
	   	    Ql_strcat((char *)FTPFILENAME,(char *)bulk_n);
	   	    Ql_strcat((char *)FTPFILENAME,(char *)(ix_Itoa(fileincr)));
	   	    Ql_strcat((char *)FTPFILENAME,"R_E");
	   	    Ql_strcat((char *)FTPFILENAME,(char *)pfile_FTPTXT);
	   	    OUT_D1EBUG(textBuf,"file Excp FTPFILENAME=%s\r\n",FTPFILENAME);
	   	    //Ql_StartTimer(&timer_ftp);
	   	   // OUT_D1EBUG(textBuf,"file write successful\r\n");
	   		cam_fl_up_flag=1;
	   		EX_FTP_TX_Flag=1;
	        ExR_cam_cmd_idx = 1;
            ExR_SendAtCmd();
            //OUT_D1EBUG(textBuf,"file sending successful\r\n");
	    }
	    else
	    {
	    	ExR_filecounter++;
	    	Ql_memset(EXR_readbuffer_cam,'\0',35000);
	    	Ql_memset(EXR_sendbuffer_cam,'\0',35000);
	    	tw_EX_C_fileread();
	    }
    }

	Ql_memset((char *)sendbuff,'\0',sizeof(sendbuff));
	sendbuff_len=0;
	//OUT_D1EBUG(textBuf,"new file function ends.\r\n");
	return;
}

void ExR_SendAtCmd(void)
{
	int fil_eret;
	int i,j;
	u32 ExR_Cam_diffcnt=0;
	s32 fileret,timeret;
    bool exec = TRUE;

    switch (ExR_cam_cmd_idx)
    {
    	//OUT_D1EBUG(textBuf,"In FTP switch\r\n");
		OUT_D1EBUG(textBuf,"\r\n IN switch (cam_cmd_idx)..cam1.\r\n");
    case 1:// Echo mode off
    	Ql_sprintf((char *)ExR_cam_buffer, "ATE0\n");
    	ExR_cam_cmd_type = AT_General;
    	//Ql_StopTimer(&GC_ftp_stuck);
    	//timeret=Ql_StartTimer(&GC_ftp_stuck);    ///timer for ftp stuck
    	//FTP_STK_cntr++;
    	//OUT_D1EBUG(textBuf,"\r\nStarting FTP 1 stuck Timer...=%d\r\n",timeret);
    	EX_FTP_TX_Flag=1;
        OUT_D1EBUG(textBuf,"Echo mode off ATE0 case 1=%s\r\n",ExR_cam_buffer);
    	//RR//OUT_D1EBUG(textBuf,(char *)cam_buffer);
    	Ql_SendToModem(ql_md_port1, (u8*)ExR_cam_buffer, Ql_strlen(ExR_cam_buffer));
    	//check_AT_res();
        break;

    case 2:// Select a foreground context
        Ql_sprintf((char *)ExR_cam_buffer, "AT+QIFGCNT=0\n");
        ExR_cam_cmd_type = AT_General;
        EX_FTP_TX_Flag=1;
        OUT_D1EBUG(textBuf,"Select a foreground context AT+QIFGCNT=1 case 2\r\n");
        //RR//OUT_D1EBUG(textBuf,(char *)cam_buffer);
        Ql_SendToModem(ql_md_port1, (u8*)ExR_cam_buffer, Ql_strlen(ExR_cam_buffer));
        //check_AT_res();
        break;

    case 3:// Select a bearer (0=CSD, 1=GPRS), if 'GPRS', set APN
        Ql_sprintf((char *)ExR_cam_buffer, "AT+QICSGP=1,\"%s\"\n",APN_NAME);
        ExR_cam_cmd_type = AT_General;
        EX_FTP_TX_Flag=1;
        OUT_D1EBUG(textBuf,"Select a GPRS AT+QICSGP=1 case 3\n");
        //RR//OUT_D1EBUG(textBuf,(char *)cam_buffer);
        Ql_SendToModem(ql_md_port1, (u8*)ExR_cam_buffer, Ql_strlen(ExR_cam_buffer));
        //check_AT_res();
        break;

    case 4:// Set user name
        Ql_sprintf((char *)ExR_cam_buffer, "AT+QFTPUSER=\"%s\"\n", FTP_USER_NAME);
        ExR_cam_cmd_type = AT_General;
        EX_FTP_TX_Flag=1;
        OUT_D1EBUG(textBuf,"Set user name AT+QFTPUSER case 4\r\n");
        //RR//OUT_D1EBUG(textBuf,(char *)cam_buffer);
        Ql_SendToModem(ql_md_port1, (u8*)ExR_cam_buffer, Ql_strlen(ExR_cam_buffer));
        //check_AT_res();
        break;

    case 5:// Set password
        Ql_sprintf((char *)ExR_cam_buffer, "AT+QFTPPASS=\"%s\"\n", FTP_PASSWORD);
        ExR_cam_cmd_type = AT_General;
        EX_FTP_TX_Flag=1;
        OUT_D1EBUG(textBuf,"Set password AT+QFTPPASS=1 case 5\r\n");
        //RR//OUT_D1EBUG(textBuf,(char *)cam_buffer);
        Ql_SendToModem(ql_md_port1, (u8*)ExR_cam_buffer, Ql_strlen(ExR_cam_buffer));
        //check_AT_res();
        break;

    case 6:// Open FTP
        Ql_sprintf((char *)ExR_cam_buffer, "AT+QFTPOPEN=\"%s\",\"%s\"\n", FTP_SVR_ADDR, FTP_SVR_PORT);
        ExR_cam_cmd_type = AT_QFTPOPEN;
        EX_FTP_TX_Flag=1;
        OUT_D1EBUG(textBuf,"Open FTP AT+QFTPOPEN=1 case 6\r\n");
        //check_AT_res();
        cam_bDoNexAT = FALSE;
        //RR//OUT_D1EBUG(textBuf,(char *)cam_buffer);
        Ql_SendToModem(ql_md_port1, (u8*)ExR_cam_buffer, Ql_strlen(ExR_cam_buffer));
        break;

    case 7:// Set local path
        Ql_sprintf((char *)ExR_cam_buffer, "AT+QFTPCFG=4,\"/UFS/ExR_swap_fl.txt\"\n");
        ExR_cam_cmd_type = AT_QFTPCFG;
        OUT_D1EBUG(textBuf,"Set local path AT+QFTPCFG=4 case 7\r\n");
        //check_AT_res();
        cam_bDoNexAT = FALSE;
        EX_FTP_TX_Flag=1;
        //RR//OUT_D1EBUG(textBuf,(char *)cam_buffer);
        Ql_SendToModem(ql_md_port1, (u8*)ExR_cam_buffer, Ql_strlen(ExR_cam_buffer));
        break;

    case 8:// Set server path
        Ql_sprintf((char *)ExR_cam_buffer, "AT+QFTPPATH=\"%s\"\n", FTP_SVR_PATH);
        ExR_cam_cmd_type = AT_QFTPPATH;
        OUT_D1EBUG(textBuf,"Set server path AT+QFTPPATH=1 case 8\r\n");
        //check_AT_res();
        EX_FTP_TX_Flag=1;
        cam_bDoNexAT = FALSE;
        //RR//OUT_D1EBUG(textBuf,(char *)cam_buffer);
        Ql_SendToModem(ql_md_port1, (u8*)ExR_cam_buffer, Ql_strlen(ExR_cam_buffer));
        break;

     case 9:// Start to upload file
    	//++img_taken;
        Ql_sprintf((char *)ExR_cam_buffer, "AT+QFTPPUT=\"%s\",%d,500\n",FTPFILENAME,ExR_readedlen);
        ExR_cam_cmd_type = AT_QFTPPUT;
       // OUT_D1EBUG(textBuf,"Total images sent=%d\r\n",img_taken);
        OUT_D1EBUG(textBuf,"Start to upload file AT+QFTPPUT=1 case 9\r\n");
        cam_bDoNexAT = FALSE;
        EX_FTP_TX_Flag=1;
        //RR//OUT_D1EBUG(textBuf,(char *)cam_buffer);
        Ql_SendToModem(ql_md_port1, (u8*)ExR_cam_buffer, Ql_strlen(ExR_cam_buffer));
        break;
    case 10:// Close FTP connection
        Ql_sprintf((char *)ExR_cam_buffer, "AT+QFTPCLOSE\n");
        ExR_cam_cmd_type = AT_QFTPCLOSE;
        EX_FTP_TX_Flag=1;
        OUT_D1EBUG(textBuf,"Close FTP AT+QFTPCLOSE case 11\r\n");
        //check_AT_res();
        cam_bDoNexAT = FALSE;
        ExR_chkftpcloseflg=0;
        //RR//OUT_D1EBUG(textBuf,(char *)cam_buffer);
		Ql_SendToModem(ql_md_port1, (u8*)ExR_cam_buffer, Ql_strlen(ExR_cam_buffer));
		//Ql_StopTimer(&timer_ftp);
        break;

    case 11:// Deactivate GPRS PDP context
        Ql_sprintf((char *)ExR_cam_buffer, "AT+QIDEACT\n");
        ExR_cam_cmd_type = AT_General;
        EX_FTP_TX_Flag=1;
        OUT_D1EBUG(textBuf,"Deactivate GPRS AT+QIDEACT case 12\r\n");
        //check_AT_res();
    	fileret=Ql_FileDelete((u8*)"ExR_swap_fl.txt");
 		OUT_D1EBUG(textBuf,"file delete fileret=%d\r\n",fileret);
 		//ftp_data=0;
 		//cam_fl_up_flag=0;
 		//RR//OUT_D1EBUG(textBuf,(char *)cam_buffer);
 		Ql_SendToModem(ql_md_port1, (u8*)ExR_cam_buffer, Ql_strlen(ExR_cam_buffer));
        break;

    default:
    	//OUT_D1EBUG(textBuf,"at commands finished.\r\n");
    	EX_FTP_TX_Flag=1;
    	ExR_cam_cmd_type = AT_General;
        exec = FALSE;
        cam_bDoNexAT = TRUE;
        //GC_ftp_stuck.timeoutPeriod = Ql_SecondToTicks(560);
        //Ql_StopTimer(&GC_ftp_stuck);
		//GC_ftp_stuck.timerId =0;
		//FTP_STK_cntr=0;
		//OUT_D1EBUG(textBuf,"at commands finished default_1\r\n\n");
        //sen_fl=1;

        if(ExR_chkftpcloseflg==0)
        {
        	EXCP_R_readcount=EXCP_R_readcount+ERx;
        	fun_Excp_cam_read_cntR();
        }
        // ftpcamsendflag=1;
		// OUT_D1EBUG(textBuf,"at commands finished default_2\r\n\n");
        ExR_readedlen=0;
		//ftp_data=0;
		OUT_D1EBUG(textBuf,"at commands finished default\r\n");
		in_EXCp_cam_rou=0;
		EX_FTP_TX_Flag=0;
		if(EX_R_CAM_bkp_flg==0)
		{
			ExR_Cam_diffcnt=Excp_cam_wrt_cntR-EXCP_R_readcount;
			OUT_D1EBUG(textBuf,"Difference between Read and write count=%d\r\n",ExR_Cam_diffcnt);
		}

		if((ExR_Cam_diffcnt >= 70000) || (EX_R_CAM_bkp_flg==1))
		{
			EX_FTP_TX_Flag=1;
			tw_EX_C_fileread();
		}
		else
		{
			EX_FTP_TX_Flag=1;
			tw_EX_CC_fileread();
		}
		break;
    }
    return;
}


void ExR_cam_modemdata(char *cam_modem_readbuffer)
{
	OUT_D1EBUG(textBuf,"IN CMD and  EX_FTP_TX_Flag=%d \r\n",EX_FTP_TX_Flag);
	if(EX_FTP_TX_Flag==1)
	{
		//stop_img_capt=1;
		//OUT_D1EBUG(textBuf,"IN cam_modemdata ftp_data==1 \r\n");
		if (Ql_strstr((char *)cam_modem_readbuffer, "Call Ready") != NULL)
		{
			//cm_timer.timeoutPeriod = Ql_SecondToTicks(2);
			OUT_D1EBUG(textBuf,"IN Callready  and  EX_FTP_TX_Flag=%d \r\n",EX_FTP_TX_Flag);
		}
		else if ((ExR_cam_cmd_type == AT_QFTPOPEN  && Ql_strstr((char*)cam_modem_readbuffer,"+QFTPOPEN:0") != NULL)
				|| (ExR_cam_cmd_type == AT_QFTPCFG  && Ql_strstr((char*)cam_modem_readbuffer, "+QFTPCFG:0")   != NULL)
				|| (ExR_cam_cmd_type == AT_QFTPPATH && Ql_strstr((char*)cam_modem_readbuffer, "+QFTPPATH:0") != NULL)
				|| (ExR_cam_cmd_type == AT_QFTPPUT  && Ql_strstr((char*)cam_modem_readbuffer, "+QFTPPUT:")   != NULL)
				|| (ExR_cam_cmd_type == AT_QFTPCLOSE && Ql_strstr((char*)cam_modem_readbuffer, "+QFTPCLOSE:0") != NULL)
		)
		{
			OUT_D1EBUG(textBuf,"IN CMD_1\r\n");
			ExR_cam_cmd_idx++;
			cam_bDoNexAT = TRUE;
			ExR_SendAtCmd();
		}
		else if ((  Ql_strstr((char*)cam_modem_readbuffer, "\r\nOK") != NULL
				|| Ql_strstr((char*)cam_modem_readbuffer, "OK\r\n") != NULL
				|| Ql_strstr((char*)cam_modem_readbuffer, "ERROR") != NULL)
				&& cam_bDoNexAT != FALSE)
		{

			OUT_D1EBUG(textBuf,"IN CMD_2 \r\n");
			ExR_cam_cmd_idx++;
			ExR_SendAtCmd();
		}
	}
	return;
 }
