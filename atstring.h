/* 
 * atstring.h
 * 
 * Agosto 2016
 * 
 * Definición de funciones para enviar AT's crudos por un puerto.
 * 
 * La idea es que mandes un AT+Comando y recibas varias cosas. Básicamente acabado 
 * con un OK. Eso significa que te devuelve un array de strings. Todos con texto
 * (vamos a eliminar las líneas vacías.
 *
 */
 #ifndef _ATSTRINGINC
#define _ATSTRINGINC

#include "ulstr.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> 
#include <string.h>
#include <stdlib.h>

#define MAXATCMD 128
#define MAXATINPUT 255
#define MAXRES 1024
#define MAXATRES 1024

#define MAXINPUTBUFFER 1024


typedef struct {
	char 	atcmd[MAXATCMD];
	ll_t	*strings;
} res_t;


int 	init_rest(res_t *);
void 	free_rest(res_t *);
void 	set_rest_cmd(res_t *, const char *);
int 	add_rest_line(res_t *,const char *);
int 	count_rest_lines(res_t);
int		getline_rest(res_t,int,char *,size_t);



void DumpATOutput(res_t);
void SyslogATOutput(const res_t);


int SendATCommand2(int, res_t*);

int is_at_ok(const res_t);
int check_at_response2(res_t,const char *);
int check_at_response(char *, const char *);





#endif
