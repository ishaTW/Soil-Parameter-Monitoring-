//*****************************  Header files**************************************************
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
#include "MRW.h"
#include "para_read.h"
#include "ACCSMTP.h"
#include "TCP_IP.h"
#include "sms_handle.h"

#define OUT_D1EBUG(x,...)  \
		Ql_memset((x),0,100);  \
		Ql_sprintf((x),__VA_ARGS__);   \
		Ql_SendToUart(ql_uart_port2,(u8 *)(x),Ql_strlen(x));

#define  PATH_Ref ((u8 *)"Ref_data.txt")

//*********************************************************************  global variables ************************************************
u16 ref_length=0;
char l_databuff[100];
u16  loc_cmd_idx=0;//by ravi 2-August
char Location_data[40]={'\0'};
char ref_read_buffer[100]={'\0'};
extern char DATE[7],TIME[7];
extern char textBuf[100];
char Phone_No[12]="09049369866\0";
char loc_AT[]="AT+QGSMLOC=1\n";
char Q_atcm[12]={'\0'};
char Q_long[12]={'\0'};
char Q_lat[12]={'\0'};
char Phone_No_A[15]={'\0'};
char Phone_No_B[15]={'\0'};
char Phone_No_C[15]={'\0'};
char IP_str[10]={'\0'};
char VEHICLE_ID[10]={'\0'};
char Helpdata[10]="Help\0";
char Exp_smsdata[100]={'\0'};
char IST_DATE[20]={'\0'};
char IST_TIME[20]={'\0'};
extern char LAT[],LONG[];
bool prev_lat_long =0;
extern unsigned char ItoaStr[];
bool flag_at_loc=0;
//********************************************* Variables for GPS data ***********************************************************
extern char STAT[2];
extern char GPRMC[90];
extern char UID[];
extern double mCurrSpeed,mTotDist;
//********************************************* Variables for Panic key ***********************************************************
//extern int TL_CnT_Lmt;   // max 10,000
//extern int TL_wrt_cnt;
//extern int TL_read_cnt;
//extern u8 STMP_TX;
s32 Ref_filehandle=0;
//char String_lat[20]={'\0'};
//********************************************* Variables for pin su7bscription ***********************************************************
QlPinParameter pinparameter;
s32 iret=0;

//*************************************************************  Panic stamp routine ***********************************************************
void PANIC_STAMP(void)
{
	u32 diff;
	char PANICstring[90];
    Ql_memset((ascii *)PANICstring,'\0',sizeof(PANICstring));

	Ql_strcpy(PANICstring,UID);
	Ql_strncat(PANICstring,"_PN,",4);
	Ql_strcat(PANICstring,GPRMC);
	//OUT_D1EBUG(textBuf,"GPRMC111=%s:\r\n",GPRMC);
	ix_Itoa(mCurrSpeed);
	//OUT_D1EBUG(textBuf,"FtoaStrcurre%s\r\n",ItoaStr);
	Ql_strcat(PANICstring,(char *)ItoaStr);///mCurrSpeed speed
	Ql_strncat(PANICstring,",",1);
	Ql_strcat(PANICstring,(char *)(ix_Itoa(mTotDist)));
	Ql_strncat(PANICstring,",",1);
	Ql_strncat(PANICstring,"0.0",5);		//PDOP
	Ql_strncat(PANICstring,",",1);
	Ql_strncat(PANICstring,STAT,1);			//Status (A/V)
	Ql_strncat(PANICstring,"\r\n",2);
	//OUT_D1EBUG(textBuf,"%s\r\n",SIStampString);
	OUT_D1EBUG(textBuf,"PANICstring=%s:\r\n",PANICstring);
	//  trackflsent=0;
	//fun_trackflsent();
	tw_filewrite((char *)PANICstring);
	/*
	tw_TL_filewrite((char *)PANICstring);

	diff=TL_wrt_cnt - TL_read_cnt;
	OUT_D1EBUG(textBuf,"Difference in read and write count=%d\r\n",diff);

	if((diff > 0) && (STMP_TX == 0))
	{
		Read_TL_Data();
	}
	*/
}

//**********************************************  GPIO pin as interrupt pin subscription *****************************************************************
void EINT_GPIO_PINSUB()
{
	pinparameter.pinparameterunion.gpioparameter.pindirection = QL_PINDIRECTION_IN;
	pinparameter.pinparameterunion.gpioparameter.pinlevel = QL_PINLEVEL_HIGH;
	//pinparameter.pinparameterunion.gpioparameter.pinlevel = QL_PINLEVEL_LOW;
	iret = Ql_pinSubscribe(QL_PINNAME_DTR,QL_PINMODE_3,&pinparameter);
	OUT_D1EBUG(textBuf,"\r\nSubscribe(%d),pin=%d,mod=%d\r\n",iret,QL_PINNAME_DTR,QL_PINMODE_3);
}

//**********************************************   interrupt pin subscription  *****************************************************************
void EINT_PINSUB()
{
	pinparameter.pinconfigversion = QL_PIN_VERSION;
	pinparameter.pinparameterunion.eintparameter.eintsensitivetype = QL_EINTSENSITIVETYPE_LEVEL;
	//	pinparameter.pinparameterunion.eintparameter.hardware_de_bounce = 10; //10 ms
	pinparameter.pinparameterunion.eintparameter.hardware_de_bounce = 60; //5 ms
	pinparameter.pinparameterunion.eintparameter.software_de_bounce = 2500; // unit is ms , max is 2559
	// pinparameter.pinparameterunion.eintparameter.pinlevel = QL_PINLEVEL_LOW; // ADDED BY MADHAVI
	iret = Ql_pinSubscribe(QL_PINNAME_DTR, QL_PINMODE_3, &pinparameter);
}

