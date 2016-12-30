/*
 * sms.c
 * December 2016
 * Code to manage the sms structures (both text and PDU)
 *
 */
 
#include "sms.h"
#include <string.h>

/*
 * GetstatTxt()
 * December 2016
 * Returns the status in a readable way.
 */
void GetStatTxt(int st, char *dest)
{
	switch (st)
	{
		case SMS_RECUNREAD:
			strcpy(dest,SMS_RECUNREAD_TXT);
		break;
		case SMS_RECREAD:
			strcpy(dest,SMS_RECREAD_TXT);
		break;
		case SMS_STOSENT:
			strcpy(dest,SMS_STOSENT_TXT);
		break;
		case SMS_STOUNSENT:
			strcpy(dest,SMS_STOUNSENT_TXT);
		break;
		case SMS_ALL:
			strcpy(dest,SMS_ALL_TXT);
		break;
		default:
			strcpy(dest,SMS_UNKNOWN_TXT);
	}
}


/*
 * DumpSMS. 
 * December 2016
 * Prints the SMS content in a readable way
 */
void DumpSMS(FILE *f,textsms_t sms)
{
	struct tm	t;
	char   tmpbuf[120];
	char	statbuf[32];
	
	GetStatTxt(sms.m_stat,statbuf);
	fprintf(f,"%d,%d,[%s],[%s],",sms.m_index,sms.m_type,statbuf,sms.m_telf);
	t = *localtime(&sms.m_date);
	strftime(tmpbuf, 120, "%a %Y-%m-%d %H:%M:%S %Z", &t);
	fprintf(f,"%s\n", tmpbuf);
	fprintf(f,"[%s]\n",sms.m_mensaje);
}
