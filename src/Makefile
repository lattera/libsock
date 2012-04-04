TARGET=libsock.so.1.0
SONAME=libsock.so.1
CC=gcc
CFLAGS=-fPIC -Wall -O2 -I../include -fvisibility=hidden
SRCS=libsock.c util.c
OBJS=libsock.o util.o

${TARGET}: ${OBJS}
	${CC} -shared -Wl,-soname,${SONAME} -o ${TARGET} ${OBJS}

install: ${TARGET}
	cp ${TARGET} ${PREFIX}/lib
	ln -sf ${PREFIX}/lib/${TARGET} ${PREFIX}/lib/libsock.so.1
	ln -sf ${PREFIX}/lib/${TARGET} ${PREFIX}/lib/libsock.so
	cp ../include/sock.h ${PREFIX}/include

deinstall:
	rm ${PREFIX}/lib/${TARGET}.so.1
	rm ${PREFIX}/lib/${TARGET}.so
	rm ${PREFIX}/lib/${TARGET}

clean:
	rm -f *.o
