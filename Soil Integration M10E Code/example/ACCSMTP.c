
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
#include "ql_fota.h"
#include "ftp.h"
#include "camera.h"
#include "camera2.h"
#include "JRM.h"
#include "ACCSMTP.h"



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

extern char CAMUID[10];
#define  PATH_ACC ((u8 *)"ACC_data.txt")
/*
char SMTP_SVR_ADDR[50]="103.8.126.138\0";
char SMTP_USER_NAME[50]="dragonfly\0";
char SMTP_PASSWORD[50]="dragonfly\0";
//#define SMTP_USER		"r_avhad@transworld-compressor.com"
char SMTP_ADDR[50]="dragonfly\0";
//#define SMTP_DST		"r_avhad@transworld-compressor.com"
char SMTP_DST[50]="avlincident@twphd.in\0";
char SMTP_SUB[50]="8021\0";
 */

char SMTP_SVR_ADDR[50]="a.mobileeye.in\0";
char SMTP_USER_NAME[50]="\0";
char SMTP_PASSWORD[50]="transworld\0";
//#define SMTP_USER		"r_avhad@transworld-compressor.com"
char SMTP_ADDR[50]="\0";
//#define SMTP_DST		"r_avhad@transworld-compressor.com"
char SMTP_DST[50]="avlincident@twphd.in\0";
char SMTP_SUB[50]="8021\0";

extern char textBuf[1000];
extern ascii uart_buffer[250];

char smtp_buffer[100];
int smtp_cmd_type=0;
bool smtp_data=0;
bool stop_acc_capt = 0;
bool smtp_bDoNexAT=TRUE;
char str13[50];
extern unsigned char UID[];
//char acc_readbuffer[20000];
char acc_readbuffer[35000];
//char acc_sendbuffer[20000];
char acc_sendbuffer[35000];
extern char check_response[50];
extern char GPRMC[];
//extern char UID[];
extern char STAT[];
extern double mCurrSpeed,mTotDist;
extern unsigned char ItoaStr[];
//u32 accd_temp_cnt = 0;  //RAHMAN_EX
s32 acc_rd_cnt = 0,accd_temp_cnt = 0;

bool ACC_Flag=0;
bool Flag_ACC_SMTP=0;

int ACCwrt_cnt=0;
int ACCwrt_cntLastLoc=994600;
extern u32 dump_accd_wrtn_cnt,dump_read_cnt;
char Accwrtcnt_readbuffer[50];
u8 accwcnt=0;
u32 accfilehandle=0;
u16  smtp_cmd_idx=0;
int in_accsmtp_rou = 0;
//extern QlTimer /* timer_sendmail,timer_sendmail_1*/;
extern char checkdata[]; //RAHMAN
u32 writecnt1 = 0;
u32 readedlen1 = 0,Len_read =0,writeedlen1=0,writeedlen=0,  acc_rd_cnt_send = 0;
extern bool send_accd_whole_data;

///////////NEW////////
u32 serial_number = 1;
char ACCD_FILE_NAME[20];
u32 No_of_read = 1,accd_dif_read = 0,accd_data_file_size = 0;
u8 accd_read_comp_flag = 0;
//u32 IXY=0;       //for test
extern char APN_NAME[];
extern QlTimer GC_ftp_stuck;
extern u32 FTP_Stuck_cntr;

void tw_writeAccData(char *AccBuffer)
{
	s32 ret;
	//s32 ret1;
	u32 writeedlen;
	s32 accfilehandle_ACC;
	//////test
	/*IXY++;
    if(IXY >= 3)
    {
    	IXY=0;
    	TEST_CAM_fun();
    }
	 */
	if(ACCwrt_cnt > ACCwrt_cntLastLoc)  //for 4 hrs  RAHMAN
	{
		ACCwrt_cnt=0;
	}

	Ql_strncat((char *)AccBuffer,(char *)"\r\n",2);
	ret = Ql_FileOpenEx(PATH_ACC,QL_FS_CREATE);

	if(ret >= QL_RET_OK)
	{
		accfilehandle_ACC = ret;
		ret = Ql_FileSeek(accfilehandle_ACC,ACCwrt_cnt,QL_FS_FILE_BEGIN);	    
		ret = Ql_FileWrite(accfilehandle_ACC, (u8*)AccBuffer,Ql_strlen((char*)AccBuffer),&writeedlen);
		ACCwrt_cnt+=Ql_strlen(AccBuffer);
		Accdata_wrtcnt();
		Ql_FileClose(accfilehandle_ACC);
		accfilehandle_ACC = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"Error in Acc file writing\r\n");
	}	
}

