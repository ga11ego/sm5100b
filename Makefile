SHELL = /bin/sh


CC = gcc

#X_LIBS = @X_LIBS@ -lX11
#X_CFLAGS = @X_CFLAGS@


CFLAGS = -g 
LDFLAGS =  
LIBS = -ldl 


ALL=sm5100b tools

LIBOBJECTS=gsm.o atstring.o sms.o serial.o ulstr.o smslist.o gsm_sms.o
OBJECTS=main.o ${LIBOBJECTS}
CSOURCES=main.c gsm.c atstring.c sms.c serial.c ulstr.c smslist.c gsm_sms.c readsms.c getunreadsms.c delsms.c
HSOURCES=gsm.h atstring.h sms.h serial.h ulstr.h smslist.h gsm_sms.h

SOURCES=${CSOURCES} ${HSOURCES}

DEPS=.depend

all: ${ALL}


tools: readsms getunreadsms delsms sendsms

readsms: readsms.o ${LIBOBJECTS}
	${CC} readsms.o ${LIBOBJECTS} -o $@ ${LDFLAGS} ${LIBS} ${CFLAGS}

getunreadsms: getunreadsms.o ${LIBOBJECTS}
	${CC} getunreadsms.o ${LIBOBJECTS} -o $@ ${LDFLAGS} ${LIBS} ${CFLAGS}
	
delsms: delsms.o ${LIBOBJECTS}
	${CC} delsms.o ${LIBOBJECTS} -o $@ ${LDFLAGS} ${LIBS} ${CFLAGS}

sendsms: sendsms.o ${LIBOBJECTS}
	${CC} sendsms.o ${LIBOBJECTS} -o $@ ${LDFLAGS} ${LIBS} ${CFLAGS}

sm5100b: ${OBJECTS}
#	echo linking main application "-->" $@
	${CC} ${OBJECTS} -o $@  ${LDFLAGS} ${LIBS} ${CFLAGS}


.SUFFIXES: .c .o
.c.o:
#	echo $< "-->" $@ 
	$(CC) ${CFLAGS} -c $< -o $@ 



clean: 
	rm -rf *.o sm5100b readsms getunreadsms delsms sendsms .depend 



.depend: ${CSOURCES} ${HSOURCES}
	$(CC) ${CFLAGS}  -M $(CSOURCES) $(HSOURCES)  > $(DEPS)

dep: .depend

include ${DEPS}


