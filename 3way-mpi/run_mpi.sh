#!/bin/bash
#
# run_pthread.sh - Submit a mpi max-char job to Slurm on the Mole nodes
#
# Usage:  ./run_mpi.sh [num_threads] [mem_per_core] [num_nodes]
#   e.g.  ./run_mpi.sh 4 1G 1
#         ./run_mpi.sh 16 512M 2
#
# Defaults: 4 threads, 1G per core

THREADS=${1:-4}
MEM=${2:-1G}
NODES=${3:-1}

sbatch <<EOF
#!/bin/bash
#SBATCH --job-name=pt2_t${THREADS}_m${MEM}_n${NODES}
#SBATCH --output=pt2_output.out
#SBATCH --error=pt2_error.err
#SBATCH --nodes=${NODES}
#SBATCH --ntasks=${THREADS}
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=${MEM}
#SBATCH --constraint=moles
#SBATCH --time=04:00:00

module load CMake/3.23.1-GCCcore-11.3.0 foss/2022a

echo "=== mpi max-char ==="
echo "Threads:      ${THREADS}"
echo "Mem/core:     ${MEM}"
echo "Host:         $(hostname)"
echo "Start:	    $(date)"
echo "========================"

CSV=results.csv
if [ ! -f "\$CSV" ]; then
    echo "threads,mem,time" >> "\$CSV"
fi

TIMEFORMAT='%R'
ELAPSED=$( { time srun ./pt2; } 2>&1 )

WALL_TIME=\$(echo "\$ELAPSED" | tail -1)

echo "End: \$(date)"

echo "${THREADS},${MEM},\$WALL_TIME" >> "\$CSV"
EOF

echo "Submitted: ${THREADS} threads, ${MEM}/core"