/////////with attachment
void SMTP_connect(void)
{	
	in_accsmtp_rou=1;
	smtp_data=1;
	s32 fileret,timeret;

	OUT_D1EBUG(textBuf,"\r\nInside ATCommand,,,,,smtp_cmd_idx=%d\r\n",smtp_cmd_idx);
	switch(smtp_cmd_idx)
	{
	case 1:
		Ql_StopTimer(&GC_ftp_stuck);
		GC_ftp_stuck.timeoutPeriod = Ql_SecondToTicks(240);
		timeret=Ql_StartTimer(&GC_ftp_stuck);    ///timer for ftp stuck
		Ql_memset(smtp_buffer,'\0',sizeof(smtp_buffer));
		Ql_sprintf((char *)smtp_buffer,"AT+QSMTPSRV=\"%s\",25\n", SMTP_SVR_ADDR);  //RAHMAN
		OUT_D1EBUG(textBuf,(char *)smtp_buffer);
		Ql_SendToModem(ql_md_port1, (u8*)smtp_buffer, Ql_strlen((char*)smtp_buffer));
		break;

	case 2:
		Ql_memset(smtp_buffer,'\0',sizeof(smtp_buffer));
		Ql_sprintf((char *)smtp_buffer,"AT+QSMTPUSER=\"%s\"\n", SMTP_USER_NAME);
		OUT_D1EBUG(textBuf,(char *)smtp_buffer);
		Ql_SendToModem(ql_md_port1, (u8*)smtp_buffer, Ql_strlen((char*)smtp_buffer));
		break;

	case 3:
		Ql_memset(smtp_buffer,'\0',sizeof(smtp_buffer));
		Ql_sprintf((char *)smtp_buffer, "AT+QSMTPPWD=\"%s\"\n", SMTP_PASSWORD);
		OUT_D1EBUG(textBuf,(char *)smtp_buffer);
		Ql_SendToModem(ql_md_port1, (u8*)smtp_buffer, Ql_strlen((char*)smtp_buffer));
		break;

	case 4:
		Ql_memset(smtp_buffer,'\0',sizeof(smtp_buffer));
		Ql_sprintf((char *)smtp_buffer, "AT+QSMTPADDR=\"%s\"\n", SMTP_ADDR);
		OUT_D1EBUG(textBuf,(char *)smtp_buffer);
		Ql_SendToModem(ql_md_port1, (u8*)smtp_buffer, Ql_strlen((char*)smtp_buffer));
		break;

	case 5:
		Ql_memset(smtp_buffer,'\0',sizeof(smtp_buffer));
		Ql_sprintf((char *)smtp_buffer, "AT+QSMTPDST=1,1,\"%s\"\n", SMTP_DST);   //RAHMAN_CMNTD
		smtp_cmd_type = 5;
		smtp_bDoNexAT = FALSE;
		OUT_D1EBUG(textBuf,(char *)smtp_buffer);
		Ql_SendToModem(ql_md_port1, (u8*)smtp_buffer, Ql_strlen((char*)smtp_buffer));
		break;

	case 8:
		Ql_memset(smtp_buffer,'\0',sizeof(smtp_buffer));
		Ql_sprintf((char *)smtp_buffer, "AT+QSMTPSUB=0,\"%s\"\n", UID);
		OUT_D1EBUG(textBuf,(char *)smtp_buffer);
		Ql_SendToModem(ql_md_port1, (u8*)smtp_buffer, Ql_strlen((char*)smtp_buffer));
		break;

	case 6:
		Ql_memset(smtp_buffer,'\0',sizeof(smtp_buffer));
		Ql_sprintf((char *)smtp_buffer, "AT+QIFGCNT=0\n");    //RAHMAN___SNDING_TWO_MAIL_ONE_TIME
		OUT_D1EBUG(textBuf,(char *)smtp_buffer);
		Ql_SendToModem(ql_md_port1, (u8*)smtp_buffer, Ql_strlen((char*)smtp_buffer));
		break;

	case 7:
		Ql_memset(smtp_buffer,'\0',sizeof(smtp_buffer));
		Ql_sprintf((char *)smtp_buffer, "AT+QICSGP=1,\"%s\"\n",APN_NAME);
		OUT_D1EBUG(textBuf,(char *)smtp_buffer);
		Ql_SendToModem(ql_md_port1, (u8*)smtp_buffer, Ql_strlen((char*)smtp_buffer));
		break;

	case 9:
		Ql_memset(smtp_buffer,'\0',sizeof(smtp_buffer));
		Ql_strcpy((char *)smtp_buffer, "AT+QSMTPATT=\"/UFS/");
		Ql_strcat((char*)smtp_buffer,(char *)ACCD_FILE_NAME);
		Ql_strncat((char *)smtp_buffer,(char *)"\"\r\n",3);
		smtp_cmd_type = 11;
		smtp_bDoNexAT = FALSE;
		Ql_SendToModem(ql_md_port1, (u8*)smtp_buffer, Ql_strlen((char*)smtp_buffer));
		OUT_D1EBUG(textBuf,(char *)smtp_buffer);
		break;

	case 10:
		Ql_memset(smtp_buffer,'\0',sizeof(smtp_buffer));
		Ql_sprintf((char *)smtp_buffer, "AT+QSMTPPUT=1500\n");    //RAHMAN_CMNTD // +QSMTPPUT: -9//ERRROR
		smtp_cmd_type = 12;
		smtp_bDoNexAT = FALSE;
		OUT_D1EBUG(textBuf,(char *)smtp_buffer);
		Ql_SendToModem(ql_md_port1, (u8*)smtp_buffer, Ql_strlen((char*)smtp_buffer));
		break;

	case 11:
		Ql_memset(smtp_buffer,'\0',sizeof(smtp_buffer));
		Ql_sprintf((char *)smtp_buffer, "AT+QSMTPCLR\n");
		smtp_cmd_type = 13;
		smtp_bDoNexAT = FALSE;
		OUT_D1EBUG(textBuf,(char *)smtp_buffer);
		Ql_SendToModem(ql_md_port1, (u8*)smtp_buffer, Ql_strlen((char*)smtp_buffer));
		smtp_bDoNexAT=TRUE;
		break;

	default:
		FTP_Stuck_cntr=0;
		Ql_StopTimer(&GC_ftp_stuck);
		GC_ftp_stuck.timeoutPeriod = Ql_SecondToTicks(600);
		/*writecnt1 = 0;
		Ql_FileDelete((u8*)"accd_data_file.txt");
		in_accsmtp_rou = 0;
		acc_rd_cnt =  acc_rd_cnt + accd_temp_cnt ;   //RAHMAN_EX
		Accdata_wrtrdcnt();
		accd_temp_cnt = 0;*/
		tw_read_accd_file();
		OUT_D1EBUG(textBuf," ZZZZ ACCSMTP mail senttt   !!!!!! \r\n");
		break;
	}
}


