/* 
 * readsms
 * Tool to read an sms without taking it out of the GSM modem.
 * Provide -i and index no. If no provided, index is 1.
 * -d is the device.
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "gsm.h"
#include "smslist.h"
#include "sms.h"
 
void main(int argc,char **argv)
{
	int 	index=1;
	char	devicename[255]="/dev/ttyUSB0";
	int		fd;
	int 	c;
	int		getmessage=0;
	int 	getphone=0;
	int		getdate=0;
	int 	getraw=0;
	
	opterr = 0;	//defined in unistd.h
	while ((c = getopt (argc, argv, "i:d:hmpfr")) != -1)
	{
		switch(c) 
		{
			case 'r':
				getraw=1;
			break;
			case 'f':
				getdate=1;
			break;
			case 'm':
				getmessage=1;
			break;
			case 'p':
				getphone=1;
			break;
			case 'i':
				index=atoi(optarg);
			break;
			case 'd':
				strncpy(devicename,optarg,255);
			break;
			case 'h':
				printf("%s:\n",argv[0]);
				printf("-h to show this message\n");
				printf("-i index to retrieve SMS with index number 'index'. Default is 1\n");
				printf("-d device to use to contact the GSM modem. Default is /dev/ttyUSB0\n");
				printf("-f Get Date\n");
				printf("-m Get Message\n");
				printf("-p Get Phone\n");
				exit(0);
			break;
			default:
				printf("Unknown option %c\n",c);
			break;
		}
	}
	//printf("Opening port... %s\n",devicename);
    if ( (fd = open(devicename, O_RDWR | O_NOCTTY | O_SYNC ))==-1 )
	{
		printf("Open failed\n");
		exit(1);
	}
	
	textsms_t	sms;
	char		tmpbuff[SMS_MAXSMSTXT];
	
	//printf("Picking message %d\n",index);
	if ( !PickTextSMSIndex(fd,index,&sms) )
	{
		fprintf(stderr,"Error picking message\n");
		exit(1);
	} else {
		if ( getmessage )
		{
			GetTextSMSMessage(tmpbuff,sms);
			printf("%s\n",tmpbuff);
		}
		if ( getphone )
		{
			GetTextSMSPhone(tmpbuff,sms);
			printf("%s\n",tmpbuff);
		}
		if ( getdate )
		{
			GetTextSMSDateString(tmpbuff,sms);
			printf("%s\n",tmpbuff);
		}
		if ( getraw )
		{
			GetTextSMSMessageRaw(tmpbuff,sms);
			printf("%s\n",tmpbuff);
		}
	}
	//DumpTextSMS(stdout,sms);
	close(fd);
	
	exit(0);
}
