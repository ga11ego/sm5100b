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
#include <stdio.h>

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

/*
 * FreeTextSMSNode()
 * Desembre 2016
 * Frees the node values and all it's childs
 */
void FreeTextSMSNode(textsmsnode_t *smsn)
{
	if ( !smsn )
		return;
	if ( smsn->m_next )
		FreeTextSMSNode(smsn->m_next);
	free(smsn->m_sms);
	free(smsn->m_next);
}

/* 
 * AddTextSMSNode()
 * December 2016
 * Adds a Node at the end.
 */
int AddTextSMSNodeEnd(textsms_t *sms, textsmsnode_t *smsnode)
{
	if ( smsnode->m_next )
		return(AddTextSMSNodeEnd(sms,smsnode->m_next));
	if ( !(smsnode->m_next=(textsmsnode_t*)malloc(sizeof(textsmsnode_t))) )
	{
		fprintf(stderr,"AddTextSMSNodeEnd: malloc for next failed\n");
		return 0;
	} else {
		smsnode->m_next->m_next=NULL;
		if ( !(smsnode->m_next->m_sms=(textsms_t*)malloc(sizeof(textsms_t))) )
		{
			fprintf(stderr,"AddTExtSMSNodeEnd: malloc for sms failed\n");
			return 0;
		} else {
			memcpy(smsnode->m_next->m_sms,sms,sizeof(textsms_t));
			return 1;
		}
	}
}


/* 
 * FreeTextSMSList()
 * December 2016
 * Frees all the nodes starting from the head. 
 */
void FreeTextSMSList(textsmslist_t *smslist)
{
	FreeTextSMSNode(smslist->m_head);
	smslist->m_head=NULL;
}


/* 
 * AddTextSMSEnd()
 * December 2016
 * Adds a Text SMS to the end of the list.
 */
int AddTextSMSEnd(textsms_t sms, textsmslist_t *list)
{
	// First, let's go to the end.
	textsmsnode_t *p;
	
	if ( list->m_head )
	{
		return(AddTextSMSNodeEnd(&sms,list->m_head));
	} else { // Empty list
		if  ( !(list->m_head=(textsmsnode_t *)malloc(sizeof(textsmsnode_t))) )
		{
			// malloc failed!!
			fprintf(stderr,"AddTextSMSEnd: malloc for head failed\n");
			return 0;
		} else {
			list->m_head->m_next=NULL;
			if ( !(list->m_head->m_sms=(textsms_t*)malloc(sizeof(textsms_t))) )
			{
				fprintf(stderr,"AddTExtSMSEnd: malloc for sms failed\n");
				return 0;
			} else {
				memcpy(list->m_head->m_sms,&sms,sizeof(textsms_t));
				return 1;
			}
		}
	}
}

void DumpTextSMSNodeList(FILE *f,textsmsnode_t *node)
{
	if ( node )
	{
		DumpTextSMS(f, *node->m_sms);
		DumpTextSMSNodeList(f,node->m_next);
	}
}

/*
 * DumpTextSMSList()
 * December 2016
 * Dumps the list to the FILE descriptor
 */
void DumpTextSMSList(FILE *f,textsmslist_t list)
{
	if (list.m_head)
		DumpTextSMSNodeList(f,list.m_head);
}


/* int CountTextSMSNodeList(textsmsnode_t *node)
 * January 2017
 * Counts all the elements. 
 */
int CountTextSMSNodeList(textsmsnode_t *node)
{
	if ( node->m_next )
		return 1+CountTextSMSNodeList(node->m_next);
	else 
		return 1;
}

/* int CountTextSMS(textsmslist_t);
 * January 2017
 * Counts the amount of SMSs in the list.
 */
int CountTextSMS(textsmslist_t smslist)
{
	if ( smslist.m_head )
		return CountTextSMSNodeList(smslist.m_head);
	else
		return 0;
}

/* GetTextSMSNode()
 * January 2017
 * returns the SMS in the selected position
 */
int GetTextSMSNode(textsms_t *psms,textsmsnode_t *node, int pos)
{
	if ( pos == 0 )
	{
		memcpy(psms,node->m_sms,sizeof(textsms_t));
		return 1;
	} else 
		if ( node->m_next )
			return GetTextSMSNode(psms,node->m_next,--pos);
	return 0;
}

/* GetTextSMS()
 * Jan 2017
 * Returns the SMS in the selected position in the list (not the index)
 */
int GetTextSMS(textsms_t *psms,textsmslist_t smslist,int pos)
{
	if ( !smslist.m_head )
		return 0;
	return GetTextSMSNode(psms,smslist.m_head,pos);
}