void SMTP_modemdata(char *smtp_modem_readbuffer)
{
	if (Ql_strstr((char*)smtp_modem_readbuffer, "+QSMTPPUT: 0")!= NULL)
	{
		Ql_memset(check_response,'\0',sizeof(check_response));
		Ql_strcpy((char *)check_response,(char *)smtp_modem_readbuffer);
		OUT_D1EBUG(textBuf,"\r\nMail sent status =%s.\r\n",smtp_modem_readbuffer);
	//	Increment Buffer After Successful completion of transmission
		writecnt1 = 0;
		Ql_FileDelete((u8*)"accd_data_file.txt");
		in_accsmtp_rou = 0;
		acc_rd_cnt =  acc_rd_cnt + accd_temp_cnt ;   //RAHMAN_EX
		Accdata_wrtrdcnt();
		accd_temp_cnt = 0;

	}
	if(smtp_data==1)
	{
		OUT_D1EBUG(textBuf,"\r\n*****smtp_modem_readbuffer = %s*****\n",(char *)smtp_modem_readbuffer);
		//OUT_D1EBUG(textBuf,(char *)smtp_modem_readbuffer);

		if ((smtp_cmd_type == 5  && Ql_strstr((char*)smtp_modem_readbuffer,"+QSMTPDST:") != NULL)
				|| (smtp_cmd_type == 9  && Ql_strstr((char*)smtp_modem_readbuffer, "CONNECT")   != NULL)
				|| (smtp_cmd_type == 10 && Ql_strstr((char*)smtp_modem_readbuffer, "+QSMTPBODY:") != NULL)
				|| (smtp_cmd_type == 12  && Ql_strstr((char*)smtp_modem_readbuffer, "+QSMTPPUT: 0")   != NULL)
				||(smtp_cmd_type == 11  && Ql_strstr((char*)smtp_modem_readbuffer, "+QSMTPATT:")   != NULL)
		)
		{
			OUT_D1EBUG(textBuf,"in IF...smtp_cmd_type.\r\n");
			smtp_cmd_idx++;
			smtp_bDoNexAT = TRUE;
			SMTP_connect();
		}

		else if ((  Ql_strstr((char*)smtp_modem_readbuffer, "\r\nOK") != NULL
				|| Ql_strstr((char*)smtp_modem_readbuffer, "OK\r\n") != NULL
				|| Ql_strstr((char*)smtp_modem_readbuffer, "OK") != NULL
				|| Ql_strstr((char*)smtp_modem_readbuffer, "ERROR") != NULL)
				&& smtp_bDoNexAT != FALSE)
		{
			OUT_D1EBUG(textBuf,"in ELSE IF....\r\n");
			smtp_cmd_idx++;
			SMTP_connect();
		}
	}
	return;
}

