/*****************************************************************************

 *  Copyright Statement:
 *  --------------------
 *  This software is protected by Copyright and the information contained
 *  herein is confidential. The software may not be copied and the information
 *  contained herein may not be used or disclosed except with the written
 *  permission of Quectel Co., Ltd. 2013
 *
 *****************************************************************************/
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
char pfileCell[20] = "CellData.txt";
extern char textBuf[1000];
u8 CEEL_ID_flg=1;
//static CB_LocInfo callback_loc = NULL;
char CEllIDinfo[150]="\0";

s32 ATResponse_Location_handler_CellID(char* line, u32 len)
{
	char* ptr = NULL;
	char* p1 = NULL;
	char* p2 = NULL;
	char* p3 = NULL;
	char strTmp[300];
	char CellId_1[20];
	char CellId_2[20];
	char CellId_3[20];
	char CellId_4[20];
	char CellId_5[20];
	char CellId_6[20];

	u8 icnt=0,k=0;;
	//ST_LocInfo locInfo;

	//+QCELLLOC:xxx.xxx,xxx.xxx
	//ROUT_D1EBUG( textBuf,"Cell Id data=%s\r\n",line);
	//((Ql_strstr((u8 *)line,"CMTI") == NULL))
	char* head = Ql_RIL_FindString(line, len, "+QENG: 1");
	Ql_strcat(line,",");
	if(head)
	{
		//  head += 10;

		///Data of Cell Id one
		for(icnt=0;icnt<=6;icnt++)
		{
			while(*head !=',')head++;
			head++;
		}
		Ql_memset(strTmp, 0x0, sizeof(strTmp));
		Ql_strcpy(strTmp,head);
		//OUT_D1EBUG(textBuf,"****CELL ID DATA =%s***\r\n",strTmp);

		Ql_memset(CellId_1, 0x0, sizeof(CellId_1));
		for(icnt=0;icnt<=3;icnt++)
		{
			while(*head !=',')
			{
				//Ql_strcpy((char *)rTXI,(char *)ptr);
				CellId_1[k]=*head;
				head++;
				k++;
			}
			CellId_1[k]=*head;
			k++;
			head++;
			//OUT_D1EBUG(textBuf,"****CELL ID one =%s***\r\n",CellId_1);
		}
		CellId_1[k]='\0';
	//R	OUT_D1EBUG(textBuf,"****CELL ID one =%s***\r\n",CellId_1);

		///Data of Cell Id two
		for(icnt=0;icnt<=5;icnt++)
		{
			while(*head !=',')head++;
			head++;
		}
		Ql_memset(strTmp, 0x0, sizeof(strTmp));
		Ql_strcpy(strTmp,head);
		//OUT_D1EBUG(textBuf,"****CELL ID DATA 2=%s***\r\n",strTmp);
		k=0;
		Ql_memset(CellId_2, 0x0, sizeof(CellId_2));
		for(icnt=0;icnt<=3;icnt++)
		{
			while(*head !=',')
			{
				//Ql_strcpy((char *)rTXI,(char *)ptr);
				CellId_2[k]=*head;
				head++;
				k++;
			}
			CellId_2[k]=*head;
			k++;
			head++;
			//OUT_D1EBUG(textBuf,"****CELL ID one =%s***\r\n",CellId_1);
		}
		CellId_2[k]='\0';
		//R	OUT_D1EBUG(textBuf,"****CELL ID two =%s***\r\n",CellId_2);

		///Data of Cell Id three
		for(icnt=0;icnt<=5;icnt++)
		{
			while(*head !=',')head++;
			head++;
		}
		Ql_memset(strTmp, 0x0, sizeof(strTmp));
		Ql_strcpy(strTmp,head);
		//OUT_D1EBUG(textBuf,"****CELL ID DATA 3=%s***\r\n",strTmp);
		k=0;
		Ql_memset(CellId_3, 0x0, sizeof(CellId_3));
		for(icnt=0;icnt<=3;icnt++)
		{
			while(*head !=',')
			{
				//Ql_strcpy((char *)rTXI,(char *)ptr);
				CellId_3[k]=*head;
				head++;
				k++;
			}
			CellId_3[k]=*head;
			k++;
			head++;
			//OUT_D1EBUG(textBuf,"****CELL ID one =%s***\r\n",CellId_1);
		}
		CellId_3[k]='\0';
		//R	OUT_D1EBUG(textBuf,"****CELL ID Three =%s***\r\n",CellId_3);

		///Data of Cell Id Four
		for(icnt=0;icnt<=5;icnt++)
		{
			while(*head !=',')head++;
			head++;
		}
		Ql_memset(strTmp, 0x0, sizeof(strTmp));
		Ql_strcpy(strTmp,head);
		//OUT_D1EBUG(textBuf,"****CELL ID DATA 4=%s***\r\n",strTmp);
		k=0;
		Ql_memset(CellId_4, 0x0, sizeof(CellId_4));
		for(icnt=0;icnt<=3;icnt++)
		{
			while(*head !=',')
			{
				//Ql_strcpy((char *)rTXI,(char *)ptr);
				CellId_4[k]=*head;
				head++;
				k++;
			}
			CellId_4[k]=*head;
			k++;
			head++;
			//OUT_D1EBUG(textBuf,"****CELL ID one =%s***\r\n",CellId_1);
		}
		CellId_4[k]='\0';
		//R	OUT_D1EBUG(textBuf,"****CELL ID Four =%s***\r\n",CellId_4);

		///Data of Cell Id Five
		for(icnt=0;icnt<=5;icnt++)
		{
			while(*head !=',')head++;
			head++;
		}
		Ql_memset(strTmp, 0x0, sizeof(strTmp));
		Ql_strcpy(strTmp,head);
		//OUT_D1EBUG(textBuf,"****CELL ID DATA 5=%s***\r\n",strTmp);
		k=0;
		Ql_memset(CellId_5, 0x0, sizeof(CellId_5));
		for(icnt=0;icnt<=3;icnt++)
		{
			while(*head !=',')
			{
				//Ql_strcpy((char *)rTXI,(char *)ptr);
				CellId_5[k]=*head;
				head++;
				k++;
			}
			CellId_5[k]=*head;
			k++;
			head++;
			//OUT_D1EBUG(textBuf,"****CELL ID one =%s***\r\n",CellId_1);
		}
		CellId_5[k]='\0';
		//R	OUT_D1EBUG(textBuf,"****CELL ID Five =%s***\r\n",CellId_5);

		///Data of Cell Id six
		for(icnt=0;icnt<=5;icnt++)
		{
			//while((*head !=',') || (*head !='\0'))head++;
			while(*head !=',')head++;
			head++;
		}
		Ql_memset(strTmp, 0x0, sizeof(strTmp));
		Ql_strcpy(strTmp,head);
		//OUT_D1EBUG(textBuf,"****CELL ID DATA 6=%s***\r\n",strTmp);
		k=0;
		Ql_memset(CellId_6, 0x0, sizeof(CellId_6));
		for(icnt=0;icnt<=3;icnt++)
		{
			while(*head !=',')
			{
				//Ql_strcpy((char *)rTXI,(char *)ptr);
				CellId_6[k]=*head;
				head++;
				k++;
			}
			CellId_6[k]=*head;
			k++;
			head++;
			//OUT_D1EBUG(textBuf,"****CELL ID one =%s***\r\n",CellId_1);
		}
		CellId_6[(k-3)]='\0';
		//R	OUT_D1EBUG(textBuf,"****CELL ID Six =%s***\r\n",CellId_6);

		Ql_memset(CEllIDinfo, 0x0, sizeof(CEllIDinfo));
		Ql_strcat(CEllIDinfo,"C1,");
		Ql_strcat(CEllIDinfo,CellId_1);
		Ql_strcat(CEllIDinfo,"C2,");
		Ql_strcat(CEllIDinfo,CellId_2);
		Ql_strcat(CEllIDinfo,"C3,");
		Ql_strcat(CEllIDinfo,CellId_3);
		Ql_strcat(CEllIDinfo,"C4,");
		Ql_strcat(CEllIDinfo,CellId_4);
		Ql_strcat(CEllIDinfo,"C5,");
		Ql_strcat(CEllIDinfo,CellId_5);
		Ql_strcat(CEllIDinfo,"C6,");
		Ql_strcat(CEllIDinfo,CellId_6);

		//R	OUT_D1EBUG(textBuf,"****CELL ID Information =%s***\r\n",CEllIDinfo);

		return 0;
	}
}
/*
s32 ATResponse_CellId_Off_Handler(char* line, u32 len, void* userdata)
{
	if((Ql_strstr(line,"OK")))
	{
		return  RIL_ATRSP_SUCCESS;
	}

	if((Ql_strstr(line,"ERROR")))
	{
		return  RIL_ATRSP_FAILED;
	}

	if((Ql_strstr(line,"+CME ERROR:")))
	{
		return  RIL_ATRSP_FAILED;
	}

	return RIL_ATRSP_CONTINUE; //continue wait
}*/
void RIL_Multi_Cell_Id(void)
{
	//s32 ret=0 ;
	char strAT[100];

	Ql_memset(strAT, 0, sizeof(strAT));
	Ql_sprintf(strAT, "AT+QENG=2,1\n");
	//R	OUT_D1EBUG( textBuf,"<-- Send Cell ID AT:%s, ret = %d -->\r\n",strAT, ret);
	Ql_SendToModem(ql_md_port1, (u8*)strAT, Ql_strlen(strAT));

}
void RIL_Multi_Cell_Id_OFF(void)
{
	//s32 ret ;
	char strAT[100];

	Ql_memset(strAT, 0, sizeof(strAT));
	Ql_sprintf(strAT, "AT+QENG=0,0\n");
	//R	OUT_D1EBUG( textBuf,"<-- Send Cell ID AT:%s, ret = %d -->\r\n",strAT, ret);
	Ql_SendToModem(ql_md_port1, (u8*)strAT, Ql_strlen(strAT));

}


