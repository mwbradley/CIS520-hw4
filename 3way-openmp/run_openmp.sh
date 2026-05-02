#!/bin/bash
#
# run_openmp.sh - Submit an OpenMP max-char job to Slurm on the Mole nodes
#
# Usage:  ./run_openmp.sh [num_threads] [mem_per_core]
#   e.g.  ./run_openmp.sh 4 1G
#         ./run_openmp.sh 16 512M
#
# Defaults: 4 threads, 1G per core

THREADS=${1:-4}
MEM=${2:-1G}

sbatch <<EOF
#!/bin/bash
#SBATCH --job-name=pt3_t${THREADS}
#SBATCH --output=pt3_t${THREADS}_m${MEM}_%j.out
#SBATCH --error=pt3_t${THREADS}_m${MEM}_%j.err
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=${THREADS}
#SBATCH --mem-per-cpu=${MEM}
#SBATCH --constraint=moles
#SBATCH --time=04:00:00

module load CMake/3.23.1-GCCcore-11.3.0 foss/2022a

echo "=== OpenMP max-char ==="
echo "Threads:      ${THREADS}"
echo "Mem/core:     ${MEM}"
echo "Host:         \$(hostname)"
echo "Start:        \$(date)"
echo "========================"

CSV=results_openmp.csv
if [ ! -f "\$CSV" ]; then
    echo "threads,mem,time" > "\$CSV"
fi

TIMEFORMAT='%R'
ELAPSED=\$( { time ./pt3 ${THREADS}; } 2>&1 | grep -v "Read\|Fork-Join\|Elapsed" )

WALL_TIME=\$(echo "\$ELAPSED" | tail -1)

echo "End: \$(date)"

echo "${THREADS},${MEM},\$WALL_TIME" >> "\$CSV"
EOF

echo "Submitted: ${THREADS} threads, ${MEM}/core"