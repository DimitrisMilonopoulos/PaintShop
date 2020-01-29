#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/time.h>   
#include <sys/resource.h> 
#include <time.h> 
#include "sem_operations.h"
#include "part.h"
#include "dynamic_Queue.h"
#include "Processes.h"
#include "file_shared_sem_operations.h"

void Construction(int Number, int Type){
    int sem_id; //id of the available-empty-full semaphore set between the construction and the painting departments
    part *sharedpart; 
    part Component;

    
    sem_id = recover_sem("sem.key"); //recover the sem
   
    int shared_id = recover_shared("shared.key",sizeof(part)); //id of the shared memory between the construction departments
                                                               //and the painting department
    sharedpart =(part*)shmat(shared_id,0,0);                    //Attatch the memory segment to a pointer

    //Iterate for the number of parts
    for (int i = 0; i < Number; i++)
    {

        //First, create a part and timestamp it
        CreatePart(&Component, Type);

        if (!semaphore_p(sem_id, 1)){ //If empty is True(1) then down() its semaphore
            exit(EXIT_FAILURE);
        }
        if (!semaphore_p(sem_id, 0)){ //If the shared memory is not busy, available is True(1)
            exit(EXIT_FAILURE);
        }
            
        //Critical Section
        //Write component to the shared memory
        *sharedpart = Component; //write to memory
        //End of Critical Section
        if (!semaphore_v(sem_id, 0)) //No need for the shared mem to stay busy, so we up() the available
            exit(EXIT_FAILURE);
        if (!semaphore_v(sem_id, 2)){ //Now the shared memory is full, so we up() the full semaphores
            exit(EXIT_FAILURE);
        }
    }
    //Detatch the shared memory segment
    shmdt(sharedpart);
}

void Painting(int Number){
    int sem_id;
    part Component;

    
    sem_id = recover_sem("sem.key");             //available-empty-full semaphore set for the shared mem between Construction and Painting 
    int sem_id1 = recover_sem("sem_check1.key"); //available semaphore for the shared mem between the Painting and the QualityTesting department of type 0
    int sem_id2 = recover_sem("sem_check2.key"); //available semaphore for the shared mem between the Painting and the QualityTesting department of type 1
    int sem_id3 = recover_sem("sem_check3.key"); //available semaphore for the shared mem between the Painting and the QualityTesting department of type 2   

    int shared_id = recover_shared("shared.key",sizeof(part)); //shared-mem for Construction-Painting
    int shared_id1 = recover_shared("shared_check1.key",sizeof(part)); //shared-mem for Painting-QualityTesting
    int shared_id2 = recover_shared("shared_check2.key",sizeof(part)); //shared-mem for Painting-QualityTesting
    int shared_id3 = recover_shared("shared_check3.key",sizeof(part)); //shared-mem for Painting-QualityTesting

    part *shared_part;
    //Attach the shared mem for Construction-Painting to a pointer
    shared_part =(part*) shmat(shared_id,0,0);

    //Attach the shared mem for Painting-QualityTesting to a pointer
    part *shared_part1;
    shared_part1 =(part*) shmat(shared_id1,0,0);
    part *shared_part2;
    shared_part2 =(part*) shmat(shared_id2,0,0);
    part *shared_part3;
    shared_part3 =(part*) shmat(shared_id3,0,0);

    //iterate for the number of parts that need to be painted.
    for (int i =0; i < Number;i++){


        if (!semaphore_p(sem_id, 2)){ //down() full semaphore 
                exit(EXIT_FAILURE);
            }
        if (!semaphore_p(sem_id, 0)){ //down() available semaphore
            exit(EXIT_FAILURE);
        }
        Component = *shared_part; //retrieve from memory
        if (!semaphore_v(sem_id, 0)){ //up() available semaphore
            exit(EXIT_FAILURE);
        }
        if (!semaphore_v(sem_id, 1)){ //up() empty semaphore
            exit(EXIT_FAILURE);}
        
        clock_gettime(CLOCK_MONOTONIC,&Component.painting_time); //Timestamp the time a Part entered the Painting Department
        usleep((Component.type+1)* SLEEP_MS);                   //Wait for it to paint
        printf("Component Painted of type %d with id %d\n",Component.type,Component.id);


        //Push the item for checking to the right QualityTesting department according to its type 
        switch(Component.type){
            
            case 0:
            if (!semaphore_p(sem_id1, 1)){ //down() the empty semapahore for the shared mem
                exit(EXIT_FAILURE);
            }
            if (!semaphore_p(sem_id1, 0)){ //down() the available for type 0 semaphore for the shared mem
                exit(EXIT_FAILURE);
            }
            *shared_part1 = Component; //write to memory
                
            if (!semaphore_v(sem_id1, 0)) //up() the available for type 0 semaphore for the shard mem
                exit(EXIT_FAILURE);
            if (!semaphore_v(sem_id1, 2)) //up() the full semaphore for the shared mem
                exit(EXIT_FAILURE);
            
            break;

            case 1:
            if (!semaphore_p(sem_id2, 1)){
                exit(EXIT_FAILURE);
            }
            if (!semaphore_p(sem_id2, 0)){ //down() the available for type 1
                exit(EXIT_FAILURE);
            }
            *shared_part2 = Component; //write to memory
                
            if (!semaphore_v(sem_id2, 0)) //up() the available for type 1
                exit(EXIT_FAILURE);
            if (!semaphore_v(sem_id2, 2))
                exit(EXIT_FAILURE);
            
            break;



            case 2:
            if (!semaphore_p(sem_id3, 1)){
                exit(EXIT_FAILURE);
            }
            if (!semaphore_p(sem_id3, 0)){ //down() the available for type 2
                exit(EXIT_FAILURE);
            }
            *shared_part3 = Component; //write to memory
            if (!semaphore_v(sem_id3, 0)) //up() the available for type 2
                exit(EXIT_FAILURE);
            if (!semaphore_v(sem_id3, 2))
                exit(EXIT_FAILURE);
            
            break;
            
            }
        }

        //detatch the memory segments
        shmdt(shared_part);
        shmdt(shared_part1);
        shmdt(shared_part2);
        shmdt(shared_part3);
    }   


