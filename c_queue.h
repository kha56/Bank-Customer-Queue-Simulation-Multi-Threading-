/*
FileName: c_queue.h
Created By: Ivan Kha
Date Created: 27 April 2023
*/

#ifndef C_QUEUE_H
#define C_QUEUE_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "pthread.h"
#include "unistd.h"
#include "stdbool.h"
#include "ctype.h"

extern pthread_mutex_t mutex;

//Define the Customer 
typedef struct 
{
    int id; //Numbering the customer
    char type; //W D I
    char arrival_time[9];

}Customer;

//Node struct for the linked list
typedef struct Node
{
    Customer data;
    struct Node* next; 
}Node;


//Queue struct for the linked list
typedef struct
{
    Node* front;
    Node* rear;
    int size;
    int max_size;// Put m here
}Queue;


//Functions Declarations
Queue* create_queue(int max_size);
void enqueue(Queue* queue, Customer customer);
Customer dequeue(Queue* queue);
void free_queue(Queue* queue);


#endif // C_QUEUE_H