#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <errno.h>
#include <sys/sem.h>
#include <time.h>
#include "sem_operations.h"
#include "part.h"
#include "Processes.h"
#include "file_shared_sem_operations.h"

int main(int argc, char **argv){
    int number; //number of parts for each type
    key_t sem_key_notused;
    int sem_id;
    int sem_fd;

    //extract the argument and check its validity
    if (argc == 2){
        number = atoi(argv[1]);
    }
    else{
        printf("Invalid number of arguments.\n");
        return -1;
    }

    sem_id = create_sem("sem.key",3,436); //Semaphore set for the available-empty-full, of the shared mem between the Construction 
                                           //and the Painting Department
    int sem_id1 = create_sem("sem_check1.key",3,62); //semaphores for the available-empty-full for the shared-mem between the Painting and the QualityTesting department type 0
    int sem_id2 = create_sem("sem_check2.key",3,435); //semaphores for the available-empty-full for the shared-mem between the Painting and the QualityTesting department type 1
    int sem_id3 = create_sem("sem_check3.key",3,5169); //semaphores for the available-empty-full for the shared-mem between the Painting and the QualityTesting department type 2
    int sem_id5 = create_sem("sem_assembly.key",3,66237);

    if(!set_semvalue(sem_id,0,1)){ //available = 1
        perror("Could not set value semaphore");
        exit(4);
    }
    
    if(!set_semvalue(sem_id,1,1)){ //empty = 1
        perror("Could not set value semaphore");
        exit(4);
    }
     if(!set_semvalue(sem_id,2,0)){ //full = 0
        perror("Could not set value semaphore");
        exit(4);
    }
    
    
    //sem paint-check
    if(!set_semvalue(sem_id1,0,1)){ //available = 1
        perror("Could not set value semaphore");
        exit(4);
    }
    
    if(!set_semvalue(sem_id1,1,1)){ //empty = 1
        perror("Could not set value semaphore");
        exit(4);
    }
     if(!set_semvalue(sem_id1,2,0)){ //full = 0
        perror("Could not set value semaphore");
        exit(4);
    }

    if(!set_semvalue(sem_id2,0,1)){ //available = 1
        perror("Could not set value semaphore");
        exit(4);
    }
    
    if(!set_semvalue(sem_id2,1,1)){ //empty = 1
        perror("Could not set value semaphore");
        exit(4);
    }
     if(!set_semvalue(sem_id2,2,0)){ //full = 0
        perror("Could not set value semaphore");
        exit(4);
    }

    if(!set_semvalue(sem_id3,0,1)){ //available = 1
        perror("Could not set value semaphore");
        exit(4);
    }
    
    if(!set_semvalue(sem_id3,1,1)){ //empty = 1
        perror("Could not set value semaphore");
        exit(4);
    }
     if(!set_semvalue(sem_id3,2,0)){ //full = 0
        perror("Could not set value semaphore");
        exit(4);
    }
    //sem paint-check
    //considered  full semaphore

    if(!set_semvalue(sem_id5,0,1)){ //available = 1
        perror("Could not set value semaphore");
        exit(4);
    }
    //considered empty semaphore
    if(!set_semvalue(sem_id5,1,1)){ //empty = 1
        perror("Could not set value semaphore");
        exit(4);
    }
     if(!set_semvalue(sem_id5,2,0)){ //full = 0
        perror("Could not set value semaphore");
        exit(4);
    }
    
    create_shared("shared.key",sizeof(part),501); //shared-mem that holds one part between Construction departments and Painting 
    create_shared("shared_check1.key",sizeof(part),35); //shared-mem that holds one part between Painting department and QualityTesting department type 0
    create_shared("shared_check2.key",sizeof(part),37); //shared-mem that holds one part between Painting department and QualityTesting departments type 1
    create_shared("shared_check3.key",sizeof(part),40); //shared-mem that holds one part between Painting department and QualityTesting departments type 2
    create_shared("shared_assembly.key",sizeof(part),42); //shared-mem that holds one part between QualityTesting departments and Assembly

    //Create the processes for the construction: #3 processes
    pid_t pid;
    for (int i =0; i<3;i++){
        pid = fork();
        if (pid ==0){
            srand(time(NULL)+i*10);
            Construction(number,i);
            exit(0);
        }
    }

    //Create the processes for the Painting: #1 process 
    pid = fork();
    if (pid ==0){
            Painting(number*3);
            exit(0);
        }

    //Create the processes for the QualityTesting
    for (int j =0; j<3;j++){
        pid = fork();
        if (pid ==0){
            QualityTesting(number,j);
            exit(0);
        }
    }

    //Create the process for the assembly
    pid = fork();
    if (pid ==0){
            Assembly(number*3);
            exit(0);
        }



    //Wait for all the children to exit: #8 processes in total, thus wait for 8 children to exit
    for (int k=0; k < 8; k++){
        wait(NULL);
    }
    
    //Now lets delete the semaphores
    delete_sem("sem.key");
    delete_sem("sem_check1.key");
    delete_sem("sem_check2.key");
    delete_sem("sem_check3.key");
    delete_sem("sem_assembly.key");
    //Lets delete the shared meme segments
    delete_shared("shared.key",sizeof(part));
    delete_shared("shared_check1.key",sizeof(part));
    delete_shared("shared_check2.key",sizeof(part));
    delete_shared("shared_check3.key",sizeof(part));
    delete_shared("shared_assembly.key",sizeof(part));

    exit(0);
}
