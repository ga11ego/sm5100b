/* checkcpin
 * Jan 2017
 * Tool to check and set CPIN for the modem.
 */
 
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "../gsm.h"

int main(int argc, char **argv)
{
	int				fd;
	char			devicename[255]="/dev/ttyUSB0";

	char			pin[32]="";
	int				c;
	
	opterr = 0;	//defined in unistd.h
	while ((c = getopt (argc, argv, "d:p:h")) != -1)
	{
		switch(c) 
		{
			case 'p':
				strncpy(pin,optarg,32);
			break;
			case 'd':
				strncpy(devicename,optarg,255);
			break;
			case 'h':
				printf("%s: Set PIN\n",argv[0]);
				printf("-h to show this message\n");
				printf("-p <PIN> \n");
				printf("-d device to use to contact the GSM modem. Default is /dev/ttyUSB0\n");
				printf("You *must* specify -p\n");
				exit(0);
			break;
			default:
				printf("Unknown option %c\n",c);
			break;
		}
	}
	if ( strlen(pin)==0 )
	{
		printf("%s: Error. You must specify a pin\n",argv[0]);
		exit(1);
	}
	
	//printf("Opening port... %s\n",devicename);
    if ( (fd = open(devicename, O_RDWR | O_NOCTTY | O_SYNC ))==-1 )
	{
		printf("Open failed\n");
		exit(1);
	}
	SetUSBUp(fd);
	
	
	switch ( GetCPIN(fd) )
	{
		case CPIN_ERROR:
			printf("CPIN returned error code\n");
			printf("Exiting\n");
			exit(-1);
		break;
		case CPIN_READY:
			printf("CPIN is ready\n");
		break;
		case CPIN_SIMPIN:
			printf("PIN is missing\n");
			// Si el pin falta, lo ponemos. 
			if ( SetCPIN(pin,fd) )
				printf("PIN set correctly\n");
			else {
				printf("Error setting PIN\nExiting\n");
				exit(-1);
			}
		break;
		case CPIN_SIMPIN2:
			printf("Enter PIN2 while ME is waiting for SIM2\n");
			printf("Exiting\n");
			exit(-1);
		break;
		case CPIN_SIMPUK:
			printf("Enter PUK\n");
			printf("Exiting\n");
			exit(-1);
		break;
		case CPIN_SIMPUK2:
			printf("Enter PUK2 while ME is waiting for SIM\n");
			printf("Exiting\n");
			exit(-1);
		break;
		case CPIN_BLOCK:
			printf("SIM is blocked\n");
			printf("Exiting\n");
			exit(-1);
		break;
		default:
			printf("Unknown CPIN code\n");
			printf("Exiting\n");
			exit(-1);
	}
	
	
	
	close(fd);
	exit(0);
}
