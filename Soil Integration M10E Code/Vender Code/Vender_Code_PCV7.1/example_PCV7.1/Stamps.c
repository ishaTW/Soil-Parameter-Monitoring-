/*-------------------------------------------------------------------------*/
/*  File       : Stamps.c                                                 
                 NGPRS,NGSM,NG stamp generation 
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
#include "Ql_filesystem.h"
#include "Ql_tcpip.h"
#include "ql_error.h"
#include "Fun.h"
#include "GPS.h"
#include "GSM_GPRS.h"
#include "TCP_IP.h"
#include "MRW.h" 
#include "SEND_DATA.h"
#include "para_read.h"
#include "Stamps.h"

#define OUT_DEBUG(x,...)  \
    Ql_memset((x),0,100);  \
    Ql_sprintf((x),__VA_ARGS__);   \
    Ql_SendToUart(ql_uart_port2,(u8 *)(x),Ql_strlen(x));
    
extern char textBuf[100];
extern char GPRMC[];
extern char UID[];
extern char STAT[];
extern double mCurrSpeed,mTotDist;
extern unsigned char ItoaStr[];


/*-------------------------------------------------------------------------*/
/*  Function   : Gen_NGSM		                                           */
/*-------------------------------------------------------------------------*/
/*  Object     : NGSM Stamp Generation				                       */
/*-------------------------------------------------------------------------*/

void Gen_NGSM(void)
{
    char NGSMstring[90]; 

    Ql_strncpy(NGSMstring,UID,7);
	Ql_strncat(NGSMstring,"_NGSM,",6);
	Ql_strcat(NGSMstring,GPRMC);
    ix_Itoa(mCurrSpeed);
	Ql_strcat(NGSMstring,(char *)ItoaStr);///mCurrSpeed speed
	Ql_strncat(NGSMstring,",",1);
	Ql_strcat(NGSMstring,(char *)(ix_Itoa(mTotDist)));
	Ql_strncat(NGSMstring,",",1);
	Ql_strncat(NGSMstring,"0.0",5);		//PDOP
    Ql_strncat(NGSMstring,",",1);
	Ql_strncat(NGSMstring,STAT,1);			//Status (A/V)
	Ql_strncat(NGSMstring,"\r\n",2);
    OUT_DEBUG(textBuf,"NGSMstring=%s:\r\n",NGSMstring);
}




void Gen_NGPRS(void)
{
    char NGPRSstring[90]; 

	Ql_strncpy(NGPRSstring,UID,7);
	Ql_strncat(NGPRSstring,"_NGPRS,",7);
	Ql_strcat(NGPRSstring,GPRMC);
    ix_Itoa(mCurrSpeed);
	Ql_strcat(NGPRSstring,(char *)ItoaStr);///mCurrSpeed speed
	Ql_strncat(NGPRSstring,",",1);
	Ql_strcat(NGPRSstring,(char *)(ix_Itoa(mTotDist)));
	Ql_strncat(NGPRSstring,",",1);
	Ql_strncat(NGPRSstring,"0.0",5);		//PDOP
    Ql_strncat(NGPRSstring,",",1);
	Ql_strncat(NGPRSstring,STAT,1);			//Status (A/V)
	Ql_strncat(NGPRSstring,"\r\n",2);
    OUT_DEBUG(textBuf,"NGPRSstring=%s:\r\n",NGPRSstring);
}





