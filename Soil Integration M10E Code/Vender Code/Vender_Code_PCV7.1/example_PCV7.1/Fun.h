/*-------------------------------------------------------------------------*/
/*  File       : Fun.h                                                 
                 Fun header file
/*-------------------------------------------------------------------------*/

#include "ql_type.h"

double atofd(char *s);
int isdigita(char s);
char ix_Retchar(unsigned int a);
char *ix_Itoa(signed long num);
char *ix_Ftoa(double value,unsigned char Dec_Pos);
void ix_Strcat (char *, char *);
double tw_DM2DD(double mInput);
int ix_AtoI(char array[],int length);