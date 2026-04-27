#!/bin/bash
#
# run_mpi.sh - Submit an MPI max-char job to Slurm on the Mole nodes
#
# Usage:  ./run_mpi.sh [num_ranks] [mem_per_core] [num_nodes]
#   e.g.  ./run_mpi.sh 4 1G 1
#         ./run_mpi.sh 8 512M 2
#
# Defaults: 4 ranks, 1G per core, 1 node
 
RANKS=${1:-4}
MEM=${2:-1G}
NODES=${3:-1}
 
sbatch <<EOF
#!/bin/bash
#SBATCH --job-name=pt2_r${RANKS}_n${NODES}
#SBATCH --output=pt2_r${RANKS}_n${NODES}_m${MEM}_%j.out
#SBATCH --error=pt2_r${RANKS}_n${NODES}_m${MEM}_%j.err
#SBATCH --nodes=${NODES}
#SBATCH --ntasks=${RANKS}
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=${MEM}
#SBATCH --constraint=moles
#SBATCH --time=04:00:00
 
module load CMake/3.23.1-GCCcore-11.3.0 foss/2022a OpenMPI/4.1.4-GCC-11.3.0
 
echo "=== MPI max-char ==="
echo "Ranks:        ${RANKS}"
echo "Nodes:        ${NODES}"
echo "Mem/core:     ${MEM}"
echo "Host:         \$(hostname)"
echo "Start:        \$(date)"
echo "===================="
 
CSV=results_mpi.csv
if [ ! -f "\$CSV" ]; then
    echo "ranks,nodes,mem,time" > "\$CSV"
fi
 
TIMEFORMAT='%R'
ELAPSED=\$( { time srun ../pt2; } 2>&1 | grep -v "rank\|size\|Read\|startPos\|Elapsed" )
 
WALL_TIME=\$(echo "\$ELAPSED" | tail -1)
 
echo "${RANKS},${NODES},${MEM},\$WALL_TIME" >> "\$CSV"
 
echo "End: \$(date)"
EOF
 
echo "Submitted: ${RANKS} ranks, ${NODES} nodes, ${MEM}/core"