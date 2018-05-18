#############################################
## makefile para crear el ejecutable 	   ##
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

CPU.o: CPU.cpp CPU.h
	$(CC) $(CFLAGS) -c CPU.cpp -o CPU.o

# Crear el ejecutable 'main'
main: task.o waitqueue.o CPU.o
	$(CC) $(CFLAGS) *.o main.cpp -o main

# Eliminar todos los .o
clean:
	$(RM) *.o

# Limpieza completa
allclean:
	$(RM) *.o main
