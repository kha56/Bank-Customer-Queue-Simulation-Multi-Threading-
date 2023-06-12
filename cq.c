/*
FileName: cq.c
Created By: Ivan Kha
Date Created: 27 April 2023
cq m tc tw td ti 
*/


//Import queue fucntions
#include "c_queue.h"
#define FILE_NAME "c_file.txt"


//Global Variables
Queue* c_queue;
int m, tw,tc, td, ti; 

int customers_served[4] = {0}; //Store the number of customers serverd by the tellers
int tellers_terminated = 0;// Keeping track of if this the last teller to terminate

void activity_log(Customer customer, char* filename);
void* customer(void* arg);
void* teller(void* arg);
bool is_integer(const char *str);


int main(int argc, char *argv[])
{
   
    //If it is not = 6 argument throw an exception
    if(argc != 6)
    {
        printf("Usage: %s m tc tw td ti\n", argv[0]);
        exit(0);
    }

    //Validating that only integer is allowed
    for (int i = 1; i < argc; i++) 
    {
        if (!is_integer(argv[i])) 
        {
            printf("Error: argument %d is not an integer.\n", i);
            exit(1);
        }
    }

    //Setting the arugments into its variables
    m = atoi(argv[1]);
    tc = atoi(argv[2]); //Time it arrives to the queue
    tw = atoi(argv[3]);//Time it takes to withdraw
    td = atoi(argv[4]); //Time it takes to deposit
    ti = atoi(argv[5]); //Time it takes to ask information

    //Proceed with the application
    //Create a queue based ont he size of the input
    c_queue = create_queue(m);


    //Creating Customer Thread
    pthread_t customer_thread;
    if(pthread_create(&customer_thread, NULL, customer, NULL))
    {
        printf("Error Creating Customer Thread\n");
        return 1;
    }

    //Wait for the customer thread to finish
    if(pthread_join(customer_thread, NULL))
    {
        printf("Error Joining thread");
        return 1;
    }


    //Creating 4 teller
    pthread_t teller_thread[4];
    int teller_nums[4] = {0,1,2,3};

    for(int i = 0; i < 4; i++)
    {
        if(pthread_create(&teller_thread[i], NULL, teller, &teller_nums[i]))
        {
            printf("Error Creating Teller Thread");
            return 1;
        }
    }
    

    //Wait for all the teller threads to finish
    for(int i = 0; i < 4; i++)
    {
        if(pthread_join(teller_thread[i], NULL))
        {
            printf("Error joining teller thread\n");
            return 1;
        }
    }

    //Clean up the queue
    while(dequeue(c_queue).id != 0)

    pthread_mutex_destroy(&mutex);

    free_queue(c_queue);

    return 0;    
}





//Create a customer function
//Integerate activity log with customer function
void* customer(void* arg)
{
    //Open the File for reading
    FILE* fp = fopen(FILE_NAME, "r");
    if(fp == NULL)
    {
        printf("Error Opening File\n");
        return NULL;
    }

    //Read the data from the file to the queue
    //printf("Everthing works just fine"); //Debugging
    int id; 
    char type;
    int lines_read = 0;
    while (fscanf(fp, "%d %c", &id, &type) == 2)
    {
        Customer c = { .id = id, .type = type};
        pthread_mutex_lock(&mutex); //Locking mutex before accessing shared resource
        enqueue(c_queue, c);
        pthread_mutex_unlock(&mutex);//Unlocking the mutex after accessing the shared resource

        //Logging the customers
        activity_log(c, "r_log.txt");
        sleep(tc); //This is to puase the thread periodically using the sleep funtion

        lines_read++;
        if(lines_read >= m)
        {
            break;
        }
    }

    

    //Close the file
    fclose(fp);
    //return(NULL)
    pthread_exit(NULL);
}


//Create a activity Log function
void activity_log(Customer customer, char* filename)
{
    FILE* fp = fopen(filename, "a"); //a file mode which specifies the file to open in append mode

    if(fp == NULL)
    {
        printf("Error File Opening: %s\n", filename);
        return;
    }

    //To store time in specific format
    char buffer[9];
    time_t now = time(NULL);
    strftime(buffer, 9, "%H:%M:%S", localtime(&now)); //using this function to store in specifc format

    //Output to the log file
    fprintf(fp, "-----------------------------------------------------------------------\n");
    fprintf(fp, "Customer%d: %c\n", customer.id, customer.type);
    fprintf(fp, "Arrival time: %s\n", buffer);
    fprintf(fp, "-----------------------------------------------------------------------\n");

    //Close the file
    fclose(fp);

}


