#############################################
## makefile para crear el ejecutable 'tls' ##
#############################################

# Definicion del compilador
CC = g++

# Definicion de flags
CFLAGS = -Wall -Wextra -ggdb -pthread

# Crear los ejecutables
default: allclean main clean

task.o: task.cpp task.h
	$(CC) $(CFLAGS) -c task.cpp -o task.o

waitqueue.o: waitqueue.cpp waitqueue.h
	$(CC) $(CFLAGS) -c waitqueue.cpp -o waitqueue.o

# Crear el ejecutable 'main'
main: task.o waitqueue.o
	$(CC) $(CFLAGS) *.o main.cpp -o main

# Eliminar todos los .o
clean:
	$(RM) *.o

# Limpieza completa
allclean:
	$(RM) *.o main
