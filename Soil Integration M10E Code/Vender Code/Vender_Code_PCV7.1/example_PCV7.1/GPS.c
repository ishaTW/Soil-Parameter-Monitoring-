
/*-----------------------------------------------------------------------
 File       : GPS.c
                 GPS Data receptions & saggregation
----------------------------------------------------------------------
----------------------------------------------------------------------
						Revision History
------------------------------------------------------------------------*/






/*-----------------------------------------------------------------------
								Headers
------------------------------------------------------------------------*/

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
/*-------------------------------------------------------------------------
							Globals
------------------------------------------------------------------------*/
//Variables for traces
#define OUT_DEBUG(x,...)  \
    Ql_memset((x),0,100);  \
    Ql_sprintf((x),__VA_ARGS__);   \
    Ql_SendToUart(ql_uart_port2,(u8 *)(x),Ql_strlen(x));
extern char textBuf[100];
//Variables for GPS data
extern char buffer[90];
extern u16 datalen;
u16 length=0;
bool mRFound = FALSE;
bool mStarFound = TRUE;
bool mSAFound = FALSE;
void *str = NULL;

ascii mGlobalGPRMC[90] = {0};
char mGlobalGPRMC1[90];
char checkdata[75] ={0};
char GPRMC[90];
extern char readbuffer[100];
int *mRMCString1 = NULL, *mRMCString2 = NULL, *mRMCString3 = NULL, *mRMCString = NULL;
char *mSearchString = NULL;

char pfile1[10]="flag.txt";
char Globalstring[90];
double mCurrSpeed=0;
double mTotDist=0;

/*-------------------------------------------------------------------------*/
/*  Function   : gpsformat		                                           */
/*-------------------------------------------------------------------------*/
/*  Object     : GPS Data Reception					                       */
/*-------------------------------------------------------------------------*/

void gprsformat1(char smsdata[],u16 length)
{
   int i,j=0,k=0;
   // OUT_DEBUG(textBuf,"in gprsformat1.\r\n");
    for(i = 0; i < length ;i++)
    {
     
       if(smsdata[i] == 'R')
       {
         checkdata[j] = 'R';
         j++;
         mRFound = TRUE;
         mStarFound = FALSE;
       }
       else if(smsdata[i] == '*')
       {
          if( mRFound == TRUE && mStarFound == FALSE )
          {
          checkdata[j] = '*';
          j++;
          checkdata[j]='\0';
	//	  Ql_strcpy((ascii *)mGlobalGPRMC,(ascii *)mRMCString);
//		 OUT_DEBUG(textBuf,"checkdata1=%s\r\n",checkdata);
		   //OUT_DEBUG(textBuf,"String1=%c\r\n",checkdata[2]);	
		  // OUT_DEBUG(textBuf,"String1=%c\r\n",checkdata[3]);
								
	    	//Data saggregation function
		  mRFound = FALSE;
          mStarFound = TRUE;	
		  //Data_Seperation_func1(checkdata);
								
		   //Distance calculation function
				
		//  tw_AvgSpeed();
		//  tw_AvgSpeed1();
				
		  //ON stamp function
		  //fn_save();	
		  //fn_save1();			
		  mRFound = FALSE;
		  mStarFound = TRUE;
          }
          else
          {
          }
       }
      else if(mRFound == TRUE && mStarFound == FALSE)
      {
          if(smsdata[i] == '$' || smsdata[i] == 'G' || smsdata[i] == 'L' || smsdata[i] == 'P' || smsdata[i] == 'Z')
          {
             while(k < i)
             {
               checkdata[k] = '\0';
               k++;
             }
          
          }      
          else
          {
             checkdata[j] = smsdata[i];
             j++;
          
          }
      }
    
    }
    
    
 
    


}






