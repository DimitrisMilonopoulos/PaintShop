#ifndef DYNAMIC_QUEUE_H
#define DYNAMIC_QUEUE_H

typedef struct node{
    struct part data; 
    struct node *next;
}node;

typedef struct InfoQueue{ //Information about the queue
    struct node *front;
    struct node *rear;
}InfoQueue;

InfoQueue* CreateQueue();
void enQueue(InfoQueue*, part);     //Enqueue a new part
void deQueue(InfoQueue*, part*);    //Dequeue a part and save it on the second argument of the function
void DestroyQueue(InfoQueue*);      //Empty and free all the items in the queue
int emptyQueue(InfoQueue*);         //Return 1 if queue is empty

#endif