void Accdata_wrtcnt(void)
{
	s32 ret1;
	char *ptr;
	u32 writeedlen;
	//ret1 = Ql_FileOpenEx((u8*)pfile1,QL_FS_CREATE);
	ret1 = Ql_FileOpenEx((u8*)"acc_flags.txt",QL_FS_CREATE);
	accwcnt=0;

	if(ret1 >= QL_RET_OK)
	{
		accfilehandle = ret1;
		ptr=ix_Itoa(ACCwrt_cnt);
		ret1 = Ql_FileSeek(accfilehandle,0,QL_FS_FILE_BEGIN);
		ret1 = Ql_FileWrite(accfilehandle, (u8*)ptr,32,&writeedlen);
		Ql_FileClose(accfilehandle);
		accfilehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"error in ACCwrt_cnt write\r\n");
	}
}

///////////////////////////////////////////////////NEW START/////////////////////////////////////////////////////

void Accdata_wrtcnt_read(void)
{
	s32 ret;
	u32 readedlen1;
	u32 filehandle;
	ret = Ql_FileOpenEx((u8*)"acc_flags.txt",QL_FS_CREATE);

	if(ret >= QL_RET_OK)
	{
		filehandle = ret;
		Ql_memset((ascii *)Accwrtcnt_readbuffer,0,sizeof(Accwrtcnt_readbuffer));
		ret = Ql_FileSeek(filehandle,0, QL_FS_FILE_BEGIN);
		ret = Ql_FileRead(filehandle, (u8 *)Accwrtcnt_readbuffer,32, &readedlen1);
		ACCwrt_cnt =Ql_atoi(Accwrtcnt_readbuffer);
		OUT_D1EBUG(textBuf,"\tACCwrt_cnt \t\t\t= %d\r\n",ACCwrt_cnt);
		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"Error in read ACCwrt_cnt \r\n");
	}
}

void Accdata_flags_read(void)
{
	s32 ret;
	u32 readedlen1;
	u32 filehandle;

	ret = Ql_FileOpenEx((u8*)"acc_flags1.txt",QL_FS_CREATE);

	if(ret >= QL_RET_OK)
	{
		filehandle = ret;
		Ql_memset((ascii *)Accwrtcnt_readbuffer,0,sizeof(Accwrtcnt_readbuffer));
		ret = Ql_FileSeek(filehandle,0, QL_FS_FILE_BEGIN);
		ret = Ql_FileRead(filehandle, (u8 *)Accwrtcnt_readbuffer,8, &readedlen1);
		send_accd_whole_data=Ql_atoi(Accwrtcnt_readbuffer);
		OUT_D1EBUG(textBuf,"\tsend_accd_whole_data \t\t= %d\r\n",send_accd_whole_data);
		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"Error in acc_flags reading**1\r\n");
	}
}

