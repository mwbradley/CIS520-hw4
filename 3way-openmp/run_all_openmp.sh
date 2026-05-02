#!/bin/bash
#
# run_all_openmp.sh - Submit all OpenMP performance test combinations
#
# Runs across:
#   Threads: 1, 2, 4, 8, 16, 32
#   Memory: 64M, 128M, 512M, 1G, 1500M, 3G
#   Repeats: 4 per combination

REPEATS=4

for MEM in 64M 128M 512M 1G 1500M 3G; do
    for THREADS in 1 2 4 8 16 32; do
        for RUN in $(seq 1 $REPEATS); do
            echo "Submitting: ${THREADS} threads, ${MEM}/core (run ${RUN}/${REPEATS})"
            ./run_openmp.sh ${THREADS} ${MEM}
            sleep 1
        done
    done
done

echo ""
echo "All jobs submitted. Monitor with: squeue -u \$USER"
echo "Results will be appended to results_openmp.csv"