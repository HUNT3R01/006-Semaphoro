# Nombre del compilador
CC = gcc

# Banderas de compilación: 
# -Wall muestra todas las advertencias (buenas prácticas)
# -pthread es OBLIGATORIO para usar hilos y semáforos en Linux
CFLAGS = -Wall -pthread

# Nombre del archivo ejecutable final
TARGET = smart_parking

# Archivos objeto que se van a generar
OBJS = main.o parking.o

# Regla principal: compila todo
all: $(TARGET)

# Cómo construir el ejecutable final uniendo los .o
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Cómo construir main.o
main.o: main.c parking.h
	$(CC) $(CFLAGS) -c main.c

# Cómo construir parking.o
parking.o: parking.c parking.h
	$(CC) $(CFLAGS) -c parking.c

# Regla para limpiar los archivos compilados (útil para reiniciar)
clean:
	rm -f $(OBJS) $(TARGET)