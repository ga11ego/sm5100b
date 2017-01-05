/* 
 * atstring.c
 * 
 * Agosto 2016
 *
 * Implementación del mecanismo de comunicación con el modem GSM.
 *
 */
 
#include "atstring.h"
#include <stdarg.h>
#include <syslog.h>
#include <regex.h>

/* 
 * init_rest()
 * Agosto 2016
 * 
 * El problema es que no podemos saber si el puntero res->str es NULL o no. No podemos saber 
 * si ha sido inicializado anteriormente. El que llame a esto deberá ser cuidadoso.
 */
void init_rest(res_t *res)
{
	memset(res->atcmd,'\0',MAXATCMD);
	res->strings=NULL;
}

/*
 * free_rest()
 * Diciembre 2016
 * Modificamos para usar ll_t. Libera toda la memoria usada por el res_t
 */
void free_rest(res_t *res)
{
	if ( res->strings )
	{
		free_ll(res->strings);
		free(res->strings);
		res->strings=NULL;
	}
}

/* 
 * set_rest_cmd()
 * Agosto 2016
 * Pone el comenado AT
 */
void set_rest_cmd(res_t *res, const char *at)
{
	strncpy(res->atcmd,at,MAXATCMD);
	free_rest(res);
}

/*
 * add_rest_line()
 * Diciembre 2016
 * Añade la cadena al rest. Si es el primero, lo hace
 */
int add_rest_line(res_t *res,const char *line)
{
	ll_t	*p;
	
	if ( !res->strings )		// Está vacío.
	{
		if ( !(res->strings=malloc(sizeof(ll_t))) )
			return 0;
		res->strings->next=NULL;
		addstring_ll(res->strings,line);
		return 1;
	} else 
		return add_ll(res->strings,line);
}



int count_rest_lines(res_t res)
{
	if ( res.strings )
		return count_ll(res.strings);
	else 
		return 0;
}



/*
 * SentATCommand2()
 * Diciembre 2016
 * Enviamos un comando que ya está puesto en el res_t.
 * Esta es la segunda versión de esta función. La primera, leía todo el
 * output del 'fd' en un buffer y luego procesaba las líneas con
 * ParseATOutput(). Este método funciona bien siempre que el output
 * quepa en ese buffer. El problema es que al leer listas de SMSs nos
 * encontramos con outputs muchísimo más grandes.
 * Esto ha puesto de manifiesto la necesidad de procesar el output tal
 * y como va llegando, con read's de un solo carácter.
 * El problema aquí es que nos quedamos bloqueados esperando output en 
 * read(). Así que necesitaremos asegurarnos de que todo queda bien.
 * Recuerda que aquí no hacemos procesado de ningún tipo. Solo 
 * leemos líneas y las metemos en el res.
 * 
 * Enero 2017. 
 * Cuando hay un error, retornamos 0 aunque dé error el modem
 * 
 * Aclaración para los errores: Si retorna 0, es que el comando "ha ido
 * bien"... es decir, no ha petado. Pero no por ello debe salir un OK 
 * al final.
 * 
 * 
 */
#define MAXATBUFF	2048
int SendATCommand2(int fd, res_t *res)
{
	char 	atcmd[MAXATINPUT];		// El comando que mandaremos.
	char 	ic	;					// Input char
	int		rr;						// read result. 0==EOF. -1=ERRR
	char	tmpbuff[MAXATBUFF];		// Aquí iremos guardando los caracteres de cada línea. 
	int		buffptr;				// Puntero dentro del tmpbuff
	regex_t	regex_error;
	int 	reti;
	
	/* Si el comando viene vacío, es un AT pelao */
	if ( strlen(res->atcmd)==0 )
		sprintf(atcmd,"AT\r");
	else
		sprintf(atcmd,"AT+%s\r",res->atcmd);
	if ( write(fd,atcmd,strlen(atcmd)) == -1 )
	{
		/* Cagada */
		return AT_ERROR_WRITEFAILED;
	}
	
	// Preparamos las regexp.
	reti = regcomp(&regex_error, "^\\+CM. ERROR:", REG_EXTENDED);
	if (reti) 
	{
		fprintf(stderr, "Could not compile regex\n");
		return AT_ERROR_REGEXPFAILED;
	}
	
	
	// Ahora vamos a ire leyendo el output caracter a caracter.
	memset(tmpbuff,'\0',MAXATBUFF);
	buffptr=0;
	while ( (rr=read(fd,&ic,1))==1 )
	{
		// las lineas terminan con <LF><CR>
		if ( ic=='\r' )
		{
			if ( (rr=read(fd,&ic,1))==1 )
			{
				if ( ic=='\n' )		// Fin de linea.
				{
					// Tenemos dos cosas por hacer.
					// 1. Copiar la cadena a res.
					// 2. Comprobar que no sea un OK pelao. Que siginifica que terminamos.
					//    O que sea un error +CMS ERROR: errno
					if ( strlen(tmpbuff)!=0 ) // Si es 0, pasamos de añadirla. 
					{
						add_rest_line(res,tmpbuff);
						// Ahora comprobamos que no es un ok.
						if ( strcmp(tmpbuff,"OK") == 0 )
						{
							regfree(&regex_error);
							return 0;
						} else {
							// Podría ser un +CMS ERROR:
							reti = regexec(&regex_error, tmpbuff, 0, NULL, 0);
							if ( !reti )
							{
								regfree(&regex_error);
								return 0;
							}
						}
						// Ponemos a 0 todo.
						memset(tmpbuff,'\0',MAXATBUFF);
						buffptr=0;
					}
				} else {			// Oh, error. Bueno, simplemente lo ignoramos. A lo mejor es un \n dentro de un mensaje.
					tmpbuff[buffptr++]='\n';
				}
			} else {
				// Muy raro. El read ha dado error. 
				fprintf(stderr,"ERROR: read() returned unexpected value %d\n",rr);
				regfree(&regex_error);
				return AT_ERROR_READFAILED;
			}
		} else {
			tmpbuff[buffptr++]=ic;
		}		
	}
	if ( rr==0 )	// EOF.
	{
		regfree(&regex_error);
		return 0;
	}
	if ( rr==-1 )
	{
		fprintf(stderr,"read() returned -1\n");
		regfree(&regex_error);
		return AT_ERROR_READFAILED;
	}
	fprintf(stderr,"Unexpected error\n");
	regfree(&regex_error);
	return AT_ERROR_UNEXPECTED;

}

