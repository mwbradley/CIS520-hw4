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
 
CC        = gcc
CFLAGS    = -Wall -O2 -std=c99
 
#
# Modify TOPDIR if you use your own include files and library files
#
 
PROGRAM   = pt1			# name of the binary
SRCS      = pt1.c		# source file
OBJS      = $(SRCS:.c=.o)		# object file
LDFLAGS   = -lpthread
 
#
# Targets
#
 
default: all
 
all: $(PROGRAM)
 
$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROGRAM) $(LDFLAGS)
 
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
 
clean:
	/bin/rm -f $(OBJS) $(PROGRAM)