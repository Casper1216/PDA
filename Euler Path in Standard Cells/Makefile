all:Lab3

CC = g++ -std=c++11

CFLAGS = -Wall

Lab3: main.o 
	$(CC) main.o $(CFLAGS) -o Lab3

main.o: main.cpp 
	$(CC) $(CFLAGS) -c main.cpp 

clean:
	rm -rf *.o Lab3