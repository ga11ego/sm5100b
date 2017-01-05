/*
 * sms.h
 * Noviembre 2016
 * Definiciones para trabajar con SMSs.
 */
 
 #ifndef _GSMSMSINC
 #define _GSMSMSINC

#include "smslist.h"
 
int GetSMSMode(int);
int SetSMSMode(int,int);

int GetTextSMSList(int, textsmslist_t *);
 
int PickTextSMSIndex(int,int,textsms_t *);
int PickTextSMSNotRead(int,textsmslist_t *);
int DeleteSMSIndex(int, int);
 
 #endif
