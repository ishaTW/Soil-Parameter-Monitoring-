//#ifdef __EXAMPLE_SMS__
 /***************************************************************************************************
 *	 Example:
 *		 
 *			 SMS Routine
 *
 *	 Description:
 *
 *			 This example demonstrates how to use sms function with APIs in OpenCPU.
 *			 Through MAIN Uart port, input the specified command, and the response message will be 
 *			 printed out through MAIN port.
 *
 *	 Usage:
 *
 *			 Compile & Run:
 *
 *				 Use "make SMS" to compile, and download bin image to module to run.
 *			 
 *			 Operation: (Through MAIN port)
 *                   If input "Ql_SetSMSStorage=(0-3)", that will set SMS storages.
 *                   If input "Ql_GetSMSStorageInfo", that will get SMS storages info.
 *                   If input "Ql_SetNewSMSDirectToTE=(0,1)", that will get SMS storages info.
 *                   If input "Ql_SetInfoCentreNum="<sca number>"", that will set SMS service centre number.
 *                   If input "Ql_GetInfoCentreNum", that will get SMS service centre number.
 *                   If input "Ql_SetNewSMSDirectToTE=(0,1)", that will choose new messages save in storage or not. 
 *                   If input "Ql_SetSMSFormat=(0,1)", that will set the format of SMS, PDU or text.
 *                   If input "Ql_ReadSMS=(1,300),(0,1)", that will read an indexed message.
 *                   If input "Ql_DeleteSMS=(1,300),(0,4)", that will delete SMS messages in current storage.
 *                   If input "Ql_ListSMS=(0,4)", that will get message list in current storage by "QlSMSStatus".
 *                   If input "Ql_GetUnReadSMS=(0,1)", that will read first unread message.
 *                   If input "Ql_SendPDUSMS", that will send a message with PDU format.
 *                   If input "Ql_SetTextSMSPara=(0-2),(0,1),(0,1)", that will set some parameters in text mode.
 *                   If input "Ql_SendTextSMS="<phone number>"", that will send a message with text format.
 *			 
 ****************************************************************************************************/


/*-------------------------------------------------------------------------*/
/*  File       : sms_handle.c
                 
                 16-Jun-2012
                 Remote parameter change
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/*								Headers									   */
/*-------------------------------------------------------------------------*/


#include<stdio.h>
#include<string.h>
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
#include "Ql_error.h"
#include "Ql_sms.h"
#include "ql_fcm.h"
#include "ql_api_type.h"
//#include "Fun.h"
//#include "GPS.h"
//#include "GSM_GPRS.h"
//#include "SEND_DATA.h"
//#include "TCP_IP.h"
//#include "MRW.h"  
//#include "para_read.h" 
#include "GPRMC.h" 
#include "Ql_filesystem.h"
#include "sms_handle.h" 
#include "fota.h" 

/*-------------------------------------------------------------------------*/
/*								Globals									   */
/*-------------------------------------------------------------------------*/
	
#define MAX_LIST_NODE_COUNT     100

#define DEBUG_BUFFER_SIZE   100

#define OUT_D1EBUG(x,...)  \
    Ql_memset((void*)(x),0,DEBUG_BUFFER_SIZE);  \
    Ql_sprintf((char*)(x),__VA_ARGS__);   \
    Ql_SendToUart(ql_uart_port2,(u8*)(x),Ql_strlen((const char*)(x)));
/*	

#define OUT_D1EBUG(x,...)  \
    Ql_memset((x),0,100);  \
    Ql_sprintf((x),__VA_ARGS__);   \
    Ql_SendToUart(ql_uart_port2,(u8 *)(x),Ql_strlen(x));

*/
void CallBack_NewSMS(u16 index,QlSMSStorage storage);
void CallBack_SendSMS(bool result, s16 cause, u8 msg_ref);
void CallBack_DeleteSMS(bool result, s16 cause,u16 index);
void CallBack_ReadPDUSMS(bool result, s16 cause,u16 index, u8 status, u8* data, u16 length);
void CallBack_NewFlashPDUSMS(u16 length, u8* pdu_string);
void CallBack_PDUStatusReport(u16 length, u8* pdu_string);
void CallBack_ReadTextSMS(bool result, s16 cause,u16 index, u8 status, QlSMSTextMsg* sms);
void CallBack_NewFlashTextSMS(QlSMSTextMsg* sms);
void CallBack_TextStatusReport(u8 fo,u8 msg_ref, u8* phone_num, QlSysTimer* scts, QlSysTimer* dt, u8 st);
char debug_buffer[DEBUG_BUFFER_SIZE];
extern char textBuf[];

