SHELL = /bin/sh


CC = gcc

#X_LIBS = @X_LIBS@ -lX11
#X_CFLAGS = @X_CFLAGS@


CFLAGS = -g 
LDFLAGS =  -L. 
LIBS = -ldl -lsms


ALL=sm5100b tools sms_listen

LIBOBJECTS=gsm.o atstring.o sms.o serial.o ulstr.o smslist.o gsm_sms.o
OBJECTS=main.o ${LIBOBJECTS}
CSOURCES=main.c gsm.c atstring.c sms.c serial.c ulstr.c smslist.c gsm_sms.c readsms.c getunreadsms.c delsms.c
HSOURCES=gsm.h atstring.h sms.h serial.h ulstr.h smslist.h gsm_sms.h
TOOLS=readsms getunreadsms delsms sendsms checkcpin 
STATICLIB=libsms.a

SOURCES=${CSOURCES} ${HSOURCES}

DEPS=.depend

all: ${ALL}


tools: ${TOOLS}

${STATICLIB}: ${LIBOBJECTS}
	ar cr ${STATICLIB} ${LIBOBJECTS}

readsms: readsms.o ${STATICLIB}
	${CC} readsms.o -o $@ ${LDFLAGS} ${LIBS} ${CFLAGS}

getunreadsms: getunreadsms.o ${STATICLIB}
	${CC} getunreadsms.o  -o $@ ${LDFLAGS} ${LIBS} ${CFLAGS}
	
delsms: delsms.o ${STATICLIB}
	${CC} delsms.o  -o $@ ${LDFLAGS} ${LIBS} ${CFLAGS}

sendsms: sendsms.o ${STATICLIB}
	${CC} sendsms.o  -o $@ ${LDFLAGS} ${LIBS} ${CFLAGS}

checkcpin: checkcpin.o ${STATICLIB}
	${CC} checkcpin.o  -o $@ ${LDFLAGS} ${LIBS} ${CFLAGS}

sms_listen: ${STATICLIB}
	$(MAKE) -C sms_listen all

sm5100b: main.o ${STATICLIB}
#	echo linking main application "-->" $@
	${CC} main.o -o $@  ${LDFLAGS} ${LIBS} ${CFLAGS}



.SUFFIXES: .c .o
.c.o:
#	echo $< "-->" $@ 
	$(CC) ${CFLAGS} -c $< -o $@ 



clean: 
	rm -rf *.o sm5100b ${TOOLS} ${STATICLIB} .depend 
	$(MAKE) -C sms_listen clean



.depend: ${CSOURCES} ${HSOURCES}
	$(CC) ${CFLAGS}  -M $(CSOURCES) $(HSOURCES)  > $(DEPS)

dep: .depend

include ${DEPS}