void QualityTesting(int Number, int Type){
    int sem_id2 = recover_sem("sem_assembly.key"); //retrieve semaphore avail-empty-full set for the QualityTesting-Assembly departments 
    int shared_id2 = recover_shared("shared_assembly.key",sizeof(part)); //retrive the shared memory for the the QualityTesting-Assembly departments
    int sem_id, shared_id;
    part Component;
    //retrieve the available semaphore according to its type. Painting-QualityTesting departments.
    switch(Type){
        case 0:
        sem_id = recover_sem("sem_check1.key");
        shared_id = recover_shared("shared_check1.key",sizeof(part)); //(0)recover shared mem id for the the Painting-QualityTesting departments
        break;
        case 1:
        sem_id = recover_sem("sem_check2.key");
        shared_id = recover_shared("shared_check2.key",sizeof(part)); //(1)recover shared mem id for the the Painting-QualityTesting departments
        break;
        case 2:
        sem_id = recover_sem("sem_check3.key");
        shared_id = recover_shared("shared_check3.key",sizeof(part)); //(2)recover shared mem id for the the Painting-QualityTesting departments
        break;
        default:
        printf("This won't happen.\n");
    }

    part *shared_part; //attach the Painting-QualityTesting shared mem to a pointer
    shared_part =(part*) shmat(shared_id,0,0);

    part *shared_part1;//attach the QualityTesting-Assembly shared mem to a pointer
    shared_part1=(part*) shmat(shared_id2,0,0);
    
    for (int i = 0 ; i < Number ; i++){
        //for the Painting-QualityTesting
        if (!semaphore_p(sem_id, 2)){ //down() full
                exit(EXIT_FAILURE);
            }
        if (!semaphore_p(sem_id, 0)){ //down() available
            exit(EXIT_FAILURE);
        }
        //Critical Section
        Component = *shared_part; //retrieve from memory

        if (!semaphore_v(sem_id, 0)){ //up() available
            exit(EXIT_FAILURE);
        }
        if (!semaphore_v(sem_id, 1)){ //up() empty
            exit(EXIT_FAILURE);   
        }

        usleep((3-Component.type)*SLEEP_MS);
        printf("Component checked of Id %d with Type %d\n",Component.id,Component.type);

        //for the QualityTesting-Aseembly section
        if (!semaphore_p(sem_id2, 1)){ //down() empty
            exit(EXIT_FAILURE);
        }
        if (!semaphore_p(sem_id2, 0)){ //down() available
            exit(EXIT_FAILURE);
        }
            
        //Critical Section
        *shared_part1 = Component; //Write to shared mem
        //End of Critical Section 
        if (!semaphore_v(sem_id2, 0)) //up() available
            exit(EXIT_FAILURE);
        if (!semaphore_v(sem_id2, 2)){ //up() full
            exit(EXIT_FAILURE);
        }

}
        //detatch shared memories
        shmdt(shared_part);
        shmdt(shared_part1);
}

