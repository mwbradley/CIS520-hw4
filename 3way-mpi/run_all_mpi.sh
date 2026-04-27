#!/bin/bash
#
# run_all_mpi.sh - Submit all MPI performance test combinations
#
# Runs across:
#   Ranks: 1, 2, 4, 8, 16
#   Nodes: 1, 2, 4
#   Memory: 512M, 1G, 1500M, 3G
#   Repeats: 4 per combination
#
# Note: Some combinations may fail (e.g., more nodes than ranks,
#       or not enough resources). That's expected per the spec.

REPEATS=4

for MEM in 512M 1G 1500M 3G; do
    for NODES in 1 2 4; do
        for RANKS in 1 2 4 8 16; do
            # skip if fewer ranks than nodes
            if [ $RANKS -lt $NODES ]; then
                continue
            fi
            for RUN in $(seq 1 $REPEATS); do
                echo "Submitting: ${RANKS} ranks, ${NODES} nodes, ${MEM}/core (run ${RUN}/${REPEATS})"
                ./run_mpi.sh ${RANKS} ${MEM} ${NODES}
                sleep 1
            done
        done
    done
done

echo ""
echo "All jobs submitted. Monitor with: squeue -u \$USER"
echo "Results will be appended to results_mpi.csv"
