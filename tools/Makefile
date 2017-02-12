SHELL = /bin/sh


CC = gcc

#X_LIBS = @X_LIBS@ -lX11
#X_CFLAGS = @X_CFLAGS@


CFLAGS = -g 
LDFLAGS =  -L.. 
LIBS = -ldl -lsms


ALL=${TOOLS}

OBJECTS=
CSOURCES=readsms.c getunreadsms.c delsms.c sendsms.c checkcpin.c
HSOURCES=../gsm.h ../atstring.h ../sms.h ../serial.h ../ulstr.h ../smslist.h ../gsm_sms.h
STATICLIB=libsms.a
TOOLS=readsms getunreadsms delsms sendsms checkcpin 
SOURCES=${CSOURCES} ${HSOURCES}

DEPS=.depend

all: ${ALL}

${STATICLIB}:
	$(MAKE) -C .. libsms.a

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


.SUFFIXES: .c .o
.c.o:
#	echo $< "-->" $@ 
	$(CC) ${CFLAGS} -c $< -o $@ 



clean: 
	rm -rf *.o ${TOOLS} ${OBJECTS} .depend 



.depend: ${CSOURCES} ${HSOURCES}
	$(CC) ${CFLAGS}  -M $(CSOURCES) $(HSOURCES)  > $(DEPS)

dep: .depend

include ${DEPS}


