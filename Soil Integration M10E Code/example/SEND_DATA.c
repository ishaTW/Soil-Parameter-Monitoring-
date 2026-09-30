/*-------------------------------------------------------------------------*/
/*  File       : SEND_DATA.c                                                 
                 SEND_DATA Connection functions
-------------------------------------------------------------------------*/
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
#include "ql_fcm.h"
#include "GSM_GPRS.h"
#include "SEND_DATA.h"
#include "TCP_IP.h"
#include "MRW.h"
#include "para_read.h"
#include "Battery.h"

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
//Variables for traces
/*
#define OUT_D1EBUG(x,...)  \
    Ql_memset((x),0,100);  \
    Ql_sprintf((x),__VA_ARGS__);   \
    Ql_SendToUart(ql_uart_port1,(u8 *)(x),Ql_strlen(x));
 */
extern u8 IMEI_API[15];     // by ambika
extern unsigned char GSN;
extern int Rx_flag;         //
extern char textBuf[1000];
extern ascii uart_buffer[1000];
extern unsigned char FtoaStr[];
extern unsigned char ItoaStr[];
//Variables for GPS data
extern char STAT[2];
extern char GPRMC[90];
extern char UID[];
extern ascii SIStampString;

//Variables for data transmission to server
//char readbuffer[100] = "UI8000_SI,021011,081505,1831.4990,N,07354.4629,E,000.0,000.6,0,0.0,A\r\n";
extern char readbuffer[];
extern ascii mSpeedAscii[];
extern double mTotDist;
extern double mCurrSpeed;
//extern s32 cgreg;
extern bool Returnj; 
extern int  detected_POID;
extern u32 rev_int_databasePOID;
extern char Unit_Type[35];
extern char CEllIDinfo[150];
extern u8 CEEL_ID_flg;
extern ascii uart_buffer[1000];
extern ascii Rxbuffer[1000];          //by ambika

//char ARDdata[350]="\0";
//extern int int_battvolt;
//extern int int_battperc;
/*-------------------------------------------------------------------------*/
/*  Function   : timer_handler_stamping                                    */
/*-------------------------------------------------------------------------*/
/*  Object     : SI stamp function							               */
/*-------------------------------------------------------------------------*/

