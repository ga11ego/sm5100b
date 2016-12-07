/* 
 * gsm.c
 *
 * Agosto 2016
 *
 */

#include "atstring.h" 
#include "gsm.h"

#include <stdarg.h>
#include <syslog.h>

/* SendAT()
 * Agost 2016
 * Envia un AT i retorna OK (1) o !OK (0)
 */
int SendAT(int fd)
{
	res_t	res;
	int 	i=0;
	char	tmpbuff[MAXATRES];
	
	init_rest(&res);
	set_rest_cmd(&res,"");
	if ( !SendATCommand2(fd,&res) )
	{
		syslog(LOG_LOCAL7,"SendAT failed");
		free_rest(&res);
		return 0;
	}
	/* Si tot va bé, tenim 1 linia i OK	*/
	if ( !getline_rest(res,0,tmpbuff,MAXATRES) )
		return 0;
	if ( strcmp(tmpbuff,"OK")==0 )
			i=1;
		
	if ( !i )
	{
		syslog(LOG_LOCAL7,"SendAT Failed");
		SyslogATOutput(res);
	}
	free_rest(&res);
	return i;
}

int GetCSQBer(int fd)
{
	res_t 	res;
	int 	ber;
	int 	i;
	char 	berstr[32];
	char 	*strptr;
	char	line0[MAXATRES];
	
	init_rest(&res);
	set_rest_cmd(&res,"CSQ");
	if ( !SendATCommand2(fd,&res) )
	{
		syslog(LOG_LOCAL7,"GetCSQBer:SendAT failed");
		free_rest(&res);
		return 0;
	}
	if ( !is_at_ok(res) )
	{
		syslog(LOG_LOCAL7,"CSQ failed");
		SyslogATOutput(res);
		free_rest(&res);
		return 0;
	}
	/* Si todo va bien, tenemos <rssi>,<ber> */
	// Hay que coger la primera y ver si es correcta.
	if ( !getline_rest(res,0,line0,MAXATRES) )
		return 0;
	if ( !check_at_response(line0,"CSQ") ) 
	{
		syslog(LOG_LOCAL7,"GetCSQBer:CSQ, bad response");
		SyslogATOutput(res);
		return 0;
	}
	memset(berstr,'\0',32);
	for(i=0;i<strlen(line0);i++)
	{
		if ( line0[i]==',' )
		{
			strptr=line0+i+1;
			strcpy(berstr,strptr);
			ber=atoi(berstr);
			return ber;
		}
	}
	return -1;
}

int GetCSQRSSI(int fd)
{
	res_t 	res;
	int 	rssi;
	int 	i;
	char 	rssistr[32];
	char	line0[MAXATRES];
	
	init_rest(&res);
	set_rest_cmd(&res,"CSQ");
	if ( !SendATCommand2(fd,&res) )
	{
		syslog(LOG_LOCAL7,"GetCSQRSSI:SendAT failed");
		free_rest(&res);
		return 0;
	}
	if ( !is_at_ok(res) )
	{
		syslog(LOG_LOCAL7,"CSQ failed");
		SyslogATOutput(res);
		free_rest(&res);
		return 0;
	}
	/* Si todo va bien, tenemos +CSQ: <rssi>,<ber> */
	if ( !getline_rest(res,0,line0,MAXATRES) )
		return 0;
	if ( !check_at_response(line0,"CSQ") ) 
	{
		syslog(LOG_LOCAL7,"GetCSQRSSI:CSQ, bad response");
		SyslogATOutput(res);
		return 0;
	}
	memset(rssistr,'\0',32);
	for(i=6;i<strlen(line0);i++)
	{
		if ( line0[i]==',' )
		{
			rssi=atoi(rssistr);
			return rssi;
		}
		rssistr[i-6]=line0[i];
	}
	return -1;
}


/*
 * GetCPIN()
 * Septiembre 2016
 * Recoge el estado de CPIN. 
 */
