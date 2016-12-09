/* smslist.h
 * Diciembre 2016
 * Implementación de listas de sms. Tanto Texto como PDU
 *
 */

#ifndef SMSLIST_INC

#define SMSLIST_INC

typedef struct {
	int 	m_index;
	int 	m_type;
	char	m_stat[16];
	char	m_telf[32];
	char 	m_mensaje[1024];
} textsms_t;

typedef struct smsnode {
	textsms_t			*m_sms;
	struct smsnode 	*m_next;
} textsmsnode_t;

typedef struct {
	textsmsnode_t *m_head;
} textsmslist_t;

void FreeTextSMSNode(textsmsnode_t *);



void 	InitTextSMSList(textsmslist_t *);
void 	FreeTextSMSList(textsmslist_t *);

int AddTextSMSEnd(textsms_t,textsmslist_t*);
int CountTextSMS(textsmslist_t);
int GetTextSMS(textsms_t *,textsmslist_t,int);
void DumpTextSMSList(textsmslist_t);








#endif