void wrt_dump_accd_wrtn_cnt(void)
{
	s32 ret1;
	char *ptr;
	u32 writeedlen;
	//ret1 = Ql_FileOpenEx((u8*)pfile1,QL_FS_CREATE);
	ret1 = Ql_FileOpenEx((u8*)"file_count.txt",QL_FS_CREATE);
	accwcnt=0;

	if(ret1 >= QL_RET_OK)
	{
		accfilehandle = ret1;
		ptr=ix_Itoa(dump_accd_wrtn_cnt);
		ret1 = Ql_FileSeek(accfilehandle,40,QL_FS_FILE_BEGIN);
		ret1 = Ql_FileWrite(accfilehandle, (u8*)ptr,32,&writeedlen);
		OUT_D1EBUG(textBuf,"\r\n Ql_FileWritewrt_cnt()dump_accd_wrtn_cnt =%d: writeedlen=%d\r\n",ret1, writeedlen);
		OUT_D1EBUG(textBuf,"Accdata_dump_read_cnt =%s\r\n",ptr);
		Ql_FileClose(accfilehandle);
		accfilehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"error in dump_accd_wrtn_cnt write\r\n");
	}
}


void dump_cnt_read(void)
{
	s32 ret;
	u32 readedlen1;
	u32 filehandle;

	ret = Ql_FileOpenEx((u8*)"file_count.txt",QL_FS_CREATE);

	if(ret >= QL_RET_OK)
	{
		filehandle = ret;
		Ql_memset((ascii *)Accwrtcnt_readbuffer,0,sizeof(Accwrtcnt_readbuffer));
		ret = Ql_FileSeek(filehandle,0, QL_FS_FILE_BEGIN);
		ret = Ql_FileRead(filehandle, (u8 *)Accwrtcnt_readbuffer,32, &readedlen1);
		// OUT_D1EBUG(textBuf,"acc_flags Ql_FileRead() = %d: readedlenfl = %d\r\n",ret, readedlen1);
		acc_rd_cnt = Ql_atoi(Accwrtcnt_readbuffer);
		OUT_D1EBUG(textBuf,"\tReaded acc_rd_cnt \t\t= %d\r\n",acc_rd_cnt);

		if(acc_rd_cnt < 0)
		{
			acc_rd_cnt = 0;
			OUT_D1EBUG(textBuf,"Very first time got reset make it zero = %d\r\n",acc_rd_cnt);
		}

		Ql_memset((ascii *)Accwrtcnt_readbuffer,0,sizeof(Accwrtcnt_readbuffer));
		ret = Ql_FileSeek(filehandle,40, QL_FS_FILE_BEGIN);
		ret = Ql_FileRead(filehandle, (u8 *)Accwrtcnt_readbuffer,32, &readedlen1);
		// OUT_D1EBUG(textBuf,"acc_flags Ql_FileRead() = %d: readedlenfl = %d\r\n",ret, readedlen1);
		dump_accd_wrtn_cnt = Ql_atoi(Accwrtcnt_readbuffer);
		OUT_D1EBUG(textBuf,"\tRead dump_accd_wrtn_cnt \t= %d\r\n",dump_accd_wrtn_cnt);
		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"Error in Dump acc_flags reading**1\r\n");
	}
}

void Accdata_wrtrdcnt(void)
{
	s32 ret1;
	char *ptr;
	u32 writeedlen;
	//ret1 = Ql_FileOpenEx((u8*)pfile1,QL_FS_CREATE);
	ret1 = Ql_FileOpenEx((u8*)"file_count.txt",QL_FS_CREATE);
	accwcnt=0;

	if(ret1 >= QL_RET_OK)
	{
		accfilehandle = ret1;
		ptr=ix_Itoa(acc_rd_cnt);
		ret1 = Ql_FileSeek(accfilehandle,0,QL_FS_FILE_BEGIN);
		ret1 = Ql_FileWrite(accfilehandle, (u8*)ptr,32,&writeedlen);
		//OUT_D1EBUG(textBuf,"\r\n Ql_FileWritewrt_cnt()=%d: writeedlen=%d\r\n",ret1, writeedlen);
		OUT_D1EBUG(textBuf,"Accdata_wrtrdcnt=%s\r\n",ptr);
		Ql_FileClose(accfilehandle);
		accfilehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"error in ACCrdcnt _cnt write\r\n");
	}
}


