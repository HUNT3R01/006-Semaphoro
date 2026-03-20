**Laboratorio 006 - Sistema de Estacionamiento Inteligente (Semáforos)**

1. Objetivo del Proyecto:
Se desarrolló una simulación en lenguaje C de un estacionamiento inteligente con N espacios limitados.
El objetivo fue gestionar la concurrencia de 10 autos intentando entrar y salir al mismo tiempo, garantizando que nunca hubiera más autos
estacionados que los espacios disponibles en la vida real.

2. Conceptos y Prácticas Implementadas:
- Multithreading (Hilos): Utilizamos la librería POSIX Threads (pthread) para representar a cada auto
como un hilo independiente que funciona de forma paralela.
- Semáforos Contadores: Implementamos un semáforo (semaphore.h) que actúa como la barrera del estacionamiento. Se inicializa en N, disminuye
cuando un auto entra, y aumenta cuando un auto sale. Si llega a 0, bloquea automáticamente a los autos entrantes.
- Exclusión Mutua (Mutex): Utilizamos candados (pthread_mutex_t) para prevenir las "condiciones de carrera". Aseguramos que
dos autos no sobreescribieran la misma variable estadística al mismo tiempo y que los registros de eventos no se mezclaran.
- Manejo de Archivos: Implementamos la escritura de un archivo .txt en tiempo de ejecución para persistir el historial de eventos del sistema
 sin interferir con la interfaz gráfica de la consola.

3. Desarrollo y Módulos:
El código se dividió estratégicamente en 3 componentes para mantener buenas prácticas de ingeniería de
software:
  3.1 main.c: Actúa como el administrador central. Inicializa los recursos, pregunta al usuario el tamaño del estacionamiento, inicializa
  los threads y al final limpia la memoria.
  3.2 parking.c / .h: Contiene el "cerebro" o ciclo de vida del auto (Llegar, Esperar en el semáforo,
  Estacionarse y Salir).
  3.3 Puntos Extra (Dashboard): Se implementó un hilo monitor adicional que limpia la terminal y dibuja en tiempo real una
  interfaz gráfica mostrando qué cajones están ocupados y cuáles están vacíos.
