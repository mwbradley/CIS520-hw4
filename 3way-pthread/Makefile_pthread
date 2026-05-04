#
# Open Systems Lab
# http://www.lam-mpi.org/tutorials/
# Indiana University
#
# MPI Tutorial
# Sample Makefile
#

# Builds the pthreads version (pt1) from the source in the project root.
#

CFLAGS    = -Wall -O2 -std=c99
SRC       = ../pt1.c
TARGET    = pt1

default: all

all: $(TARGET)

$(TARGET): $(SRC)
	gcc $(CFLAGS) -o $(TARGET) $(SRC) -lpthread

clean:
	/bin/rm -f $(TARGET)