void read_stop_acc_capt(void)
{
	s32 ret;
	u32 readedlen1;
	u32 filehandle;

	ret = Ql_FileOpenEx((u8*)"acc_flags1.txt",QL_FS_CREATE);
	// OUT_D1EBUG(textBuf,"stop_acc_capt flags \r\n");

	if(ret >= QL_RET_OK)
	{
		filehandle = ret;
		Ql_memset((ascii *)Accwrtcnt_readbuffer,0,sizeof(Accwrtcnt_readbuffer));
		ret = Ql_FileSeek(filehandle,9, QL_FS_FILE_BEGIN);
		ret = Ql_FileRead(filehandle, (u8 *)Accwrtcnt_readbuffer,8, &readedlen1);
		//  OUT_D1EBUG(textBuf," AT  READ stop_acc_capt = %d: readedlen1 = %d\r\n",ret, readedlen1);
		stop_acc_capt = Ql_atoi(Accwrtcnt_readbuffer);
		OUT_D1EBUG(textBuf,"\tREAD stop_acc_capt  \t\t= %d\r\n",send_accd_whole_data);
		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"Error  instop_acc_capt reading\r\n");
	}
}

//stop_acc_capt
void write_stop_acc_capt(void)
{
	s32 ret1;
	char *ptr;
	u32 writeedlen;
	ret1 = Ql_FileOpenEx((u8*)"acc_flags1.txt",QL_FS_CREATE);
	accwcnt=0;

	if(ret1 >= QL_RET_OK)
	{
		accfilehandle = ret1;
		ptr=ix_Itoa(stop_acc_capt);
		ret1 = Ql_FileSeek(accfilehandle,9,QL_FS_FILE_BEGIN);
		ret1 = Ql_FileWrite(accfilehandle, (u8*)ptr,8,&writeedlen);
		OUT_D1EBUG(textBuf," AT WRITE stop_acc_capt  =%s\r\n",ptr);
		Ql_FileClose(accfilehandle);
		accfilehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"error in stop_acc_capt write\r\n");
	}
}

//send_accd_whole_data
void send_accd_whole_data_wrt(void)
{
	s32 ret1;
	char *ptr;
	u32 writeedlen;
	ret1 = Ql_FileOpenEx((u8*)"acc_flags1.txt",QL_FS_CREATE);
	accwcnt=0;

	if(ret1 >= QL_RET_OK)
	{
		accfilehandle = ret1;
		ptr=ix_Itoa(send_accd_whole_data);
		ret1 = Ql_FileSeek(accfilehandle,0,QL_FS_FILE_BEGIN);
		ret1 = Ql_FileWrite(accfilehandle, (u8*)ptr,8,&writeedlen);
		OUT_D1EBUG(textBuf,"send_accd_whole_data=%s\r\n",ptr);
		Ql_FileClose(accfilehandle);
		accfilehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"error in send_accd_whole_datawrite\r\n");
	}
}

void write_in_file(char *acc_read)
{
	s32 send_file;
	s32 send_accd_file;
	u32 readedlen2 = 0;

	readedlen2 =  Ql_strlen((char*)acc_read);
	OUT_D1EBUG(textBuf,"ZZZZZ IN file write buf size =%d\r\n",readedlen2);
	send_file = Ql_FileOpenEx((u8*)"accd_data_file.txt",QL_FS_CREATE);

	if(send_file >= QL_RET_OK)
	{
		send_accd_file = send_file;
		send_file = Ql_FileSeek(send_accd_file,writecnt1,QL_FS_FILE_BEGIN);
		send_file = Ql_FileWrite(send_accd_file, (u8*)acc_read,Ql_strlen((char*)acc_read),&writeedlen);

		OUT_D1EBUG(textBuf,"At temp file write written:=%d\r\n",writeedlen);
		writecnt1+=writeedlen;
		Ql_FileClose(send_accd_file);
		send_accd_file = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"ZZZZ ERRROR!!!!  in ACC_Data writing file\r\n");
	}
	//Changed from 7 to 2
	if(( No_of_read >= 7) || (accd_read_comp_flag == 1))
	{

		OUT_D1EBUG(textBuf,"ZZZZZ  IN No_of_read >= 7 \r\n");
		No_of_read = 1;
		accd_read_comp_flag = 0;
		writecnt1 = 0;
		acc_rd_cnt =  acc_rd_cnt - accd_temp_cnt ;//RAHMAN_EX
		Accdata_wrtrdcnt();
		Ql_memset((char *)ACCD_FILE_NAME,'\0',sizeof(ACCD_FILE_NAME));
		Ql_strcpy((char *)ACCD_FILE_NAME,(char *)"accd_data_file.txt");
		OUT_D1EBUG(textBuf,"file ACCD_FILE_NAME =%s\r\n",ACCD_FILE_NAME);
		Ql_FileGetSize((u8*)"accd_data_file.txt", &accd_data_file_size);
		OUT_D1EBUG(textBuf,"accd_data_file_size Send =%d\r\n",accd_data_file_size);

		if(accd_data_file_size >= 50 )
		{
			smtp_cmd_idx=1;
			in_accsmtp_rou=1;
			//stop_acc_capt=1;
			SMTP_connect();
		}
		else
		{
			OUT_D1EBUG(textBuf," Consider as read completed  Read completed \r\n");
			OUT_D1EBUG(textBuf,"In tw_read_accd_file  acc_rd_cnt  =%d \r\n",acc_rd_cnt);
			OUT_D1EBUG(textBuf,"In tw_read_accd_file  dump_accd_wrtn_cnt  =%d \r\n",dump_accd_wrtn_cnt);
			Gen_ICD();
			send_accd_whole_data = 0;
			send_accd_whole_data_wrt();
			stop_acc_capt=0;
			write_stop_acc_capt();
			acc_rd_cnt = 0;
			Accdata_wrtrdcnt();
			in_accsmtp_rou=0;
			Ql_FileDelete((u8*)"accd_data_file.txt");
			return;
		}
	}
	else
	{
		No_of_read ++;
		Ql_memset(acc_readbuffer,'\0',sizeof(acc_readbuffer));
		Ql_memset(acc_sendbuffer,'\0',sizeof(acc_sendbuffer));
		tw_read_accd_file();
	}
}


