/*
 * sms.c
 * December 2016
 * Code to manage the sms structures (both text and PDU)
 *
 */
 
#include "sms.h"


void DumpSMS(FILE *f,textsms_t sms)
{
	struct tm	t;
	char   tmpbuf[120];
	
	fprintf(f,"%d,%d,%d,[%s],",sms.m_index,sms.m_type,sms.m_stat,sms.m_telf);
	t = *localtime(&sms.m_date);
	strftime(tmpbuf, 120, "%a %Y-%m-%d %H:%M:%S %Z", &t);
	fprintf(f,"%s\n", tmpbuf);
	fprintf(f,"[%s]\n",sms.m_mensaje);
}