void Assembly(int Number){
    int sem_id = recover_sem("sem_assembly.key"); //retrieve semaphoreset available-empty-full set for QualityTesting-Assembly
    int shared_id = recover_shared("shared_assembly.key",sizeof(part)); //recover QualityTesting-Assembly shared mem id
    part *shared_part;
    shared_part =(part*) shmat(shared_id,0,0);
    part Component;
    part ComponentDeq1;
    part ComponentDeq2;
    part ComponentDeq3;
    FinalProduct Fproduct;

    //three dynamic queues one for each part type
    InfoQueue* InfoQueue_type1 = CreateQueue();
    InfoQueue* InfoQueue_type2 = CreateQueue();
    InfoQueue* InfoQueue_type3 = CreateQueue();

    //Variables for the average times

    long sec, ns;
    long double wait_time=0;
    long double product_time=0;

    for(int i=0 ; i < Number; i++){
     if (!semaphore_p(sem_id, 2)){ //down() full
                exit(EXIT_FAILURE);
            }
        if (!semaphore_p(sem_id, 0)){ //down() available
            exit(EXIT_FAILURE);
        }
        Component = *shared_part;
        if (!semaphore_v(sem_id, 0)){ //up() available
            exit(EXIT_FAILURE);
        }
        if (!semaphore_v(sem_id, 1)){ //up() empty
            exit(EXIT_FAILURE);}
        printf("Component to assemble of Id: %d and Type %d\n",Component.id, Component.type);
        //calculate the time needed between construction and paintshop for this component
        sec = Component.painting_time.tv_sec - Component.creation_time.tv_sec;
        ns = Component.painting_time.tv_nsec - Component.creation_time.tv_nsec;

        if (Component.creation_time.tv_nsec>Component.painting_time.tv_nsec){
            --sec;
            ns+=1000000000;
        }

        wait_time += (long double)sec + (long double)ns/(long double)1000000000;

        //lets add the component to the appropriate queue
        switch(Component.type){
            case 0:
                enQueue(InfoQueue_type1, Component);
                break;
            case 1:
                enQueue(InfoQueue_type2, Component);
                break;
            case 2:
                enQueue(InfoQueue_type3, Component);
                break;
            default:
                perror("Value error");
        }
        //if there is a part of each type we can create the final product
        if (!emptyQueue(InfoQueue_type1) &&!emptyQueue(InfoQueue_type2) && !emptyQueue(InfoQueue_type3) ){
            deQueue(InfoQueue_type1,&ComponentDeq1); //retrieve part
            deQueue(InfoQueue_type2,&ComponentDeq2);
            deQueue(InfoQueue_type3,&ComponentDeq3);       
            usleep(SLEEP_MS); //wait for the product to be created
            CreateFinalProduct(&Fproduct,ComponentDeq1,ComponentDeq2,ComponentDeq3);   
            printf("***Final Product Created with id:  %s***\n",Fproduct.id);
            sec = Fproduct.production_time.tv_sec - Fproduct.smallest_time.tv_sec;
            ns = Fproduct.production_time.tv_nsec - Fproduct.smallest_time.tv_nsec;

            if (Fproduct.smallest_time.tv_nsec>Fproduct.production_time.tv_nsec){
                --sec;
                ns+=1000000000;
            }

            product_time += (long double)sec + (long double)ns/(long double)1000000000;
        }

     }

    //Destroy the queues
    DestroyQueue(InfoQueue_type1);
    DestroyQueue(InfoQueue_type2);
    DestroyQueue(InfoQueue_type3);
    //calculate average waiting time from creation to entering the painting department
    wait_time = (long double)wait_time/Number;
    //Calculate average production time from first part to final product
    product_time = product_time/(Number/3);
    printf("Average waiting time from production to entering the Painting department: %Lf seconds\n",wait_time);
    printf("Average production time of Final Product: %Lf seconds\n",product_time);

    shmdt(shared_part);
}
