/* 
 * serial.c
 * 
 * http://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c
 *
 */

#include "serial.h"



void SetUSBUp(int fd)
{
	speed_t baud = B9600; /* baud rate */

	/* set the other settings (in this case, 9600 8N1) */
	struct termios settings;
	tcgetattr(fd, &settings);

	cfsetospeed(&settings, baud); /* baud rate */
	settings.c_cflag &= ~PARENB; /* no parity */
	settings.c_cflag &= ~CSTOPB; /* 1 stop bit */
	settings.c_cflag &= ~CSIZE;
	settings.c_cflag |= CS8 | CLOCAL; /* 8 bits */
	settings.c_lflag = ~ICANON; /* non canonical mode */
	settings.c_cc[VMIN] = 1;
	settings.c_cc[VTIME]=60;
	settings.c_oflag &= ~OPOST; /* raw output */

	tcsetattr(fd, TCSANOW, &settings); /* apply the settings */
	tcflush(fd, TCOFLUSH);
	
	//fcntl(fd, F_SETFL, FNDELAY);
}

