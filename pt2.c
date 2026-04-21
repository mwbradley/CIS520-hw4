#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define NUM_THREADS 4
int NUM_THREADS;

#define ARRAY_SIZE 1200000 // took this from pt1
#define STRING_SIZE 65536 // took this from pt1
#define ALPHABET_SIZE 26

int char_counts[ALPHABET_SIZE];			// global count of individual characters
int local_char_count[ALPHABET_SIZE];
int num_lines = 0;
char **lines = NULL;				// dynamically allocated line storage
int *max_vals = NULL;				// per-line max ASCII value

void init_arrays()
{
	int capacity = ARRAY_SIZE;
	char buf[STRING_SIZE];
 
	FILE* f = fopen("/homes/eyv/cis520/wiki_dump.txt", "r");
	if (f == NULL) { // error checking
		printf("Error opening file\n");
		exit(1);
	}

	printf("Initializing arrays.\n"); fflush(stdout);

	lines = malloc(capacity * sizeof(char *));
	if (!lines) {
		printf("Error allocating lines array\n");
		exit(1);
	}
 
	while (fgets(buf, sizeof(buf), f) != NULL) {
		size_t len = strnlen(buf, STRING_SIZE);
 
		/* grow array if needed */
		if (num_lines >= capacity) {
			capacity *= 2;
			char **tmp = realloc(lines, capacity * sizeof(char *));
			if (!tmp) {
				printf("Error reallocating lines array\n");
				exit(1);
			}
			lines = tmp;
		}
 
		lines[num_lines] = malloc(len + 1);
		if (!lines[num_lines]) {
			printf("Error allocating line %d\n", num_lines);
			exit(1);
		}
		memcpy(lines[num_lines], buf, len + 1);
		num_lines++;
	}
 
	fclose(f);
 
	max_vals = calloc(num_lines, sizeof(int));
	if (!max_vals) {
		printf("Error allocating max_vals array\n");
		exit(1);
	}
}

void count_array(int *rank)
{
	int i, j;
	unsigned char theChar;
	int max;
	long myID = (long)(*rank);
	
	int startPos = ((long) myID) * (num_lines / NUM_THREADS);
	int endPos = startPos + (num_lines / NUM_THREADS);
	
	/* last thread picks up any remainder lines */
	if ( ((long) myID) == NUM_THREADS - 1 ) {
		endPos = num_lines;
	}
	
	fprintf(stderr, "myID = %ld startPos = %d endPos = %d \n", (long) myID, startPos, endPos);
 
	// find max ASCII value for each line in our chunk
	for ( i = startPos; i < endPos; i++) {
		max = 0;
		for ( j = 0; lines[i][j] != '\0' && lines[i][j] != '\n'; j++ ) {
			theChar = (unsigned char) lines[i][j];
			if (theChar > max) max = theChar;
			if (theChar >= 'a' && theChar <= 'z') local_char_count[theChar - 'a']++;
		}
		max_vals[i] = max;
	}
}

void print_results()
{
	int i;
 
  	for ( i = 0; i < ALPHABET_SIZE; i++ ) {
    	printf("%c: %d\n", 'a' + i, char_counts[i]);

  	}
}

int main(int argc, char* argv[]) 
{
	int rc;
	int numtasks, rank;
	//MPI_Status Status;


	rc = MPI_Init(&argc,&argv);
	if (rc != MPI_SUCCESS) {
	  printf ("Error starting MPI program. Terminating.\n");
          MPI_Abort(MPI_COMM_WORLD, rc);
        }

        MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
        MPI_Comm_rank(MPI_COMM_WORLD,&rank);

	NUM_THREADS = numtasks;
	printf("size = %d rank = %d\n", numtasks, rank);
	fflush(stdout);

	if ( rank == 0 ) {
		init_arrays();
	}
	MPI_Bcast(&num_lines, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	if (rank != 0) {
    	lines = malloc(num_lines * sizeof(char *));
	}
	for (int i = 0; i < num_lines; i++) {
		int len;
		if (rank == 0) len = strlen(lines[i]) + 1 {
			MPI_Bcast(&len, 1, MPI_INT, 0, MPI_COMM_WORLD);
		}
		if (rank != 0) lines[i] = malloc(len) {
			MPI_Bcast(lines[i], len, MPI_CHAR, 0, MPI_COMM_WORLD);
		}
	}
		
	count_array(&rank);

	MPI_Reduce(local_char_count, char_counts, ALPHABET_SIZE, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);


	if ( rank == 0 ) {
		print_results();
	}

	MPI_Finalize();
	return 0;
}