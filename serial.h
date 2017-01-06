/* 
 * serial.h
 *
 */


#ifndef _SERIALINC

#define _SERIALINC

#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <syslog.h>

void SetUSBUp(int);

#endif
