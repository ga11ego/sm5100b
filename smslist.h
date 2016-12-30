/* smslist.h
 * Diciembre 2016
 * Implementación de listas de sms. Tanto Texto como PDU
 *
 */

#ifndef SMSLIST_INC

#define SMSLIST_INC

#include "sms.h"

typedef struct smsnode {
	textsms_t			*m_sms;
	struct smsnode 	*m_next;
} textsmsnode_t;

typedef struct {
	textsmsnode_t *m_head;
} textsmslist_t;

void 	FreeTextSMSNode(textsmsnode_t *);
int 	AddTextSMSNodeEnd(textsms_t*,textsmsnode_t*);
void 	DumpTExtSMSNodeList(FILE *,textsmsnode_t*);


void 	InitTextSMSList(textsmslist_t *);
void 	FreeTextSMSList(textsmslist_t *);

int AddTextSMSEnd(textsms_t,textsmslist_t*);
int CountTextSMS(textsmslist_t);
int GetTextSMS(textsms_t *,textsmslist_t,int);
void DumpTextSMSList(FILE*,textsmslist_t);








#endif