int GetCPIN(int fd)
{
	res_t 	res;
	int 	i;
	char 	cpinstr[64];
	char	line0[MAXATRES];
	
	init_rest(&res);
	set_rest_cmd(&res,"CPIN?");
	if ( !SendATCommand2(fd,&res) )
	{
		syslog(LOG_LOCAL7,"GetCPIN:SendAT failed");
		free_rest(&res);
		return 0;
	}
	if ( !is_at_ok(res) )
	{
		syslog(LOG_LOCAL7,"CPIN? failed");
		SyslogATOutput(res);
		free_rest(&res);
		return 0;
	}
	/* Si todo va bien, tenemos +CPIN: <code> */
	if ( !getline_rest(res,0,line0,MAXATRES) )
		return 0;
	if ( !check_at_response(line0,"CPIN") ) 
	{
		syslog(LOG_LOCAL7,"GetCPIN: CPIN?, bad response");
		SyslogATOutput(res);
		free_rest(&res);
		return 0;
	}
	memset(cpinstr,'\0',64);
	for(i=7;i<strlen(line0) && line0[i]>31;i++)
		cpinstr[i-7]=line0[i];
	free_rest(&res);
	if ( strcmp(cpinstr,"READY")==0 )
		return CPIN_READY;
	if ( strcmp(cpinstr,"SIM PIN")==0 )
		return CPIN_SIMPIN;
	if ( strcmp(cpinstr,"SIM PUK")==0 )
		return CPIN_SIMPUK;
	if ( strcmp(cpinstr,"SIM PIN2")==0 )
		return CPIN_SIMPIN2;
	if ( strcmp(cpinstr,"SIM PUK2")==0 )
		return CPIN_SIMPUK2;
	if ( strcmp(cpinstr,"BLOCK")==0 )
		return CPIN_BLOCK;
	return 0;
}

/* 
 * SetCPIN(const char *,int fd)
 * Septiembre 2016
 * 
 * Nos permite fijar el PIN
 * 
 */
int SetCPIN(const char *pin, int fd)
{
	int 	i;
	res_t	res;
	char	comp[128];
	
	
	// Si no lo has puesto de 4 cifras, nos vamos
	if ( strlen(pin)!=4 )
		return 0;
	// Si no son 4 cifras, nos vamos.
	for(i=0;i<4;i++)
		if ( pin[i]<48 || pin[i]>57 )
			return 0;
	//Vale, pues vamos allà.
	init_rest(&res);
	sprintf(comp,"CPIN=\"%s\"",pin);
	set_rest_cmd(&res,comp);
	if ( !SendATCommand2(fd,&res) )
	{
		syslog(LOG_LOCAL7,"SetCPIN:SendAT failed");
		SyslogATOutput(res);
		free_rest(&res);
		return 0;
	}
	// Si tenemos un OK, es que todo ha ido bien.
	if ( !is_at_ok(res) )
	{
		syslog(LOG_LOCAL7,"SetCPIN:Failed");
		SyslogATOutput(res);
		free_rest(&res);
		return 0;
	}
	free_rest(&res);
	return 1;
}


/* 
 * GetCCID()
 * Septiembre 2016
 * Para recoger el CCID en una cadena.
 * El formato es:
 * +CCID: "8934190014010295908F"
  * OK
 * Empezamos a contar en el 8 hasta encontrar una comilla.
 */
int GetCCID(char *ccid,int maxlen, int fd)
{
	res_t 	res;
	int		i;
	char	line0[MAXATRES];
	
	init_rest(&res);
	set_rest_cmd(&res,"CCID");
	if ( !SendATCommand2(fd,&res) )
	{
		syslog(LOG_LOCAL7,"GetCCID: SendAT Failed");
		free_rest(&res);
		return 0;
	}
	if ( !is_at_ok(res) )
	{
		syslog(LOG_LOCAL7,"CCID failed");
		SyslogATOutput(res);
		free_rest(&res);
		return 0;
	}
	memset(ccid,'\0',maxlen);
	if ( !getline_rest(res,0,line0,MAXATRES) )
		return 0;
	for ( i=8; i<strlen(line0) && (i-8)<maxlen && line0[i]!='"' ;i++)
	{
		ccid[i-8]=line0[i];
	}
	free_rest(&res);
	return 1;
}

/*
 * GetIMEI()
 * Septiembre 2016
 * Recoge el IMEI del dispositivo
 * El formato es una retahíla de números (15 cifras)
 */ 
 int GetIMEI(char *imei,int maxlen,int fd)
 {
	res_t 	res;
	int		i;
	char	line0[MAXATRES];
	
	init_rest(&res);
	set_rest_cmd(&res,"CGSN");
	if ( !SendATCommand2(fd,&res) )
	{
		syslog(LOG_LOCAL7,"GetIMEI: SendAT Failed");
		free_rest(&res);
		return 0;
	}
	if ( !is_at_ok(res) )
	{
		syslog(LOG_LOCAL7,"IMEI failed");
		SyslogATOutput(res);
		free_rest(&res);
		return 0;
	}
	memset(imei,'\0',maxlen);
	if ( !getline_rest(res,0,line0,MAXATRES) )
		return 0;
	for ( i=0; i<strlen(line0) && i<maxlen && line0[i]<58 && line0[i]>47 ;i++)
	{
		imei[i]=line0[i];
	}
	free_rest(&res);
	return 1; 
 }

