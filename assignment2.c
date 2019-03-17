//
//  Assignment2.c
//  
//
//  Created by Abhiram Santhosh on 3/13/19.
// Gabriele sarwar,101010867
//

#include "Assignment2.h"

static int semID[3]; //there will be three semaphores total, because theres three pairs of two array sections that need to be accessed

//standard stuff
union semun sem_union;

sem_union.val = 1;
if (semctl(SID[i], 0, SETVAL, sem_union) == -1) return(0);
return(1);
}

static void del_semvalue(int i) {
    union semun sem_union;
    if (semctl(SID[i-1], 0, IPC_RMID, sem_union) == -1)
        fprintf(stderr, "Failed to delete semaphore\n");
}

static int semaphore_p(int i) {
    struct sembuf sem_b;
    sem_b.sem_num = 0;
    sem_b.sem_op = -1; /* P() */
    sem_b.sem_flg = SEM_UNDO;
    if (semop(SID[i-1], &sem_b, 1) == -1) {
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
    if (semop(SID[i-1], &sem_b, 1) == -1) {
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
    pid_t pid;
    int waitTIme;
    int debug=0;
    char input[30];
    bool invalid = 0;
    
    srand((unsigned int)getpid());
    
    for(int i = 0; i < 3; i++){
        //creating three semaphores
        
        semID[i] = semget((key_t)1234+i, 1, 0666 | IPC_CREAT);
        //populate array with semaphores
        
        if(!sem_semvalue(i)){
            fprintf(stderr, "Failed to init semaphore\n");
            exit(EXIT_FAILURE);
        }
        
        
        shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);
        
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
        
        
        
        //user input stuff
    do{
        invalid = 0;
        printf("do you want to access debug mode? type yes or no");
        scanf("%s",userinput);
        if(userinput[1] != '\0'){
            invalid = 1;
            printf("not a valid argument\n");
        }
        else if(userinput[0] == 'yes'){	//  debug mode on
            debug = 1;
        }
        else if(userinput[0] == 'no'){	 
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
            if(valid){	// convert to integer to see input
                shared_stuff->B[i] = atoi(numberinput);
                finished = 1;
            }else{
                printf("Invalid! try again\n");
            }
        }while(finished == 0);
    }
        
        
        
        
        
        
        
        
        
        
   
        
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
                printf("error for failed \n");
                exit(1);
                
            case 0:
                
                //child processes
                //the while loop is essential because
                //you need to iterate multiple times, doing what I did last assignment wont work because that doesn't use a loop
                //this is a bubble sort algorithm
                //using other algorithms might be an option, but would make it harder
                
                //seperated for readability
                
                int temp = 0; //temp for switching variables around
                
                while (!(shared_stuff->B[0] >shared_stuff->B[1] &&
                         
                shared_stuff->B[1] > shared_stuff->B[2] &&
                         
                shared_stuff->B[2] > shared_stuff->B[3] &&
                         
                shared_stuff->B[3] > shared_stuff->B[4])) {
                    
                    
                    if(index == 0){
                        //first child
                        
                        //waiting for semaphore 1
                        if (!semaphore_p(1))  exit(EXIT_FAILURE);
                        
                        if(shared_stuff->B[0] < shared_stuff->B[1]){
                            
                            temp = shared_stuff->B[0];
                            shared_stuff->B[0] = shared_stuff->B[1];
                            shared_stuff->B[1] = temp;
                            
                           if(debug)printf("Process P1: performed swapping\n");
                    }else{
                        if(debug)printf("Process P1: No swapping\n");
                    }
                    if(!semaphore_p(1)) exit(EXIT_FAILURE);
                        
                       
                        
                        
                    } else if(index == 1){
                        
                        //second paid
                        if(!semaphore_p(1) || !semaphore_p(2)) exit(EXIT_FAILURE);
                        
                        
                        if(shared_stuff->B[1] < shared_stuff->B[2]){
                            
                            temp = shared_stuff->B[1];
                            shared_stuff->B[1] = shared_stuff->B[2];
                            shared_stuff->B[2] = temp;
                            
                             if(debug)printf("Process P2: performed swapping\n");
                    }else{
                        if(debug)printf("Process P2: No swapping\n");
                    }
                    if(!semaphore_p(1)) exit(EXIT_FAILURE);
                    if(!semaphore_p(2)) exit(EXIT_FAILURE);
                        
            } else if(index == 2) {
                        
                        if(!semaphore_p(2) || !semaphore_p(3)) exit(EXIT_FAILURE);
                    
                    
                    if(shared_stuff->B[2] < shared_stuff->B[3]){
                        
                        temp = shared_stuff->B[2];
                        shared_stuff->B[2] = shared_stuff->B[3];
                        shared_stuff->B[3] = temp;
                        
                        if(debug)printf("Process P3: performed swapping\n");
                    }else{
                        if(debug)printf("Process P3: No swapping\n");
                    }
                    if(!semaphore_p(2)) exit(EXIT_FAILURE);
                    if(!semaphore_p(3)) exit(EXIT_FAILURE);
                        
            } else if(index == 3) {
                        
                        //third paid
                        if(!semaphore_p(3)) exit(EXIT_FAILURE);
                        
                        
                        if(shared_stuff->B[3] < shared_stuff->B[4]){
                            
                            temp = shared_stuff->B[3];
                            shared_stuff->B[3] = shared_stuff->B[4];
                            shared_stuff->B[4] = temp;
                             if(debug)printf("Process P4: performed swapping\n");
                    }else{
                        if(debug)printf("Process P4: No swapping\n");
                    }
                    if(!semaphore_p(3)) exit(EXIT_FAILURE);
                    if(!semaphore_p(4)) exit(EXIT_FAILURE);
            }
                    
                    
        }
                
            default:
                
                if(pid != 0){
                    for (int k = 0; k < 4; k++) wait(&waitTime);
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
