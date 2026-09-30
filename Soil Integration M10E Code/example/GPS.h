/*-------------------------------------------------------------------------*/
/*  File       : GPS.h                                                 
                 GPS data handler header file
-------------------------------------------------------------------------*/

void gprsformat1(ascii smsdata[],u16 length);
void Data_Seperation_func1(u8 checkdata[]);
float tw_ReturnSec(ascii mHHMMSS[7]);
//void tw_AvgSpeed1();
void ReadLatLong(void);
double dist_lat_long(double lat1,double long1,double lat2,double long2);
double tw_sqrt(double m);
void prvRMC_store(void);