void tw_read_accd_file(void)
{
	s32 ret1;
	s32 accfilehandle_ACC;
	u32 length_last=0, find_new_line = 0,New_line_place =0,length_to_copy = 0;
	OUT_D1EBUG(textBuf,"In tw_read_accd_file  acc_rd_cnt  =%d \r\n",acc_rd_cnt);
	//acc_rd_cnt = 0;
	if(acc_rd_cnt <= dump_accd_wrtn_cnt)
	{
		ret1 = Ql_FileOpenEx(PATH_ACC,QL_FS_CREATE);
		if(ret1 >= QL_RET_OK)
		{
			accfilehandle_ACC=ret1;
			Ql_memset(acc_readbuffer,'\0',sizeof(acc_readbuffer));
			ret1 = Ql_FileSeek(accfilehandle_ACC,acc_rd_cnt, QL_FS_FILE_BEGIN);
			ret1 = Ql_FileRead(accfilehandle_ACC, (u8 *)acc_readbuffer,10020, &readedlen1);
			//		//OUT_D1EBUG(textBuf,"Ql_FileRead whole file =%d \r\n",readedlen1);
			Ql_FileClose(accfilehandle_ACC);
			accfilehandle_ACC = -1;
			find_new_line = readedlen1;
			//		//OUT_D1EBUG(textBuf,"String length of file = %d:\r\n",find_new_line);

			while(find_new_line >= 0)
			{
				if(acc_readbuffer[find_new_line] == '\r' || acc_readbuffer[find_new_line] == '\n')
				{
					break;
				}
				else
				{
					New_line_place++;
					// break;
				}
				find_new_line--;
			}
			//		OUT_D1EBUG(textBuf,"New line postn from last=%d \r\n",New_line_place);
			length_to_copy = readedlen1 - New_line_place;
			accd_temp_cnt =  accd_temp_cnt + length_to_copy;
			acc_rd_cnt = acc_rd_cnt + length_to_copy;
			accd_dif_read =  dump_accd_wrtn_cnt - acc_rd_cnt;

			if(accd_dif_read <= 100)
			{
				accd_read_comp_flag = 1;
			}

			//		OUT_D1EBUG(textBuf,"Length of data to copy for file:=%d \r\n",length_to_copy);
			Ql_memset(acc_sendbuffer,'\0',35000);
			Ql_strncpy(acc_sendbuffer,acc_readbuffer,length_to_copy);
			write_in_file(acc_sendbuffer);
		}
		else
		{
			OUT_D1EBUG(textBuf,"ZZZZZ ERROR IN reading main file  = %d:\r\n",ret1);

			if(!(Ql_strncmp((ascii *)acc_readbuffer,"\0",1)))
			{
				OUT_D1EBUG(textBuf,"No data\r\n");
				Ql_FileClose(accfilehandle_ACC);
				accfilehandle_ACC = -1;
				ACC_Flag=0;
			}
		}
	}
	else
	{
		OUT_D1EBUG(textBuf,"Alll data readed..  Read completed \r\n");
		Gen_ICD();
		send_accd_whole_data = 0;
		send_accd_whole_data_wrt();
		stop_acc_capt=0;
		write_stop_acc_capt();
		acc_rd_cnt = 0;
		Accdata_wrtrdcnt();
		in_accsmtp_rou=0;
		Ql_FileDelete((u8*)"accd_data_file.txt");
	}
}

