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
	int		atret;
	
	init_rest(&res);
	set_rest_cmd(&res,"CMGF?");
	if ( atret=SendATCommand2(fd,&res) )
	{
		fprintf(stderr,"GetSMSMode: SendAT failed with code %d\n",atret);
		free_rest(&res);
		return SMSMODE_ERROR;
	}
	if ( !is_at_ok(res) )
	{
		fprintf(stderr,"CMGF? failed\n");
		free_rest(&res);
		return SMSMODE_ERROR;
	}
	
	if ( !getline_rest(res,0,line0,MAXATRES) )
		return 0;
	if ( strlen(line0) != 8 )
	{
		fprintf(stderr,"CMGF?: failed: Bad Format\n");
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
	int		atret;
	
	init_rest(&res);
	
	sprintf(comp,"CMGF=%d",(mode==SMSMODE_PDU)?0:1);
	set_rest_cmd(&res,comp);
	if ( atret=SendATCommand2(fd,&res) )
	{
		fprintf(stderr,"SetSMSMode: SendAT failed with code %d\n",atret);
		free_rest(&res);
		return 0;
	}
	if ( !is_at_ok(res) )
	{
		fprintf(stderr,"CMGF= failed\n");
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
	int			atret;
	
	/* Damos por hecho que la lista viene inicializada. */
	
	/* Ponemos el modem en modo Texto */
	if ( !SetSMSMode(fd,SMSMODE_TEXT) )
	{
		fprintf(stderr,"GetTextSMSList: SetSMSMode() failed.\n");
		FreeTextSMSList(smslist);
		return 0;
	} 
	// Ahora ejecutamos el CMGL="ALL"
	init_rest(&res);
	set_rest_cmd(&res,"CMGL=\"ALL\"");
	
	/* No podemos usar SendATCommand porque lo que vuelve es larguísimo. */
	
	if ( atret=SendATCommand2(fd,&res) )
	{
		fprintf(stderr,"GetTextSMSList: SendAT2 failed with code %d\n",atret);
		free_rest(&res);
		return 0;
	}
	
	// Al final de todo hay un OK.
	if ( !is_at_ok(res) )
	{
		fprintf(stderr,"CMGL=\"ALL\" failed\n");
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
	// egrep regexp: ^\+CMGL: *([0-9]+),([0-9]+),"([^"]*)","([+0-9]+)","(.*)"
	reti = regcomp(&regex, "^\\+CMGL: *([0-9]+),([0-9]+),\"([^\"]*)\",\"([+0-9]+)\",\"([0-9]+)/([0-9]+)/([0-9]+),([0-9]+):([0-9]+):([0-9]+)([+-][0-9]+)\"$", REG_EXTENDED);
	if (reti) 
	{
		fprintf(stderr, "GetTextSMSList: Could not compile regex\n");
		return(0);
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
			sms.m_stat=GetStatInt(tmpbuf);
			
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
			
			/* So we have the SMS in the sms structure.
			 * Now we add it to the list.
			 */
			if ( !AddTextSMSEnd(sms,smslist) )
			{
				fprintf(stderr,"GetTextSMSList():: Error adding SMS to list\n");
				regfree(&regex);
				free_rest(&res);
				return 0;
			}
		} else 
			if (reti == REG_NOMATCH) 
			{
			} else 
			{
				regerror(reti, &regex, msgbuf, sizeof(msgbuf));
				fprintf(stderr, "Regex match failed: %s\n", msgbuf);
				regfree(&regex);
				free_rest(&res);
				return 0;
			}
	}
	regfree(&regex);
	free_rest(&res);
	return 1;
}

/*
 * PickTextSMSIndex()
 * Lee el SMS con el index indicado. No borra el mensaje del modem.
 * Se basa en AT+CMGR=index
 * Example:
 * Mensaje correcto:
 * +CMGR: "REC READ",2,"121","16/09/01,18:33:32+08"
00410020007000610072007400690072002000640065002000650073007400650020006D006F006D0065006E0074006F0020007000750065006400650073002000640069007300660072007500740061007200200064006500200074007500200062006F006E006F0020006400650020003200300030004D0042002000730069006E0020006C
 *
OK
 * Si hay error
+CMS ERROR: 321
 */
int PickTextSMSIndex(int fd,int index,textsms_t *sms)
{
	res_t	res;
	char	cmd[MAXATCMD];
	char	tmpbuff[MAXATRES];
	char 	field[MAXATRES];
	regex_t	regex;
	regmatch_t	matches[11];	//Esperamos 10
	int		i, reti, atret;
	
	
	/* Ponemos el modem en modo Texto */
	if ( !SetSMSMode(fd,SMSMODE_TEXT) )
	{
		fprintf(stderr,"PickTextSMSIndex: SetSMSMode failed\n");
		return 0;
	}
	sprintf(cmd,"CMGR=%d",index);
	init_rest(&res);
	set_rest_cmd(&res,cmd);
	if ( atret=SendATCommand2(fd,&res) )
	{
		fprintf(stderr,"PickTextSMSIndex:SendAT failed with code %d\n",atret);
		free_rest(&res);
		return 0;
	}
	
	// Hasta aquí, lo básico ha ido bien. El comando puede tener un OK (es decir, el mensaje con ese index
	// existe. O un +CMS Error.
	if ( is_cm_error(res) )
	{
		free_rest(&res);
		return 0;
	}
	// El comenaod puede haber ido bien, pero no haber dado OK.
	if ( !is_at_ok(res) )
	{
		
		free_rest(&res);
		return 0;
	}
	// Vale, tenemos un OK. El res tiene 3 líneas. Vamos a decodificarlas.
	if ( count_rest_lines(res) != 3 )
	{
		fprintf(stderr,"PickTextSMSIndex: Unexpected return from CMGR\n");
		DumpATOutput(res);
		free_rest(&res);
		return 0;
	}
	getline_rest(res,0,tmpbuff,MAXATRES);
	reti = regcomp(&regex, "^\\+CMGR: * \"([^\"]*)\",([0-9]+),\"([+0-9]+)\",\"([0-9]+)/([0-9]+)/([0-9]+),([0-9]+):([0-9]+):([0-9]+)([+-][0-9]+)\"$", REG_EXTENDED);
	if (reti) 
	{
		fprintf(stderr, "PickTextSMSIndex: Could not compile regex\n");
		free_rest(&res);
		return 0;
	}
	reti = regexec(&regex, tmpbuff, 12, matches, 0);
	if ( reti )
	{
		if (reti == REG_NOMATCH) 
			{
				free_rest(&res);
				regfree(&regex);
				fprintf(stderr,"PickTextSMSIndex: Bad message header: [%s]\n",tmpbuff);
				return 0;
			} else 
			{
				regerror(reti, &regex, tmpbuff, MAXATRES);
				fprintf(stderr, "PickTextSMSIndex: Regex match failed: %s\n", tmpbuff);
				regfree(&regex);
				free_rest(&res);
				return 0;
			}
	}
	// Tenemos match!!. 
	
	// El index es directo.
	sms->m_index=index;
	
	// El estado es el siguiente campo.
	memset(field,'\0',MAXATRES);
	for(i=matches[1].rm_so;i<matches[1].rm_eo;i++)
		field[i-matches[1].rm_so]=tmpbuff[i];
	sms->m_stat=GetStatInt(field);
	
	// Ahora vamos a por el tipo.
	memset(field,'\0',MAXATRES);
	for(i=matches[2].rm_so;i<matches[2].rm_eo;i++)
		field[i-matches[2].rm_so]=tmpbuff[i];
	sms->m_type=atoi(field);
	
	
	// El teléfono del remitente
	memset(field,'\0',MAXATRES);
	for(i=matches[3].rm_so;i<matches[3].rm_eo;i++)
		field[i-matches[3].rm_so]=tmpbuff[i];
	strncpy(sms->m_telf,field,SMS_MAXTELF);
	
	// Y ahora la fecha. Un infierno.
	struct tm	t;
			
	// Year
	memset(field,'\0',MAXATRES);
	for(i=matches[4].rm_so;i<matches[4].rm_eo;i++)
		field[i-matches[4].rm_so]=tmpbuff[i];
	t.tm_year=100+atoi(field);
			
	// Month
	memset(field,'\0',MAXATRES);
	for(i=matches[5].rm_so;i<matches[5].rm_eo;i++)
		field[i-matches[5].rm_so]=tmpbuff[i];
	t.tm_mon=atoi(field)-1;
			
	// Day
	memset(field,'\0',MAXATRES);
	for(i=matches[6].rm_so;i<matches[6].rm_eo;i++)
		field[i-matches[6].rm_so]=tmpbuff[i];
	t.tm_mday=atoi(field);
			
	// Hour
	memset(field,'\0',MAXATRES);
	for(i=matches[7].rm_so;i<matches[7].rm_eo;i++)
		field[i-matches[7].rm_so]=tmpbuff[i];
	t.tm_hour=atoi(field);
	
	// Minutes
	memset(field,'\0',MAXATRES);
	for(i=matches[8].rm_so;i<matches[8].rm_eo;i++)
		field[i-matches[8].rm_so]=tmpbuff[i];
	t.tm_min=atoi(field);
			
	// Seconds		
	memset(field,'\0',MAXATRES);
	for(i=matches[9].rm_so;i<matches[9].rm_eo;i++)
		field[i-matches[9].rm_so]=tmpbuff[i];
	t.tm_sec=atoi(field);
	t.tm_isdst=-1;
	sms->m_date=mktime(&t);
	
	// Last, but not least, the message.
	getline_rest(res,1,sms->m_mensaje,SMS_MAXSMSTXT);
	free_rest(&res);
	return 1;
}


int PickTextSMSNotRead(int fd,textsmslist_t *smslist)
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
	int			atret;
	
	/* Damos por hecho que la lista viene inicializada. */
	
	/* Ponemos el modem en modo Texto */
	if ( !SetSMSMode(fd,SMSMODE_TEXT) )
	{
		fprintf(stderr,"PickTextSMSNotRead: SetSMSMode() failed.\n");
		FreeTextSMSList(smslist);
		return 0;
	} 
	// Ahora ejecutamos el CMGL="ALL"
	init_rest(&res);
	set_rest_cmd(&res,"CMGL=\"REC UNREAD\"");
	
	/* No podemos usar SendATCommand porque lo que vuelve es larguísimo. */
	
	if ( atret=SendATCommand2(fd,&res) )
	{
		fprintf(stderr,"PickTextSMSNotRead: SendAT2 failed with code %d\n",atret);
		free_rest(&res);
		return 0;
	}
	
	// Al final de todo hay un OK.
	if ( !is_at_ok(res) )
	{
		fprintf(stderr,"CMGL=\"REC UNREAD\" failed\n");
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
	reti = regcomp(&regex, "^\\+CMGL: *([0-9]+),([0-9]+),\"([^\"]*)\",\"([+0-9]+)\",\"([0-9]+)/([0-9]+)/([0-9]+),([0-9]+):([0-9]+):([0-9]+)([+-][0-9]+)\"$", REG_EXTENDED);
	if (reti) 
	{
		fprintf(stderr, "PickTextSMSNotRead: Could not compile regex\n");
		return(0);
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
			sms.m_stat=GetStatInt(tmpbuf);
			
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
			
			/* So we have the SMS in the sms structure.
			 * Now we add it to the list.
			 */
			if ( !AddTextSMSEnd(sms,smslist) )
			{
				fprintf(stderr,"PicTextSMSNotRead():: Error adding SMS to list\n");
				regfree(&regex);
				free_rest(&res);
				return 0;
			}
		} else 
			if (reti == REG_NOMATCH) 
			{
			} else 
			{
				regerror(reti, &regex, msgbuf, sizeof(msgbuf));
				fprintf(stderr, "Regex match failed: %s\n", msgbuf);
				regfree(&regex);
				free_rest(&res);
				return 0;
			}
	}
	regfree(&regex);
	free_rest(&res);
	return 1;
}
