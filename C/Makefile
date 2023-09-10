PROG=	sws
OBJS=	main.o request.o server.o client.o response.o
CC = gcc

# If commented out, defaults will be used. If uncommented, these values
# will be used, even though there is no other explicit mention of this
# variable in this Makefile.
CFLAGS=	-Wall -g

all: ${PROG}

showvars:
	@echo "CC=${CC}"
	@echo "CFLAGS=${CFLAGS}"
	@echo "LDFLAGS=${LDFLAGS}"
	@echo "PROG=${PROG}"
	@echo "OBJS=${OBJS}"

${PROG}: ${OBJS}
	@echo $@ depends on $?
	${CC} ${CFLAGS} ${OBJS} -o ${PROG}

clean:
	rm -f sws *.o