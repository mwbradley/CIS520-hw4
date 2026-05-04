# pthreads

## Setup
Before running any jobs, initialize the environment on Beocat:

./setup.sh

This script loads the required modules needed for compilation and execution.

If this does not work it holds the dependencies that you will need to install and you can just do it on the command line.

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

### Running MPI

Jobs are submitted using the run_mpi.sh script, which launches a Slurm job on the Mole nodes.

Syntax
./run_pthread.sh <num_threads> <mem_per_core> <num_cores>
Examples
./run_pthread.sh 4 1G 1
./run_pthread.sh 16 512M 2
./run_pthread.sh 1 1500M 4
Default Settings

If no arguments are provided, the script uses:

Threads: 4
Memory per core: 1G
Cores: 1

Example:

./run_mpi.sh

is equivalent to:

./run_pthread.sh 4 1G 1
What the Script Does

The run_mpi.sh script:

Submits a Slurm job to Mole nodes
Sets thread count, memory limits and core count
Executes the mpi program (pt2)
Logs output to .out files
Logs errors to .err files
Measures execution time using time
Output Files

Each job generates:

pt2_t*_m*_*.out
pt2_t*_m*_*.err

Example:

pt2_t4_m1G_1234567.out
pt2_t4_m1G_1234567.err

Another script to run is run_all_mpi.sh which will run the different configurations four times to give results. Same output/error files and will be put into a results.csv


### Running OpenMP

Jobs are submitted using the run_openmp.sh script, which launches a Slurm job on the Mole nodes.

Syntax
./run_openmp.sh <num_threads> <mem_per_core>
Examples
./run_openmp.sh 4 1G
./run_openmp.sh 16 512M
./run_openmp.sh 1 1500M
Default Settings

If no arguments are provided, the script uses:

Threads: 4
Memory per core: 1G

Example:

./run_openmp.sh

is equivalent to:

./run_openmp.sh 4 1G
What the Script Does

The run_openmp.sh script:

Submits a Slurm job to Mole nodes
Sets thread count and memory limits
Executes the openmp program (pt3)
Logs output to .out files
Logs errors to .err files
Measures execution time using time
Output Files

Each job generates:

pt3_t*_m*_*.out
pt3_t*_m*_*.err

Example:

pt3_t4_m1G_1234567.out
pt3_t4_m1G_1234567.err

Another script to run is run_all_openmp.sh which will run the different configurations four times to give results. Same output/error files and will be put into a results.csv

## Collecting Results

After running multiple configurations, generate a CSV file for analysis:

./make_csv.sh -- outdated, after pt1, pt2, pt3 runs it will automatically make a results.csv file to hold the information

This produces:

results.csv

containing:

thread count
memory allocation
execution time

Generate performance graphs using Python:

You will need to install pandas and module load SciPy-bundle/2022.05-foss-2022a to run the graph scripts.

python graph.py

This outputs .png files showing:

speedup vs threads
comparisons across memory configurations