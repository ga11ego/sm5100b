/*
 * sms_listen.c
 * Feb 2017
 * Daemon for listening for incoming SMSs to process and/or sending 
 * smss if things happen.
 * This is an example project. I will actually develop it further 
 * in a separate project to fulfill my domotic needs. 
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
	char			configfile[255]="";
	int				c;
	
	
	opterr = 0;	//defined in unistd.h
	while ((c = getopt (argc, argv, "c:d:h")) != -1)
	{
		switch(c) 
		{
			case 'd':
				strncpy(devicename,optarg,255);
			break;
			case 'c':
				strncpy(configfile,optarg,255);
			break;
			case 'h':
				printf("%s: sms_listener\n",argv[0]);
				printf("-h to show this message\n");
				printf("-d device to use to contact the GSM modem. Default is /dev/ttyUSB0\n");
				printf("-c <configfile> Specify the configuration file\n");
				printf("You *must* specify -c\n");
				exit(0);
			break;
			default:
				printf("Unknown option %c\n",c);
			break;
		}
	}
	if ( strlen(configfile)==0 )
	{
		printf("%s: Error. You must specify a configuration file\n",argv[0]);
		exit(1);
	}
	exit(0);
	
}
