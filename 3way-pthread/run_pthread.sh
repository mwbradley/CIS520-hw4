#!/bin/bash
#
# run_pthread.sh - Submit a pthreads max-char job to Slurm on the Mole nodes
#
# Usage:  ./run_pthread.sh [num_threads] [mem_per_core]
#   e.g.  ./run_pthread.sh 4 1G
#         ./run_pthread.sh 16 512M
#
# Defaults: 4 threads, 1G per core

THREADS=${1:-4}
MEM=${2:-1G}

sbatch <<EOF
#!/bin/bash
#SBATCH --job-name=pt1_t${THREADS}
#SBATCH --output=pt1_t${THREADS}_m${MEM}.out
#SBATCH --error=pt1_t${THREADS}_m${MEM}.err
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=${THREADS}
#SBATCH --mem-per-cpu=${MEM}
#SBATCH --constraint=moles
#SBATCH --time=04:00:00

module load CMake/3.23.1-GCCcore-11.3.0 foss/2022a

echo "=== pthread max-char ==="
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
ELAPSED=\$( { time ./pt1 ${THREADS}; } 2>&1 )

WALL_TIME=\$(echo "\$ELAPSED" | tail -1)

echo "End: \$(date)"

echo "${THREADS},${MEM},\$WALL_TIME" >> "\$CSV"
EOF

echo "Submitted: ${THREADS} threads, ${MEM}/core"
