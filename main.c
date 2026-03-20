#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "parking.h"

sem_t parking_semaphore;
pthread_mutex_t log_mutex;
pthread_mutex_t stats_mutex;
pthread_mutex_t spots_mutex;

int total_cars_parked = 0;
double total_wait_time = 0.0;

int* parking_spots;
int num_spots;

void* dashboard_thread_routine(void* arg) {
    while(1) {
        printf("\033[2J\033[H"); 
        
        printf("==========================================\n");
        printf("      SMART PARKING LOT DASHBOARD         \n");
        printf("==========================================\n\n");
        
        // Bloqueamos para leer el arreglo visual sin errores
        pthread_mutex_lock(&spots_mutex);
        for(int i = 0; i < num_spots; i++) {
            if(parking_spots[i] == -1) {
                printf("[ Spot %d: VACIO  ] ", i);
            } else {
                printf("[ Spot %d: AUTO %02d] ", i, parking_spots[i]);
            }
            if ((i + 1) % 5 == 0) printf("\n");
        }
        pthread_mutex_unlock(&spots_mutex);
        
        printf("\n\n------------------------------------------\n");
        printf(">> Presiona CTRL+C para detener la simulacion <<\n");
        
        usleep(300000);
    }
    return NULL;
}

int main() {
    // Limpiar el archivo de logs al iniciar una nueva simulación
    FILE *init_log = fopen("parking_log.txt", "w");
    if(init_log) fclose(init_log);

    // Pedimos el tamaño del estacionamiento
    printf("Ingrese el numero de espacios en el estacionamiento (N): ");
    if (scanf("%d", &num_spots) != 1 || num_spots <= 0) {
        printf("Numero invalido. Usando N = 5 por defecto.\n");
        num_spots = 5;
    }

    // Inicializamos el arreglo del Dashboard (todos en -1 = vacíos)
    parking_spots = (int*)malloc(num_spots * sizeof(int));
    for (int i = 0; i < num_spots; i++) {
        parking_spots[i] = -1; 
    }

    // Inicializamos el semaforo
    sem_init(&parking_semaphore, 0, num_spots);
    pthread_mutex_init(&log_mutex, NULL);
    pthread_mutex_init(&stats_mutex, NULL);
    pthread_mutex_init(&spots_mutex, NULL);

    pthread_t dashboard_thread;
    pthread_create(&dashboard_thread, NULL, dashboard_thread_routine, NULL);

    // Se crean los 10 threads 
    int NUM_CARS = 10;
    pthread_t cars[NUM_CARS];

    for (int i = 0; i < NUM_CARS; i++) {
        CarInfo* info = (CarInfo*)malloc(sizeof(CarInfo));
        info->car_id = i;
        pthread_create(&cars[i], NULL, car_thread_routine, (void*)info);
    }

    for (int i = 0; i < NUM_CARS; i++) {
        pthread_join(cars[i], NULL);
    }

    pthread_cancel(dashboard_thread);
    pthread_join(dashboard_thread, NULL);

    printf("\033[2J\033[H"); 
    printf("\n=== SIMULACION TERMINADA ===\n");
    printf("Total de autos estacionados: %d\n", total_cars_parked);
    if (total_cars_parked > 0) {
        printf("Tiempo de espera promedio: %.2f segundos\n", total_wait_time / total_cars_parked);
    }

    // Limpimos la memoria
    sem_destroy(&parking_semaphore);
    pthread_mutex_destroy(&log_mutex);
    pthread_mutex_destroy(&stats_mutex);
    pthread_mutex_destroy(&spots_mutex);
    free(parking_spots);

    return 0;
}