#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <time.h>
#include "parking.h"

oid get_current_time_str(char* buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "[%a %b %d %H:%M:%S %Y]", t);
}

void* car_thread_routine(void* arg) {
    CarInfo* info = (CarInfo*)arg;
    int car_id = info->car_id;
    char time_str[64];
    struct timeval arrival_time, park_time;
    
    // NUEVO: Variable para recordar en qué espacio (índice) nos estacionamos
    int my_spot_index = -1; 
    
    // --- FASE 1: LLEGAR ---
    gettimeofday(&arrival_time, NULL); 
    
    pthread_mutex_lock(&log_mutex);
    get_current_time_str(time_str, sizeof(time_str));
    printf("%s Car %d: Arrived at parking lot\n", time_str, car_id);
    pthread_mutex_unlock(&log_mutex);

    // --- FASE 2: ESPERAR EN LA ENTRADA ---
    sem_wait(&parking_semaphore);

    // --- FASE 3: ESTACIONARSE ---
    gettimeofday(&park_time, NULL); 
    double wait_time = (park_time.tv_sec - arrival_time.tv_sec) + 
                       (park_time.tv_usec - arrival_time.tv_usec) / 1000000.0;

    pthread_mutex_lock(&stats_mutex);
    total_cars_parked++;
    total_wait_time += wait_time;
    pthread_mutex_unlock(&stats_mutex);

    // NUEVO: Buscar un cajón vacío (-1) en el mapa del estacionamiento
    pthread_mutex_lock(&spots_mutex);
    for (int i = 0; i < num_spots; i++) {
        if (parking_spots[i] == -1) { 
            parking_spots[i] = car_id; // Ocupamos el lugar con nuestro ID
            my_spot_index = i;         // Memorizamos dónde nos estacionamos
            break;
        }
    }
    pthread_mutex_unlock(&spots_mutex);

    pthread_mutex_lock(&log_mutex);
    get_current_time_str(time_str, sizeof(time_str));
    // Agregamos en qué lugar se estacionó al log
    printf("%s Car %d: Parked successfully in spot %d (waited %.2f seconds)\n", time_str, car_id, my_spot_index, wait_time);
    pthread_mutex_unlock(&log_mutex);

    // El auto hace sus compras (1 a 5 segundos)
    int park_duration = (rand() % 5) + 1;
    sleep(park_duration);

    // --- FASE 4: SALIR ---
    pthread_mutex_lock(&log_mutex);
    get_current_time_str(time_str, sizeof(time_str));
    printf("%s Car %d: Leaving parking lot from spot %d\n", time_str, car_id, my_spot_index);
    pthread_mutex_unlock(&log_mutex);

    // NUEVO: Liberar visualmente el lugar en el mapa del estacionamiento
    pthread_mutex_lock(&spots_mutex);
    if (my_spot_index != -1) {
        parking_spots[my_spot_index] = -1; // -1 significa que vuelve a estar vacío
    }
    pthread_mutex_unlock(&spots_mutex);

    // Avisamos al semáforo que ya nos fuimos
    sem_post(&parking_semaphore);

    free(info);
    pthread_exit(NULL);
}