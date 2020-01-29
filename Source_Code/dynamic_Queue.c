#include <stdio.h>
#include <stdlib.h>
#include "part.h"
#include "dynamic_Queue.h"

InfoQueue* CreateQueue(){
    InfoQueue *infoq =  malloc(sizeof(struct InfoQueue)); //allocate memory for the InfoQueue
    infoq->front = NULL;
    infoq->rear = NULL;
    return infoq;
}

void enQueue(InfoQueue* info,part item)
{
    struct node *nptr = malloc(sizeof(struct node)); //allocate memory for the new node
    nptr->data = item;
    nptr->next = NULL;
    if (info->rear == NULL)
    {
        info->front = nptr;
        info->rear = nptr;
    }
    else
    {
        info->rear->next = nptr;
        info->rear = info->rear->next;
    }
}

void deQueue(InfoQueue* info,struct part *data)
{
    if (info->front == NULL)
    {
        printf("\nQueue is empty \n");
    }
    else
    {
        struct node *temp; //save the pointer for the free here
        temp = info->front;
        info->front = info->front->next;
        if (info->front == NULL){
            info->rear = NULL;
        }
        *data = temp->data; //copy the data from the poped
        free(temp); //free the queue node
    }
}

void DestroyQueue(InfoQueue* info){
    struct part temp;
    while(info->front != NULL){ //Our Queue is then empty
        deQueue(info,&temp);
    }
    free(info);
}

int emptyQueue(InfoQueue* info){
    if (info->front == NULL)
    {
        //Queue is empty
        return 1;
    }
    return 0;
}
