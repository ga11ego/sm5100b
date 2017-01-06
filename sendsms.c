/* sendsms.c
 * Jan 2017
 * Tool for SMS sending. 
 */
 
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "gsm_sms.h" 
#include "serial.h"
 
int main(int argc, char **argv)
{
	int				fd;
	char			devicename[255]="/dev/ttyUSB0";
	char			phone[255]="";
	char			message[141]="";
	int				c;
	
	
	opterr = 0;	//defined in unistd.h
	while ((c = getopt (argc, argv, "m:p:d:h")) != -1)
	{
		switch(c) 
		{
			case 'm':
				strncpy(message,optarg,140);
			break;
			case 'p':
				strncpy(phone,optarg,255);
			break;
			case 'd':
				strncpy(devicename,optarg,255);
			break;
			case 'h':
				printf("%s: Send SMS\n",argv[0]);
				printf("-h to show this message\n");
				printf("-p <number> to specify destination\n");
				printf("-m <message> to specify the message\n");
				printf("-d device to use to contact the GSM modem. Default is /dev/ttyUSB0\n");
				printf("You *must* specify -m and -p\n");
				exit(0);
			break;
			default:
				printf("Unknown option %c\n",c);
			break;
		}
	}
	if ( strlen(message)==0 )
	{
		printf("%s: Error. You must specify a message\n",argv[0]);
		exit(1);
	}
	if ( strlen(phone) == 0 )
	{
		printf("%s: Error. You must specify a phone number\n",argv[0]);
		exit(1);
	}
	//printf("Opening port... %s\n",devicename);
    if ( (fd = open(devicename, O_RDWR | O_NOCTTY | O_SYNC ))==-1 )
	{
		printf("Open failed\n");
		exit(1);
	}
	SetUSBUp(fd);
	if ( !SendTextSMS(fd,phone,message) )
	{
		fprintf(stderr,"SendSMS Failed\n");
		exit(1);
	}
	close(fd);
	exit(0);
}
