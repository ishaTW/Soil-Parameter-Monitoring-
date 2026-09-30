/*-------------------------------------------------------------------------*/
/*  File       : Fun.c                                                 
                 Basic Conversion Functions
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
#include "GSM_GPRS.h"
#include "TCP_IP.h"
#include "GPS.h"
#include "SEND_DATA.h"
#include "MRW.h"
#include "Fun.h"

/*-------------------------------------------------------------------------*/
/*								Globals									   */
/*-------------------------------------------------------------------------*/
//Variables for traces

#define OUT_DEBUG(x,...)  \
    Ql_memset((x),0,100);  \
    Ql_sprintf((x),__VA_ARGS__);   \
    Ql_SendToUart(ql_uart_port2,(u8 *)(x),Ql_strlen(x));

extern char textBuf[100];
unsigned char ItoaStr[15];
unsigned char FtoaStr[20];
/*-------------------------------------------------------------------------*/
/*  Function   : ix_Retchar				                                   */
/*-------------------------------------------------------------------------*/
/*  Object     : Return Char					               			   */
/*-------------------------------------------------------------------------*/
char ix_Retchar(unsigned int a)
{
	char ch;
	ch = 0x30 + a;
	return ch;
}
/*-------------------------------------------------------------------------*/
/*  Function   : ix_Itoa				                                   */
/*-------------------------------------------------------------------------*/
/*  Object     : Convert Interger to Ascii					               */
/*-------------------------------------------------------------------------*/
char *ix_Itoa(signed long num)
{
	char *retstr;
	unsigned char strTemp[15] = {'\0'};
	unsigned char i = 0, j = 0;

	if (num < 0)
	{
		ItoaStr[j] = 0x2D;
		j++;
		num = num * (-1);
	}
		
	if ((num >= 0) && (num <= 9))
	{
		ItoaStr[j] = ix_Retchar(num);
		j++;
	}
	else
	{
		while(1)
		{
			strTemp[i] = ix_Retchar(num % 10);
			i++;
			num = num / 10;
			if (num<10)
			{
				strTemp[i] = ix_Retchar(num);
				i++;
				do
				{
					i--;
					ItoaStr[j] = strTemp[i];
					j++;
				}while(i != 0);
				break;
			}
		}
	}
	ItoaStr[j] = '\0';
	retstr = (char *)ItoaStr;
	return retstr;
}
/*-------------------------------------------------------------------------*/
/*  Function   : isdigita				                                   */
/*-------------------------------------------------------------------------*/
/*  Object     : Convert Interger to Ascii					               */
/*-------------------------------------------------------------------------*/
int isdigita(char s)
{
	if(s>=0x30 && s<=0x39)
	{
		return 2;
	}
	else
	{
		return 0;
	}
}
/*-------------------------------------------------------------------------*/
/*  Function   : atofd					                                   */
/*-------------------------------------------------------------------------*/
/*  Object     : Convert Ascii To Double					               */
/*-------------------------------------------------------------------------*/
double atofd(char *s)
{
	double a = 0.0;
	int e = 0;
	int c;
	
    c = *s;
	for(c = *s++;c!='\0' && isdigita(c);*s++)
	{	
		a = a*10.0 + (c - '0');
		c = *s;   	 	
	}

	if(c == '.')
	{          c=*s;
		for(c = *s++;c!='\0' && isdigita(c);*s++)
		{
			a = a * 10.0 + (c - '0');
			e = e - 1;
			c = *s;
		}
	}

	if (c == 'e' || c == 'E')
	{
		int sign = 1;
		int i = 0;
		
		c = *s++;
		if (c == '+')
			c = *s++;
		else if (c == '-')
		{
			c = *s++;
			sign = -1;
		}
		 
        if((isdigita(c))>=0) 
        {
			 i = i*10 + (c - '0');
			 c = *s++;
		}
		e += i*sign;
	}
	
    for(e;e >= 0;e--);
	{
		a *= 10.0;	
	}
    
    for(e;e <= 0;e++) 
	{
		a *= 0.1;	
	}	
	return a;
}


/*-------------------------------------------------------------------------*/
/*  Function   : Ftoa					                                   */
/*-------------------------------------------------------------------------*/
/*  Object     : Convert Float To Ascii					               */
/*-------------------------------------------------------------------------*/

char *ix_Ftoa(double value,unsigned char Dec_Pos)
{
	char *retstr;
	unsigned char j = 1;
	long x  = 0;
	int i=0;
	unsigned int y = 0;
	char arr[2] = {0};
	int tot=0;
//    unsigned char FtoaStr[20];
	FtoaStr[0] = 0x00;
	if (Dec_Pos>=1 && Dec_Pos<=6)
	{
		if (value < 0)
		{
			FtoaStr[0] = 0x2D;
			value = value * (-1);
		}
		x = (long)value;
	//	OUT_DEBUG(textBuf,"x=%d\r\n",x);
		ix_Strcat ((char *)FtoaStr,(char *)ix_Itoa(x));
		value = value - x;
		arr[0] = '.';
		arr[1] = 0;
		ix_Strcat ((char *)FtoaStr,(char *)arr);
		for (j = 1; j<=Dec_Pos; j++)
		{
			value = value * 10;
			y = (unsigned int)value;
			value = value - y;
			ix_Strcat ((char *)FtoaStr,(char *)ix_Itoa(y));
		}
		arr[0] = 0;
		arr[1] = 0;
		ix_Strcat ((char *)FtoaStr,(char *)arr);
	}
	else if(Dec_Pos == 0)
	{
		if (value < 0)
		{
			FtoaStr[0] = 0x2D;
			value = value * (-1);
		}
		x = (long)value;
		ix_Strcat ((char *)FtoaStr,(char *)ix_Itoa(x));
		ix_Strcat ((char *)FtoaStr,(char *)arr);
	}
	else
	{
		return 0;
	}
	
//	OUT_DEBUG(textBuf,"FtoaStr%s\r\n",FtoaStr);

	retstr = (char *)FtoaStr;
	return retstr;
}

int ix_AtoI(char array[],int length)
{
    int i;
    int num=0,tot=0;
    for(i=0;i<length;i++)
    {
//            OUT_DEBUG(textBuf,"array[i] = %d\r\n",array[i]); 
       num=array[i]-48;
       tot=tot*10+num;
    
    }
    return tot;

}



void ix_Strcat(char *target, char *source)
{
	while(*target != '\0')
	{
		target++;
	}
	while(*source != '\0')
	{
		*target = *source;
		source++;
		target++;
	}
	*target = '\0';
}


double tw_DM2DD(double mInput)/*07353.2166*/

{
	double mOutput;
	int x = (int)mInput/100;
	mOutput = x + ((mInput - (x * 100)) / 60);
	return mOutput;
//	OUT_DEBUG(textBuf,"mOutput=%lf\r\n",mOutput);
	
}
