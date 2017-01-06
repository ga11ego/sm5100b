/*
 * delsms
 * Jan 2017
 * Tool to delete SMSs.
 * One by one or all unread.
 */

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "gsm_sms.h"

int main(int argc,char **argv)
{
	int				fd;
	char			devicename[255]="/dev/ttyUSB0";
	int				allnr=-1;
	int 			index=-1;
	int				c;
	
	
	opterr = 0;	//defined in unistd.h
	while ((c = getopt (argc, argv, "i:ahd:")) != -1)
	{
		switch(c) 
		{
			case 'a':
				allnr=1;
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
				printf("-i index to delete. Index in GSM modem\n");
				printf("-d device to use to contact the GSM modem. Default is /dev/ttyUSB0\n");
				printf("You *must* specify -a or -i <index>\n");
				exit(0);
			break;
			default:
				printf("Unknown option %c\n",c);
			break;
		}
	}
	
	if ( allnr==-1 && index==-1 )
	{
		printf("%s: Error. You must specify -a or -i\n",argv[0]);
		exit(1);
	}
	if ( allnr==1 && index!=-1 )
	{
		printf("%s: Error. You can't specify both -a and -i\n",argv[0]);
		exit(1);
	}
	
	//printf("Opening port... %s\n",devicename);
    if ( (fd = open(devicename, O_RDWR | O_NOCTTY | O_SYNC ))==-1 )
	{
		printf("Open failed\n");
		exit(1);
	}
		
	if ( allnr==1 )
	{
		if ( DeleteSMSAllRead(fd) )
			exit(0);
		else 
			exit(1);
	} else {
		if ( DeleteSMSIndex(fd,index) )
			exit(0);
		else 
			exit(1);
	}
}