char buffer1[250];
char sms_buffer[200];
char sms_set[25];
extern char UID[8];
extern char TXI[8];
extern char STI[8];
extern u32 filehandle;
extern char pfile2[15];
u16 sms_index=0;
extern u8 Fota_Flag;
u8 Fota_http_Flag=0;
extern PortData_Event* pPortEvt;
bool camdumpflag=0;
extern bool camcaptureflag;
extern int camindicatorflag;

void CallBack_NewSMS(u16 index,QlSMSStorage storage)
{
    s16 ret;
    OUT_D1EBUG(debug_buffer,"\r\nCB_NewSMS: index=%d,storage=%d\r\n",index,storage);
    // you can read it right now
	ret=Ql_SetSMSFormat(1);
    OUT_D1EBUG(debug_buffer,"\r\nQl_SetSMSFormat()=%d\r\n",ret);
    ret=Ql_ReadSMS(index,0);
    OUT_D1EBUG(debug_buffer,"\r\nQl_ReadSMS(index=%d,mode=%d)=%d\r\n",index,0,ret);

}

void CallBack_SendSMS(bool result, s16 cause, u8 msg_ref)
{
    OUT_D1EBUG(debug_buffer,"\r\nCB_SendSMS: result=%d,cause=%d,msg_ref=%d\r\n",result,cause,msg_ref);
}

void  CallBack_DeleteSMS(bool result, s16 cause,u16 index)
{
    OUT_D1EBUG(debug_buffer,"\r\nCB_DeleteSMS: result=%d,cause=%d,index=%d\r\n",result,cause,index);
}

void CallBack_ReadPDUSMS(bool result, s16 cause,u16 index, u8 status, u8* data, u16 length)
{
    OUT_D1EBUG(debug_buffer,"\r\nCB_ReadPDUSMS: result=%d,cause=%d,index=%d,status=%d,length=%d\r\n",
                                                    result,cause,index,status,length);
    if(result)
    {
        if(length)
        {
            int i;
            for(i=0;i<length;i++)
            {
                OUT_D1EBUG(debug_buffer,"%02X",data[i]);
            }
            OUT_D1EBUG(debug_buffer,"\r\n");
        }
    }
}

void CallBack_NewFlashPDUSMS(u16 length, u8* pdu_string)
{
    OUT_D1EBUG(debug_buffer,"\r\nCB_NewFlashPDUSMS: length=%d\r\n",length);
    
    if(pdu_string)
    {
        int i;
        for(i=0;i<length;i++)
        {
            OUT_D1EBUG(debug_buffer,"%02X",pdu_string[i]);
        }
        OUT_D1EBUG(debug_buffer,"\r\n");
    }
}

void CallBack_PDUStatusReport(u16 length, u8* pdu_string)
{
    OUT_D1EBUG(debug_buffer,"\r\nCB_PDUStatusReport: length=%d\r\n",length);    
    if(length)
    {
        int i;
        for(i=0;i<length;i++)
        {
            OUT_D1EBUG(debug_buffer,"%02X",pdu_string[i]);
        }
        OUT_D1EBUG(debug_buffer,"\r\n");
    }
}

