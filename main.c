#include <stdio.h>
#include <unistd.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>

#include "serial.h"
#include "atstring.h"
#include "gsm.h"
#include "smslist.h"
#include "sms.h"

int main() 
{
    char byte;
	char in[255];
	int fd;
	ssize_t size;
	int ber,rssi;
	
	textsmslist_t	smslist;
	
	/* A probar cosas */
	printf("Opening port... /dev/ttyUSB0\n");
    if ( (fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_SYNC ))==-1 )
	{
		printf("Open failed\n");
		return 1;
	}
	
	SetUSBUp(fd);
	
	printf("main.c: Sending an AT to the modem\n");
	if ( SendAT(fd) == 1 )
		printf("OK Response\n");
	else {
		printf("Bad response. Check Syslog\n");
		exit(-1);
	}
	
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
			if ( SetCPIN("5555",fd) )
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
	
	
	if ( GetCCID(in,255,fd) )
	{
		printf("CCID: %s\n",in);
	} else {
		printf("GetCCID failed\n");
	}

	if ( GetIMEI(in,255,fd) )
	{
		printf("IMEI: %s\n",in);
	} else {
		printf("GetIMEI failed\n");
	}	
	
	ber=GetCSQBer(fd);
	if ( ber!=-1 )
		printf("BER=%d\n",ber);
	else 
		printf("Error reading BER\n");
		
	rssi=GetCSQRSSI(fd);
	if ( rssi!=-1 )
		printf("RSSI=%d\n",rssi);
	else
		printf("Error reading RSSI\n");	
		
	printf("SMS Mode: ");
	switch (GetSMSMode(fd))
	{
		case SMSMODE_PDU:
			printf("PDU");
		break;
		case SMSMODE_TEXT:
			printf("TEXT");
		break;
		default:
			printf("ERROR");
	}
	printf("\n");
	
	if ( SetSMSMode(fd,SMSMODE_TEXT) )
		printf("SMS Mode set to TEXT\n");
	else
		printf("Error setting SMS mode to TEXT\n");
	
	// Vamos a empezar el baile.
	printf("Now reading SMS's. Please wait...\n");
	InitTextSMSList(&smslist);
	GetTextSMSList(fd,&smslist);
	DumpTextSMSList(stdout,smslist);
	FreeTextSMSList(&smslist);
	
	

	
	
	
	close(fd);
    return 0;
}
