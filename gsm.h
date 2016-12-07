/* 
 * gsm.h
 * 
 * Agosto 2016
 * Interface para leer cosas GSM. Es un interface funcional a los comandos AT.
 * Solo está implementado lo que me interesa ;-)
 * 
 */
 
#ifndef _GSMINC
#define _GSMINC

#define CPIN_ERROR 0
#define CPIN_READY 1
#define CPIN_SIMPIN 2
#define CPIN_SIMPUK 3
#define CPIN_SIMPIN2 4
#define CPIN_SIMPUK2 5
#define CPIN_BLOCK 6

#define SMSMODE_ERROR 0
#define SMSMODE_PDU 1
#define SMSMODE_TEXT 2

int SendAT(int);

int GetCSQBer(int);
int GetCSQRSSI(int);
int GetCPIN(int);
int SetCPIN(const char *,int);
int GetCCID(char *,int,int);
int GetIMEI(char *,int,int);











#endif
