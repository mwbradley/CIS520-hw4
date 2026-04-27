#!/bin/bash
#
# verify_mpi.sh - Verify pt2 (MPI) correctness
#
# Compares MPI output against pt1 (pthreads) reference output.
# Also checks that output is identical across different rank counts.
#
# Run from the 3way-mpi/ directory after building from project root with 'make'
 
set -e
 
echo "=== Step 1: Generate pthreads reference output ==="
echo "Running pt1 with 1 thread..."
../pt1 1 > ref_pthread.txt 2>/dev/null
 
echo ""
echo "=== Step 2: Test MPI across rank counts ==="
 
PASS=true
for R in 1 2 4; do
    echo -n "Running pt2 with ${R} ranks... "
    mpirun -np ${R} ../pt2 > mpi_r${R}.txt 2>/dev/null
 
    if diff -q ref_pthread.txt mpi_r${R}.txt > /dev/null 2>&1; then
        echo "PASS (matches pthreads output)"
    else
        echo "FAIL (differs from pthreads output)"
        diff ref_pthread.txt mpi_r${R}.txt | head -10
        PASS=false
    fi
done
 
echo ""
if [ "$PASS" = true ]; then
    echo "=== ALL TESTS PASSED ==="
else
    echo "=== SOME TESTS FAILED ==="
fi
 
rm -f ref_pthread.txt mpi_r*.txt