
/***************************************************************************************************
Battery charging Status STAMPS PO,PF,BL By Ravikumar.Nelavai on 12th-May-2015
 ****************************************************************************************************/
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
#include "Battery.h"
#include "canparareadwrt.h"

/************************************************************************************************************
 * Debug
 *************************************************************************************************************/
#define OUT_D1EBUG(x,...)  \
		Ql_memset((x),0,100);  \
		Ql_sprintf((x),__VA_ARGS__);   \
		Ql_SendToUart(ql_uart_port2,(u8 *)(x),Ql_strlen(x));
/*************************************************************************************************************/

extern char textBuf[100];
extern char GPRMC[];
extern char UID[];
extern char STAT[];
extern double mCurrSpeed,mTotDist;
extern unsigned char ItoaStr[];

//for battery
extern int int_battvolt;
extern int int_battperc;
bool PO_STATUS=0;

void PO_Stamp(void)
{
	char POstring[90];
	Ql_memset((ascii *)POstring,'\0',sizeof(POstring));

	Ql_strcpy(POstring,UID);
	Ql_strncat(POstring,"_PO,",7);
	Ql_strcat(POstring,GPRMC);
	ix_Itoa(mCurrSpeed);
	Ql_strcat(POstring,(char *)ItoaStr);///mCurrSpeed speed
	Ql_strncat(POstring,",",1);
	Ql_strcat(POstring,(char *)(ix_Itoa(mTotDist)));
	Ql_strncat(POstring,",",1);
	Ql_strncat(POstring,"0.0",5);		//PDOP
	Ql_strncat(POstring,",",1);
	Ql_strncat(POstring,STAT,1);			//Status (A/V)
	Ql_strncat(POstring,"\r\n",2);
	OUT_D1EBUG(textBuf,"POstring=%s:\r\n",POstring);

	tw_filewrite((char *)POstring);	
}

void PF_Stamp(void)
{
	char PFstring[90];
	Ql_memset((ascii *)PFstring,'\0',sizeof(PFstring));

	Ql_strcpy(PFstring,UID);
	Ql_strncat(PFstring,"_PF,",7);
	Ql_strcat(PFstring,GPRMC);
	ix_Itoa(mCurrSpeed);
	Ql_strcat(PFstring,(char *)ItoaStr);///mCurrSpeed speed
	Ql_strncat(PFstring,",",1);
	Ql_strcat(PFstring,(char *)(ix_Itoa(mTotDist)));
	Ql_strncat(PFstring,",",1);
	Ql_strncat(PFstring,"0.0",5);		//PDOP
	Ql_strncat(PFstring,",",1);
	Ql_strncat(PFstring,STAT,1);			//Status (A/V)
	Ql_strncat(PFstring,"\r\n",2);
	OUT_D1EBUG(textBuf,"PFstring=%s:\r\n",PFstring);

	tw_filewrite((char *)PFstring);	
}


void BL_Stamp(void)
{
	char BLstring[100];
	unsigned int in_batt_vltg;
	unsigned int batt_prc;
	char battvolt[10];
	char battper[5];

	Ql_memset((ascii *)BLstring,'\0',sizeof(BLstring));
	Ql_strcpy(BLstring,UID);
	Ql_strncat(BLstring,"_BL,",7);
	Ql_strcat(BLstring,GPRMC);
	ix_Itoa(mCurrSpeed);
	Ql_strcat(BLstring,(char *)ItoaStr);///mCurrSpeed speed
	Ql_strncat(BLstring,",",1);
	Ql_strcat(BLstring,(char *)(ix_Itoa(mTotDist)));
	Ql_strncat(BLstring,",",1);
	Ql_strncat(BLstring,"0.0",5);		//PDOP
	Ql_strncat(BLstring,",",1);
	Ql_strncat(BLstring,STAT,1);//Status (A/V)
	Ql_strncat(BLstring,",",1);	
	Ql_strcat(BLstring,(char *)(ix_Itoa(int_battvolt)));
	Ql_strncat(BLstring,",",1);	
	Ql_strcat(BLstring,(char *)(ix_Itoa(int_battperc)));	
	Ql_strncat(BLstring,"\r\n",2);

	OUT_D1EBUG(textBuf,"BLstring=%s:\r\n",BLstring);
	tw_filewrite((char *)BLstring);	
}



