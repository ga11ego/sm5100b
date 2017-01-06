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

int GetStatInt(const char *tmpbuf)
{
	if ( strcmp(tmpbuf,SMS_RECREAD_TXT) == 0 )
		return SMS_RECREAD;
	else
		if ( strcmp(tmpbuf,SMS_RECUNREAD_TXT) == 0 )
			 return SMS_RECUNREAD;
		else 
			if ( strcmp(tmpbuf,SMS_STOSENT_TXT) == 0 )
				return SMS_STOSENT;
			else 
				if ( strcmp(tmpbuf,SMS_STOUNSENT_TXT) == 0 )
					return SMS_STOUNSENT;
				else
					if ( strcmp(tmpbuf,SMS_ALL_TXT) == 0 )
						return SMS_ALL;
					else
						return SMS_UNKNOWN;
}

/*
 * DumpSMS. 
 * December 2016
 * Prints the SMS content in a readable way
 */
void DumpTextSMS(FILE *f,textsms_t sms)
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

/* 
 * GetTextSMSDate()
 * Enero 2017
 * Retorna la fecha en un solo time_t
 */
time_t GetTextSMSDate(textsms_t sms)
{
	return sms.m_date;
}

/* 
 * GetTextSMSDate()
 * Enero 2017
 * Retorna el tipo de codificación
 */
int GetTextSMSType(textsms_t sms)
{
	return sms.m_type;
}


/*
 * GetTextSMSDateString()
 * Enero 2017
 * Retorna la cadena de la fecha.
 */
void GetTextSMSDateString(char *buff,textsms_t sms)
{
	struct tm	t;
	t = *localtime(&sms.m_date);
	strftime(buff, 120, "%d/%m/%Y %H:%M:%S", &t);
}


/* 
 * GetTextSMSPhone()
 * Enero 2017
 * Retorna el teléfono remitente.
 */
void GetTextSMSPhone(char *buff,textsms_t sms)
{
	strcpy(buff,sms.m_telf);
}


/* 
 * GetTextSMSMessageRaw()
 * Enero 2017
 * Retorna el mensaje sin decodificar.
 */
void GetTextSMSMessageRaw(char *buff, textsms_t sms)
{
	strcpy(buff,sms.m_mensaje);
}

/* 
 * GetTextSMSMessage()
 * Enero 2017
 * Retorna el texto del mensaje. Si es de tipo 2 (unicode) lo transforma
 * a ascii.
 */
void GetTextSMSMessage(char *buff,textsms_t sms)
{
	if ( sms.m_type == SMS_TYPE_DEFAULT || sms.m_type == SMS_TYPE_8BIT )
	{
		strcpy(buff,sms.m_mensaje);
		return;
	}
	// Vamos a decodificar. Vamos leyendo los grupos de caracteres de 
	// 4 en 4.
	
	long int li;
	int 	l=strlen(sms.m_mensaje);
	int     c,i,k;
	char 	tmpbuff[5];
	
	memset(tmpbuff,'\0',5);
	
	for ( c=0, k=0; c<(l-4) ; c+=4 )
	{
		for ( i =0 ; i<4 ; i++ )
			tmpbuff[i]=sms.m_mensaje[i+c];
		li=strtol(tmpbuff,NULL,16);
		//printf("%s -> %d -> %c\n",tmpbuff,li,li);
		buff[k++]=li;
	}	
	
}

/* GetTextSMSHeader()
 * Jan 2017
 * Compiles a header of the SMS in a single buffer. Index, phone and date.
 */
void GetTextSMSHeader(char *buff,textsms_t sms)
{
	char	tmpbuff[1024];
	
	GetTextSMSDateString(tmpbuff,sms);
	sprintf(buff,"%d %s %s",sms.m_index, sms.m_telf, tmpbuff);
}
