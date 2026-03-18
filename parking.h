#ifndef PARKING_H
#define PARKING_H

#include <pthread.h>
#include <semaphore.h>

extern sem_t parking_semaphore;
extern pthread_mutex_t log_mutex;
extern pthread_mutex_t stats_mutex;
extern int total_cars_parked;
extern double total_wait_time;
extern pthread_mutex_t spots_mutex;
extern int* parking_spots; 
extern int num_spots;

typedef struct {
    int car_id;
} CarInfo;

void* car_thread_routine(void* arg);

#endif