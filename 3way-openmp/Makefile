#
# Open Systems Lab
# http://www.lam-mpi.org/tutorials/
# Indiana University
#
# MPI Tutorial
# Sample Makefile
#

# Builds the OpenMP version (pt3) from the source in the project root.
#

CFLAGS    = -Wall -O2 -std=c99 -fopenmp
SRC       = ../pt3.c
TARGET    = pt3

default: all

all: $(TARGET)

$(TARGET): $(SRC)
	gcc $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	/bin/rm -f $(TARGET)