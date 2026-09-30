/*-------------------------------------------------------------------------*/
/*  File       : SEND_DATA.c                                                 
                 SEND_DATA Connection functions
/*-------------------------------------------------------------------------*/
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
#include "Ql_tcpip.h"
#include "ql_error.h"
#include "Fun.h"
#include "GPS.h"
#include "GSM_GPRS.h"
#include "SEND_DATA.h"
#include "TCP_IP.h"
#include "MRW.h"
#include "para_read.h"
//#include "Fun.h"
/*-------------------------------------------------------------------------*/
/*								Globals									   */
/*-------------------------------------------------------------------------*/
//Variables for traces
#define OUT_DEBUG(x,...)  \
    Ql_memset((x),0,100);  \
    Ql_sprintf((x),__VA_ARGS__);   \
    Ql_SendToUart(ql_uart_port2,(u8 *)(x),Ql_strlen(x));


extern char textBuf[100];
extern unsigned char FtoaStr[];
extern unsigned char ItoaStr[];
//Variables for GPS data
extern char STAT[2];
extern char GPRMC[90];
extern char UID[];
//Variables for data transmission to server
//char readbuffer[100] = "UI8000_SI,021011,081505,1831.4990,N,07354.4629,E,000.0,000.6,0,0.0,A\r\n";
extern char readbuffer[];
extern ascii mSpeedAscii[];
extern double mTotDist;
extern double mCurrSpeed;
//extern s32 cgreg;


/*-------------------------------------------------------------------------*/
/*  Function   : timer_handler_stamping                                    */
/*-------------------------------------------------------------------------*/
/*  Object     : SI stamp function							               */
/*-------------------------------------------------------------------------*/

void timer_handler_stamping(void)
{
	ascii SIStampString[90];
//	OUT_DEBUG(textBuf,"in timer handler stamping\r\n");
	Ql_memset((ascii *)SIStampString,0,sizeof(SIStampString));
//	Ql_strncpy((ascii *)SIStampString,"UI8002_SI,",10);
	Ql_strncpy(SIStampString,UID,7);
	Ql_strncat(SIStampString,"_SI,",4);

	Ql_strcat(SIStampString,GPRMC);
	/*
	ix_Ftoa(mCurrSpeed,2);
	OUT_DEBUG(textBuf,"FtoaStr%s\r\n",FtoaStr);
    Ql_strcat((char *)SIStampString,(char *)FtoaStr);///speed
    Ql_strncat((char *)SIStampString,",",1);
    Ql_strcat(SIStampString,(char *)(ix_Itoa(mTotDist)));	//Distance
    OUT_DEBUG(textBuf,"mTotDistin si%lf\r\n",mTotDist);
	//Ql_strcat(OnStampString,(ascii *)(itoa(mTotDist)));	//Distance
	*/
//	Ql_strcat((ascii *)SIStampString,(ascii *)mSpeedAscii);
  //  ix_Ftoa(mCurrSpeed,2);
//	OUT_DEBUG(textBuf,"FtoaStr%s\r\n",FtoaStr);
  //  Ql_strcat((char *)SIStampString,(char *)FtoaStr);///speed
//    ix_Ftoa(mCurrSpeed,2);
    ix_Itoa(mCurrSpeed);
	//	OUT_DEBUG(textBuf,"FtoaStrcurre%s\r\n",ItoaStr);
	Ql_strcat(SIStampString,(char *)ItoaStr);///mCurrSpeed speed

	Ql_strncat(SIStampString,",",1);
	Ql_strcat(SIStampString,(char *)(ix_Itoa(mTotDist)));
	Ql_strncat(SIStampString,",",1);
	Ql_strncat(SIStampString,"0.0",5);		//PDOP
    Ql_strncat(SIStampString,",",1);
	Ql_strncat(SIStampString,STAT,1);			//Status (A/V)
	Ql_strncat(SIStampString,"\r\n",2);
//	OUT_DEBUG(textBuf,"%s\r\n",SIStampString);	
	//tw_filewrite((char *)SIStampString);
	//Ql_strcpy((ascii *)readbuffer,(ascii *)SIStampString);//copy SI stamp from SIstampstring to readbuffer
	//OUT_DEBUG(textBuf,"readbuffer=%c\r\n",readbuffer);
	
}
//-------------------------------------------------------------------------------------------------------//