/* is_at_ok()
 * Agost 2016
 * Retorna 1 si la última cadena te un "OK". 
 */
int is_at_ok(const res_t res)
{
	ll_t *p;
	// Primero ir al último
	if ( res.strings )
		p=gotolast_ll(res.strings);
	else 
		return 0;
	if ( strcmp(p->str,"OK")==0 )
		return 1;
	return 0;
}

/* 
 * is_cm_error
 * Enero 2017
 * Retorna 1 si es un +CM. ERROR.
 */
int is_cm_error(const res_t res)
{
	regex_t	regex_error;
	int		reti;
	
	ll_t *p;
	if ( res.strings )
		p=gotolast_ll(res.strings);
	else 
		return 0;
	// Preparamos las regexp.
	reti = regcomp(&regex_error, "^\\+CM. ERROR:", REG_EXTENDED);
	if (reti) 
	{
		fprintf(stderr, "Could not compile regex\n");
		return 0;
	}
	reti = regexec(&regex_error, p->str, 0, NULL, 0);
	regfree(&regex_error);
	if ( !reti )
	{
		return 1;
	}
	return 0;
}

/* check_at_response
 * Setembre 2016
 * retorna 1 si la reposta en la cadena és coherent amb cmd
 * This is: +cmd:
 */
int check_at_response(char *atstr,const char *cmd)
{
	char *strptr;
	
	if ( atstr[0]!='+' )
		return 0;
	strptr=atstr+1;
	if ( strncmp(strptr,cmd,strlen(cmd))==0 )
		if ( atstr[strlen(cmd)+1]==':' )
			return 1;
	return 0;
}

/* check_at_response2
 * Desembre 2016
 * retorna 1 si la reposta en la cadena[0] és coherent amb cmd
 * This is: +cmd:
 */
int check_at_response2(res_t res,const char *cmd)
{
	char 	*strptr;
	char	line0[MAXATRES];
	
	
	if ( !getline_rest(res,0,line0,MAXATRES) )
		return 0;
	if ( line0[0]!='+' )
		return 0;
	strptr=line0+1;
	if ( strncmp(strptr,cmd,strlen(cmd))==0 )
		if ( line0[strlen(cmd)+1]==':' )
			return 1;
	return 0;
}

void dl(const char *l)
{
	fprintf(stderr,"%s\n",l);
}
void DumpATOutput(res_t res)
{
	fprintf(stderr,"Command: <%s>\n",res.atcmd);
	fprintf(stderr,"Number of lines: %d\n",count_rest_lines(res));
	
	exec_ll(res.strings,dl);
}


/*
 * getline_rest()
 * Coge la enésima línea. 
 */
int getline_rest(res_t res,int l,char *dest,size_t max)
{
	ll_t	*p=res.strings;
	
	return(getline_ll(p,l,dest,max));
}

void dl2(const char *l)
{
	syslog(LOG_LOCAL7,"%s\n",l);
}
void SyslogATOutput(const res_t res)
{
	syslog(LOG_LOCAL7,"Command: <%s>\n",res.atcmd);
	syslog(LOG_LOCAL7, "Number of lines: %d\n",count_rest_lines(res));
	exec_ll(res.strings,dl2);
}
