/*
 * sms.h
 * December 2016
 * Basic definition of SMS structures. One for Text SMS and another for 
 * PDU SMS. 
 * The smslist's will be based on these.
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


#endif
