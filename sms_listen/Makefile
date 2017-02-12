SHELL = /bin/sh


CC = gcc

#X_LIBS = @X_LIBS@ -lX11
#X_CFLAGS = @X_CFLAGS@


CFLAGS = -g 
LDFLAGS =  -L.. 
LIBS = -ldl -lsms


ALL=sms_listen

OBJECTS=sms_listen.o 
CSOURCES=sms_listen.c
HSOURCES=../gsm.h ../atstring.h ../sms.h ../serial.h ../ulstr.h ../smslist.h ../gsm_sms.h
STATICLIB=libsms.a

SOURCES=${CSOURCES} ${HSOURCES}

DEPS=.depend

all: ${ALL}



sms_listen: sms_listen.o 
	${CC} sms_listen.o  -o $@ ${LDFLAGS} ${LIBS} ${CFLAGS}


.SUFFIXES: .c .o
.c.o:
#	echo $< "-->" $@ 
	$(CC) ${CFLAGS} -c $< -o $@ 



clean: 
	rm -rf *.o sms_listen ${OBJECTS} .depend 



.depend: ${CSOURCES} ${HSOURCES}
	$(CC) ${CFLAGS}  -M $(CSOURCES) $(HSOURCES)  > $(DEPS)

dep: .depend

include ${DEPS}