void CallBack_ReadTextSMS(bool result, s16 cause,u16 index, u8 status, QlSMSTextMsg* sms)
{
    OUT_D1EBUG(debug_buffer,"\r\nCB_ReadTextSMS: result=%d,cause=%d,index=%d,status=%d\r\n",
                                                    result,cause,index,status);
    if(result)
    {
        if(sms)
        {
            int i;
            OUT_D1EBUG(debug_buffer,"phone_num=\"%s\",num_type=%d,chset=%d\r\n",
                                                        sms->phone_num,sms->num_type,sms->chset);
            if(sms->scts.year != 0)
            {
                OUT_D1EBUG(debug_buffer,"scts=20%d-%d-%d %d:%d:%d\r\n",
                                                            sms->scts.year,sms->scts.month,sms->scts.day,sms->scts.hour,sms->scts.minute,sms->scts.second);     
            }
            
            if(sms->udh_len)
            {
                for(i=0;i<sms->udh_len;i++)
                {
                    OUT_D1EBUG(debug_buffer,"%02X", sms->data_len);
                }
                OUT_D1EBUG(debug_buffer,"\r\n");
            }
            
            if(sms->data)
            {
                s16 ret;
                Ql_SMS_DCS_ToCSCS(sms,CSCS_CHSET_GSM);
                OUT_D1EBUG(debug_buffer,"CallBack_ReadTextSMS data_len=%d\r",sms->data_len);
                OUT_D1EBUG(debug_buffer,"\nCallBack_ReadTextSMS data=%s\r",sms->data);
                
                read_sms(sms->data,sms->data_len);
                
                for(i=0;i<sms->data_len;i++)
                {
                //OUT_D1EBUG(debug_buffer,"sms data:\r\n");
                    if(QL_SMS_CHSET_GSM == sms->chset)
                    {
                        OUT_D1EBUG(debug_buffer,"%c",(sms->data)[i]);
                    }
                    else
                    {
                        OUT_D1EBUG(debug_buffer,"%02X",(sms->data)[i]);
                    }
                }   
                OUT_D1EBUG(debug_buffer,"\r\n");
//you can send message right now
/* 
                ret = Ql_SendTextSMS(sms->phone_num, sms->data_len, sms->data);
                OUT_D1EBUG(debug_buffer, "\r\nQl_SendTextSMS(phone_num=\"%s\")=%d\r\n",sms->phone_num,ret);
*/
            }
        }
    }
}

void CallBack_NewFlashTextSMS(QlSMSTextMsg* sms)
{
    if(sms)
    {
        int i;
        OUT_D1EBUG(debug_buffer,"\r\nCB_NewFlashTextSMS: phone_num=%s,num_type=%d,chset=%d\r\n",
                                                    sms->phone_num,sms->num_type,sms->chset);
        OUT_D1EBUG(debug_buffer,"scts=20%d-%d-%d  %d:%d:%d\r\n",
                                                    sms->scts.year,sms->scts.month,sms->scts.day,sms->scts.hour,sms->scts.minute,sms->scts.second);        
        if(sms->udh)
        {
            for(i=0;i<sms->udh_len;i++)
            {
                OUT_D1EBUG(debug_buffer,"%02X",(sms->udh)[i]);
            }
            OUT_D1EBUG(debug_buffer,"\r\n");
        }
        
        if(sms->data)
        {
            for(i=0;i<sms->data_len;i++)
            {
                if(QL_SMS_CHSET_GSM == sms->chset)
                {
                    OUT_D1EBUG(debug_buffer,"%c",(sms->data)[i]);
                }
                else
                {
                    OUT_D1EBUG(debug_buffer,"%02X",(sms->data)[i]);
                }
            }   
            OUT_D1EBUG(debug_buffer,"\r\n");
        }
    }
}

void CallBack_TextStatusReport(u8 fo,u8 msg_ref, u8* phone_num, QlSysTimer* scts, QlSysTimer* dt, u8 st)
{
    OUT_D1EBUG(debug_buffer,"\r\nCB_TextStatusReport: fo=%d,msg_ref=%d,phone_num=%s,st=%d\r\n",
                                                fo,msg_ref,phone_num,st);
    OUT_D1EBUG(debug_buffer,"scts=20%d-%d-%d %d:%d:%d\r\n",
                                                scts->year,scts->month,scts->day,scts->hour,scts->minute,scts->second);
    OUT_D1EBUG(debug_buffer,"dt=20%d-%d-%d %d:%d:%d\r\n",
                                                scts->year,scts->month,scts->day,scts->hour,scts->minute,scts->second);
}

