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

waitqueue.o: waitqueue.cpp waitqueue.h task.o
	$(CC) $(CFLAGS) -c waitqueue.cpp -o waitqueue.o

CPU.o: CPU.cpp CPU.h CFS_RQ.o task.o waitqueue.o
	$(CC) $(CFLAGS) -c CPU.cpp -o CPU.o

CFS_RQ.o: CFS_RQ.cpp CFS_RQ.h task.o
	$(CC) $(CFLAGS) -c CFS_RQ.cpp -o CFS_RQ.o

balancer.o: balancer.cpp balancer.h task.o waitqueue.o
	$(CC) $(CFLAGS) -c balancer.cpp -o balancer.o -std=c++11

console_safe.o: console_safe.cpp console.h
	$(CC) -c console_safe.cpp -o console_safe.o -lncurses -lm

# Crear el ejecutable 'main'
main: task.o waitqueue.o CPU.o balancer.o console_safe.o
	$(CC) $(CFLAGS) *.o main.cpp -o main -lncurses -lm -std=c++11

# Eliminar todos los .o
clean:
	$(RM) *.o

# Limpieza completa
allclean:
	$(RM) *.o main