//Creating Teller
void* teller(void* arg)
{



    int teller_num = *((int*)arg); //Indicate which teller is serving the customer

    //Record the start time of the teller thread
    time_t start_time = time(NULL);
    char start_time_str[9];
    strftime(start_time_str, sizeof(start_time_str), "%H:%M:%S", localtime(&start_time));

    //Open the files
    FILE *fp = fopen("r_log.txt", "a");

    while(1)
    {
        
        pthread_mutex_lock(&mutex);
        //Ensuring that there are still customers in the queue before trying to dequeue them
        if(c_queue->size == 0)
        {
            //If the queue is empty release the lock 
            pthread_mutex_unlock(&mutex);
            break;//This is to break out of the while loop
        }

        Customer customer = dequeue(c_queue);
        //Release the lock after accessing the queue
        pthread_mutex_unlock(&mutex);
        if(customer.id == 0)
        {
            //If there is no customers left, exit
           break;
           //return NULL;
        }

        //Arrival time need to be fixed since it is the same time as the response
        char arrival_time[9];
        strncpy(arrival_time, customer.arrival_time, sizeof(arrival_time));

        
        // Define response_time once
        char response_time[9];

        time_t t = time(NULL);
        

        //Server the customer based on their type
        switch(customer.type)
        {
            case 'W':
            //Withdrawl option
            printf("Teller %d serving customer %d: withdrawal transaction\n", teller_num, customer.id);
            strftime(response_time, sizeof(response_time), "%H:%M:%S", localtime(&t));
            sleep(tw);
            //Write to log file
            fprintf(fp, "Teller: %d\nCustomer: %d\nArrival Time: %s\nResponse Time: %s\n\n", teller_num, customer.id, arrival_time, response_time);
            customers_served[teller_num]++;
            break;

            //Deposit Option
            case 'D':
            printf("Teller %d serving customer %d: deposit transaction\n", teller_num, customer.id);
            strftime(response_time, sizeof(response_time), "%H:%M:%S", localtime(&t));
            sleep(td);
            fprintf(fp, "Teller: %d\nCustomer: %d\nArrival Time: %s\nResponse Time: %s\n\n", teller_num, customer.id, arrival_time, response_time);
            customers_served[teller_num]++;
            break;

            //Information Option
            case 'I':
            printf("Teller %d serving customer %d: Information request\n", teller_num, customer.id);
            strftime(response_time, sizeof(response_time), "%H:%M:%S", localtime(&t));
            sleep(ti);
            fprintf(fp, "Teller: %d\nCustomer: %d\nArrival Time: %s\nResponse Time: %s\n\n", teller_num, customer.id, arrival_time, response_time);
            customers_served[teller_num]++;
            break;


            default:
            printf("Invalid customer type\n");
            break;
            
        }

        //Record completion time
        char completion_time[9];
        time_t t2 = time(NULL);
        strftime(completion_time, sizeof(completion_time), "%H:%M:%S", localtime(&t2));
        //Write to a file
        fprintf(fp, "Teller: %d\nCustomer: %d\nArrival Time: %s\nCompletion Time: %s\n\n", teller_num, customer.id, arrival_time, completion_time);
        
    }
    

    // Record the termination time of the teller thread
    time_t end_time = time(NULL);
    char end_time_str[9];
    strftime(end_time_str, sizeof(end_time_str), "%H:%M:%S", localtime(&end_time));

    // Write the termination information to the log file
    fprintf(fp, "Termination: Teller %d\n", teller_num);
    fprintf(fp, "Served Customers: %d\n", customers_served[teller_num]);
    fprintf(fp, "Start time: %s\n", start_time_str);
    fprintf(fp, "Termination Time: %s\n\n", end_time_str);

    
    //Increment the number of terminated teller threads
    pthread_mutex_lock(&mutex);
    tellers_terminated++;
    pthread_mutex_unlock(&mutex);

    //Condition to check if the current teller is the last one to terminate
    pthread_mutex_lock(&mutex);
    if (tellers_terminated == 4) 
    {
        fprintf(fp, "Teller Statistic\n");
        for (int i = 0; i < 4; i++) 
        {
            fprintf(fp, "Teller-%d serves %d customers.\n", i, customers_served[i]);
        }
        int total_customers_served = customers_served[0] + customers_served[1] + customers_served[2] + customers_served[3];
        fprintf(fp, "Total number of customers: %d customers.\n", total_customers_served);
    }
    pthread_mutex_unlock(&mutex);

   
    fclose(fp);
    pthread_exit(NULL);
}


//Validating the Input is only integer
bool is_integer(const char *str) 
{
    for (int i = 0; str[i] != '\0'; i++) 
    {
        if (!isdigit(str[i])) {
            return false;
        }
    }
    return true;
}