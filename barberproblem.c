/*
--------------------------------------------------------------------------------------------
------------------------------------------------------
Application of semaphore
------------------------------------------------------
--------------------------------------------------------------------------------------------
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/types.h>
#include <sys/time.h>
void * barber_function(void *idp);
void * customer_function(void *idp);
void service_customer();
void * make_customer_function();
pthread_mutex_t srvCust;    
sem_t barber_ready;         
sem_t customer_ready;
sem_t modifySeats;
int chair_cnt;              
int total_custs;
int available_seats;
int no_served_custs = 0;
time_t waiting_time_sum;
void * barber_function(void *idp){
    int counter = 0;
    while(1){
        sem_wait(&customer_ready);                 
        sem_wait(&modifySeats);                    
        available_seats++;                         
        sem_post(&modifySeats);                    
        sem_post(&barber_ready);                  
    if(available_seats >= 1){
        available_seats--;                     
        printf("Customer[pid = %lu] is waiting.\n", pthread_self());
        printf("Available seats in waiting area: %d\n", available_seats);
        gettimeofday(&start, NULL);            
        sem_post(&customer_ready);             
        sem_post(&modifySeats);                
        sem_wait(&barber_ready);               
        gettimeofday(&stop, NULL);             
        double sec = (double)(stop.tv_usec - start.tv_usec) / 1000000 + (double)(stop.tv_sec - start.tv_sec);
        waiting_time_sum += 1000 * sec;        
        printf("Customer[pid = %lu] is being served. \n", pthread_self());
    }
    else{
        sem_post(&modifySeats);             
        no_served_custs++;
        printf("A Customer left(no seats available in waiting).\n");
    }
    pthread_exit(NULL);
}
void service_customer(){
    int s = rand() % 401;           
    s = s * 1000;               
    usleep(s);
}
void * make_customer_function(){
    int tmp;
    int counter = 0;
    while(counter < total_custs){
        pthread_t customer_thread;
        tmp = pthread_create(&customer_thread, NULL, (void *)customer_function, NULL);
        if(tmp)
            printf("Failed to create thread.");
        counter++;
        usleep(100000);            
    }	 	  	 	      	      	        	 	
}
int main(){
    srand(time(NULL));
    pthread_t barber_1;                      
    pthread_t customer_maker;                
    int tmp;
    pthread_mutex_init(&srvCust, NULL);
    sem_init(&customer_ready, 0, 0);        
    sem_init(&barber_ready, 0, 0);
    sem_init(&modifySeats, 0, 1);
    printf("Please enter the number of seats available in waiting area: \n");
    scanf("%d", &chair_cnt);
    printf("Please enter the total customers: \n");
    scanf("%d", &total_custs);
    available_seats = chair_cnt;
    tmp = pthread_create(&barber_1, NULL, (void *)barber_function, NULL);                   
    if(tmp)
        printf("Failed to create thread.");
    tmp = pthread_create(&customer_maker, NULL, (void *)make_customer_function, NULL);      
    if(tmp)
        printf("Failed to create thread.");
    pthread_join(barber_1, NULL);                                                           
    pthread_join(customer_maker, NULL);
    printf("\n------------------------------------------------\n");
    printf("Average customers' waiting time: %f ms.\n", (waiting_time_sum / (double) (total_custs - no_served_custs)));
    printf("Number of customers that were forced to leave: %d\n", no_served_custs);
}

/*

OUTPUT:
----------------------------------------------------------------------------
--------------------------------------------------------------

Please enter the number of seats available in waiting area:
3
Please enter the total customers:
6
Customer[pid = 10102604303] is waiting.
Available seats in waiting area: 2
Customer[pid = 10102434360] is being served.
Customer[pid = 10128034360] is waiting.
Available seats in waiting area: 2
Customer[pid = 34360101536] is waiting.
Available seats in waiting area: 1
Customer[pid = 34360101792] is waiting.
Available seats in waiting area: 0
Customer was served.
Customer[pid = 34360101280] is being served.
Customer[pid = 34360102048] is waiting.
Available seats in waiting area: 0
A Customer left(no seats available in waiting).
Customer was served.
Customer[pid = 34360101536] is being served.
Customer was served.
Customer[pid = 34360101792] is being served.
Customer was served.
Customer[pid = 34360102048] is being served.
Customer was served.

----------------------------------------------------------------
-----------------------------------------------------------------------------
Average customers' waiting time: 312.800000 ms.
Number of customers that were forced to leave: 1

*/
	