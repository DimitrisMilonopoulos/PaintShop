CC      = gcc
CFLAGS  = -g
RM      = rm -f

default: Simulation

dynamic_Queue.o: dynamic_Queue.c dynamic_Queue.h
	gcc -c dynamic_Queue.c -o dynamic_Queue.o

part.o: part.c part.h
	gcc -c part.c -o part.o

sem_operations.o: sem_operations.c sem_operations.h
	gcc -c sem_operations.c -o sem_operations.o

file_shared_sem_operations.o: file_shared_sem_operations.c file_shared_sem_operations.h
	gcc -c file_shared_sem_operations.c -o file_shared_sem_operations.o

Processes.o: Processes.h dynamic_Queue.o part.o sem_operations.o file_shared_sem_operations.o
	gcc -c Processes.c -o Processes.o
	
main.o: dynamic_Queue.o part.o sem_operations.o file_shared_sem_operations.o Processes.o
	gcc -c main.c -o main.o
	
Simulation: dynamic_Queue.o part.o sem_operations.o file_shared_sem_operations.o Processes.o main.o
	gcc dynamic_Queue.o part.o sem_operations.o file_shared_sem_operations.o Processes.o main.o -o Simulation

clean:
	-rm -f *.key *.o Simulation
