#define _POSIX_C_SOURCE 200809L
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
 
int NUM_THREADS;
 
#define INITIAL_CAPACITY 1200000
#define READ_BUF_SIZE 65536
 
int num_lines = 0;
char **lines = NULL;				// dynamically allocated line storage
int *max_vals = NULL;				// per-line max ASCII value (rank 0 only, full size)
int *local_max_vals = NULL;			// per-rank local results
 
void init_arrays()
{
	int capacity = INITIAL_CAPACITY;
	char buf[READ_BUF_SIZE];
 
	FILE* f = fopen("/homes/eyv/cis520/wiki_dump.txt", "r");
	if (f == NULL) {
		fprintf(stderr, "Error opening file\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
	}
 
	lines = malloc(capacity * sizeof(char *));
	if (!lines) {
		fprintf(stderr, "Error allocating lines array\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
	}
 
	while (fgets(buf, sizeof(buf), f) != NULL) {
		size_t len = strnlen(buf, READ_BUF_SIZE);
 
		if (num_lines >= capacity) {
			capacity *= 2;
			char **tmp = realloc(lines, capacity * sizeof(char *));
			if (!tmp) {
				fprintf(stderr, "Error reallocating lines array\n");
				MPI_Abort(MPI_COMM_WORLD, 1);
			}
			lines = tmp;
		}
 
		lines[num_lines] = malloc(len + 1);
		if (!lines[num_lines]) {
			fprintf(stderr, "Error allocating line %d\n", num_lines);
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
		memcpy(lines[num_lines], buf, len + 1);
		num_lines++;
	}
 
	fclose(f);
}
 
void find_max(int rank, int chunk_size, int my_count)
{
	int i, j;
	unsigned char theChar;
	int max;
	
	int startPos = rank * chunk_size;
	int endPos = startPos + my_count;
	
	fprintf(stderr, "rank = %d startPos = %d endPos = %d\n", rank, startPos, endPos);
 
	/* find max ASCII value for each line in our chunk */
	for (i = 0; i < my_count; i++) {
		max = 0;
		int line_idx = startPos + i;
		for (j = 0; lines[line_idx][j] != '\0' && lines[line_idx][j] != '\n'; j++) {
			theChar = (unsigned char) lines[line_idx][j];
			if (theChar > max) max = theChar;
		}
		local_max_vals[i] = max;
	}
}
 
void print_results()
{
	int i;
 
	for (i = 0; i < num_lines; i++) {
		printf("%d: %d\n", i, max_vals[i]);
	}
}
 
int main(int argc, char* argv[]) 
{
	int i, rc;
	int numtasks, rank;
 
	rc = MPI_Init(&argc, &argv);
	if (rc != MPI_SUCCESS) {
		fprintf(stderr, "Error starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
 
	MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
 
	NUM_THREADS = numtasks;
	fprintf(stderr, "size = %d rank = %d\n", numtasks, rank);
 
	/*
	 * Every rank reads the file independently.
	 * Beocat uses NFS, so all ranks can access the file directly.
	 * This avoids the overhead of broadcasting 1.7GB of strings.
	 */
	init_arrays();
 
	fprintf(stderr, "Rank %d: read %d lines.\n", rank, num_lines);
 
	/* compute chunk sizes */
	int chunk_size = num_lines / numtasks;
	int my_count = chunk_size;
	if (rank == numtasks - 1) {
		my_count = num_lines - (numtasks - 1) * chunk_size;
	}
 
	/* allocate local results buffer for this rank's chunk */
	local_max_vals = calloc(my_count, sizeof(int));
	if (!local_max_vals) {
		fprintf(stderr, "Error allocating local_max_vals on rank %d\n", rank);
		MPI_Abort(MPI_COMM_WORLD, 1);
	}
 
	/* synchronize before timing */
	MPI_Barrier(MPI_COMM_WORLD);
 
	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);
 
	/* compute max for this rank's chunk into local_max_vals */
	find_max(rank, chunk_size, my_count);
 
	/* gather all local results into max_vals on rank 0 */
	int *recvcounts = NULL;
	int *displs = NULL;
 
	if (rank == 0) {
		max_vals = malloc(num_lines * sizeof(int));
		recvcounts = malloc(numtasks * sizeof(int));
		displs = malloc(numtasks * sizeof(int));
		if (!max_vals || !recvcounts || !displs) {
			fprintf(stderr, "Error allocating gather buffers\n");
			MPI_Abort(MPI_COMM_WORLD, 1);
		}
		for (i = 0; i < numtasks; i++) {
			displs[i] = i * chunk_size;
			recvcounts[i] = chunk_size;
		}
		recvcounts[numtasks - 1] = num_lines - (numtasks - 1) * chunk_size;
	}
 
	MPI_Gatherv(local_max_vals, my_count, MPI_INT,
		    max_vals, recvcounts, displs, MPI_INT,
		    0, MPI_COMM_WORLD);
 
	clock_gettime(CLOCK_MONOTONIC, &end);
 
	double elapsed =
		(end.tv_sec - start.tv_sec) +
		(end.tv_nsec - start.tv_nsec) / 1e9;
 
	if (rank == 0) {
		print_results();
		printf("Elapsed time: %.6f seconds\n", elapsed);
	}
 
	/* cleanup */
	for (i = 0; i < num_lines; i++)
		free(lines[i]);
	free(lines);
	free(local_max_vals);
	if (rank == 0) {
		free(max_vals);
		free(recvcounts);
		free(displs);
	}
 
	MPI_Finalize();
	return 0;
}