/*
s32 RIL_GetLocationByCell(ST_CellInfo* cell, CB_LocInfo cb_loc)
{
    s32 ret = RIL_AT_SUCCESS;
    char strAT[200];

    if(NULL != cb_loc )
    {        
        callback_loc = cb_loc;        
        Ql_memset(strAT, 0, sizeof(strAT));
        Ql_sprintf(strAT, "AT+QCELLLOC=3,%d,%d,%d,%d,%d,%d\n",cell->cellId,cell->lac,cell->mnc,cell->mcc,cell->rssi,cell->timeAd);
        ret = Ql_RIL_SendATCmd(strAT,Ql_strlen(strAT),ATResponse_Location_handler_CellID,NULL,0);
        OUT_D1EBUG( textBuf,"<-- Send AT:%s, ret = %d -->\r\n",strAT, ret);

        if (RIL_AT_SUCCESS != ret)
        {
            return ret;
        }
    }
    return ret;
}*/

void wrt_Cell_ID_flg(u8 CellBKFlg)     ///23
{
	s32 ret1,filehandle;
	char *ptr=NULL;
	u32 writeedlen,wcnt;
	ret1 =Ql_FileOpenEx((char*)pfileCell,QL_FS_CREATE);
	wcnt=1;
	OUT_D1EBUG(textBuf,"in Cell ID flag writing =%d,\r\n",CellBKFlg);
	ptr=ix_Itoa(CellBKFlg);// 23

	if(ret1 >= QL_RET_OK)
	{
		filehandle = ret1;
		ret1 = Ql_FileSeek(filehandle,wcnt,QL_FS_FILE_BEGIN);
		ret1 = Ql_FileWrite(filehandle, (u8*)ptr,10,&writeedlen);                                   // Memory Location = 119 to 131
		OUT_D1EBUG(textBuf,"\r\n Ql_FileWritewrt_cnt()=%d: writeedlen=%d\r\n",ret1, writeedlen);
		OUT_D1EBUG(textBuf,"wrt Cell ID flag =%s,     ret = %d\r\n",ptr,ret1);

		Ql_FileClose(filehandle);
		filehandle = -1;
	}
	else
	{
		OUT_D1EBUG(textBuf,"error in Cell ID flag number write\r\n");
	}
}



