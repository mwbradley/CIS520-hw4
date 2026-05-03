#define _POSIX_C_SOURCE 200809L
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int NUM_THREADS = 4;

#define INITIAL_CAPACITY 1200000
#define READ_BUF_SIZE 65536

int num_lines = 0;
char **lines = NULL;                // dynamically allocated line storage
int *max_vals = NULL;               // shared per-line max ASCII value

void init_arrays(int max_lines)
{
    int capacity = INITIAL_CAPACITY;
    char buf[READ_BUF_SIZE];

    FILE* f = fopen("/homes/eyv/cis520/wiki_dump.txt", "r");
    if (f == NULL) {
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }

    lines = malloc(capacity * sizeof(char *));
    if (!lines) {
        fprintf(stderr, "Error allocating lines array\n");
        exit(1);
    }

    while (fgets(buf, sizeof(buf), f) != NULL && num_lines < max_lines) {
        size_t len = strnlen(buf, READ_BUF_SIZE);

        if (num_lines >= capacity) {
            capacity *= 2;
            char **tmp = realloc(lines, capacity * sizeof(char *));
            if (!tmp) {
                fprintf(stderr, "Error reallocating lines array\n");
                exit(1);
            }
            lines = tmp;
        }

        lines[num_lines] = malloc(len + 1);
        if (!lines[num_lines]) {
            fprintf(stderr, "Error allocating line %d\n", num_lines);
            exit(1);
        }
        memcpy(lines[num_lines], buf, len + 1);
        num_lines++;
    }

    fclose(f);

    // Allocate the shared array to store the max values
    max_vals = calloc(num_lines, sizeof(int));
    if (!max_vals) {
        fprintf(stderr, "Error allocating max_vals array\n");
        exit(1);
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
    int i;

    /* Read thread count from command line if provided */
    if (argc > 1) {
        sscanf(argv[1], "%d", &NUM_THREADS);
        if (NUM_THREADS < 1) NUM_THREADS = 1;
    }

    /* Method to start up parallel threads dynamically */
    omp_set_num_threads(NUM_THREADS);

    init_arrays(100000);

    fprintf(stderr, "Read %d lines, using %d threads (OpenMP).\n", num_lines, NUM_THREADS);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    /* * FORK: Master thread creates a team of threads.
     */
    #pragma omp parallel default(shared)
    {
        int my_id = omp_get_thread_num();
        
        int active_threads = omp_get_num_threads();

        // Print diagnostic info from each thread once
        #pragma omp single
        {
            fprintf(stderr, "Fork-Join: Master thread forked %d active threads.\n", active_threads);
        }

        /* * Work-sharing construct: Divides the loop among the forked threads.
         */
        #pragma omp for
        for (i = 0; i < num_lines; i++) {
            int max = 0;
            int j;
            unsigned char theChar;
            
            for (j = 0; lines[i][j] != '\0' && lines[i][j] != '\n'; j++) {
                theChar = (unsigned char) lines[i][j];
                if (theChar > max) max = theChar;
            }
            
            max_vals[i] = max;
        }
    } /* JOIN: Threads synchronize and terminate, leaving only master thread */

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed =
        (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec) / 1e9;

    print_results();
    printf("Elapsed time: %.6f seconds\n", elapsed);

    /* cleanup */
    for (i = 0; i < num_lines; i++) {
        free(lines[i]);
    }
    free(lines);
    free(max_vals);

    return 0;
}