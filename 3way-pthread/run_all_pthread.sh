#!/bin/bash
#
# run_all_pthread.sh - Submit all Pthreads performance test combinations
#
# Runs across:
#   Input Sizes: 1000, 10000, 100000, 1200000
#   Memory: 512M, 1G, 1500M, 3G
#   Threads: 1, 2, 4, 8, 16
#   Repeats: 4 per combination

REPEATS=4

for INPUT_SIZE in 1000 10000 100000 1200000; do
    for MEM in 512M 1G 1500M 3G; do
        for THREADS in 1 2 4 8 16; do
            for RUN in $(seq 1 $REPEATS); do
                echo "Submitting: ${THREADS} threads, ${MEM}/core, Size ${INPUT_SIZE} (run ${RUN}/${REPEATS})"
                
                ./run_pthread.sh ${THREADS} ${MEM} ${INPUT_SIZE}
                
                sleep 1
            done
        done
    done
done

echo ""
echo "All jobs submitted. Monitor with: squeue -u \$USER"
echo "Results will be appended to results.csv"