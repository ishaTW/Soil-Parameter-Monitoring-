/*---------------------------------------------------------------------
 *  This example establish a transparent transmission tunnel between
 *  UART PORT 1 and UART PORT 2.
 *--------------------------------------------------------------------*/
#ifdef __CUSTOMER_CODE__
#include "ql_type.h"
#include "ql_appinit.h"
#include "ql_trace.h"
#include "ql_interface.h"
#include "ql_fcm.h"
#include "ql_stdlib.h"


QlEventBuffer g_event;

QlPort g_src_port = ql_uart_port2;
QlPort g_des_port = ql_uart_port1;
static u32 g_counter = 0;
static char s_strTraceMsg[100];

void uart_init(s32 rate)
{ 
    Ql_SetPortOwner(g_src_port,ql_main_task);
    Ql_SetPortOwner(g_des_port,ql_main_task);
    Ql_SetUartBaudRate(g_src_port, rate);
    Ql_SetUartBaudRate(g_des_port, rate);
	//Ql_SetUartDCBConfig(g_src_port, rate, 8, sb_One, pb_none);
	//Ql_SetUartDCBConfig(g_des_port, rate, 8, sb_One, pb_none);
}
void ql_entry(void)
{
    s32 ret;
	s32 len;
    Ql_SetDebugMode(BASIC_MODE);
    Ql_DebugTrace("OpenCPU: transparently transfer data from uart2 to uart1\r\n");
    
    uart_init(9600);
    
    while (TRUE)
    {
        Ql_GetEvent(&g_event);
        switch(g_event.eventType)
        {
            case EVENT_UARTDATA:
            {
                // TODO: receive and handle data from UART
                QlPort destPort = ql_max_port;
                PortData_Event* pDataEvt = (PortData_Event*)&g_event.eventData.uartdata_evt;
                //Ql_DebugTrace("EVENT_UARTDATA port:%d\r\n",pDataEvt->port);
                
                // Establish transparent transmission tunnel between UART1 and UART2
                if (g_src_port == pDataEvt->port || g_des_port == pDataEvt->port)
                {
                    if (g_src_port == pDataEvt->port)
                    {
                        destPort = g_des_port;
                    }else{
                        destPort = g_src_port;
                    }
                    ret = Ql_SendToUart(destPort, (u8*)pDataEvt->data, pDataEvt->len);

					Ql_memset(s_strTraceMsg, 0x0, sizeof(s_strTraceMsg));
                    if (ret == pDataEvt->len)
                    {
                        // Finish.
                        //Ql_DebugTrace("<-- Data package[%d] Ok, [len=%d] -->\r\n", ++g_counter, pDataEvt->len);
						len = Ql_sprintf(s_strTraceMsg, "<-- Data package[%d] Ok, [len=%d] -->\r\n", ++g_counter, pDataEvt->len);
						Ql_SendToUart(destPort, s_strTraceMsg, len);
                    } 
                    else if (ret > 0 && ret < pDataEvt->len)
                    {
                        // Part of data is sent out
                        //Ql_DebugTrace("<-- Only part of data is sent out to UART[%d] -->\r\n", destPort);
						len = Ql_sprintf(s_strTraceMsg, "<-- Only part of data is sent out to UART[%d] -->\r\n", destPort);
						Ql_SendToUart(destPort, s_strTraceMsg, len);
                    }else{
                        //Error
                        //Ql_DebugTrace("<-- Fail to send data to UART[%d] -->\r\n", destPort);
						len = Ql_sprintf(s_strTraceMsg, "<-- Fail to send data to UART[%d] -->\r\n", destPort);
						Ql_SendToUart(destPort, s_strTraceMsg, len);
                    }
                }
                break;
            }
            case EVENT_MODEMDATA:
            {
                PortData_Event* pDataEvt = (PortData_Event*)&g_event.eventData.modemdata_evt;
                Ql_DebugTrace("MODEM DATA IS COMMING\r\n");
                break;
            }
            default:
                break;
        }
    }
}

#endif 
