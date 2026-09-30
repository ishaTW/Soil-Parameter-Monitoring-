/*-------------------------------------------------------------------------*/
/*  File       : main.c                 
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*								Headers									   */
/*-------------------------------------------------------------------------*/

#include<stdio.h>
#include<string.h>
#include<math.h>

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
#include "ql_filesystem.h"
#include "ql_trace.h"
#include "ql_error.h"
#include "ql_fcm.h"
#include "Ql_error.h"
#include "Ql_sms.h"
#include "fota.h"
#include "Fun.h"
#include "GPS.h"
#include "GSM_GPRS.h"
#include "SEND_DATA.h"
#include "TCP_IP.h"
#include "MRW.h"  
#include "para_read.h" 
#include "ql_fcm.h"
#include "GPRMC.h" 
#include "fota.h"
#include "camera.h"
#include "ftp2.h"
#include "ftp.h"
#include "camera2.h"
#include "sms_handle.h" 
#include "JRM.h"
#include "JRM_RD.h"
#include "canparareadwrt.h"
#include "SIM_lock.h"
#include "ACCSMTP.h"
#include "exce_camera_C.h"
#include "exce_camera_R.h"
#include "Cell_ID.h"
#include "Battery.h"
//#include "GSM_ON_OFF.h"
#include "Stamps.h"
#include "BulkFota.h"
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

char textBuf[1000];
int mGSMRegister = 0;
extern unsigned char CCID;
extern unsigned char GSN;
extern u8 data_p[];
extern char UID[8];
char *pData, *p;
//char read[];  // 4june nitesh
//Variables for Tx & St - Shweta 22-Nov-2011

u8 CanUpdate_flag=0;
u16 OneMinTimer = 0, MS500timer = 0;
extern u8 mSocketClose;
//Variabes for GPS data - Atul 21-Nov-2011
ascii uart_buffer[1000];
char Rxbuffer[30];            //by ambika
int Rx_flag=0;

//char pPortEvt->data[250];
//char pPortEvt->data[];
u16 datalen=0;
u16 ucamdatalen=0;
u16 ucamdatalen2=0;
extern int uCount;
extern char modem_str[];
//u16 datalen;
//u32 query_dns_number = 0;
extern u32 readedlen; 
//extern bool uFlag=FALSE;  
extern int uwrt_cnt;
extern int uread_cnt;
int track_gen=0;
bool rec_flag=0;
int NGcount=0;
u16 PF_STATUS=0;
//u8 CanUpdate_flag=0;
extern char STI[8];
extern char TXI[8];
//u32 fileSize;
s32 hd_file=0;
PortData_Event* pPortEvt;
//PortData_Event* pDataEvt;
extern bool SIM_Flag;
extern bool camdumpflag;
//Variables for GSM-GPRS registeration - Shweta 22-Nov-2011
//int powerStamp = 0; 
int onesectimerflag=0;       //counter for one second
extern bool Synchronise_cmd;
extern bool Synchronise_cmd2;
extern u32 cam_datalen;
bool cam_bDoNexAT = TRUE;
char cam_buffer[100];
bool cam_fl_up_flag=0;
char GSM_buffer[100];
//camera stuck flag
extern int cam1_stk_flg;
extern int cam2_stk_flg;
//Battery
extern bool POStmpGEN;
extern bool PFStmpGEN;


/// Code Version  ///////////////////////////
char Code_version[30]="LITE2_1.7P4v1.0";
/////////////////////////////////////////////

//cam timer flags
bool synctimer=0;
bool firsttimer=0;
bool timer_tm=0;
bool stucktimer=0;
bool imageget=0;
bool in_cam_rou=0;
Ref_Voltage=3.700000;
u8 PowerOn_Count=0;
u8 PowerOn_Flag=0;
u8 PowerOff_Count=0;
u8 PowerOff_Flag=0;
u8 onmainflg=0;
u8 onbattcount=0;
u8 onbattflag=0;
int int_battvolt=0;
int int_battperc=0;
//cam2 flags
bool synctimer2=0;
bool firsttimer2=0;
bool timer_tm2=0;
bool stucktimer2=0;
bool imageget2=0;
bool in_cam_rou2=0;
bool rtFileAbsent=0;
extern char Unit_Type[35];
bool cam1dump=0;
bool cam2dump=0;
int onestuck1=0;
int onestuck2=0;
//bool cam_bDoNexAT;
//u8 cam_cmd_type;
u16  cam_cmd_idx=0;
bool cam_cap_time=0;
bool cam_connect_success=0;
char cam_uart_readbuffer[3000];
char cam_uart_readbuffer2[3000];
extern bool getdata_cmd_fire;
extern bool stop_img_capt;
extern u32 readcount;
extern u32 dump_cam_wrt_cnt;
extern char DATE[7];
extern char TIME[7];
extern u16  cam_cmd_idx;
extern  char gps_type[10];
extern bool onoff_buzz;
extern int BUZZ_COUNTER;
s32 tm_ret=0;
bool send_accd_whole_data = 0;  //RAHMAN
extern u32 writecnt1;
bool start_accd_data = 0,earlier_sending =0;
extern bool read_complete;
s32 fileret1;
u32 dump_accd_wrtn_cnt = 0,dump_read_cnt = 0;
//extern int acc_rd_cnt;
u8 CANDUMPflg=0;
extern u32 acc_rd_cnt;
char check_response[50];
bool trackfile_accd,reset_sending = 0;
extern char current_rt_scan[30];
extern bool ftp_rt_DoNext;
extern char str13[50];
extern u32 writeedlen1;
char FTPFILENAME[20];
s32 fileret3,ret4;
extern u32 jrm_route_id;
// char  PATH_JRM[30];
extern char ftp_buf_cmd[100];
extern u32 in_EXCp_cam_rou;
extern u32 in_EXCp_C_cam_rou;
extern int in_route_ftp;
bool dwn_finished =0, ftp_cmd_check =0;
extern u16  ftp_rt_cmd_idx;
extern bool ftp_rt_data;
extern route_id[10];
extern QlTimer ftp_route_start,jrm_rt_reset;
char uart3_buffer[100];
//QlSysTimer SysTime;
//QlTimer WD_MyTimer;
//WD_MyTimer.timerId = 0;

u32 NGSM_reset_cnt = 1;
u8 NGSM_flag = 0;

extern u32 Data_send_count;
u32 size_4hr_accd_file = 0;
extern bool ftp_data;
extern bool cam_fl_up_flag;
///////fota
extern int doing_fota;
///JRM_DL
extern int in_route_ftp;
extern char jrmstatus[10];
u8 rt_dwnld_fail_flag = 0,rt_jrm_incmp_flag = 0;
extern u8 rt_dwnld_fail_flag_cnt;
u32 rt_dwnld_fail_cnt = 0;
//////mrw
extern int wrt_cnt;
extern int read_cnt;
////
int jrm_cntr=0;
extern double mCurrSpeed;
extern int mOverSpeedLimit;
/////acc data
extern u16 smtp_cmd_idx;
extern int in_accsmtp_rou;
extern bool smtp_bDoNexAT;
extern bool stop_acc_capt;
extern int osbuzzer_flg;
extern u8 Fota_http_Flag;
extern bool camcaptureflag;
extern int camindicatorflag;
extern bool timerstarted;
extern bool datareading;
extern int trackflsent;
extern u32 cam_wrt_cnt;
extern u32 OSflag;				// OS exception image flag
extern u32 OSflag_C;
char ICacpTI[8];          	    //Image capture time interval
char ITxTI[8];					//Image Transmission time interval
extern u32 EX_FTP_TX_Flag;
extern bool OSRed;
extern bool OSYellow;
extern int mOverSpeedLimit1;
extern int mOverSpeedLimit2;
extern char CEllIDinfo[150];
extern u8 CEEL_ID_flg;
extern bool route_DL_command;
extern ascii SIStampString[500];
extern char Rxbuffer[]; 

//bulk FOTA
extern int in_Bulk_fota_rou;
extern u16  bulk_fota_cmd_idx;
extern int bulk_fota_cmd_type;
extern char SMSdatabuffer[];
extern bool Bulk_DinProg;
ascii uart_buffer[1000];
/////


