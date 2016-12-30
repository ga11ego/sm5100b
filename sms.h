/*
 * sms.h
 * December 2016
 * Basic definition of SMS structures. One for Text SMS and another for 
 * PDU SMS. 
 * The smslist's will be based on these.
 */
 
#ifndef _SMSINC

#define _SMSINC 

#include <time.h>
#include <stdio.h>

#define SMS_UNKNOWN		-1
#define SMS_RECUNREAD	0
#define SMS_RECREAD		1
#define SMS_STOUNSENT	2
#define SMS_STOSENT		3
#define SMS_ALL			4


#define SMS_RECUNREAD_TXT	"REC UNREAD"
#define SMS_RECREAD_TXT		"REC READ"
#define SMS_STOUNSENT_TXT	"STO UNSENT"
#define SMS_STOSENT_TXT		"STO SENT"
#define SMS_ALL_TXT			"ALL"
#define SMS_UNKNOWN_TXT		"UNKNOWN"

#define SMS_MAXTELF			32
#define SMS_MAXSMSTXT		1024
 
typedef struct {
	int 	m_index;
	int 	m_type;
	int		m_stat;
	char	m_telf[SMS_MAXTELF];
	time_t	m_date;
	char 	m_mensaje[SMS_MAXSMSTXT];
} textsms_t;

void DumpSMS(FILE *,textsms_t);
void GetStatTxt(int,char*);

#endif
