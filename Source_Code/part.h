#ifndef PART_H 
#define PART_H

//The struct used to get the time. Predefined
// struct timespec {                                                                                     
//    time_t   tv_sec;        /* seconds */                                                             
//    long     tv_nsec;       /* nanoseconds */                                                         
// };  


//Struct of the Part-Component
typedef struct part{
    int type;                       //Type of the Part. Types: 0 - 2
    unsigned int id;                //Id of the Part is a 4 digit number
    struct timespec creation_time; //The timestamp of when the part was created
    struct timespec painting_time; //The timestamp of when the part entered the paintshop
}part;

void CreatePart(part *,int); //Creates a part, assigns it's values and timestamps it


//This is the Final product Created by 3 different type Parts
typedef struct FinalProduct{
    char id[13];                        //The id of the Final Product is the combination of the 3 part ids
    struct timespec smallest_time;      //The timestamp of the first part that was created
    struct timespec production_time;    //The timestamp of the creation time of the product
}FinalProduct;

void CreateFinalProduct(struct FinalProduct*, part, part, part); //Creates the final product from three parts and timestamps it
#endif