u32 FTP_Stuck_cntr=0;
u8 CELL_Id_Cap=1;
u8 LIST_FIles=0;
///sms
u8 current_mem=0;
u8 used_mem=0;
u8 total_mem=0;
u16 PO_CNT=0;
u16 PF_CNT=0;
bool flagfr2=0;
bool flagfr30=0;
extern bool PO_STATUS;
float Current_volt=0.0;
void CallBack_NewSMS(u16 index,QlSMSStorage storage);
void CallBack_SendSMS(bool result, s16 cause, u8 msg_ref);
void CallBack_DeleteSMS(bool result, s16 cause,u16 index);
void CallBack_ReadPDUSMS(bool result, s16 cause,u16 index, u8 status, u8* data, u16 length);
void CallBack_NewFlashPDUSMS(u16 length, u8* pdu_string);
void CallBack_PDUStatusReport(u16 length, u8* pdu_string);
void CallBack_ReadTextSMS(bool result, s16 cause,u16 index, u8 status, QlSMSTextMsg* sms);
void CallBack_NewFlashTextSMS(QlSMSTextMsg* sms);
void CallBack_TextStatusReport(u8 fo,u8 msg_ref, u8* phone_num, QlSysTimer* scts, QlSysTimer* dt, u8 st);

///Timers    
QlTimer tm={0,0},tm2={0,0},TX_tmr={0,0},jrm_nextstation={0,0},jrm_delayripit={0,0},onoff={0,0},cap_intv={0,0},cam_tx={0,0};
QlTimer tm11={0,0},get_img={0,0},get_img2={0,0},jrm={0,0},zone_scanning={0,0},osbuzzer={0,0},timer_sendmail={0,0},timer_sendmail_1={0,0};
QlTimer timer_1={0,0},JRM_timer={0,0},timer_stuck={0,0},timer_sync={0,0},timer_ftp={0,0},timer_sync2={0,0},timer_12={0,0},timer_stuck2={0,0},GC_ftp_stuck={0,0};
QlTimer onemintmr={0,0},timersimflg={0,0};
QlTimer onesecdummy={0,0};
QlTimer battery={0,0},batteryBL={0,0},BulkFota_tmr={0,0},remote_bulChek={0,0};;
QlTimer deviation_tmr={0,0},OS_ExcpTmr={0,0},OS_ExcpTmr_C={0,0},CELLIDTmr={0,0};
/*-------------------------------------------------------------------------*/
/*								Main task								   */
/*-------------------------------------------------------------------------*/

QlEventBuffer flSignalBuffer; //Set flSignalBuffer to global variables  may as well, otherwise it will occupy stack space
QlEventBuffer flSignalBuffer_subtask1;


