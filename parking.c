#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include "parking.h"

void get_current_time_str(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "[%a %b %d %H:%M:%S %Y]", t);
}

void* car_thread_routine(void* arg) {
    CarInfo* info = (CarInfo*)arg;
    int car_id = info->car_id;
    char time_str[64];
    struct timeval arrival_time, park_time;
    FILE *log_file;
    
    int my_spot_index = -1; 
    usleep(rand() % 2000000); 

    // --- FASE 1: LLEGAR ---
    gettimeofday(&arrival_time, NULL); 
    
    pthread_mutex_lock(&log_mutex);
    get_current_time_str(time_str, sizeof(time_str));
    log_file = fopen("parking_log.txt", "a");
    if (log_file) {
        fprintf(log_file, "%s Car %d: Arrived at parking lot\n\n", time_str, car_id);
        fclose(log_file);
    }

    printf("%s Car %d: Arrived at parking lot\n\n", time_str, car_id);
    pthread_mutex_unlock(&log_mutex);

    // --- FASE 2: ESPERAR ---
    sem_wait(&parking_semaphore);

    // --- FASE 3: ESTACIONARSE ---
    gettimeofday(&park_time, NULL); 
    double wait_time = (park_time.tv_sec - arrival_time.tv_sec) + 
                       (park_time.tv_usec - arrival_time.tv_usec) / 1000000.0;

    pthread_mutex_lock(&stats_mutex);
    total_cars_parked++;
    total_wait_time += wait_time;
    pthread_mutex_unlock(&stats_mutex);

    pthread_mutex_lock(&spots_mutex);
    for (int i = 0; i < num_spots; i++) {
        if (parking_spots[i] == -1) { 
            parking_spots[i] = car_id; 
            my_spot_index = i;         
            break;
        }
    }
    pthread_mutex_unlock(&spots_mutex);

    pthread_mutex_lock(&log_mutex);
    get_current_time_str(time_str, sizeof(time_str));
    
    log_file = fopen("parking_log.txt", "a");
    if (log_file) {
        fprintf(log_file, "%s Car %d: Parked successfully in spot %d (waited %.2f seconds)\n\n", time_str, car_id, my_spot_index, wait_time);
        fclose(log_file);
    }
    pthread_mutex_unlock(&log_mutex);

    int park_duration = (rand() % 5) + 1;
    sleep(park_duration);

    // --- FASE 4: SALIR ---
    pthread_mutex_lock(&log_mutex);
    get_current_time_str(time_str, sizeof(time_str));
    
    log_file = fopen("parking_log.txt", "a");
    if (log_file) {
        fprintf(log_file, "%s Car %d: Leaving parking lot from spot %d\n\n", time_str, car_id, my_spot_index);
        fclose(log_file);
    }
    pthread_mutex_unlock(&log_mutex);

    pthread_mutex_lock(&spots_mutex);
    if (my_spot_index != -1) {
        parking_spots[my_spot_index] = -1; 
    }
    pthread_mutex_unlock(&spots_mutex);

    sem_post(&parking_semaphore);

    free(info);
    pthread_exit(NULL);
}