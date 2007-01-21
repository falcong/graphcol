#Schema Makefile multipurpose

#Configurazioni generali

#Nome del programma
NAMEPROG = color

# ------------------------------------------------------------------------------

#Flag di compilazione

CC = gcc
CFLAGDEBUG = -O0 -g3 -Wall -fmessage-length=0
CFLAGRELEASE = -O3 -Wall -fmessage-length=0
CFLAGOBJ = -lm

#Flag per il Debug
CFLAG = $(CFLAGDEBUG)

#Flag per Release
#CFLAG = $(CFLAGRELEASE)

# ------------------------------------------------------------------------------

#PATH

VPATH = src:inc

#File programma
OBJECT = main.o routines.o node.o graph.o nodesList.o pnode.o pnodesList.o
SOURCES = $(INC) $(SRC)
INC =	routines.h node.h graph.h defs.h nodesList.h pnode.h pnodesList.h
SRC = main.c routines.c node.c graph.c nodesList.c pnode.c pnodesList.c

# ------------------------------------------------------------------------------

#Regole

default: cleanbin $(NAMEPROG) clean

$(NAMEPROG): $(OBJECT)
	@echo Link oggetto: $^
	gcc $(CFLAG) -o $(NAMEPROG) $^
	@echo

$(OBJECT): $(SOURCES)
	@echo Compile sorgenti: $^
	gcc $(CFLAG) -c $^
	@echo

cleanbin:
	@echo Rimozione binari
	rm -f $(NAMEPROG)

cleanobj:
	@echo Rimozione file oggetto
	rm -f $(OBJECT)

cleanbckp:
	@echo Rimozione file di backup
	rm -f *~ inc/*~ src/*~ inc/*.gch src/*.gch *.d
	@echo


.PHONY : clean
clean: cleanbin cleanobj cleanbckp