//RAHMAN read whole incident file
void read_whole_incident_file(void)
{
	s32 ret,size_ret;
	u32 /*readedlen1 = 0,*/i= 0,read_complete = 0,whole_no_bytes = 0;
	u32 filehandle,filesize = 0;
	char temp_read[2000];
	OUT_D1EBUG(textBuf,"IN  dump incident file \r\n");
	Ql_Sleep(500);
	size_ret = Ql_FileGetSize((u8*)PATH_ACC, &filesize);
	OUT_D1EBUG(textBuf,"GCC full size of incident file =%d\r\n",filesize);
	OUT_D1EBUG(textBuf,"Incident file name  = %s\r\n",PATH_ACC);
	ret = Ql_FileOpenEx((u8 *)PATH_ACC,QL_FS_READ_ONLY);

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


void Gen_ICA(void)
{
	char CAstring[150];
	Ql_memset((ascii *)CAstring,'\0',sizeof(CAstring));

	Ql_strcpy(CAstring,UID);
	Ql_strncat(CAstring,"_ICA,",6);
	Ql_strcat(CAstring,GPRMC);
	Ql_strcat(CAstring,(char *)(ix_Itoa(acc_rd_cnt)));			//cam 1 read count
	Ql_strncat(CAstring,",",1);
	Ql_strcat(CAstring,(char *)(ix_Itoa(dump_accd_wrtn_cnt)));			//cam 1 dump count
	Ql_strncat(CAstring,",",1);
	Ql_strncat(CAstring,STAT,1);			//Status (A/V)
	Ql_strncat(CAstring,"\r\n",2);
	OUT_D1EBUG(textBuf,"ICA Stamp =%s:\r\n",CAstring);
	tw_filewrite((char *)CAstring);
}
void Gen_IDC(void)
{
	char CAstring[150];

	Ql_memset((ascii *)CAstring,'\0',sizeof(CAstring));

	Ql_strcpy(CAstring,UID);

	Ql_strncat(CAstring,"_IDC,",6);
	Ql_strcat(CAstring,GPRMC);
	Ql_strcat(CAstring,(char *)(ix_Itoa(acc_rd_cnt)));			//cam 1 read count
	Ql_strncat(CAstring,",",1);
	Ql_strcat(CAstring,(char *)(ix_Itoa(dump_accd_wrtn_cnt)));			//cam 1 dump count
	Ql_strncat(CAstring,",",1);
	Ql_strncat(CAstring,STAT,1);			//Status (A/V)
	Ql_strncat(CAstring,"\r\n",2);
	OUT_D1EBUG(textBuf,"IDC Stamp =%s:\r\n",CAstring);
	tw_filewrite((char *)CAstring);
}

void Gen_ICD(void)
{
	char CAstring[150];
	Ql_memset((ascii *)CAstring,'\0',sizeof(CAstring));
	Ql_strcpy(CAstring,UID);

	Ql_strncat(CAstring,"_ICD,",6);
	Ql_strcat(CAstring,GPRMC);
	Ql_strcat(CAstring,(char *)(ix_Itoa(acc_rd_cnt)));			//cam 1 read count
	Ql_strncat(CAstring,",",1);
	Ql_strcat(CAstring,(char *)(ix_Itoa(dump_accd_wrtn_cnt)));			//cam 1 dump count
	Ql_strncat(CAstring,",",1);
	Ql_strncat(CAstring,STAT,1);			//Status (A/V)
	Ql_strncat(CAstring,"\r\n",2);
	OUT_D1EBUG(textBuf,"ICD Stamp =%s:\r\n",CAstring);
	tw_filewrite((char *)CAstring);
}