void read_Cell_ID_flg(void)
{

	s32 ret;
	u32 writeedlen,readedlen1,wcnt;
	u8 k;
	s32 filehandle=-1;
	char CanPara_readbuffer[50]="\0";

	ret = Ql_FileOpenEx((char*)pfileCell,QL_FS_CREATE);
	wcnt=1;
	if(ret >= QL_RET_OK)
	{
		filehandle = ret;
		Ql_memset(CanPara_readbuffer,'\0',sizeof(CanPara_readbuffer));
		ret =Ql_FileSeek(filehandle,wcnt, QL_FS_FILE_BEGIN);							///Mem loc 1 from this file  use next from 100
		ret = Ql_FileRead(filehandle, (u8 *)CanPara_readbuffer,10, &readedlen1);
		//OUT_D1EBUG(textBuf,"Ql_FileRead() = %d: readedlenfl = %d,,,,%s\r\n",ret,readedlen1,CanPara_readbuffer);

		if(!(Ql_strncmp(CanPara_readbuffer,"\0",1)))
		{
			OUT_D1EBUG(textBuf,"\tCELL ID flag Buffer empty\r\n");
			Ql_strcpy(CanPara_readbuffer,(char *)"0");
		}

		CEEL_ID_flg=Ql_atoi(CanPara_readbuffer);
		OUT_D1EBUG(textBuf,"\tCELL ID flag Buffer \t\t= %d\r\n",CEEL_ID_flg);

		Ql_FileClose(filehandle);
		filehandle = -1;
	}
}
