all:Lab1

CC = g++ -std=c++11

CFLAGS = -Wall

Lab1: main.o 
	$(CC) main.o $(CFLAGS) -o Lab1

main.o: main.cpp 
	$(CC) $(CFLAGS) -c main.cpp 

clean:
	rm -rf *.o Lab1