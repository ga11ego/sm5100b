SHELL = /bin/sh


CC = gcc

#X_LIBS = @X_LIBS@ -lX11
#X_CFLAGS = @X_CFLAGS@


CFLAGS = -g 
LDFLAGS =  
LIBS = -ldl 


ALL=sm5100b
OBJECTS=main.o gsm.o atstring.o sms.o serial.o ulstr.o smslist.o
CSOURCES=main.c gsm.c atstring.c sms.c serial.c ulstr.c smslist.c
HSOURCES=gsm.h atstring.h sms.h serial.h ulstr.h smslist.h

SOURCES=${CSOURCES} ${HSOURCES}

DEPS=.depend

all: ${ALL}

sm5100b: ${OBJECTS}
#	echo linking main application "-->" $@
	${CC} ${OBJECTS} -o $@  ${LDFLAGS} ${LIBS} ${CFLAGS}


.SUFFIXES: .c .o
.c.o:
#	echo $< "-->" $@ 
	$(CC) ${CFLAGS} -c $< -o $@ 



clean: 
	rm -rf *.o sm5100b .depend 



.depend: ${CSOURCES} ${HSOURCES}
	$(CC) ${CFLAGS}  -M $(CSOURCES) $(HSOURCES)  > $(DEPS)

dep: .depend

include ${DEPS}