void read_sms(char* buffer, u16 smslength)
{
    char *ptr;
//    char *ptr1;
	s32 ret;
    u8 k;
	k=0;

    if (smslength==0)
    {
//	    OUT_D1EBUG(textBuf,"Callback:smslength==0\r\n%s\r\n", buffer);
	    OUT_D1EBUG(textBuf,"Callback:smslength==0\r\n%s\r\n", buffer);
	    //Ql_SMSUnInitialize(); 
        return;
	}
    
    if (buffer != NULL)
    {
	    	OUT_D1EBUG(textBuf,"Callback:\r\n%s\r\n", buffer);	    
//   		Ql_strcpy((char *)sms_buffer,(char *)buffer);
//	    	OUT_D1EBUG(textBuf,"sms_buffer:\r\n%s\r\n",sms_buffer);
	    	ptr=Ql_strstr((char *)buffer,(char *)UID);
// 			if(Ql_strstr((char *)sms_buffer,(char *)UID) != NULL)
//  	    OUT_D1EBUG(textBuf,"ptr:\r\n%s\r\n",ptr); 
            if(ptr != NULL)
     		{
	    		OUT_D1EBUG(textBuf,"Unit id matched\r\n");
//   		ptr1=ptr;
	    		
 	    		while(*ptr !=':')
                {
//          		Ql_strcpy((char *)sms_buffer,(char *)ptr);
					sms_set[k]=*ptr;
                    ptr++;
                    k++;
                }
	    	    OUT_D1EBUG(textBuf,"sms_set:\r\n%s\r\n",sms_set);                
	    		
		        Change_RemoteParameter(ptr);    
     	
     		}
	 		else
	 		{
//////////////////////////////////////////////////////////////////////////////////////////////
///////----------------------Fota App----------------------------------//////////////////////
	 		
	 		    if(Ql_strstr((char *)buffer,(char *)"#DOTA") !=NULL)
	 		    {
	 		    OUT_D1EBUG(textBuf,"Do dota\r\n");
//	 		       tw_DofotaUpgrade();	 		    
                   Fota_http_Flag=1;
                   	/*clrcam1flags();
					clrcam2flags();
					camcaptureflag=0;
					camindicatorflag=0;
					stop_acc_capt=1;
					stop_jrm_timer();*/
                   fota_function();	//fota initial function
         	     ret=Ql_DeleteSMS(sms_index,1);
	    	      OUT_D1EBUG(textBuf,"Ql_DeleteSMS=%d\r\n",ret);
  			      //Ql_SMSUnInitialize(); 



	 		    }
//////////////////////////////////////////////////////////////////////////////////////////////
///////----------------------Fota ACC DATA---------------------------------//////////////////////
	 		
	 		    /*if(Ql_strstr((char *)buffer,(char *)"#ACCD") !=NULL)
	 		    {
	 		    OUT_D1EBUG(textBuf,"SEND ACC DATA\r\n");
	 		       smtp_cmd_idx=1;
					in_accsmtp_rou=1;
					stop_acc_capt=1;
					SMTP_connect();									//fota initial function
  	    	      ret=Ql_DeleteSMS(sms_index,1);
	    	      OUT_D1EBUG(textBuf,"Ql_DeleteSMS=%d\r\n",ret);
  			      //Ql_SMSUnInitialize(); 



	 		    }*/
////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////-----------------------CAMDUMP------------------------------------////////////
				/*if(Ql_strstr((char *)buffer,(char *)"#CMDO") !=NULL)
	 		    {
	 		    OUT_D1EBUG(textBuf,"#CAMDUMP FIRED\r\n");
//	 		       tw_DofotaUpgrade();	 		    
           //        Fota_Flag=1;
  	    	      ret=Ql_DeleteSMS(sms_index,1);
	    	      OUT_D1EBUG(textBuf,"Ql_DeleteSMS=%d\r\n",ret);
	    	      stop_img_capt=1;
	    	 //     stop_img_capt2=1;
                 //     Ql_StopTimer(&timer_stuck); 
                 //    open_ftp_flg = 1;
                   //  readcount =0;
                   Ql_StopTimer(&onesectimer);
                     if(dump_cam_wrt_cnt == 0)
  						  {
  						   readcount =0;
  						   dump_cam_wrt_cnt = cam_wrt_cnt;
  						   fun_dump_cam_wrt_cnt();
  						   /////save dump_cam_wrt_cnt
  						  } 
  					//cam 2
  				//	 readcount22 =0;
                     if(dump_cam_wrt_cnt2 == 0)
  						  {
  						   readcount22 =0;
  						   dump_cam_wrt_cnt2 = cam_wrt_cnt2;
  						 //  fun_dump_cam_wrt_cnt();
  						   /////save dump_cam_wrt_cnt
  						  } 
  						  camdumpflag=1;
  						  fun_camdumpflag();
  						  cam1dump=1;
  						  fun_cam1dump();
  						 // cam2dump=1;
  						//  fun_cam2dump();
  						 
  						//   tw_fileread2();	  
  						 //  tw_fileread();
  			   //   //Ql_SMSUnInitialize(); 



	 		    }
	 			 */		

/////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////-----------------------CAM2 DUMP------------------------------------////////////
				/*if(Ql_strstr((char *)buffer,(char *)"#2CMDO") !=NULL)
	 		    {
	 		    OUT_D1EBUG(textBuf,"#CAM2 DUMP FIRED\r\n");
//	 		       tw_DofotaUpgrade();	 		    
           //        Fota_Flag=1;
  	    	      ret=Ql_DeleteSMS(sms_index,1);
	    	      OUT_D1EBUG(textBuf,"Ql_DeleteSMS=%d\r\n",ret);
	    	  //    stop_img_capt2=1;
                 //     Ql_StopTimer(&timer_stuck); 
                 //    open_ftp_flg = 1;
                     readcount22 =0;
                     if(dump_cam_wrt_cnt2 == 0)
  						  {
  						   readcount22 =0;
  						   dump_cam_wrt_cnt2 = cam_wrt_cnt2;
  						 //  fun_dump_cam_wrt_cnt();
  						   /////save dump_cam_wrt_cnt
  						  } 
  						  
  						   tw_fileread2();
  			   //   //Ql_SMSUnInitialize(); 



	 		    }*/
	 			 		

/////////////////////////////////////////////////////////////////////////////////////////////
                else
                {
	    		  OUT_D1EBUG(textBuf,"do nothing......exit\r\n");	
  	    	      ret=Ql_DeleteSMS(sms_index,1);
	    	      OUT_D1EBUG(textBuf,"Ql_DeleteSMS=%d\r\n",ret);
  			      //Ql_SMSUnInitialize(); 
	    		  return;
	    		}
	 		}	    	    
	    	
	}
	else
	{

	    OUT_D1EBUG(textBuf,"Callback: No content.\r\n");
	    //Ql_SMSUnInitialize(); 
	}
}


