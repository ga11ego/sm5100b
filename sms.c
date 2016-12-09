/*
 * sms.c
 * Noviembre 2016
 *
 */

#include "atstring.h" 
#include "gsm.h" 
#include "sms.h"

 
#include <stdarg.h>
#include <syslog.h>
 
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
	res_t 	res;
	
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
	DumpATOutput(res);
	
	
	free_rest(&res);
	return 1;
}
