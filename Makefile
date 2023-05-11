#
# Makefile for make
#
# Compiler: gcc
#

NAME_EXEC = run
PROGS   = sistemaDomotico
CFLAGST = -D_REENTRANT -Wall
LIBS    = pthread

all: $(PROGS)

sistemaDomotico: phtrdsMsgLyr.o sistemaDomotico.o
	$(CC) $(CFLAGST) -o $(NAME_EXEC) sistemaDomotico.o phtrdsMsgLyr.o -l $(LIBS)

phtrdsMsgLyr.o: phtrdsMsgLyr.c phtrdsMsgLyr.h pMLusrConf.h
	$(CC) $(CFLAGST) -g -c phtrdsMsgLyr.c

sistemaDomotico.o: sistemaDomotico.c pMLusrConf.h
	$(CC) $(CFLAGST) -g -c sistemaDomotico.c

clean:
	rm -f $(PROGS) $(NAME_EXEC) *.o
