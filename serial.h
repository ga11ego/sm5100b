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

int set_interface_attribs (int, int , int );
void set_blocking (int , int );


#endif
