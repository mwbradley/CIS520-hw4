#
# Open Systems Lab
# http://www.lam-mpi.org/tutorials/
# Indiana University
#
# MPI Tutorial
# Sample Makefile
#
 
# "mpicc" adds the directories for the include and lib files.  Hence,
# -I and -L for MPI stuff is not necessary
#
 
CFLAGS    = -Wall -O2 -std=c99
 
#
# Targets
#
 
default: all
 
all: pt1 pt2
 
pt1: pt1.c
	gcc $(CFLAGS) -o pt1 pt1.c -lpthread
 
pt2: pt2.c
	mpicc $(CFLAGS) -o pt2 pt2.c
 
clean:
	/bin/rm -f pt1 pt2