/* 
 * smslist.c
 * Diciembre 2016
 * Implementación de las listas de SMSs
 * 
 */
 
#include "smslist.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* 
 * InitTextSMSList();
 * Diciembre 2016
 * Inicializa una lista de SMSs en modo texto.
 * Pone la cabecera en NULL.
 */
void InitTextSMSList(textsmslist_t *smslist)
{
	smslist->m_head=NULL;
}


void FreeTextSMSNode(textsmsnode_t *smsn)
{
	if ( !smsn )
		return;
	if ( smsn->m_next )
		FreeTextSMSNode(smsn->m_next);
	free(smsn->m_sms);
	free(smsn->m_next);
}

void FreeTextSMSList(textsmslist_t *smslist)
{
	FreeTextSMSNode(smslist->m_head);
	smslist->m_head=NULL;
}
