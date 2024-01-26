all:Lab4

CC = g++ -std=c++11

CFLAGS = -Wall

Lab4: main.o 
	$(CC) main.o $(CFLAGS) -o Lab4

main.o: main.cpp 
	$(CC) $(CFLAGS) -c main.cpp 

clean:
	rm -rf *.o Lab4