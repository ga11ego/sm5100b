/*
 * sms.c
 * Noviembre 2016
 *
 */

#include "atstring.h" 
#include "gsm.h" 
#include "smslist.h"
#include "sms.h"

 
#include <stdarg.h>
#include <syslog.h>
#include <regex.h>
 
 /*
 * GetSMSMode()
 * Septiembre 2016
 * Saber si es PDU o TEXT
 * Formato:
 * +CMGF: 1
 * 01234567
 */
int GetSMSMode(int fd)
{
	res_t 	res;
	int		result;
	char	line0[MAXATRES];
	
	init_rest(&res);
	set_rest_cmd(&res,"CMGF?");
	if ( !SendATCommand2(fd,&res) )
	{
		syslog(LOG_LOCAL7,"GetSMSMode: SendAT Failed");
		free_rest(&res);
		return SMSMODE_ERROR;
	}
	if ( !is_at_ok(res) )
	{
		syslog(LOG_LOCAL7,"CMGF? failed");
		SyslogATOutput(res);
		free_rest(&res);
		return SMSMODE_ERROR;
	}
	
	if ( !getline_rest(res,0,line0,MAXATRES) )
		return 0;
	if ( strlen(line0) != 8 )
	{
		syslog(LOG_LOCAL7,"CMGF? failed: Bad format");
		SyslogATOutput(res);
		free_rest(&res);
		return SMSMODE_ERROR;
	}
	switch ( line0[7] )
	{
		case '0':
			result=SMSMODE_PDU;
		break;
		case '1': 
			result=SMSMODE_TEXT;
		break;
	}
	free_rest(&res);
	return result; 
}


/* 
 * SetSMSMode(int,int)
 * Noviembre 2016
 * Para fijar si PDU o TEXT. 
 * Formato:
 * +CMGF=(1|0)
 * Para evitar errores, usamos SMSMODE_PDU y SMSMODE_TEXT
 */
int SetSMSMode(int fd,int mode)
{
	res_t 	res;
	char 	comp[128];
	int		result;
	
	init_rest(&res);
	
	sprintf(comp,"CMGF=%d",(mode==SMSMODE_PDU)?0:1);
	set_rest_cmd(&res,comp);
	if ( !SendATCommand2(fd,&res) )
	{
		syslog(LOG_LOCAL7,"SetSMSMode: SendAT Failed");
		free_rest(&res);
		return 0;
	}
	if ( !is_at_ok(res) )
	{
		syslog(LOG_LOCAL7,"CMGF= failed");
		SyslogATOutput(res);
		free_rest(&res);
		return 0;
	}
	// Todo ha ido bien.
	free_rest(&res);
	return 1;
}


 
/* GetTextSMSList()
 * Noviembre 2016
 * Ejemplos de recepcción:

------------------------------------- 
En modo texto:
+CMGL: 14,2,"REC READ","121","16/11/01,18:51:25+04"
0069006D006900740065002000640065002000760065006C006F00630069006400610064002E

En modo PDU
+CMGL: 14, 1, ,59

07914356060053F54003A121F10008611110811552402C0500037302020069006D006900740065002000640065002000760065006C006F00630069006400610064002E

---------------------------------------
En modo texto:
+CMGL: 19,0,"REC READ","22412","16/11/19,12:29:07+04"
simyo:@Mejoramos las tarifas! 3GB baja a 12,5 euros, 4GB (ahora 5GB) baja a 17euros y 10GB baja a 30 euros + info en tu e-mail o en blog.simyo.es

En modo PDU:
+CMGL: 19, 1, ,148

07914356060053F54005A12214F2000061119121927040960324010298A7DBF9B70ED82CABDFF270FB3D07B1C373103D2C4F9BC3F31068761482C46175181406C564AC1AA85C97BFE72C10ED2804A1C2E8B73C0CAA1D85299038AC0E83C2A0D8AD5C97BFE7A03C28063B0A41E2B03A0C0A83663050B92E7FCF412B50DA6D7E83CA6E10BD0E2AB7DAE1341BF40695DD2031FB7D76CDD3EDFCDB559E03
 */