//**********************************************  Panic buzzer on routine  *****************************************************************
void PANIC_BUZZER_high(void)
{
	int iret;
	OUT_D1EBUG(textBuf,"BUZZER BUZZ!!!!!!!!!!!!!!!!!!!!!\r\n");
	// onoff_buzz=TRUE;
	QlPinParameter pinparameter;
	pinparameter.pinconfigversion = QL_PIN_VERSION;
	pinparameter.pinparameterunion.gpioparameter.pinpullenable = QL_PINPULLENABLE_ENABLE;
	pinparameter.pinparameterunion.gpioparameter.pindirection = QL_PINDIRECTION_OUT;
	pinparameter.pinparameterunion.gpioparameter.pinlevel = QL_PINLEVEL_HIGH;
	iret = Ql_pinSubscribe(QL_PINNAME_GPIO4, QL_PINMODE_2, &pinparameter);
	OUT_D1EBUG(textBuf,"\r\nSubscribe(%d),pin=%d,mod=%d,pul=%d,dir=%d,lev=%d\r\n",iret,QL_PINNAME_GPIO2,QL_PINMODE_1,QL_PINPULLENABLE_ENABLE,QL_PINDIRECTION_OUT,QL_PINLEVEL_HIGH);

	return;
}

//**********************************************  Panic buzzer off routine  *****************************************************************
void PANIC_BUZZER_low(void)
{
	int iret;

	QlPinParameter pinparameter;
	pinparameter.pinconfigversion = QL_PIN_VERSION;
	pinparameter.pinparameterunion.gpioparameter.pinpullenable = QL_PINPULLENABLE_ENABLE;
	pinparameter.pinparameterunion.gpioparameter.pindirection = QL_PINDIRECTION_OUT;
	pinparameter.pinparameterunion.gpioparameter.pinlevel = QL_PINLEVEL_LOW;
	iret = Ql_pinSubscribe(QL_PINNAME_GPIO4, QL_PINMODE_2, &pinparameter);
	OUT_D1EBUG(textBuf,"\r\nSubscribe(%d),pin=%d,mod=%d,pul=%d,dir=%d,lev=%d\r\n",iret,QL_PINNAME_GPIO2,QL_PINMODE_1,QL_PINPULLENABLE_ENABLE,QL_PINDIRECTION_OUT,QL_PINLEVEL_LOW);

	return;
}

//**********************************************  Panic timer init routine  *****************************************************************
void PANIC_TIMER_INIT()
{
	QlTimer Panic;
	Panic.timeoutPeriod = Ql_SecondToTicks(2);
	Panic.timerId =0;
}

void INT_unsub(void)
{
int iret;
OUT_D1EBUG(textBuf,"JRM_GPIO_buzzer_unsub\r\n");
iret = Ql_pinUnSubscribe(QL_PINNAME_DTR);
OUT_D1EBUG(textBuf,"\r\nUnSubscribe(%d),pin=%d\r\n",iret,QL_PINNAME_DTR);

return;
}

/*void PANIC_TIMERUP()
{
	if(flSignalBuffer.eventData.timer_evt.timer_id == Panic.timerId)
	{
		//smtp_cmd_idx++;
		OUT_D1EBUG(textBuf,"PANIC Buzzer OFF !!!!!!!!! \r\n");

		//  Exception_sms();
		// PANIC_SMS();
		PANIC_SMS();

		JRM_GPIO_buzzer_unsub();
		PANIC_BUZZER_low();
		Panic.timerId=0;


		OUT_D1EBUG(textBuf,"cOMMAND FIRED AT_CCLK OFF !!!!!!!!! \r\n");
		Ql_SendToModem(ql_md_port1, (u8*)AT_CCLK, Ql_strlen(AT_CCLK));

		//  Ql_StartTimer(&Panic);
	}
}*/

/*void PANIC_ISR()
{
	if(flSignalBuffer.eventData.intr_evt.pinState == 1)
	{
		//OUT_D1EBUG(textBuf,"EVENT_INTR=pinName(%d),pinState=%d\r\n",flSignalBuffer.eventData.intr_evt.pinName,flSignalBuffer.eventData.intr_evt.pinState);
		OUT_D1EBUG(textBuf,"\r\EVENT OCCURED AT EINT\r\n");

		//the EVENT will report when you execute command 3.
		OUT_D1EBUG(textBuf,"\r\nEVENT_INTR=pinName(%d),pinState=%d\r\n",flSignalBuffer.eventData.intr_evt.pinName,flSignalBuffer.eventData.intr_evt.pinState);

		JRM_GPIO_buzzer_unsub();

		PANIC_BUZZER_high();
		CCLK_FLAG=1;
		Get_loc();
		Ql_StartTimer(&Panic);
		if(Loc_Command==0)
		{
			Loc_Command=1;
		}
		//
	}
	if(flSignalBuffer.eventData.intr_evt.pinState == 0)
	{
		OUT_D1EBUG(textBuf,"\r\nEVENT_NOT OCCURED\r\n");
		OUT_D1EBUG(textBuf,"\r\nEVENT_INTR=pinName(%d),pinState=%d\r\n",flSignalBuffer.eventData.intr_evt.pinName,flSignalBuffer.eventData.intr_evt.pinState);
	}
	//OUT_D1EBUG(textBuf,"EVENT_SERIALSTATUS port=%d type=%d val=%d\r\n",port,type,val);
	//  OUT_D1EBUG(textBuf,"EVENT_SERIALSTATUS port=%d type=%d val=%d\r\n",port,type,val);
}*/
