/* 
 * ulstr.c
 * Diciembre 2016
 * Implementación de la cadena linkada de cadenas.
 *
 */

#include "ulstr.h"

/* 
 * free_ll()
 * Borra toda la lista MENOS EL PRIMERO.
 */
void free_ll(ll_t *ll)
{
	if (ll->next)
		free_ll(ll->next);
	// Si llegamos aquí, es que es el último.
	free(ll->str);
	free(ll->next);
	ll->next=NULL;
}


/*
 * addstring_ll()
 * Diciembre 2016
 * Añade la cadena al nodo suministrado. Ojo, que no comprueba nada.
 */
int addstring_ll(ll_t *node,const char *str)
{
	if ( !(node->str=malloc(sizeof(char)*(strlen(str)+1))) )
		return 0;
	memset(node->str,'\0',strlen(str)+1);
	strcpy(node->str,str);
	return 1;
}

/*
 * add_ll()
 * Diciembre 2016
 * Añade una cadena al final de la lista.
 */
int add_ll(ll_t *ll, const char *line)
{
	if (ll->next==NULL)		// Es el último
	{
		ll->next=malloc(sizeof(ll_t));
		if ( !(ll->next->str=malloc(sizeof(char)*(strlen(line)+1))) )
			return 0;
		memset(ll->next->str,'\0',strlen(line)+1);
		strcpy(ll->next->str,line);
		ll->next->next=NULL;
	} else {
		return add_ll(ll->next,line);
	}
	return 1;
	
}

int count_ll(ll_t *ll)
{
	if ( !ll->next )
		return 1;
	return 1+count_ll(ll->next);
}

ll_t *gotolast_ll(ll_t *ll)
{
	if ( ll->next )
		return ( gotolast_ll(ll->next) );
	return ll;
}


ll_t	*getnode_ll(ll_t *ll,int n)
{
	int i;
	ll_t *p=ll;
	
	for (i=0;i<n;i++)
		if ( p )
			p=p->next;
	return(p);
}

int getline_ll(ll_t *ll,int n,char *dest,size_t max)
{
	ll_t *p=getnode_ll(ll,n);
	
	memset(dest,'\0',max);
	if (p)
	{
		if ( p->str )
		{
			if ( strlen(p->str)<=max )
				strcpy(dest,p->str);
			else
				strncpy(dest,p->str,max);
			return 1;
		}
	}
	return 0;
}

void exec_ll(ll_t *ll,void f(const char *))
{
	if ( !ll )
		return;
	
	f(ll->str);
	exec_ll(ll->next,f);
}