void timer_handler_stamping(void)
{
	//ascii SIStampString[300];
	ascii SIStampString[1000];
	//ascii uart_buffer[250];
	ascii Rxbuffer[250];          //by ambika
	//int battvolt1;
	//int battperc1;
	char poid[5];
	u8 chgStat;
	/*u32 cap,ret;
	u32 vol;
	u8 Pw=1;
	float battvlt=0.0;
	int int_battvolt1;
	int int_battperc1;

	ret = Ql_GetPowerSupply(&chgStat, &cap, &vol);
	int_battvolt1=vol;
	int_battperc1=cap;

	OUT_D1EBUG(textBuf,"\r\nint_battvolt1=%d\r\n",int_battvolt1);
	OUT_D1EBUG(textBuf,"\r\nint_battperc1=%d\r\n",int_battperc1);
	 */
	OUT_D1EBUG(textBuf,"in timer handler stamping\r\n");
	Ql_memset((ascii *)SIStampString,0,sizeof(SIStampString));
	//	Ql_strncpy((ascii *)SIStampString,"UI8002_SI,",10);
	/*Ql_strcpy(SIStampString,Rxbuffer);
	Ql_strncat(SIStampString,"_Data,",4);*/
	Ql_strcpy(SIStampString,UID);
	Ql_strncat(SIStampString,"_SI,",3);
    Ql_strcat(SIStampString,GPRMC);
	/*
	ix_Ftoa(mCurrSpeed,2);
	OUT_D1EBUG(textBuf,"FtoaStr%s\r\n",FtoaStr);
    Ql_strcat((char *)SIStampString,(char *)FtoaStr);///speed
    Ql_strncat((char *)SIStampString,",",1);
    Ql_strcat(SIStampString,(char *)(ix_Itoa(mTotDist)));	//Distance
    OUT_D1EBUG(textBuf,"mTotDistin si%lf\r\n",mTotDist);
	Ql_strcat(OnStampString,(ascii *)(itoa(mTotDist)));	//Distance
	 */
	//	Ql_strcat((ascii *)SIStampString,(ascii *)mSpeedAscii);
	//  ix_Ftoa(mCurrSpeed,2);
	OUT_D1EBUG(textBuf,"FtoaStr==%s\r\n",FtoaStr);
	//  Ql_strcat((char *)SIStampString,(char *)FtoaStr);///speed
	//    ix_Ftoa(mCurrSpeed,2);
	ix_Itoa(mCurrSpeed);
	//	OUT_D1EBUG(textBuf,"FtoaStrcurre%s\r\n",ItoaStr);
	Ql_strcat(SIStampString,(char *)ItoaStr);///mCurrSpeed speed

	Ql_strncat(SIStampString,",",1);
	Ql_strcat(SIStampString,(char *)(ix_Itoa(mTotDist)));
	Ql_strncat(SIStampString,",",1);
	Ql_strncat(SIStampString,"0.0",5);		//PDOP
	Ql_strncat(SIStampString,",",1);
	Ql_strncat(SIStampString,STAT,1);			//Status (A/V)


	if(detected_POID > 0)
	{

		if((Returnj == TRUE) && (rev_int_databasePOID > 0))
		{
			OUT_D1EBUG(textBuf,"IN revese direction\r\n");
			Ql_strncat(SIStampString,",",1);
			Ql_strncat(SIStampString,"$",1);
			Ql_strcat(SIStampString,(char *)(ix_Itoa(rev_int_databasePOID)));
		}
		else if(Returnj == FALSE)
		{
			Ql_strncat(SIStampString,",",1);
			Ql_strncat(SIStampString,"$",1);
			Ql_strcat(SIStampString,(char *)(ix_Itoa(detected_POID)));

		}
	}

	if((CEEL_ID_flg==1) && (!(Ql_strstr((char *)Unit_Type,"JRM") != NULL)||(Ql_strstr((char *)Unit_Type,"jrm") != NULL)))
	{
		Ql_strncat(SIStampString,",$",2);
		Ql_strcat(SIStampString,CEllIDinfo);
	}


	/// Battery Status
	Ql_strncat(SIStampString,",",1);
	//Ql_strcat(SIStampString,(char *)(ix_Itoa(int_battvolt1)));
	Ql_strncat(SIStampString,",",1);
	//Ql_strcat(SIStampString,(char *)(ix_Itoa(int_battperc1)));
	/// Battery Status

    Ql_strncat(SIStampString,",@",2);
//	Ql_strncat(SIStampString,",",1);
	OUT_D1EBUG(textBuf,"FtoaStr==%s\r\n",FtoaStr);
//	Ql_strcat(SIStampString,ARDdata);

	/*by ambika 02/08/24
	//size_t RX_flag;

	Rx_flag = Ql_strlen(Rxbuffer);

	if (Rx_flag == 0)            //by ambika
	{
		OUT_D1EBUG(textBuf,"No Rxbuffer data");
		Ql_strncat(SIStampString,"@",1);
		Ql_strncat(SIStampString,",",1);
		Ql_strncat(SIStampString,"-",1);
		Ql_strncat(SIStampString,",",1);
		Ql_strncat(SIStampString,"-",1);
	}

	else if(Rx_flag > 0)
	{
		OUT_D1EBUG(textBuf,"Rxbuffer data");
		Ql_strncat(SIStampString,"@",1);
		Ql_strncat(SIStampString,",",1);
		Ql_strncat(SIStampString,"#",Rxbuffer);
		Ql_strncat(SIStampString,",",1);
	}
*/

	// by ambika   02/08/24

	Ql_strcat((ascii*)SIStampString,(ascii*)uart_buffer);

	Ql_strncat(SIStampString,"\r\n",2);
	OUT_D1EBUG(textBuf,"%s\r\n",SIStampString);

	tw_filewrite((char *)SIStampString);
	OUT_D1EBUG(textBuf,"After SI Cat readbuffer=%s\r\n",SIStampString);

	OUT_D1EBUG(textBuf,"before SI Stamp=%s\r\n",uart_buffer);
	//Ql_strncat(SIStampString,"#",1);
	//Ql_strcat((ascii*)SIStampString,(ascii*)uart_buffer);

	//tw_filewrite((char *)SIStampString);
	//OUT_D1EBUG(textBuf,"%s\r\n",SIStampString);
	//Ql_strcpy((ascii *)readBuffer,(ascii *)SIStampString);//copy SI stamp from SIstampstring to readbuffer
	//OUT_D1EBUG(textBuf,"After SI Cat readbuffer=%s\r\n",SIStampString);
     Ql_memset((ascii*)uart_buffer,0,sizeof(uart_buffer));


/*  // by ambika

 Ql_strcpy(Rxbuffer,uart_buffer);
	OUT_D1EBUG(textBuf,"%s\r\n",Rxbuffer);
	Ql_strcpy(SIStampString,Rxbuffer);                        //by ambika
	Ql_strcpy(SIStampString,uart_buffer);
	Ql_strncat(SIStampString,"_Data,",5);
	OUT_D1EBUG(textBuf,"%s\r\n",SIStampString);*/


}

//-------------------------------------------------------------------------------------------------------//


