/*
 * getunreadsms.c
 * Jan 2017
 * Small program to get the unread SMS.
 * it answers with the number of SMS's and the enumeration of them.
 */

#include <stdlib.h>
#include <fcntl.h>

#include "../gsm_sms.h"
#include "../smslist.h"

int main(int argc, char **argv)
{
	int				fd;
	char			devicename[255]="/dev/ttyUSB0";
	textsmslist_t	smslist;
	int 			i,nsms;
	textsms_t		sms;
	char			tmpbuff[SMS_MAXSMSTXT];
	
	if ( (fd = open(devicename, O_RDWR | O_NOCTTY | O_SYNC ))==-1 )
	{
		printf("Open failed\n");
		exit(1);
	}
	
	InitTextSMSList(&smslist);
	if ( !PickTextSMSNotRead(fd,&smslist) )
	{
		fprintf(stderr,"%s failed\n",argv[0]);
		FreeTextSMSList(&smslist);
		exit(1);
	}
	printf("%d\n",nsms=CountTextSMS(smslist));
	
	for(i=0; i<nsms; i++)
	{
		GetTextSMS(&sms,smslist,i);
		GetTextSMSHeader(tmpbuff,sms);
		printf("%s\n",tmpbuff);
		GetTextSMSMessage(tmpbuff,sms);
		printf("%s\n",tmpbuff);
		
	}
	FreeTextSMSList(&smslist);
	close(fd);
	exit(0);
}
