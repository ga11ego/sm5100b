/*
 * sms.h
 * Noviembre 2016
 * Definiciones para trabajar con SMSs.
 */
 
 #ifndef _SMSINC
 #define _SMSINC

typedef struct {
	int 	m_index;
	int 	m_type;
	char	m_stat[16];
	char	m_telf[32];
	char 	m_mensaje[1024];
} textsms_t;

void 	InitTextSMS(textsms_t *);

typedef struct smsnode {
	textsms_t			m_sms;
	struct smsnode 	*next;
} textsmsnode_t;



int 	InitTextSMSList(textsmsnode_t *);
void 	FreeTextSMSList(textsmsnode_t *);

int AddTextSMS(textsms_t,textsmsnode_t*);
int GetTextSMSListSize(textsmsnode_t *);
int GetTextSMS(textsms_t *,textsmsnode_t,int);
 
int GetSMSMode(int);
int SetSMSMode(int,int);

int GetTextSMSList(int, textsmsnode_t *);
 
 #endif
