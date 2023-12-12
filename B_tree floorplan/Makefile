all:Floorplan

CC = g++ -std=c++11

CFLAGS = -Wall

Floorplan: main.o 
	$(CC) main.o $(CFLAGS) -o Floorplan

main.o: main.cpp 
	$(CC) $(CFLAGS) -c main.cpp 

clean:
	rm -rf *.o Floorplan