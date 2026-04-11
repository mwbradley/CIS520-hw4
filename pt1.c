#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
int NUM_THREADS = 4;
 
#define ARRAY_SIZE 2000000
#define STRING_SIZE 65536
 
char char_array[ARRAY_SIZE][STRING_SIZE];
int max_vals[ARRAY_SIZE];			// per-line max ASCII value
 
int num_lines = 0;
 
void init_arrays()
{
	int i;
 
	FILE* f = fopen("/homes/eyv/cis520/wiki_dump.txt", "r");
	
	if (f == NULL) {
		printf("Error opening file\n");
		exit(1);
	}
 
	while (num_lines < ARRAY_SIZE && fgets(char_array[num_lines], STRING_SIZE, f) != NULL) {
    num_lines++;
  }
 
  fclose(f);
 
  for ( i = 0; i < num_lines; i++ ) {
  	max_vals[i] = 0;
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
 
  printf("myID = %ld startPos = %d endPos = %d \n", (long) myID, startPos, endPos);
 
					// find max ASCII value for each line in our chunk
  for ( i = startPos; i < endPos; i++) {
	max = 0;
	for ( j = 0; char_array[i][j] != '\0' && char_array[i][j] != '\n'; j++ ) {
	         theChar = (unsigned char) char_array[i][j];
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
		NUM_THREADS = atoi(argv[1]);
		if ( NUM_THREADS < 1 ) NUM_THREADS = 1;
	}
 
	pthread_t *threads = malloc(NUM_THREADS * sizeof(pthread_t));
	if ( !threads ) {
		printf("ERROR; could not allocate thread array\n");
		exit(-1);
	}
 
	/* Initialize and set thread detached attribute */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
 
	init_arrays();
 
	fprintf(stderr, "Read %d lines, using %d threads.\n", num_lines, NUM_THREADS);
 
	/* cap threads to line count */
	if ( NUM_THREADS > num_lines ) NUM_THREADS = num_lines;
 
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
 
	free(threads);
	printf("Main: program completed. Exiting.\n");
 
	return 0;
}