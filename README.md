# pthreads

## Setup
Before running any jobs, initialize the environment on Beocat:

./setup.sh

This script loads the required modules needed for compilation and execution.

## Running Pthreads

Jobs are submitted using the run_pthread.sh script, which launches a Slurm job on the Mole nodes.

Syntax
./run_pthread.sh <num_threads> <mem_per_core>
Examples
./run_pthread.sh 4 1G
./run_pthread.sh 16 512M
./run_pthread.sh 1 1500M
Default Settings

If no arguments are provided, the script uses:

Threads: 4
Memory per core: 1G

Example:

./run_pthread.sh

is equivalent to:

./run_pthread.sh 4 1G
What the Script Does

The run_pthread.sh script:

Submits a Slurm job to Mole nodes
Sets thread count and memory limits
Executes the pthreads program (pt1)
Logs output to .out files
Logs errors to .err files
Measures execution time using time
Output Files

Each job generates:

pt1_t*_m*_*.out
pt1_t*_m*_*.err

Example:

pt1_t4_m1G_1234567.out
pt1_t4_m1G_1234567.err


## Collecting Results

After running multiple configurations, generate a CSV file for analysis:

./make_csv.sh

This produces:

results.csv

containing:

thread count
memory allocation
execution time

Generate performance graphs using Python:

python graph.py

This outputs .png files showing:

speedup vs threads
comparisons across memory configurations