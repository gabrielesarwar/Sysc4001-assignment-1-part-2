//Gabriele Sarwar, 101010867

#include <sys/types.h> 
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <unistd.h> 
#include <stdlib.h> 
#include <errno.h>
#define MICRO_SEC_TO_SEC 1000000
#define Size_of_matrix 4


long (*finalmatrix)[Size_of_matrix];

void print_matrix (long (*matrix)[Size_of_matrix])
{
    for (int i = 0; i < Size_of_matrix; i++) {
        for (int j = 0; j < Size_of_matrix; j++) {
            printf("%3ld  ", matrix[i][j]);
        }
        printf("\n");
    }
}

/**
 *  elapsed time method from the labs
 */
long time_interval (struct timeval *start, struct timeval *end)
{
    return ((end->tv_sec * MICRO_SEC_TO_SEC + end->tv_usec) - (start->tv_sec * MICRO_SEC_TO_SEC + start->tv_usec));
}


/**
 *  this method is used to calculate the rows using a common algorith which i found online.
 */
void row_multiplication (int firstrow, int desired_rows, long (*m)[Size_of_matrix] ,long (*n)[Size_of_matrix], long (*out)[Size_of_matrix])
{
    for (int i = firstrow; i < (firstrow + desired_rows); i++) {
        for (int z = 0; z < Size_of_matrix; z++) {
            out[i][z] = 0;
            for (int j = 0; j < Size_of_matrix; j++) {
                out[i][z] += m[i][j] * n[j][z];
            }
        }
    }
}

int main (int argc, char **argv)
{
//read argc for argument count
    int child_processes = 1;
    
    if (argc > 1) {
    char *end;
    	long argument = strtol(argv[1], &end, 10);
       //assuming its a valid value, 1 2 or 4
       
	printf("argument is %d\n", argc);
	
        child_processes = Size_of_matrix / argument;
    }
    
  
    int shmid = shmget((key_t)1234, sizeof(long) * Size_of_matrix * Size_of_matrix, 0666 | IPC_CREAT); 
    if (shmid == -1) {
        fprintf(stderr, "Failed to get shared memory: %s\n", strerror(errno));
        exit(1);
    }

    finalmatrix = shmat(shmid, NULL, 0);
    if ((int)finalmatrix == -1) {
        fprintf(stderr, "Failed to attach shared memory: %s\n", strerror(errno));
        exit(1);
    }

  //  test case matricees
	long matrix_1[Size_of_matrix][Size_of_matrix] = {
				{1, 2, 3, 4},
				{5, 6, 7, 8},
				{4, 3, 2, 1},
        {8, 7, 6, 5}};

	long matrix_2[Size_of_matrix][Size_of_matrix] = {
       {1, 3, 5, 7},
       {2, 4, 6, 8},
       {7, 3, 5, 7},
       {8, 6, 4, 2}};
  //calculate time passed like we did in labs
    struct timeval start_time;
    gettimeofday(&start_time, NULL);
//----------------------------------------------------------------------------------------
    pid_t subprocesses[Size_of_matrix];
    
    for (int i = 0; i < Size_of_matrix; i += child_processes) {
       
     subprocesses[i] = fork();
        
    if (subprocesses[i] == -1) {
            fprintf(stderr, "shmget failure\n");
		        exit(EXIT_FAILURE);
        } else if (subprocesses[i] == 0) {
          
            if (child_processes == 1) {
                printf("first row: %d\n", i);
            } else {
                printf(" multiple rows, i is: %d\n ", i);
                for (int z = 1; z < (child_processes - 1); z++) {
                    printf(", %d", i + z);
                }
                printf("  %d\n", i + (child_processes - 1));
            }
            
            row_multiplication(i, child_processes, matrix_1, matrix_2, finalmatrix);
            exit(0);
        }
    }
    
    //iterate trhough child processes
    for (int i = 0; i < Size_of_matrix; i++) {
        waitpid(subprocesses[i], NULL, 0);
    }
   //---------------------------------------------------------------------------------------- 


    struct timeval end_time;
    
    gettimeofday(&end_time, NULL);
    

    printf("Time passed: %ld µs\n", time_interval(&start_time, &end_time));

    printf("1:\n");
    
    print_matrix(matrix_1);
    
    printf("2:\n");
    
    print_matrix(matrix_2);
    
    printf("result:\n");
    
    print_matrix(finalmatrix);
    
    shmdt(finalmatrix);

}