void Change_RemoteParameter(char *ptr)
{       
//   if(!(Ql_strcmp((char *)sms_set,(char *)(Ql_strncat((char *)UID,(char *)"\0",1)))))
     char *ptr1;
     char rTXI[8];
     char rSTI[8];
     char rUID[8];
     s32 ret;     
     u32 writeedlen;
     u16 CanCount;
     u8 k;

     
     ptr1=sms_set;
//   ptr1=ptr1+6;   
     while(*ptr1 !='#') ptr1++;
     OUT_D1EBUG(textBuf,"ptr:%s\r\n",ptr);      
     OUT_D1EBUG(textBuf,"ptr1:%s\r\n",ptr1); 
//   if(!(Ql_strcmp((char *)sms_set),(char *)(Ql_strncat((char *)UID,(char *)"\0",1)))))     
     if((!(Ql_strncmp((char *)sms_set,(char *)UID,Ql_strlen((char *)UID)))) && (!(Ql_strncmp((char *)ptr1,(char *)"#SET0003",8))))
     {
         
// 		OUT_D1EBUG(textBuf,"change stamping and transmission\r\n");
        ptr++;
        k=0;
        if(!(Ql_strncmp((char *)ptr,(char *)"0000",4)))
        {
    		OUT_D1EBUG(textBuf,"routine for changing tx and st\r\n");
    		ptr=ptr+5;
	        OUT_D1EBUG(textBuf,"ptr tx:%s\r\n",ptr);    		
    		while(*ptr !=',')
    		{
// 				Ql_strcpy((char *)rTXI,(char *)ptr);    		   
				rTXI[k]=*ptr;
    		    ptr++;
    		    k++;
    		}
    		rTXI[k]='\0';
  		    OUT_D1EBUG(textBuf,"rTXI:%s\r\n",rTXI);
  		    ptr++;
  		    k=0;   
  		    while(*ptr != ',') ptr++;
  		    ptr++;
  		    while(*ptr !=',')
  		    {
				rSTI[k]=*ptr;
    		    ptr++;
    		    k++; 
  		    }
  		    rSTI[k]='\0';
  		    OUT_D1EBUG(textBuf,"rSTI:%s\r\n",rSTI);  		    
		    //////////////////////////////////////////////////////////////////////////////////////////////////
		    /////-----------------------Write Remote TX in to memory-------------------------------------/////
    	    ret = Ql_FileOpenEx((u8*)pfile2,QL_FS_CREATE);

        	if(ret >= QL_RET_OK)
    	    {
		       filehandle = ret;    
			   CanCount=1+Ql_strlen((char *)UID)+Ql_strlen((char *)STI)+1;
               
//			   CanCount=2+6+3+1;
//      	   CanCount=10;
	    	   OUT_D1EBUG(textBuf,"CanCountTXI=%d\r\n",CanCount);	    					
//			   ret = Ql_FileSeek(filehandle,CanCount,QL_FS_FILE_BEGIN);
			   ret = Ql_FileSeek(filehandle,9,QL_FS_FILE_BEGIN);
//	    	   ret = Ql_FileWrite(filehandle,(u8 *)TXI,Ql_strlen((char *)TXI),&writeedlen);
	    	   ret = Ql_FileWrite(filehandle,(u8 *)rTXI,4,&writeedlen);
	    	   OUT_D1EBUG(textBuf,"Ql_CanTXIWrite()=%d: writeedlen=%d\r\n",ret,writeedlen);	    		

		    //////////////////////////////////////////////////////////////////////////////////////////////////
		    /////-----------------------Write Remote Stamping in to memory-------------------------------/////
  
//	    	   OUT_D1EBUG(textBuf,"Ql_strlen(UID)=%d\r\n",Ql_strlen((char *)UID));	    		
			   CanCount=1+Ql_strlen((char *)UID)+1;
//			   CanCount=2+6+1;
//             CanCount=7;
	    	   OUT_D1EBUG(textBuf,"CanCountSTI=%d\r\n",CanCount);	    					
//		 	   ret = Ql_FileSeek(filehandle,CanCount,QL_FS_FILE_BEGIN);
		 	   ret = Ql_FileSeek(filehandle,13,QL_FS_FILE_BEGIN);
//	    	   ret = Ql_FileWrite(filehandle,(u8 *)STI,Ql_strlen((char *)STI),&writeedlen);
	     	   ret = Ql_FileWrite(filehandle,(u8 *)rSTI,4,&writeedlen);
	    	   OUT_D1EBUG(textBuf,"Ql_CanSTIWrite()=%d: writeedlen=%d\r\n",ret,writeedlen);	    		



    	       Ql_FileClose(filehandle);
	    	   filehandle = -1;
//			   Ql_SMSInitialize(NULL);

	    	   ret=Ql_DeleteSMS(sms_index,1);
	    	   OUT_D1EBUG(textBuf,"Ql_DeleteSMS=%d\r\n",ret);
			   //Ql_SMSUnInitialize();	    	   	    						    	   
               delay();
           	   Ql_Reset(2);	 
            }

		    else
	    	{
			   OUT_D1EBUG(textBuf,"Error in canopara file writing remotely....\r\n");
		    }
		    
        }
        else
        {
    		OUT_D1EBUG(textBuf,"ooh wrong password.....exit\r\n");        
            return;
        }
     
     }
	 /////////////////////////////////////////////////////////////////////////////////////////////////////////////     
	 //-----------------------------Unit Reset-------------------------------------------------------------------
	 ////////////////////////////////////////////////////////////////////////////////////////////////////////////
     else if((!(Ql_strncmp((char *)sms_set,(char *)UID,Ql_strlen((char *)UID)))) && (!(Ql_strncmp((char *)ptr1,(char *)"#SET0001",8))))
     {
     
        ptr++;
//      k=0;
        if(!(Ql_strncmp((char *)ptr,(char *)"0000",4)))
        {
    	   ret=Ql_DeleteSMS(sms_index,1);
    	   OUT_D1EBUG(textBuf,"Ql_DeleteSMS=%d\r\n",ret);
		   //Ql_SMSUnInitialize();	    	   	    						    	   
           delay();            
       	   Ql_Reset(2);	 
     
        }
        else
        {
    		OUT_D1EBUG(textBuf,"ooh wrong password.....exit\r\n");        
            return;
        }
        
     
     }
	 /////////////////////////////////////////////////////////////////////////////////////////////////////////////     
	 //-----------------------------Unit Id-------------------------------------------------------------------
	 ////////////////////////////////////////////////////////////////////////////////////////////////////////////
     
//     unit id 
     else if((!(Ql_strncmp((char *)sms_set,(char *)UID,Ql_strlen((char *)UID)))) && (!(Ql_strncmp((char *)ptr1,(char *)"#SET0004",8))))
     {

        ptr++;
        k=0;
        if(!(Ql_strncmp((char *)ptr,(char *)"0000",4)))
        {
    		OUT_D1EBUG(textBuf,"routine for changing Unit id\r\n");
    		ptr=ptr+5;
	        OUT_D1EBUG(textBuf,"ptr Unit id:%s\r\n",ptr);    		
    		while(*ptr !=',')
    		{
// 				Ql_strcpy((char *)rTXI,(char *)ptr);    		   
				rUID[k]=*ptr;
    		    ptr++;
    		    k++;
    		}
    		rUID[k]='\0';
  		    OUT_D1EBUG(textBuf,"rUID:%s\r\n",rUID);
  		    ptr++;
  		    k=0;   

 //              delay();     
 //      	   Ql_Reset(2);	 
     
        }
		    //////////////////////////////////////////////////////////////////////////////////////////////////
		    /////-----------------------Write Unit Id in to memory-------------------------------/////
  

    	    ret = Ql_FileOpenEx((u8*)pfile2,QL_FS_CREATE);

        	if(ret >= QL_RET_OK)
    	    {
		       filehandle = ret;    
//			   CanCount=1+Ql_strlen((char *)UID)+Ql_strlen((char *)STI)+1;
//			   CanCount=2+6+3+1;
//      	   CanCount=10;
	    	   OUT_D1EBUG(textBuf,"CanCountTXI=%d\r\n",CanCount);	    					
			   ret = Ql_FileSeek(filehandle,3,QL_FS_FILE_BEGIN);
//	    	   ret = Ql_FileWrite(filehandle,(u8 *)TXI,Ql_strlen((char *)TXI),&writeedlen);
	    	   ret = Ql_FileWrite(filehandle,(u8 *)rUID,Ql_strlen((char *)rUID),&writeedlen);
	    	   OUT_D1EBUG(textBuf,"Ql_CanTXIWrite()=%d: writeedlen=%d\r\n",ret,writeedlen);	    		




    	       Ql_FileClose(filehandle);
	    	   filehandle = -1;
	    	   ret=Ql_DeleteSMS(sms_index,1);
	    	   OUT_D1EBUG(textBuf,"Ql_DeleteSMS=%d\r\n",ret);
			   //Ql_SMSUnInitialize();	    	   	    						    	   
               delay();       
           	   Ql_Reset(2);	 
            }

		    else
	    	{
			   OUT_D1EBUG(textBuf,"Error in canopara file writing remotely....\r\n");
		    }
		    




     }

}

//#endif //__EXAMPLE_SMS__

