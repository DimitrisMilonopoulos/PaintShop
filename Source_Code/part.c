#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/time.h>   
#include <sys/resource.h> 
#include <time.h> 
#include "part.h"



void CreatePart(part *Component, int Type){
    Component->type = Type;
    Component->id =  rand() % 9000+1000;                                 //Produce a random 4 digit number
    clock_gettime(CLOCK_MONOTONIC,&Component->creation_time);           //Timestamp the part, use of monotonic for better accuracy
    printf("Part created of type %d with id %d \n",Component->type,Component->id); //Notify about the part information
}

void CreateFinalProduct(struct FinalProduct *Product, part part1, part part2, part part3){
    sprintf(Product->id,"%d%d%d",part1.id,part2.id,part3.id); //Combine the part ids for the final product
    struct timespec smallest_time; 
    struct timespec production_time;

    clock_gettime(CLOCK_MONOTONIC,&production_time); //Timestamp the Final Product

    //Find the smallest creation time of the three parts
    if (part1.creation_time.tv_sec<part2.creation_time.tv_sec){ //Compare secs
        smallest_time = part1.creation_time;
    }
    else if (part1.creation_time.tv_sec==part2.creation_time.tv_sec){
        if (part1.creation_time.tv_nsec<part2.creation_time.tv_nsec){ //If secs are the same compare nsecs
            smallest_time = part1.creation_time;
        }
        else{
            smallest_time = part2.creation_time;
        }
    }
    else{
        smallest_time = part2.creation_time;
    }

    //same procedure
    if (smallest_time.tv_sec>part3.creation_time.tv_sec){
        smallest_time = part3.creation_time;
    }
    else if (smallest_time.tv_sec==part3.creation_time.tv_sec){
        if (smallest_time.tv_nsec>part3.creation_time.tv_nsec){
            smallest_time = part3.creation_time;
        }
    }
    //Assign the timestamps on the final Product
    Product->smallest_time = smallest_time;
    Product->production_time = production_time;
    }
