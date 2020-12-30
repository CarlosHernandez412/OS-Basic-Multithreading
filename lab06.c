#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      /* header file for the POSIX API */
#include <string.h>      /* string handling functions */
#include <errno.h>       /* for perror() call */
#include <pthread.h>     /* POSIX threads */


#define MAX_SIZE 	100000
#define MAX_NAME	256
#define NUM_THREADS 6

int array[MAX_SIZE];
int count;
typedef struct {
	int threadNum;
	int startIndex;
	int endIndex;
}	thread;

void *thread_func(void *ptr){
	
	thread *tmp = ptr;
	printf ("Thread data: id=%d start=%d end=%d  \n", tmp->threadNum, tmp->startIndex, tmp->endIndex);
	int i = 0;
	for(i = tmp->startIndex; i< tmp->endIndex - 1; i++)
		array[i] *=2;
	
	printf("Thread %d done with task.\n", tmp->threadNum);
	
	pthread_exit((void*)EXIT_SUCCESS);
}

int main(int argc, char *argv[]){

	int tmp;
	int ret;
	int rc;
	char filein[MAX_NAME];
	char fileout[MAX_NAME];
	FILE *infile;
	FILE *outfile;
	long t;
	
	pthread_t threads[NUM_THREADS]; 
	thread threadArray[NUM_THREADS];
	
	if (argc<3) {
	printf ("Usage: %s <input_file> <output_file>\n", argv[0]);
	exit(1);
}

	count = 0;
	strncpy(filein, argv[1], MAX_NAME);
	strncpy(fileout, argv[2], MAX_NAME);
	infile = fopen(filein, "r");
	if(infile == NULL) {
		perror ("fopen: ");
		exit(EXIT_FAILURE);
	}
	
	outfile = fopen(fileout, "w");
	if(outfile == NULL) {
		perror("fopen: ");
		exit (EXIT_FAILURE);
	}
	
	do {
		ret = fscanf(infile, "%d", &tmp);
		if(ret > 0) array[count++] = tmp;
	}while (ret != EOF && count < MAX_SIZE);
	printf ("Read in %d integers from file %s. \n", count, filein);
	fclose(infile);
	
	
	int chunk = count / NUM_THREADS;
	printf("Passing %d elements to each child thread. \n", chunk);
	
	for (t=0; t < NUM_THREADS; t++){
		threadArray[t].threadNum = t;
		threadArray[t].startIndex = t * chunk;
		if (t== NUM_THREADS - 1){
			threadArray[t].endIndex = count;
		}
		else {
			threadArray[t].endIndex = t *chunk +chunk;
		}
		printf("Calling pthread_create for thread %ld...\n", t);
		rc = pthread_create (&threads[t], NULL, thread_func, (void *) &threadArray[t]);
		if (rc) {
			printf("ERROR; return code from pthread_create() is %d\n", rc);
			exit(-1);
		}
	}
	for (t=0; t < NUM_THREADS; t++){
		if (pthread_join(threads[t], NULL ) < 0) {
			perror ("pthread_join: ");
		}
		printf ("Calling pthread_join for thread %ld...\n", t);
	}
	
	for(tmp = 0; tmp < count; tmp++) {
		fprintf(outfile, "%d" , array [tmp]);
		if(tmp && ((tmp % 10) == 0 )) fprintf (outfile, "\n") ;
	}
	fprintf (outfile, "\n");
	printf ("%d doubled integers written to %s.\n", count, fileout);
	fclose (outfile);
	pthread_exit (NULL);
	
	return 0; 
}
	
	
	
	