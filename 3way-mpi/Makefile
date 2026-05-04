#
# Open Systems Lab
# http://www.lam-mpi.org/tutorials/
# Indiana University
#
# MPI Tutorial
# Sample Makefile
#

# Builds the MPI version (pt2) from the source in the project root.
#

CFLAGS    = -Wall -O2 -std=c99
SRC       = ../pt2.c
TARGET    = pt2

default: all

all: $(TARGET)

$(TARGET): $(SRC)
	mpicc $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	/bin/rm -f $(TARGET)