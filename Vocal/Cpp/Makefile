############################################################################
# FILE        : Makefile
# LAST REVISED: 2003-04-16
# AUTHOR      : (C) Copyright 2003 by Peter Chapin
#
# This is the makefile for the vocal language implementation.
############################################################################

vocalc: 	main.o node-types.o lex.yy.o vocal.tab.o
	g++ -g -o vocalc main.o node-types.o lex.yy.o vocal.tab.o -lfl

vocal.tab.cpp:	vocal.ypp
	bison -d vocal.ypp

vocal.tab.hpp:	vocal.ypp
	bison -d vocal.ypp

lex.yy.c:	vocal.l
	flex vocal.l

lex.yy.o:	lex.yy.c vocal.tab.hpp node-types.h
	g++ -x c++ -g -c lex.yy.c

vocal.tab.o:	vocal.tab.cpp vocal.tab.hpp node-types.h
	g++ -g -c vocal.tab.cpp

main.o:		main.cpp
	g++ -g -c main.cpp

node-types.o:	node-types.cpp node-types.h
	g++ -g -c node-types.cpp

#
# Other nicities.
#

clean:
	rm -f *.o

distclean:
	rm -f *.o
	rm -f *.exe
	rm -f vocal.tab.cpp vocal.tab.hpp lex.yy.c