int GetTextSMSList(int fd, textsmslist_t *smslist)
{
	res_t 		res;
	int			nreslines;
	regex_t		regex;
	int			reti;
	char 		msgbuf[100];
	char		tmpbuf[MAXATRES];
	char		line[MAXATRES];
	int			nl;
	textsms_t	sms;
	regmatch_t	matches[12];	//Esperamos 11
	
	/* Damos por hecho que la lista viene inicializada. */
	
	/* Ponemos el modem en modo Texto */
	if ( !SetSMSMode(fd,SMSMODE_TEXT) )
	{
		syslog(LOG_LOCAL7,"GetTextSMSList: SetSMSMode() failed.");
		FreeTextSMSList(smslist);
		return 0;
	} 
	// Ahora ejecutamos el CMGL="ALL"
	init_rest(&res);
	set_rest_cmd(&res,"CMGL=\"ALL\"");
	
	/* No podemos usar SendATCommand porque lo que vuelve es larguísimo. */
	
	if ( !SendATCommand2(fd,&res) )
	{
		syslog(LOG_LOCAL7,"GetTextSMSList:SendAT2 failed");
		free_rest(&res);
		return 0;
	}
	
	// Al final de todo hay un OK.
	if ( !is_at_ok(res) )
	{
		syslog(LOG_LOCAL7,"CMGL=\"ALL\" failed");
		SyslogATOutput(res);
		free_rest(&res);
		return 0;
	}
	
	// Ahora viene la gracia de procesar todas las líneas.
	//DumpATOutput(res);
	
	/* Each SMS starts with +CMGL. We iterate the whole answer looking 
	 * for this. It ends with an OK.
	 * We iterate counting lines to make sure we don't get lost 
	 * and that we can give some sort of feedback if something fails.
	 */
	nreslines=count_rest_lines(res);
	printf("We are going to process %d lines\n",nreslines);
	// egrep regexp: ^\+CMGL: *([0-9]+),([0-9]+),"([^"]*)","([+0-9]+)","(.*)"
	reti = regcomp(&regex, "^\\+CMGL: *([0-9]+),([0-9]+),\"([^\"]*)\",\"([+0-9]+)\",\"([0-9]+)/([0-9]+)/([0-9]+),([0-9]+):([0-9]+):([0-9]+)([+-][0-9]+)\"$", REG_EXTENDED);
	if (reti) 
	{
		fprintf(stderr, "Could not compile regex\n");
		exit(1);
	}

	for(nl=0;nl<nreslines;nl++)
	{
		getline_rest(res,nl,line,MAXATRES);
		reti = regexec(&regex, line, 12, matches, 0);
		if (!reti) 
		{
			// Tenemos un match. Vamos a por los campos. 
			
			int k;
			memset(tmpbuf,'\0',MAXATRES);
			for(k=matches[1].rm_so;k<matches[1].rm_eo;k++)
				tmpbuf[k-matches[1].rm_so]=line[k];
			sms.m_index=atoi(tmpbuf);
			memset(tmpbuf,'\0',MAXATRES);
			for(k=matches[2].rm_so;k<matches[2].rm_eo;k++)
				tmpbuf[k-matches[2].rm_so]=line[k];
			sms.m_type=atoi(tmpbuf);
			memset(tmpbuf,'\0',MAXATRES);
			for(k=matches[3].rm_so;k<matches[3].rm_eo;k++)
				tmpbuf[k-matches[3].rm_so]=line[k];
			if ( strcmp(tmpbuf,SMS_RECREAD_TXT) == 0 )
				sms.m_stat=SMS_RECREAD;
			else
				if ( strcmp(tmpbuf,SMS_RECUNREAD_TXT) == 0 )
					sms.m_stat=SMS_RECUNREAD;
				else 
					if ( strcmp(tmpbuf,SMS_STOSENT_TXT) == 0 )
						sms.m_stat=SMS_STOSENT;
					else 
						if ( strcmp(tmpbuf,SMS_STOUNSENT_TXT) == 0 )
							sms.m_stat=SMS_STOUNSENT;
						else
							if ( strcmp(tmpbuf,SMS_ALL_TXT) == 0 )
								sms.m_stat=SMS_ALL;
							else
								sms.m_stat=SMS_UNKNOWN;
			memset(tmpbuf,'\0',MAXATRES);
			for(k=matches[4].rm_so;k<matches[4].rm_eo;k++)
				tmpbuf[k-matches[4].rm_so]=line[k];
			strncpy(sms.m_telf,tmpbuf,SMS_MAXTELF);
			
			struct tm	t;
			
			memset(tmpbuf,'\0',MAXATRES);
			for(k=matches[5].rm_so;k<matches[5].rm_eo;k++)
				tmpbuf[k-matches[5].rm_so]=line[k];
			t.tm_year=100+atoi(tmpbuf);
			memset(tmpbuf,'\0',MAXATRES);
			for(k=matches[6].rm_so;k<matches[6].rm_eo;k++)
				tmpbuf[k-matches[6].rm_so]=line[k];
			t.tm_mon=atoi(tmpbuf)-1;
			memset(tmpbuf,'\0',MAXATRES);
			for(k=matches[7].rm_so;k<matches[7].rm_eo;k++)
				tmpbuf[k-matches[7].rm_so]=line[k];
			t.tm_mday=atoi(tmpbuf);
			memset(tmpbuf,'\0',MAXATRES);
			for(k=matches[8].rm_so;k<matches[8].rm_eo;k++)
				tmpbuf[k-matches[8].rm_so]=line[k];
			t.tm_hour=atoi(tmpbuf);
			memset(tmpbuf,'\0',MAXATRES);
			for(k=matches[9].rm_so;k<matches[9].rm_eo;k++)
				tmpbuf[k-matches[9].rm_so]=line[k];
			t.tm_min=atoi(tmpbuf);
			memset(tmpbuf,'\0',MAXATRES);
			for(k=matches[10].rm_so;k<matches[10].rm_eo;k++)
				tmpbuf[k-matches[10].rm_so]=line[k];
			t.tm_sec=atoi(tmpbuf);
			t.tm_isdst=-1;
			sms.m_date=mktime(&t);
			
			// Only the message is missing.
			nl++;
			getline_rest(res,nl,line,MAXATRES);
			strncpy(sms.m_mensaje,line,SMS_MAXSMSTXT);
			
			DumpSMS(stdout,sms);
			//memset(tmpbuf,'\0',MAXATRES);
			//for(k=matches[11].rm_so;k<matches[11].rm_eo;k++)
			//	tmpbuf[k-matches[11].rm_so]=line[k];
			//printf("Offset: [%s] %d\n",tmpbuf,atoi(tmpbuf));
		} else 
			if (reti == REG_NOMATCH) 
			{
			} else 
			{
				regerror(reti, &regex, msgbuf, sizeof(msgbuf));
				fprintf(stderr, "Regex match failed: %s\n", msgbuf);
				exit(1);
			}
	}
	
	regfree(&regex);
	
	free_rest(&res);
	return 1;
}
