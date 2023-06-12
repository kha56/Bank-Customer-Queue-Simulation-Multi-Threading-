/*
FileName: c_queue.c
Created By: Ivan Kha
Date Created: 27 April 2023
*/

#include "c_queue.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


//Initalizes a new empty queue
Queue* create_queue(int max_size)
{
    Queue* queue = (Queue*) malloc(sizeof(Queue)); //allocate the size
    queue->front = NULL;
    queue->rear = NULL;
    queue->size= 0;
    queue->max_size = max_size;
    return queue;
}

//Adds a new element to the reat of the queue
void enqueue(Queue* queue, Customer customer)
{
    if(queue->size == queue->max_size)
    {
        printf("Queue is full\n"); //When it says full other items are not added to  the list
        return;
    }

    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = customer;
    newNode->next = NULL;

    time_t t = time(NULL);
    strftime(newNode->data.arrival_time, sizeof(newNode->data.arrival_time), "%H:%M:%S", localtime(&t)); // set the customer arrival time

    if(queue->rear == NULL)
    {
        queue->front = queue->rear = newNode;
        queue->size++;
        return;
    }

    queue->rear->next = newNode;
    queue->rear = newNode;
    queue->size++;

}

//Removes the elemement at the front of the queue and replace its data
Customer dequeue(Queue* queue)
{
   
    if(queue->front == NULL)
    {
        printf("Queue is empty\n");
        
        return (Customer){ .id = 0, .type = '\0' };

    }
    Node* temp = queue->front;
    Customer data = temp->data;
    queue->front = queue->front->next;

    if(queue->front == NULL)
    {
        queue->rear = NULL;
    }

    free(temp);
    queue->size--;
    return data;
}

//Free the queue
void free_queue(Queue* queue) 
{
    Node* curr = queue->front;
    while (curr != NULL) 
    {
        Node* temp = curr;
        curr = curr->next;
        free(temp);
    }
    free(queue);
    
}