void ql_entry(void)
{
	//ascii uart_buffer[250];
	ascii SIStampString[500];

	unsigned char Power_event=0;
	s32 ret=0,retRes=0,retRes2=0,fl_sz=0;
	// QlTimer onemintmr,timersimflg;
	u32 fileSize=0,funret=0,Freespace=0;
	u32 TotalSpace=0;
	QlPinParameter pinparameter;
	QlPinLevel pinlevel;//Added by Ravikumar Nelavai to Check Power Status 4-March-2016
	//    u32 writtenlen;
	char strAT[100];

	Ql_SMS_Callback cb_func={CallBack_NewSMS,CallBack_SendSMS,CallBack_DeleteSMS,                 //public
			CallBack_ReadPDUSMS,CallBack_NewFlashPDUSMS,CallBack_PDUStatusReport,               //pdu
			CallBack_ReadTextSMS,CallBack_NewFlashTextSMS,CallBack_TextStatusReport};           //text
	ret=Ql_SMSInitialize(&cb_func);

	//OUT_D1EBUG(textBuf,"\r\nQl_SMSInitialize=%d\r\n",ret);

	bool keepGoing = TRUE;
	Ql_SetDebugMode(BASIC_MODE);
	Ql_UartClrRxBuffer(ql_uart_port2);       //BY AMBIKA
	//Open UART1
	Ql_OpenModemPort(ql_md_port1);
	Ql_OpenModemPort(ql_md_port2);
	//Ql_OpenModemPort(ql_md_port3);             //by ambika
	Ql_SetUartBaudRate(ql_uart_port2,115200);
	Ql_SetUartBaudRate(ql_uart_port3,115200);           //by ambika
	//Ql_SetUartBaudRate(ql_uart_port1,115200);           //by ambika
	Ql_UartSetGenericThreshold(ql_uart_port2,TRUE,1024,50);    // set port2 as receiver by ambika
	//Ql_UartSetGenericThreshold(ql_uart_port1,FALSE,1024,50);      //by ambika
	//Ql_UartSetGenericThreshold(ql_uart_port3,FALSE,1024,50);      //by ambika
	//Ql_SMSInitialize();

	JRM_GPIO_reset();				//fot JRM
	JRM_DATA_READ();               ///for JRM DATA
	//tw_CanParaRead();
	tw_updatedCanParaRead();			//to read parameters
	//2F_Init();						//for FOTA
	tw_para_read();
	remote_pararead();
	read_Cell_ID_flg();				////Cell ID enable Flag
	cam_mem_loc_read();				////for camera memory locations
	cam_mem_loc_readextra();
	camR_EXCP_mem_loc_read();
	cam_C_EXCP_mem_loc_read();
	//batt_flag_read();
	JRM_GPIO_reset();				//fot JRM


	//R_//,"memory locations done... \r\n");
	if(Ql_strstr((char *)gps_type,"L80") != NULL)
	{
		//R_//,"GPS is L80.... \r\n");
		Ql_SetUartBaudRate(ql_uart_port1,9600);
		//Ql_SetUartBaudRate(ql_uart_port1,115200);
	}
	else
	{ 
		Ql_SetUartBaudRate(ql_uart_port1,4800);
	}

	pinparameter.pinparameterunion.gpioparameter.pindirection = QL_PINDIRECTION_IN;
	pinparameter.pinparameterunion.gpioparameter.pinlevel = QL_PINLEVEL_HIGH;
	ret = Ql_pinSubscribe(QL_PINNAME_GPIO0,QL_PINMODE_2,&pinparameter);
	//Disable other NMEA strings received from GR301 mouse - Atul 21-Nov-2011
	Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF103,00,00,00,01*24\r\n",25);
	Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF103,01,00,00,01*25\r\n",25);
	Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF103,03,00,00,01*27\r\n",25);
	Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF103,05,00,00,01*21\r\n",25);
	Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF103,06,00,00,01*22\r\n",25);
	Ql_SendToUart(ql_uart_port1,(u8 *)"$PSRF103,08,00,00,01*2C\r\n",25);
	Ql_SendToUart(ql_uart_port1,(u8 *)"$PMTK397,0.7*3A\r\n",17);
	Ql_SendToUart(ql_uart_port1,(u8 *)"$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n",53);   
    Ql_SendToUart(ql_uart_port1,(u8 *)"ambika\r\n",7);     //by ambika

	timersimflg.timeoutPeriod = Ql_SecondToTicks(30);
	tm_ret= Ql_StartTimer(&timersimflg);
	//remote_bulChek.timeoutPeriod = Ql_SecondToTicks(120);
	//BulkFota_tmr.timeoutPeriod = Ql_SecondToTicks(900);       // timer for Bulk FOTA and remote command
	//Ql_StartTimer(&BulkFota_tmr);
	GC_ftp_stuck.timeoutPeriod = Ql_SecondToTicks(600);
	onesecdummy.timeoutPeriod = Ql_MillisecondToTicks(2000);
	timer_sync.timeoutPeriod = Ql_MillisecondToTicks(500);
	timer_sendmail.timeoutPeriod = Ql_SecondToTicks(3);
	timer_sendmail_1.timeoutPeriod = Ql_SecondToTicks(10);
	JRM_timer.timeoutPeriod = Ql_MillisecondToTicks(150);

	///only for JRM
	/*if(((Ql_strstr((char *)Unit_Type,"JRM") != NULL)||(Ql_strstr((char *)Unit_Type,"jrm") != NULL)) && (Ql_strstr((char *)jrmstatus,"JRMON") != NULL))
	{
		//JRM_timer.timerId =0;
		//	JRM_ROUT_chk();
		funret = chk_file_compltnV2((char *)route_id);
		OUT_D1EBUG(textBuf,"chk_file_compltn returns from main  =%d\r\n",funret);
		if(funret==1)
		{
			Ql_StartTimer(&JRM_timer);
		}
		else
		{
			rtFileAbsent=1;
		}
		//Ql_StartTimer(&JRM_timer);
		//R_//,"\r\nStarted timer JRM_timer \n");
	}
	else
	{
		//R_//,"This is not a JRM device... \r\n");
	}  */

	tm.timeoutPeriod = Ql_SecondToTicks(8);
	jrm.timeoutPeriod = Ql_MillisecondToTicks(1000);
	onoff.timeoutPeriod = Ql_MillisecondToTicks(2000);
	jrm_delayripit.timeoutPeriod = Ql_MillisecondToTicks(1000);
	//jrm_delayripit.timeoutPeriod = Ql_MillisecondToTicks(1500);
	jrm_nextstation.timeoutPeriod = Ql_MillisecondToTicks(1000);
	zone_scanning.timeoutPeriod = Ql_MillisecondToTicks(1000);
	//zone_scanning.timeoutPeriod = Ql_MillisecondToTicks(1500);
	osbuzzer.timeoutPeriod = Ql_MillisecondToTicks(500);
	get_img.timeoutPeriod = Ql_MillisecondToTicks(2000);
	cam_tx.timeoutPeriod = Ql_SecondToTicks(300);
	Ql_StartTimer(&cam_tx);
	/////camera2 timers////////////////////
	tm2.timeoutPeriod = Ql_SecondToTicks(8);
	timer_sync2.timeoutPeriod = Ql_MillisecondToTicks(1200);
	get_img2.timeoutPeriod = Ql_MillisecondToTicks(2000);

	//////////////////////////////////
	Accdata_wrtcnt_read();  //RAHMAN
	Accdata_flags_read();
	read_stop_acc_capt();
	dump_cnt_read();
	PO_STAT_READ();//Read Power Status 5-March-2016 by Ravikumar Nelavai
	//PF_flag_read();
	// multiple_calls();
	//	fl_sz=Ql_FileGetFreeSize();

	if(((Ql_strstr((char *)Unit_Type,"BAT") != NULL) || (Ql_strstr((char *)Unit_Type,"bat") != NULL)) || ((Ql_strstr((char *)Unit_Type,"BATTERY") != NULL) || (Ql_strstr((char *)Unit_Type,"battery") != NULL)))
	{
		//Battery Timer
		OUT_D1EBUG(textBuf,"This is BATTERY device... ,PFStmpGEN=%d\r\n",PFStmpGEN);
		battery.timeoutPeriod = Ql_SecondToTicks(10);
		Ql_StartTimer(&battery);

		batteryBL.timeoutPeriod = Ql_SecondToTicks(900);
		if(PFStmpGEN == 1)
		{
			OUT_D1EBUG(textBuf,"Starting BL stamp Timer.... \r\n");
			Ql_StartTimer(&batteryBL);
		}
	}
	else
	{
		OUT_D1EBUG(textBuf,"This is not a BATTERY device... \r\n");
	}
	OUT_D1EBUG(textBuf,"\r\n############ Device Memory Details ################\r\n");

	//OUT_D1EBUG(textBuf,"Free Space in Device  =%d\r\n",fl_sz);
	fl_sz=Ql_FileSys_GetSpaceInfo(1,&Freespace,&TotalSpace);
	OUT_D1EBUG(textBuf,"(Return=%d)Free Space=%d,Total Space=%d\r\n",fl_sz,Freespace,TotalSpace);

	OUT_D1EBUG(textBuf,"####################################################\r\n");

	while(keepGoing)
	{
		Ql_GetEvent(&flSignalBuffer);
		
		switch(flSignalBuffer.eventType)
		{
		case EVENT_TIMER:
		{
			//OUT_D1EBUG(textBuf,"In EVENT_TIMER.... \r\n");
			//R_//,"In ET & timer ID=%d\r\n",flSignalBuffer.eventData.timer_evt.timer_id);

			if(flSignalBuffer.eventData.timer_evt.timer_id == onesecdummy.timerId)
			{
				// OUT_D1EBUG(textBuf,"one sec timer timer up\r\n");
				//onesecdummy.timerId =0;
				//	if( onesectimerflag>=3600)
				if( onesectimerflag >= 3600)
				{
					onesectimerflag=0;
					// Ql_StopTimer(&onesectimer);
					firsttimer=1;
					// onesectimerflag=0;
					cam_timer();

				}
				if((onestuck1==2)&&(cam1_stk_flg==1)&&(datareading==0)&&(camdumpflag==0))
				{
					//DCOUT_D1EBUG(textBuf,"onestuck1_up_camera 1 stuck\r\n");
					Synchronise_cmd=1;
					onestuck1=0;
					//timer_sync.timeoutPeriod = Ql_MillisecondToTicks(500);
					//R_//,"\r\nStarting timer timer_sync 2\n");
					//timer_sync.timerId =0;
					Ql_StartTimer(&timer_sync);
					//R_//,"\r\n Started timer timer_sync1 \n");
				}
				/////second camera

				if((onestuck2==2)&&(cam2_stk_flg==1)&&(datareading==0)&&(camdumpflag==0))
				{
					onestuck2=0;
					Synchronise_cmd2=1;
					//	timer_sync2.timerId =0;
					Ql_StartTimer(&timer_sync2);
				}


				cam1_stk_flg=1;
				cam2_stk_flg=1;
				onestuck1=onestuck1+1;
				onestuck2=onestuck2+1;
				onesectimerflag=onesectimerflag+2;
				OUT_D1EBUG(textBuf,"one sec timer flag=%d\r\n",onesectimerflag);
				//Ql_StartTimer(&onesectimer);
				fun_onesec_cnt();
				//onesecdummy.timerId =0;
				tm_ret=Ql_StartTimer(&onesecdummy);
				//R_//,"\r\n onesectimer with timer ID in main=%d\r\n", tm_ret);
				//	onesectimer.timeoutPeriod = Ql_SecondToTicks(2);
				//	onesectimer.timerId =0;
				//	Ql_StartTimer(&onesectimer);
				// Ql_StartTimer(&onesectimer);
			}

			if( flSignalBuffer.eventData.timer_evt.timer_id == timersimflg.timerId)
				//if( timersimflg.timerId == flSignalBuffer.eventData.timer_evt.timer_id)
			{
				//R_//,"In timersimflg.... \r\n");
				Ql_StopTimer(&timersimflg);
				///JRM

				if(SIM_Flag == 0)
				{
					//R_//,"Go for SIM Lock \r\n");
					//GSMGPRS();
					SIM_LockRoutines(1);
					//timer.timeoutPeriod = Ql_MillisecondToTicks(30);
					//Ql_StartTimer(&timer);
				}

				//OUT_D1EBUG(textBuf,"STI___=====%s \r\n",STI);
				//OUT_D1EBUG(textBuf,"STI___=Ql_atoi====%d \r\n",(Ql_atoi(STI)));
				//onemintmr.timeoutPeriod = Ql_MillisecondToTicks(1000);
				onemintmr.timeoutPeriod = Ql_MillisecondToTicks((Ql_atoi(STI))*100);
				//onemintmr.timerId =0;
				tm_ret=Ql_StartTimer(&onemintmr);
				//R_//,"onemintmr with timer ID in main=%d\r\n", tm_ret);
				TX_tmr.timeoutPeriod = Ql_MillisecondToTicks((Ql_atoi(TXI))*100);
				//TX_tmr.timerId =0;
				tm_ret=Ql_StartTimer(&TX_tmr);
				//R_//,"TX_tmr with timer ID in main=%d\r\n", tm_ret);
				OneMinTimer=OneMinTimer+1;
			}

			if(flSignalBuffer.eventData.timer_evt.timer_id == TX_tmr.timerId)
			{
				//TX_tmr.timerId =0;
				tm_ret=Ql_StartTimer(&TX_tmr);
				//R_//,"In Transmission timer.... \r\n");
				//R_//,"TX_tmr with timer ID in main=%d\r\n", tm_ret);
				//tw_fileread_mrw();
				TCP_IP();

			}
			//by ambika 07/08/24
			////bulk DOTA timer
			if(flSignalBuffer.eventData.timer_evt.timer_id ==BulkFota_tmr.timerId)
			{
				Bulk_DinProg=1;
				OUT_D1EBUG(textBuf,"Bulk DOTA / Remote command check timer up road.\r\n");
				BulkFota_tmr.timeoutPeriod = Ql_SecondToTicks(3600);
				Ql_StartTimer(&BulkFota_tmr);

				if((EX_FTP_TX_Flag == 0) && (doing_fota == 0) && (camdumpflag == 0) && (send_accd_whole_data == 0))
				{
					OUT_D1EBUG(textBuf,"DOTA command through Timer. \r\n");
					bulk_fota_cmd_idx=1;
					bulk_fota_cmd_type=0;
					Bulk_fota_connect();
				}
				else
				{
					OUT_D1EBUG(textBuf," OTHER TRANSMISSION IN PROGRESS . \r\n");
				}
			}

			/// Timer for Remote command check
			if(flSignalBuffer.eventData.timer_evt.timer_id ==remote_bulChek.timerId)
			{
				OUT_D1EBUG(textBuf,"Remote command check timer UP..\r\n");

				Ql_StopTimer(&remote_bulChek);

				if(Bulk_DinProg == 0)
				{
					OUT_D1EBUG(textBuf,"Sending SMS command through timer.= %s\r\n",SMSdatabuffer);
					read_sms(SMSdatabuffer,Ql_strlen(SMSdatabuffer));
				}
				else
				{
					Ql_StartTimer(&remote_bulChek);
				}
			}

			///OS exception timer
			if(flSignalBuffer.eventData.timer_evt.timer_id == OS_ExcpTmr.timerId)
			{
				OUT_D1EBUG(textBuf,"OS exception timer up road.\r\n");
				if( ((OSRed == TRUE)&&(mCurrSpeed > mOverSpeedLimit1))||((OSYellow == TRUE)&&(mCurrSpeed > mOverSpeedLimit2))||(mCurrSpeed > mOverSpeedLimit))
					//if(mCurrSpeed > mOverSpeedLimit)
				{
					OSflag=1;
				}
			}

			///OS exception timer cabin camera
			if(flSignalBuffer.eventData.timer_evt.timer_id == OS_ExcpTmr_C.timerId)
			{
				OUT_D1EBUG(textBuf,"OS exception timer up for cabin.\r\n");
				if( ((OSRed == TRUE)&&(mCurrSpeed > mOverSpeedLimit1))||((OSYellow == TRUE)&&(mCurrSpeed > mOverSpeedLimit2))||(mCurrSpeed > mOverSpeedLimit))
					//if(mCurrSpeed > mOverSpeedLimit)
				{
					OSflag_C=1;
				}
			}
			//if( onemintmr.timerId  == flSignalBuffer.eventData.timer_evt.timer_id)
			if(flSignalBuffer.eventData.timer_evt.timer_id == onemintmr.timerId)
			{
				//onemintmr.timerId =0;
				tm_ret=Ql_StartTimer(&onemintmr);
				//R_//,"onemintmr with timer ID in main=%d\r\n", tm_ret);
				//R_//,"In onemintmr.... \r\n");

				if(mGSMRegister == 0 && SIM_Flag == 1)
				{
					//R_//,"Go for GSM GPRS registeration_0\r\n");
					GSMGPRS();
				}

				if((CELL_Id_Cap ==1) && (CEEL_ID_flg==1))
				{
					CELL_Id_Cap=1;
					OUT_D1EBUG(textBuf,"Get Location By Cell ID..\r\n");
					RIL_Multi_Cell_Id();
					//CELLIDTmr.timeoutPeriod = Ql_SecondToTicks(20);
					//Ql_StartTimer(&CELLIDTmr);
				}
				if(CEEL_ID_flg==1)
				{
					//CELL_Id_Cap=1;
					OUT_D1EBUG(textBuf,"Get Location By Cell ID..\r\n");
					RIL_Multi_Cell_Id();
					//CELLIDTmr.timeoutPeriod = Ql_SecondToTicks(20);
					//Ql_StartTimer(&CELLIDTmr);
				}

				timer_handler_stamping();
			}

			///Timwr for camera transmission interval
			if(((Ql_strstr((char *)Unit_Type,"SCAM") != NULL)||(Ql_strstr((char *)Unit_Type,"scam") != NULL)) && (cam_tx.timerId== flSignalBuffer.eventData.timer_evt.timer_id))
			{
				OUT_D1EBUG(textBuf,"camera exception tx timer up\r\n");
				if((EX_FTP_TX_Flag==0) && (Bulk_DinProg == 0) && (doing_fota!=1) && (in_cam_rou!=1) && (in_cam_rou2!=1)&& (in_accsmtp_rou!=1)&& (in_route_ftp!=1))
				{
					tw_EX_C_fileread();				//read file for transmission
				}
				else
				{
					OUT_D1EBUG(textBuf,"Camera exception tx in progress with EX_FTP_TX_Flag=%d\r\n", EX_FTP_TX_Flag);
				}

				Ql_StopTimer(&cam_tx);
				tm_ret=Ql_StartTimer(&cam_tx);   //start it for transmission
				OUT_D1EBUG(textBuf,"started exception camera tx with timer ID in main 2 =%d\r\n", tm_ret);
			}

			if(((Ql_strstr((char *)Unit_Type,"SCAM") != NULL)||(Ql_strstr((char *)Unit_Type,"scam") != NULL)) && (flSignalBuffer.eventData.timer_evt.timer_id == timer_sync.timerId))
			{
				//R_//,"timer_sync timer up\r\n");
				//camindicatorflag=0;
				synctimer=1;
				//Synchronise_cmd=1;
				cam_timer();
			}

			//if(jrm.timerId== flSignalBuffer.eventData.timer_evt.timer_id)
			if(((Ql_strstr((char *)Unit_Type,"JRM") != NULL)||(Ql_strstr((char *)Unit_Type,"jrm") != NULL)) && (flSignalBuffer.eventData.timer_evt.timer_id == jrm.timerId))
			{
				OUT_D1EBUG(textBuf,"jrm timer up\r\n");
				// jrm.timerId =0;
				// jrm_rout_scanning();
				//reas_zone_stamp();
				// Ql_StartTimer(&jrm);
			}

			//if(GC_ftp_stuck.timerId== flSignalBuffer.eventData.timer_evt.timer_id)
			if((flSignalBuffer.eventData.timer_evt.timer_id == GC_ftp_stuck.timerId) && ((Ql_strstr((char *)Unit_Type,"SCAM") != NULL)||(Ql_strstr((char *)Unit_Type,"scam") != NULL)))
			{
				Ql_StopTimer(&GC_ftp_stuck);
				//R_//,"GC_ftp_stuck timer up\r\n");
				FTP_Stuck_cntr++;

				if((FTP_Stuck_cntr >= 3) && (camdumpflag == 1))
				{
					Ql_Sleep(500);
					Ql_Reset(0);
				}
				ftp_data=0;
				cam_fl_up_flag=1;
				cam_bDoNexAT = TRUE;
				ftp_data=1;
				cam_cmd_idx = 1;
				SendAtCmd();
				//}
			}

			if((flSignalBuffer.eventData.timer_evt.timer_id == GC_ftp_stuck.timerId) && (send_accd_whole_data == 1))
			{
				OUT_D1EBUG(textBuf,"SMTP data transmission stuck .\r\n");
				Ql_StopTimer(&GC_ftp_stuck);
				FTP_Stuck_cntr++;

				if((FTP_Stuck_cntr >= 2) && (send_accd_whole_data == 1))
				{
					Ql_Sleep(500);
					Ql_Reset(0);
				}

				Ql_FileDelete((u8*)"accd_data_file.txt");
				tw_read_accd_file();
			}

			if((flSignalBuffer.eventData.timer_evt.timer_id == GC_ftp_stuck.timerId) && (Bulk_DinProg == 1))
			{
				OUT_D1EBUG(textBuf,"BULK FOTA Process stuck .\r\n");
				Ql_StopTimer(&GC_ftp_stuck);
				FTP_Stuck_cntr++;
				Bulk_DinProg=0;
			}

			if((flSignalBuffer.eventData.timer_evt.timer_id == GC_ftp_stuck.timerId) && (route_DL_command == 1))
			{
				OUT_D1EBUG(textBuf,"Route download stuck .\r\n");
				Ql_StopTimer(&GC_ftp_stuck);
				FTP_Stuck_cntr++;

				if((FTP_Stuck_cntr >= 3) && (route_DL_command == 1))
				{
					gen_rt_dwnld_fail_stamp();
					Ql_Sleep(500);
					Ql_Reset(0);
				}

				OUT_D1EBUG(textBuf," FILE doesn't exist download the file in main.  \r\n");
				ftp_rt_DoNext = TRUE;
				ftp_rt_data  = 1;
				ftp_rt_cmd_idx = 1;
				JRM_FTP_DL();
			}

			//if(onoff.timerId== flSignalBuffer.eventData.timer_evt.timer_id)
			//if(((Ql_strstr((char *)Unit_Type,"JRM") != NULL)||(Ql_strstr((char *)Unit_Type,"jrm") != NULL)) && (flSignalBuffer.eventData.timer_evt.timer_id == onoff.timerId))
			/*if((flSignalBuffer.eventData.timer_evt.timer_id == onoff.timerId))
			{
				OUT_D1EBUG(textBuf,"onoff timer up\r\n");
				//onoff.timerId =0;

				if(onoff_buzz==TRUE)
				{
					OUT_D1EBUG(textBuf,"onoff timer up  onoff_buzz==TRUE=====%d\r\n",BUZZ_COUNTER);
					onoff_buzz=FALSE;

					if(BUZZ_COUNTER==3)
					{
						OUT_D1EBUG(textBuf,"onoff timer up BUZZ_COUNTER==3=====%d\r\n",BUZZ_COUNTER);
						onoff.timeoutPeriod = Ql_MillisecondToTicks(1000);
						//onoff.timerId =0;
						Ql_StartTimer(&onoff);
						OUT_D1EBUG(textBuf,"Started timer onoff m1 \n");
						JRM_GPIO_buzzer_unsub();
						JRM_GPIO_BUZZER_low();
						//Ql_StartTimer(&onoff);
					}
					else if(BUZZ_COUNTER==6)
					{
						OUT_D1EBUG(textBuf,"onoff timer up BUZZ_COUNTER==6=====%d\r\n",BUZZ_COUNTER);
						Ql_StopTimer(&onoff);
						// onoff.timerId =0;
						JRM_GPIO_buzzer_unsub();
						JRM_GPIO_BUZZER_low();
					}
					else
					{
						OUT_D1EBUG(textBuf,"onoff timer up BUZZ_COUNTER in else=====%d\r\n",BUZZ_COUNTER);
						//onoff.timeoutPeriod = Ql_MillisecondToTicks(500);
						// onoff.timerId =0;
						Ql_StartTimer(&onoff);
						OUT_D1EBUG(textBuf,"\r\nStarted timer onoff m2 \n");
						JRM_GPIO_buzzer_unsub();
						JRM_GPIO_BUZZER_low();
					}
				}
				else if(onoff_buzz==FALSE)
				{
					onoff_buzz=TRUE;
					OUT_D1EBUG(textBuf,"onoff timer up  onoff_buzz==FALSE=====%d\r\n",BUZZ_COUNTER);
					BUZZ_COUNTER++;
					onoff.timeoutPeriod = Ql_MillisecondToTicks(500);
					// onoff.timerId =0;
					Ql_StartTimer(&onoff);
					OUT_D1EBUG(textBuf,"Started timer onoff m3 \n");
					JRM_GPIO_buzzer_unsub();
					JRM_GPIO_BUZZER_high();
					//BUZZ_COUNTER++;
				}
				// Ql_StartTimer(&onoff);
			}

			if(((Ql_strstr((char *)Unit_Type,"JRM") != NULL)||(Ql_strstr((char *)Unit_Type,"jrm") != NULL)) && (flSignalBuffer.eventData.timer_evt.timer_id == deviation_tmr.timerId))
			{
				OUT_D1EBUG(textBuf,"Deviation timer value up.\r\n");

				if(flagfr2==1)
				{
					flagfr2=0;
					flagfr30=1;
					OUT_D1EBUG(textBuf," yellow LED OFF \r\n");
					JRM_GPIO_unsub();
					JRM_GPIO_reset();
					Ql_StopTimer(&deviation_tmr);
					deviation_tmr.timeoutPeriod = Ql_SecondToTicks(30);				//start 30 sec timer
					Ql_StartTimer(&deviation_tmr);
					flagfr30=1;

				}
				else if(flagfr30==1)
				{
					flagfr30=0;
					flagfr2=1;
					OUT_D1EBUG(textBuf," yellow LED ON \r\n");
					JRM_GPIO_unsub();
					JRM_YELLOW_led();
					Ql_StopTimer(&deviation_tmr);
					deviation_tmr.timeoutPeriod = Ql_MillisecondToTicks(2000);				//start two sec timer
					Ql_StartTimer(&deviation_tmr);

				}

			}

			//if(osbuzzer.timerId== flSignalBuffer.eventData.timer_evt.timer_id)
			//	if(((Ql_strstr((char *)Unit_Type,"JRM") != NULL)||(Ql_strstr((char *)Unit_Type,"jrm") != NULL)) && ((flSignalBuffer.eventData.timer_evt.timer_id == osbuzzer.timerId)&&( ((OSRed == TRUE)&&(mCurrSpeed > mOverSpeedLimit1))||((OSYellow == TRUE)&&(mCurrSpeed > mOverSpeedLimit2))||(mCurrSpeed > mOverSpeedLimit))))
			if(((flSignalBuffer.eventData.timer_evt.timer_id == osbuzzer.timerId)&&( ((OSRed == TRUE)&&(mCurrSpeed > mOverSpeedLimit1))||((OSYellow == TRUE)&&(mCurrSpeed > mOverSpeedLimit2))||(mCurrSpeed > mOverSpeedLimit))))
			{
				//R_//,"osbuzzer timer up\r\n");
				osbuzzer_flg=1;
				OS_buzzer();
				Ql_StartTimer(&osbuzzer);
				//R_//,"Started timer osbuzzer m1 \r\n");
			}

			//if(jrm_nextstation.timerId== flSignalBuffer.eventData.timer_evt.timer_id)
			if(((Ql_strstr((char *)Unit_Type,"JRM") != NULL)||(Ql_strstr((char *)Unit_Type,"jrm") != NULL)) && (flSignalBuffer.eventData.timer_evt.timer_id == jrm_nextstation.timerId))
			{
				//R_//,"jrm jrm_nextstation up\r\n");
				// jrm_rout_scanning();
				//tw_findcurrentpos();
				jrm_rout_scanning();
				//Ql_StartTimer(&jrm_nextstation);
			}

			// if(jrm_delayripit.timerId== flSignalBuffer.eventData.timer_evt.timer_id)
			if(((Ql_strstr((char *)Unit_Type,"JRM") != NULL)||(Ql_strstr((char *)Unit_Type,"jrm") != NULL)) && (flSignalBuffer.eventData.timer_evt.timer_id == jrm_delayripit.timerId))
			{
				//R_//,"jrm jrm_delayripit up\r\n");
				// jrm_rout_scanning();
				timer_handler_DelayRipit();
				// Ql_StartTimer(&jrm_delayripit);
			}

			//if(zone_scanning.timerId== flSignalBuffer.eventData.timer_evt.timer_id)
			if(((Ql_strstr((char *)Unit_Type,"JRM") != NULL)||(Ql_strstr((char *)Unit_Type,"jrm") != NULL)) && (flSignalBuffer.eventData.timer_evt.timer_id == zone_scanning.timerId))
			{
				//R_//,"zone_scanning timer up\r\n");
				// jrm_rout_scanning();
				//after_detection();
				jrm_rout_scanning();
				// Ql_StartTimer(&jrm);
			}

			//if( JRM_timer.timerId  == flSignalBuffer.eventData.timer_evt.timer_id)
			if(((Ql_strstr((char *)Unit_Type,"JRM") != NULL)||(Ql_strstr((char *)Unit_Type,"jrm") != NULL)) && (flSignalBuffer.eventData.timer_evt.timer_id == JRM_timer.timerId))
			{
				//R_//,"jrm JRM_timer up\r\n");
				Ql_StopTimer(&JRM_timer);
				//jrm_rout_scanning();
				jrm_rout_readcount();
				//multiple_calls();
				//Ql_StartTimer(&JRM_timer);
			}

			//if( tm.timerId  == flSignalBuffer.eventData.timer_evt.timer_id)
			if(((Ql_strstr((char *)Unit_Type,"SCAM") != NULL)||(Ql_strstr((char *)Unit_Type,"scam") != NULL)) && (flSignalBuffer.eventData.timer_evt.timer_id == tm.timerId))
			{
				//R_//,"tm timer up\r\n");
				timer_tm=1;
				cam_timer();
			}

			//if( timer_stuck.timerId  == flSignalBuffer.eventData.timer_evt.timer_id)
			if(((Ql_strstr((char *)Unit_Type,"SCAM") != NULL)||(Ql_strstr((char *)Unit_Type,"scam") != NULL)) && (flSignalBuffer.eventData.timer_evt.timer_id == timer_stuck.timerId))
			{
				//R_//,"timer_stuck timer up\r\n");
				stucktimer=1;
				cam_timer();
			}

			//if( get_img.timerId  == flSignalBuffer.eventData.timer_evt.timer_id)
			if(((Ql_strstr((char *)Unit_Type,"SCAM") != NULL)||(Ql_strstr((char *)Unit_Type,"scam") != NULL)) && (flSignalBuffer.eventData.timer_evt.timer_id == get_img.timerId))
			{
				//R_//,"get_img timer up\r\n");
				imageget=1;
				cam_timer();
			}*/

			//if(timer_sendmail.timerId  == flSignalBuffer.eventData.timer_evt.timer_id)
			if(flSignalBuffer.eventData.timer_evt.timer_id == timer_sendmail.timerId)
			{
				//OUT_DEBUG(textBuf,"mailstr=%s\r\n",mailstr);
				//Ql_SendToModem(ql_md_port1, (u8*)sendbuffer_1, Ql_strlen((char *)sendbuffer_1));
				//R_//,"timer_sendmail timer up\r\n");
				smtp_cmd_idx++;
				smtp_bDoNexAT = TRUE;
				SMTP_connect();
			}

			//if( timer_sendmail_1.timerId  == flSignalBuffer.eventData.timer_evt.timer_id)
			if(flSignalBuffer.eventData.timer_evt.timer_id == timer_sendmail_1.timerId)
			{
				//smtp_cmd_idx++;
				OUT_D1EBUG(textBuf,"timer_sendmail_1 timer up\r\n");
				smtp_bDoNexAT = TRUE;
				smtp_cmd_idx++;
				SMTP_connect();
			}

			/////cam2 timer
			if( timer_sync2.timerId  == flSignalBuffer.eventData.timer_evt.timer_id)
			{
				OUT_D1EBUG(textBuf,"timer_sync2 timer up\r\n");
				//   camindicatorflag=0;
				synctimer2=1;
				cam_timer2();
			}
			/////  CELLIDTmr
			if((CELLIDTmr.timerId  == flSignalBuffer.eventData.timer_evt.timer_id) && (!((Ql_strstr((char *)Unit_Type,"JRM") != NULL)||(Ql_strstr((char *)Unit_Type,"jrm") != NULL))))
			{
				OUT_D1EBUG(textBuf,"Cell ID timer UP timer up\r\n");
				OUT_D1EBUG(textBuf,"Get Location By Cell ID..\r\n");
				RIL_Multi_Cell_Id();
				Ql_StartTimer(&CELLIDTmr);
			}

			if( timer_12.timerId  == flSignalBuffer.eventData.timer_evt.timer_id)
			{
				//R_//,"timer_12 timer up\r\n");
				firsttimer2=1;
				cam_timer2();
			}
			if( get_img2.timerId  == flSignalBuffer.eventData.timer_evt.timer_id)
			{

				imageget2=1;
				cam_timer2();
			}

			if(battery.timerId== flSignalBuffer.eventData.timer_evt.timer_id)   // /battery status
			{
				u8 chgStat;
				u32 cap;
				u32 vol;
				u8 Pw=1;
				float battvlt=0.0;
				ret = Ql_GetPowerSupply(&chgStat, &cap, &vol);
				int_battvolt=vol;
				int_battperc=cap;
				battvlt=((float)vol/1000);

				//  power_test=power_test+1;
				OUT_D1EBUG(textBuf,"Battery Voltage= %f ,  Battery %% = %d %%\r\n",battvlt,cap);
				Current_volt=battvlt;

				//PO_STAT_READ();//Read Power Status 5-March-2016 by Ravikumar Nelavai
				ret = Ql_pinRead(QL_PINNAME_GPIO0, &pinlevel);
				OUT_D1EBUG(textBuf, "\r\n Battery Charge Input Read(%d),pin=%d,level=%d\r\n",ret,QL_PINNAME_GPIO0,pinlevel);

				if(pinlevel == 1)
				{
					OUT_D1EBUG(textBuf,"Battery Charging.Device on Mains,...\r\n");
					PO_CNT++;
					if(PO_CNT >=6)
					{
						PO_CNT=0;
						if( POStmpGEN == 0 )
						{
							Ql_StopTimer(&batteryBL);
							PO_Stamp();
							PO_STATUS=1;
							PO_STAT_WRITE();
							POStmpGEN=1;
							PO_GEN_WRITE();
							PFStmpGEN=0;
							PF_GEN_WRITE();
						}
					}
				}
				else if(pinlevel == 0)
				{
					OUT_D1EBUG(textBuf,"Battery Discharging.Device on BAttery,...\r\n");
					PF_CNT++;
					if(PF_CNT >=6)
					{
						PF_CNT=0;
						if( PFStmpGEN == 0 )
						{
							Ql_StartTimer(&batteryBL);
							PF_Stamp();
							PO_STATUS=0;
							PO_STAT_WRITE();
							POStmpGEN=0;
							PO_GEN_WRITE();
							PFStmpGEN=1;
							PF_GEN_WRITE();
						}
					}
				}
				Ql_StartTimer(&battery);
			}
			if(batteryBL.timerId== flSignalBuffer.eventData.timer_evt.timer_id)   // /battery status
			{
				OUT_D1EBUG(textBuf,"Generate BL stamp.\r\n");
				Ql_StartTimer(&batteryBL);
				BL_Stamp();
			}

			break;
		}
		case EVENT_MODEMDATA:
		{
			if(!((Ql_strstr((char *)pPortEvt->data,"+QFLST:") != NULL)))
			{
				OUT_D1EBUG(textBuf,"<-- EVENT_MODEMDATA event:%d... -->\r\n", flSignalBuffer.eventType);
			}
			//PortData_Event* pPortEvt = (PortData_Event*)&flSignalBuffer.eventData.modemdata_evt;
			pPortEvt = (PortData_Event*)&flSignalBuffer.eventData.modemdata_evt;
			OUT_D1EBUG(textBuf,"in EVENT_MODEMDATA \n ");
			OUT_D1EBUG(textBuf,"EVENT_MODEMDATA buffer = %s,port=%d\n",pPortEvt->data,pPortEvt->port);

			if(((Ql_strstr((char *)pPortEvt->data,"+QFLST:") != NULL)))
			{
				OUT_D1EBUG(textBuf,"\t%s\n",pPortEvt->data);
			}

			if(((Ql_strstr((char *)pPortEvt->data,"+QENG: 1,") != NULL)))
			{
				OUT_D1EBUG(textBuf,"This is Cell Id String.. \n ");

				retRes2=ATResponse_Location_handler_CellID((char *)pPortEvt->data,pPortEvt->len);

				OUT_D1EBUG(textBuf,"****CELL ID Information =%s***\r\n",CEllIDinfo);
				if(retRes2==0)
				{
					OUT_D1EBUG(textBuf,"Cell Id data capture Successful\r\n");
					//CELL_Id_Cap=1;
					//RIL_Multi_Cell_Id_OFF();
					Ql_memset(strAT, 0, sizeof(strAT));
					Ql_sprintf(strAT, "AT+QENG=0,0\n");
					Ql_SendToModem(ql_md_port1, (u8*)strAT, Ql_strlen(strAT));
					OUT_D1EBUG(textBuf,"Cell Id data capture Stopped\r\n");
				}
				timer_handler_stamping();

		}

			else if(doing_fota==1)
				//if(doing_fota==1)
			{
				fota_uart();
			}

			//Bulk DOTA
			else if((in_Bulk_fota_rou==1)&&(doing_fota!=1))
			{
				OUT_D1EBUG(textBuf,"Bulk_Fota_modemdata fired-- Prev. Return = %s\n ",pPortEvt->data);
				Bulk_Fota_modemdata((char *)pPortEvt->data);
			}

			else if((in_cam_rou==1)&&(doing_fota!=1))
			{
				OUT_D1EBUG(textBuf,"cam_modemdata fired\n ");
				cam_modemdata((char *)pPortEvt->data);
			}

			else if((in_cam_rou2==1)&&(doing_fota!=1))
			{
				cam_modemdata2((char *)pPortEvt->data);
			}
			else if((in_accsmtp_rou==1)&&(doing_fota!=1))
			{
				// in_accsmtp_rou=0;
				//R_//,"ACCSMTP_modemdata fired\n ");
				//R_//,"\r\nbuffer = %s\n",pPortEvt->data);
				SMTP_modemdata((char *)pPortEvt->data);
			}
			else if((in_route_ftp == 1)&& (doing_fota!=1))
			{
				OUT_D1EBUG(textBuf,"Route FTP fired\r\n ");
				JRM_FTP_AT_RES((char *)pPortEvt->data);
			}
			else if((in_EXCp_cam_rou==1)&&(doing_fota!=1))
			{
				OUT_D1EBUG(textBuf,"EXR_cam_modemdata fired\n ");
				ExR_cam_modemdata((char *)pPortEvt->data);
			}
			else if((in_EXCp_C_cam_rou==1)&&(doing_fota!=1))
			{
				OUT_D1EBUG(textBuf,"EXC_cam_modemdata fired\n ");
				ExC_cam_modemdata((char *)pPortEvt->data);
			}
			else if(DATA_TCP_T == pPortEvt->type)
			{
				u32 writtenlen;
				ret = Ql_FileWrite(hd_file,(u8*)pPortEvt->data, pPortEvt->len, &writtenlen);
				//	OUT_D1EBUG(textBuf,"DATA_TCP_T, len:%d\r\n", pPortEvt->len);
				//ret = Ql_FileWrite(hd_file, (u8*)pPortEvt->data, pPortEvt->len, &writtenlen);
				fileSize += writtenlen;
				pPortEvt->data[0]='\0';
				//	OUT_D1EBUG(textBuf,"fileSize=%d\r\n",fileSize);
			}

			else if((Ql_strstr((u8 *)pPortEvt->data,"CMTI") == NULL))
			{
				//R_//,"\n in checkATresponse\r\n");
				checkATresponse();
			}

			else if(in_cam_rou2==1)
				{
			     OUT_D1EBUG(textBuf,"cam2_modemdata fired\n ");
				  cam_modemdata2((char *)pPortEvt->data);

		   		 }


		case EVENT_UARTDATA:
		{
			//AT command send to BB, goto run
			OUT_D1EBUG(textBuf,"<-- EVENT_UARTDATA event:%d... -->\r\n", flSignalBuffer.eventType);    //by ambika

			PortData_Event* pDataEvt = (PortData_Event*)&flSignalBuffer.eventData.uartdata_evt;       //by ambika
			pPortEvt = (PortData_Event*)&flSignalBuffer.eventData;                                    //by ambika
			OUT_D1EBUG(textBuf,"Uart Data at Port [%d]: %d\r\n",pDataEvt->port,pDataEvt->len);        //by ambika

			//ret= Ql_osGetCurrenTaskRemainStackSize();				//get Current Task Stack Size remaining
			//OUT_D1EBUG(textBuf,"<-- GetCurrenTaskRemainStackSize:%d... -->\r\n",ret);

		Ql_strcpy(uart_buffer,((char *)flSignalBuffer.eventData.modemdata_evt.data));     //by ambika
	    OUT_D1EBUG(textBuf,"\r\n Data is = %s\n",uart_buffer);        //by ambika

	    timer_handler_stamping();

		// by ambika
			/*datalen = Ql_strlen(uart_buffer);    //by ambika
			Ql_strcpy(uart_buffer,((char *)flSignalBuffer.eventData.modemdata_evt.data));
			//Ql_strncat(SIStampString,buffer,15);
			OUT_D1EBUG(textBuf,"<-- EVENT_UARTDATA event:%d... -->\r\n", flSignalBuffer.eventType);*/
			/*if((pDataEvt->port==3)  && (Ql_strstr(buffer,("S"))))
				{
					smtp_cmd_idx=1;
					in_accsmtp_rou=1;
					SMTP_connect();
					fota_function();
				}*/
				if(Fota_http_Flag==1)
				{
					//fota_function();
				}

			/*if((pDataEvt->port==3)  && (Ql_strstr(buffer,("fota"))))
			{
				OUT_D1EBUG(textBuf,"DOTA command through UART. \r\n");
				bulk_fota_cmd_idx=1;
				bulk_fota_cmd_type=0;
				Bulk_fota_connect();
			}*/

			if((trackflsent==1) && (send_accd_whole_data == 1))
			{
				trackflsent=0;
				//Go for remaining ACCD dump
				//R_//,"\n Go for remaining ACCD dump  \r\n");
				Gen_IDC();
				Ql_FileDelete((u8*)"accd_data_file.txt");
				tw_read_accd_file();
			}

			if(((Ql_strstr((char *)Unit_Type,"JRM") != NULL)||(Ql_strstr((char *)Unit_Type,"jrm") != NULL)))
				{
					if(rt_dwnld_fail_flag_cnt == 1)
					{
						//R_//,"\n rt dwn fail flag up \r\n");
						rt_dwnld_fail_cnt++;
						///	if(rt_dwnld_fail_cnt >= 30)

						if(rt_dwnld_fail_cnt >= 120)
						{
							rt_dwnld_fail_flag_cnt = 0;
							rt_dwnld_fail_cnt = 0;
							JRM_ROUT_chk();
						}
					}
				}

			if(((Ql_strstr((char *)Unit_Type,"JRM") != NULL)||(Ql_strstr((char *)Unit_Type,"jrm") != NULL)) && (Ql_strstr((char *)jrmstatus,"JRMON") != NULL) && (funret==1))
			{
				jrm_cntr++;

				if((jrm_cntr >= 150)&& (rt_jrm_incmp_flag == 0))
				{
					//R_//,"\r\nJRM scanning got stuck \n");
					jrm_cntr=0;
					stop_jrm_timer();
					jrm_rout_scanning();
				}
			}

			if(((Ql_strstr((char *)Unit_Type,"SCAM") != NULL)||(Ql_strstr((char *)Unit_Type,"scam") != NULL))&&(trackflsent==1) && (camdumpflag==1))
			{
				Gen_TC();
				// Ql_Sleep(3000);
				trackflsent=0;
				tw_fileread();
			}

			if(((Ql_strstr((char *)Unit_Type,"SCAM") != NULL)||(Ql_strstr((char *)Unit_Type,"scam") != NULL))&&(camindicatorflag==1)&&(datareading==0)&&(camdumpflag==0))
			{
				camindicatorflag=2;
				timerstarted=0;
				Synchronise_cmd=1;
				Synchronise_cmd2=1;
				//	timer_sync.timerId =0;
				Ql_StartTimer(&timer_sync);
				//timer_sync2.timerId =0;
				Ql_StartTimer(&timer_sync2);
				//RROUT_D1EBUG(textBuf,"\r\nStarted timer timer_sync2 m1 \n");
			}

			/*if(pDataEvt->port==2)
			{
				Ql_memset((ascii *)uart_buffer,0,sizeof(uart_buffer));
				gprsformat1(uart_buffer,datalen);
				OUT_D1EBUG(textBuf,"\r\nData received = %s\n",uart_buffer);
				Ql_memset((ascii *)flSignalBuffer.eventData.modemdata_evt.data,0,sizeof(flSignalBuffer.eventData.modemdata_evt.data));
				//Ql_memset((ascii *)uart_buffer,0,sizeof(uart_buffer));
		        OUT_D1EBUG(textBuf,"########DATE=%s\r\n",DATE);
				OUT_D1EBUG(textBuf,"########TIME=%s\r\n",TIME);
				//Ql_strcat((char *)cam_uart_readbuffer,(char *)pDataEvt->data);
			}*/
			if(pDataEvt->port==4)
			{
				datalen=pDataEvt->len;
			    OUT_D1EBUG(textBuf,"Uart data pDataEvt->len [%d]********&*****:datalen  %d\r\n",pDataEvt->len,datalen);    //by ambika
				Ql_memset((char *)uart_buffer,0,sizeof(uart_buffer));                                                      //by ambika
				Ql_strcat((char *)uart_buffer,(char *)pDataEvt->data);                                                     //by ambika
				//cam_uartdata((char *)pDataEvt->data);
			    //cam_uartdata();
		        //Ql_memset((ascii *)uart_buffer,0,sizeof(uart_buffer));
			   // gprsformat1(uart_buffer,datalen);
			    OUT_D1EBUG(textBuf,"\r\nData received = %s\n",uart_buffer);               //by ambika
			    Ql_memset((ascii *)flSignalBuffer.eventData.modemdata_evt.data,0,sizeof(flSignalBuffer.eventData.modemdata_evt.data));   //by ambika
			    //Ql_memset((ascii *)uart_buffer,0,sizeof(uart_buffer));
	            //OUT_D1EBUG(textBuf,"########DATE=%s\r\n",DATE);                       ////by ambika
			    //OUT_D1EBUG(textBuf,"########TIME=%s\r\n",TIME);                         ////by ambika
			   //Ql_strcat((char *)uart_buffer,(char *)pDataEvt->data);
			    timer_handler_stamping();
			}

		/*if(pDataEvt->port==4)
			{
				ucamdatalen=pDataEvt->len;
				//	OUT_D1EBUG(textBuf,"Uart data pDataEvt->len [%d]********&*****:ucamdatalen  %d\r\n",pDataEvt->len,ucamdatalen);
				Ql_memset((char *)cam_uart_readbuffer,0,sizeof(cam_uart_readbuffer));
				//Ql_strcat((char *)cam_uart_readbuffer,(char *)pDataEvt->data);
				cam_uartdata((char *)pDataEvt->data);
				//cam_uartdata();

			}*/

			/*if(pDataEvt->port==3)
			{
				if((pDataEvt->port==3)  && (Ql_strstr(buffer,("CELLID"))))
				{
					OUT_D1EBUG(textBuf,"Calling Cell Id routine\r\n");
					RIL_Multi_Cell_Id();
				}
				if((pDataEvt->port==3)  && ((Ql_strstr(buffer,("AT+DUMPCAN"))) || (Ql_strstr(buffer,("at+dumpcan")))))
				{
					OUT_D1EBUG(textBuf,"Taking CAN DUMP\r\n");
					CANDUMPflg=1;
					CAN_Dump();
				}
				if((pDataEvt->port==3)  && ((Ql_strstr(buffer,("AT+FLIST"))) || (Ql_strstr(buffer,("at+flist")))))
				{
					OUT_D1EBUG(textBuf,"LIST OF FILES FROM DEVICE MEMORY\r\n");
					Ql_sprintf((char *)cam_buffer, "AT+QFLST\n");
					Ql_SendToModem(ql_md_port1, (u8*)cam_buffer, Ql_strlen(cam_buffer));
				}

				if((pDataEvt->port==3)  && ((Ql_strstr(buffer,("AT+DUMPACC"))) || (Ql_strstr(buffer,("at+dumpacc")))))
				{
					OUT_D1EBUG(textBuf,"Incident Data Dump.\r\n");
					read_whole_incident_file();
				}
				if((pDataEvt->port==3)  && ((Ql_strstr(buffer,("AT+DUMPSTAMP"))) || (Ql_strstr(buffer,("at+dumpstamp")))))
				{
					OUT_D1EBUG(textBuf,"Stamp Data Dump.\r\n");
					read_whole_STAMP_file();
				}
				if((pDataEvt->port==3)  && ((Ql_strstr(buffer,("AT+BUZZ"))) || (Ql_strstr(buffer,("at+buzz")))))
				{
					OUT_D1EBUG(textBuf,"Test Buzzer.\r\n");
					JRM_GPIO_buzzer_unsub();
					JRM_GPIO_BUZZER_high();
					Ql_Sleep(2000);
					JRM_GPIO_buzzer_unsub();
					JRM_GPIO_BUZZER_low();

				}
				if((pDataEvt->port==3)  && ((Ql_strstr(buffer,("AT+RED"))) || (Ql_strstr(buffer,("at+red")))))
				{
					OUT_D1EBUG(textBuf,"Test RED LED.\r\n");
					JRM_GPIO_unsub();
					JRM_RED_led();
					Ql_Sleep(1500);
					JRM_GPIO_unsub();
					JRM_GPIO_reset();
				}
				if((pDataEvt->port==3)  && ((Ql_strstr(buffer,("AT+YELLOW"))) || (Ql_strstr(buffer,("at+yellow")))))
				{
					OUT_D1EBUG(textBuf,"Test YELLOW LED.\r\n");
					JRM_GPIO_unsub();
					JRM_YELLOW_led();
					Ql_Sleep(1500);
					JRM_GPIO_unsub();
					JRM_GPIO_reset();
				}
				if((pDataEvt->port==3)  && ((Ql_strstr(buffer,("AT+GREEN"))) || (Ql_strstr(buffer,("at+green")))))
				{
					OUT_D1EBUG(textBuf,"Test GREEN LED.\r\n");
					JRM_GPIO_unsub();
					JRM_GREEN_led();
					Ql_Sleep(1500);
					JRM_GPIO_unsub();
					JRM_GPIO_reset();
				}
				else
				{

					ucamdatalen2=pDataEvt->len;
					//	OUT_D1EBUG(textBuf,"Uart data pDataEvt->len [%d]********&*****:ucamdatalen2  %d\r\n",pDataEvt->len,ucamdatalen2);
					Ql_memset((char *)cam_uart_readbuffer2,0,sizeof(cam_uart_readbuffer2));
					//Ql_strcat((char *)cam_uart_readbuffer,(char *)pDataEvt->data);
					cam_uartdata2((char *)pDataEvt->data);
					//Ql_memset((ascii *)flSignalBuffer.eventData.modemdata_evt.data,0,sizeof(flSignalBuffer.eventData.modemdata_evt.data));
					//Ql_memset((ascii *)buffer,0,sizeof(buffer));
					//cam_uartdata();
				}
			}
			break;*/

		}
		case EVENT_SERIALSTATUS:
		{
			PortStatus_Event* pPortStatus = (PortStatus_Event*)&flSignalBuffer.eventData.portstatus_evt;

			bool val = pPortStatus->val;
			u8 port = pPortStatus->port;
			u8 type = pPortStatus->type;
			OUT_D1EBUG(textBuf,"EVENT_SERIALSTATUS port=%d type=%d val=%d\r\n",port,type,val);
			//  OUT_D1EBUG(textBuf,"EVENT_SERIALSTATUS port=%d type=%d val=%d\r\n",port,type,val);
			break;
		}
		case EVENT_MSG:
		{
			OUT_D1EBUG(textBuf,"EVENT_MSG\r\n");
			//  MyEvent.eventData.msg_evt.data1, MyEvent.eventData.msg_evt.data2);
			break;
		}

		case EVENT_INTR:
		{
			if(((Ql_strstr((char *)Unit_Type,"BAT") != NULL)||(Ql_strstr((char *)Unit_Type,"bat") != NULL)) || ((Ql_strstr((char *)Unit_Type,"BATTERY") != NULL)||(Ql_strstr((char *)Unit_Type,"battery") != NULL)))
			{
				if(flSignalBuffer.eventData.intr_evt.pinState == 1)
				{ // Gen_POStamp(); //added
					PowerOn_Count=1;
					PowerOn_Flag=1;
					onmainflg=1;
					onbattflag=0;
					Power_event=1;
					OUT_D1EBUG(textBuf,"PowerOn_Count=(%d),PowerOn_Flag=%d\r\n",PowerOn_Count,PowerOn_Flag);
				}
				if(flSignalBuffer.eventData.intr_evt.pinState == 0)
				{
					PowerOff_Count=1;
					PowerOff_Flag=1;
					onbattflag=1;
					Power_event=0;
					// OUT_D1EBUG(textBuf,"EVENT_INTR=pinName(%d),pinState=%d\r\n",flSignalBuffer.eventData.intr_evt.pinName,flSignalBuffer.eventData.intr_evt.pinState);
					OUT_D1EBUG(textBuf,"PowerOn_Count2=(%d),PowerOn_Flag2=%d\r\n",PowerOff_Count,PowerOff_Flag)
				}
			}
			break;

		}
		default:
			//OUT_D1EBUG(textBuf,"<-- Other event:%s\r\n", Rxbuffer.eventType);
			break;
		}
		}
		}
		}



