//
//  Assignment2.c
//  
//
//  Created by Abhiram Santhosh on 3/13/19.
//

#include "Assignment2.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <time.h>
#include <sys/time.h>
#include <ctype.h>
#include <sys/sem.h>
#include <stdbool.h>

//#include "semun.h"
//#include "com.h"
//#include "semun.h"
//#include "shm_com.h"

static int semID[3]; //there will be three semaphores total, because theres three pairs of two array sections that need to be accessed

//standard stuff
static int set_semvalue(int i) {
    union semun sem_union;
    sem_union.val = 1;
    if (semctl(semID[i], 0, SETVAL, sem_union) == -1) return(0);
    return(1);
}

static void del_semvalue(int i) {
    union semun sem_union;
    if (semctl(semID[i-1], 0, IPC_RMID, sem_union) == -1)
        fprintf(stderr, "Failed to delete semaphore\n");
}

static int semaphore_p(int i) {
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = -1; /* P() */
    sem_b.sem_flg = SEM_UNDO;
    if (semop(semID[i-1], &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_p failed\n");
        return(0);
    }
    return(1);
}

static int semaphore_v(int i) {
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = 1; /* V() */
    sem_b.sem_flg = SEM_UNDO;
    if (semop(semID[i-1], &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_v failed\n");
        return(0);
    }
    return(1);
}

int main(){
    
    int shmid;
    void *shared_memory = (void *)0;
    struct shared_use *shared_stuff;
    int index;
    pid_t pid = getpid();
    int waitTime;
    int temp; //temp for switching variables around
    int debug = 0;
    
    for(int i = 0; i < 3; i++){
        //creating three semaphores
        
        semID[i] = semget((key_t)1234+i, 1, 0666 | IPC_CREAT);
        //populate array with semaphores
        
        if(!set_semvalue(i)){
            fprintf(stderr, "Failed to init semaphore\n");
            exit(EXIT_FAILURE);
        }
    }
    
    
    shmid = shmget((key_t)1234, sizeof(struct shared_use), 0666 | IPC_CREAT);
    
    if (shmid == -1) {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }
    
    shared_memory = shmat(shmid, (void *)0, 0); //attach shared memory
    
    if (shared_memory == (void *)-1) {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }
    
    shared_stuff = (struct shared_use *)shared_memory;
    
    //populate array
    shared_stuff->B[0] = 5;
    shared_stuff->B[1] = 6;
    shared_stuff->B[2] = 8;
    shared_stuff->B[3] = 2;
    shared_stuff->B[4] = 7;
    
        printf("%d,%d,%d,%d,%d\n", shared_stuff->B[0], shared_stuff->B[1], shared_stuff->B[2], shared_stuff->B[3], shared_stuff->B[4]);
    
   
      //user input stuff
     char userinput[20];
     bool invalid = 0;
    
    do{
        invalid = 0;
        printf("do you want to access debug mode? type y for yes or no for no");
        scanf("%s",userinput);
        if(userinput[1] != '\0'){
            invalid = 1;
            printf("not a valid argument\n");
        }
        else if(userinput[0] == 'y'){	//  debug mode on
            debug = 1;
        }
        else if(userinput[0] == 'n'){	 
            debug = 0;
        }else{
            invalid = 1;
            printf("not a valid argument\n");
        }
    }while(invalid);
    
    //ask for the 5 integeres
    printf("Enter 5 different integers\n");
    
    for(int i=0; i<5; i++){
        int finished = 0;
        do{
            bool validinput = 1;
            char numberinput[30];
            printf("Enter integer : ");
            scanf("%s", numberinput);
            for(int b = 0;  numberinput[b] != '\0'; b++){
                if(!isdigit(numberinput[b])){
                    validinput = 0;
                    break;
                }
            }
            if(validinput){	// convert to integer to see input
                shared_stuff->B[i] = atoi(numberinput);
                finished = 1;
            }else{
                printf("Invalid! try again\n");
            }
        }while(finished == 0);
    
      
    
    //fork 4 times, four children
    index = -1;
    
    for(int i = 0; i < 4; i++){
        pid = fork();
        index++;
        
        /*
         Having "index" makes sure that the proccesses only work on the sections they should work on. The first child will get index = 0, and thus should only work on index and index + 1. Starting at -1 ensures that there isn't an out of bounds error.
         
         Index is also a way to identify which child has which pair
         */
        
        if(pid == 0) break;
        
    }
    
    switch(pid) {
        case -1:
            //error
            printf("error fork failed \n");
            exit(1);
            
        case 0:
            printf("child\n");
            //child processes
            //the while loop is essential because
            //you need to iterate multiple times, doing what I did last assignment wont work because that doesn't use a loop
            //this is a bubble sort algorithm
            //using other algorithms might be an option, but would make it harder
            
            //seperated for readability
            
            
            
            while (!
                   (shared_stuff->B[0] > shared_stuff->B[1] &&
                    shared_stuff->B[1] > shared_stuff->B[2] &&
                    shared_stuff->B[2] > shared_stuff->B[3] &&
                    shared_stuff->B[3] > shared_stuff->B[4])){
                       
                if (index == 0) {
                    //first pair
                    if (!semaphore_p(1)) exit(EXIT_FAILURE); //Wait
                    
                    if (shared_stuff->B[index] < shared_stuff->B[index+1]){
                        
                        temp = shared_stuff->B[index];
                        shared_stuff->B[index] = shared_stuff->B[index+1];
                        shared_stuff->B[index+1] = temp;
                       if(debug)printf("Process P1: performed swapping\n");
                    }else{
                        if(debug)printf("Process P1: No swapping\n");
                    }
                    if (!semaphore_v(1)) exit(EXIT_FAILURE); //Signal
                }
                       
                else if (index == 1) {
                    //second paid
                    if (!semaphore_p(1)) exit(EXIT_FAILURE);
                    if (!semaphore_p(2)) exit(EXIT_FAILURE);
                    
                    if (shared_stuff->B[index] < shared_stuff->B[index+1]) {
                        
                        temp = shared_stuff->B[index];
                        shared_stuff->B[index] = shared_stuff->B[index+1];
                        shared_stuff->B[index+1] = temp;
                        
                    if(debug)printf("Process P2: performed swapping\n");
                    }else{
                        if(debug)printf("Process P2: No swapping\n");
                    }
                    
                    if (!semaphore_v(1)) exit(EXIT_FAILURE);
                    if (!semaphore_v(2)) exit(EXIT_FAILURE);
                }
                       
                else if (index == 2) {
                    //third pair
                    if (!semaphore_p(2)) exit(EXIT_FAILURE);
                    if (!semaphore_p(3)) exit(EXIT_FAILURE);
                    
                    if (shared_stuff->B[index] < shared_stuff->B[index+1]) {
                        
                        temp = shared_stuff->B[index];
                        shared_stuff->B[index] = shared_stuff->B[index+1];
                        shared_stuff->B[index+1] = temp;
                  
                        if(debug)printf("Process P3: performed swapping\n");
                    }else{
                        if(debug)printf("Process P3: No swapping\n");
                    }
                    
                    if (!semaphore_v(2)) exit(EXIT_FAILURE);
                    if (!semaphore_v(3)) exit(EXIT_FAILURE);
                }
                       
                else if (index == 3) {
                    //fourth pair
                    if (!semaphore_p(3)) exit(EXIT_FAILURE);
                    
                    if (shared_stuff->B[index] < shared_stuff->B[index+1]) {
                        
                        temp = shared_stuff->B[index];
                        shared_stuff->B[index] = shared_stuff->B[index+1];
                        shared_stuff->B[index+1] = temp;
                         if(debug)printf("Process P4: performed swapping\n");
                    }else{
                        if(debug)printf("Process P4: No swapping\n");
                    }
                    
                    if (!semaphore_v(3)) exit(EXIT_FAILURE);
                }
            }
            
        default:
            
            if(pid != 0){
                for (int k = 0; k < 4; k++) {
                    printf("Parent");
                    wait(&waitTime);
                    
                }
                
                
                printf("%d,%d,%d,%d,%d\n", shared_stuff->B[0], shared_stuff->B[1], shared_stuff->B[2], shared_stuff->B[3], shared_stuff->B[4]);
                printf("Lowest value: %d\n", shared_stuff->B[4]);
                printf("Median value: %d\n", shared_stuff->B[2]);
                printf("Highest value: %d\n", shared_stuff->B[0]);
                
                //delete semaphores
                del_semvalue(1);
                del_semvalue(2);
                del_semvalue(3);
                
                //detach shared_mem
                if (shmdt(shared_memory) == -1) {
                    fprintf(stderr, "shmdt failed\n");
                    exit(EXIT_FAILURE);
                }
                if (shmctl(shmid, IPC_RMID, 0) == -1) {
                    fprintf(stderr, "shmctl(IPC_RMID) failed\n");
                    exit(EXIT_FAILURE);
                }
                
            }
    }
    
    exit(EXIT_SUCCESS);
}
}





