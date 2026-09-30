/*-------------------------------------------------------------------------*/
/*  File       : GPS.h                                                 
                 GPS data handler header file
/*-------------------------------------------------------------------------*/

void gprsformat1(ascii smsdata[],u16 length);
//void Data_Seperation_func1(char checkdata[]);
//float tw_ReturnSec(ascii mHHMMSS[7]);
//void tw_AvgSpeed1();
//void ReadLatLong();
//double dist_lat_long(double lat1,double long1,double lat2,double long2);
//double tw_sqrt(double m);


#define OUT_DEBUG(x,...)  \
    Ql_memset((x),0,100);  \
    Ql_sprintf((x),__VA_ARGS__);   \
    Ql_SendToUart(ql_uart_port3,(x),Ql_strlen(x));
