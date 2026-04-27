#define _POSIX_C_SOURCE 200809L
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
 
//#define NUM_THREADS 4
int NUM_THREADS;
 
#define INITIAL_CAPACITY 1200000
#define READ_BUF_SIZE 65536
 
int num_lines = 0;
char **lines = NULL;				// dynamically allocated line storage
int *max_vals = NULL;				// per-line max ASCII value
 
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
 
		/* grow array if needed */
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
 
	max_vals = calloc(num_lines, sizeof(int));
	if (!max_vals) {
		fprintf(stderr, "Error allocating max_vals array\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
	}
}
 
void find_max(int *rank)
{
	int i, j;
	unsigned char theChar;
	int max;
	int myID = *rank;
	
	int startPos = myID * (num_lines / NUM_THREADS);
	int endPos = startPos + (num_lines / NUM_THREADS);
	
	/* last rank picks up any remainder lines */
	if (myID == NUM_THREADS - 1) {
		endPos = num_lines;
	}
	
	fprintf(stderr, "rank = %d startPos = %d endPos = %d\n", myID, startPos, endPos);
 
	// find max ASCII value for each line in our chunk
	for (i = startPos; i < endPos; i++) {
		max = 0;
		for (j = 0; lines[i][j] != '\0' && lines[i][j] != '\n'; j++) {
			theChar = (unsigned char) lines[i][j];
			if (theChar > max) max = theChar;
		}
		max_vals[i] = max;
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
 
	rc = MPI_Init(&argc,&argv);
	if (rc != MPI_SUCCESS) {
		fprintf(stderr, "Error starting MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
 
	MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
 
	NUM_THREADS = numtasks;
	fprintf(stderr, "size = %d rank = %d\n", numtasks, rank);
 
	/*
	 * Every rank reads the file independently.
	 * Beocat uses a shared filesystem (NFS), so all ranks
	 * can access wiki_dump.txt directly. This avoids the
	 * complexity and overhead of broadcasting 1.7GB of
	 * variable-length string data.
	 */
	init_arrays();
 
	fprintf(stderr, "Rank %d: read %d lines.\n", rank, num_lines);
 
	/* synchronize before timing the computation */
	MPI_Barrier(MPI_COMM_WORLD);
 
	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);
 
	/* each rank computes max for its chunk */
	find_max(&rank);
 
	/*
	 * Gather results to rank 0.
	 * Each rank computed max_vals for indices [startPos, endPos).
	 * We use MPI_Gatherv since the last rank may have extra lines.
	 */
	int chunk_size = num_lines / numtasks;
	int *recvcounts = NULL;
	int *displs = NULL;
 
	if (rank == 0) {
		recvcounts = malloc(numtasks * sizeof(int));
		displs = malloc(numtasks * sizeof(int));
		for (i = 0; i < numtasks; i++) {
			displs[i] = i * chunk_size;
			recvcounts[i] = chunk_size;
		}
		/* last rank gets the remainder */
		recvcounts[numtasks - 1] = num_lines - (numtasks - 1) * chunk_size;
	}
 
	int my_count = chunk_size;
	if (rank == numtasks - 1) {
		my_count = num_lines - (numtasks - 1) * chunk_size;
	}
 
	MPI_Gatherv(&max_vals[rank * chunk_size], my_count, MPI_INT,
		    max_vals, recvcounts, displs, MPI_INT,
		    0, MPI_COMM_WORLD);
 
	clock_gettime(CLOCK_MONOTONIC, &end);
 
	double elapsed =
		(end.tv_sec - start.tv_sec) +
		(end.tv_nsec - start.tv_nsec) / 1e9;
 
	if (rank == 0) {
		print_results();
		fprintf(stderr, "Elapsed time: %.6f seconds\n", elapsed);
	}
 
	/* cleanup */
	for (i = 0; i < num_lines; i++)
		free(lines[i]);
	free(lines);
	free(max_vals);
	if (rank == 0) {
		free(recvcounts);
		free(displs);
	}
 
	MPI_Finalize();
	return 0;
}