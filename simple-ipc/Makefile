CC = g++
CFLAGS = -Wall -pthread

all: parent child

parent: parent.cpp shared_memory.h
	$(CC) $(CFLAGS) -o parent parent.cpp

child: child.cpp shared_memory.h
	$(CC) $(CFLAGS) -o child child.cpp

clean:
	rm -f parent child