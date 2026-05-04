#define _POSIX_C_SOURCE 200809L
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
 
int NUM_THREADS = 4;
 
#define INITIAL_CAPACITY 1200000
#define READ_BUF_SIZE 65536
 
char **lines = NULL;				// dynamically allocated line storage
int *max_vals = NULL;				// per-line max ASCII value
 
int num_lines = 0;
 
void init_arrays(int max_input_size)
{
	int capacity = INITIAL_CAPACITY;
	char buf[READ_BUF_SIZE];
 
	FILE* f = fopen("/homes/eyv/cis520/wiki_dump.txt", "r");
	
	if (f == NULL) {
		printf("Error opening file\n");
		exit(1);
	}
 
	lines = malloc(capacity * sizeof(char *));
	if (!lines) {
		printf("Error allocating lines array\n");
		exit(1);
	}
 
	while (num_lines < max_input_size && fgets(buf, sizeof(buf), f) != NULL) {
		size_t len = strnlen(buf, READ_BUF_SIZE);
 
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
 
void *find_max(void *myID)
{
  int i, j;
  unsigned char theChar;
  int max;
 
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
		 if ( theChar > max ) {
		 	max = theChar;
		 }
	}
	max_vals[i] = max;
  }
 
  /* no mutex needed - each thread writes to its own indices */
 
  pthread_exit(NULL);
}
 
void print_results()
{
  int i;
 
  for ( i = 0; i < num_lines; i++ ) {
     printf("%d: %d\n", i, max_vals[i]);
  }
}
 
int main(int argc, char* argv[])
{
	int i, rc;
	pthread_attr_t attr;
	void *status;
 
	/* optional thread count from command line */
	if ( argc > 1 ) {
		sscanf(argv[1], "%d", &NUM_THREADS);
		if ( NUM_THREADS < 1 ) NUM_THREADS = 1;
	}

	int max_input_size = 2000000; // default to more than the whole file
	if ( argc > 2 ) {
		sscanf(argv[2], "%d", &max_input_size);
	}

	pthread_t *threads = malloc(NUM_THREADS * sizeof(pthread_t));
	if ( !threads ) {
		printf("ERROR; could not allocate thread array\n");
		exit(-1);
	}
 
	init_arrays(100000);
	/*Caps threads*/ 
 	if ( NUM_THREADS > num_lines ) NUM_THREADS = num_lines;

	/* Initialize and set thread detached attribute */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	fprintf(stderr, "Read %d lines, using %d threads.\n", num_lines, NUM_THREADS);
 
	struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
 
	for (i = 0; i < NUM_THREADS; i++ ) {
	      rc = pthread_create(&threads[i], &attr, find_max, (void *)(long)i);
	      if (rc) {
	        printf("ERROR; return code from pthread_create() is %d\n", rc);
		exit(-1);
	      }
	}
 
	/* Free attribute and wait for the other threads */
	pthread_attr_destroy(&attr);
	for(i=0; i<NUM_THREADS; i++) {
	     rc = pthread_join(threads[i], &status);
	     if (rc) {
		   printf("ERROR; return code from pthread_join() is %d\n", rc);
		   exit(-1);
	     }
	}
 
	print_results();

	clock_gettime(CLOCK_MONOTONIC, &end);

	double elapsed =
		(end.tv_sec - start.tv_sec) +
		(end.tv_nsec - start.tv_nsec) / 1e9;

	printf("Elapsed time: %.6f seconds\n", elapsed);
 
	/* cleanup */
	for (i = 0; i < num_lines; i++)
		free(lines[i]);
	free(lines);
	free(max_vals);
	free(threads);
	fprintf(stderr, "Main: program completed. Exiting.\n");
 
	return 0;
}