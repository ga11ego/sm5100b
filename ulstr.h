/*
 * ulstr.h
 * Diciembre 2016
 * Linked list de cadenas. Para gestionar los outputs.
 */
 
#ifndef ULSTR_INC

#define ULSTR_INC

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> 
#include <string.h>
#include <stdlib.h>


typedef struct ll {
	char *str;
	struct ll *next;
} ll_t;


void 	free_ll(ll_t *);
int 	add_ll(ll_t *, const char *);
int 	getline_ll(ll_t *,int,char *,size_t);
int 	count_ll(ll_t *);
ll_t 	*gotolast_ll(ll_t *ll);
int 	addstring_ll(ll_t *,const char *);

void    exec_ll(ll_t *,void f(const char*));

#endif
