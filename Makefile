CC = gcc
CFLAGS = -Wall
LIBS = -pthread

all: cq

cq: cq.o c_queue.o
	$(CC) $(CFLAGS) -o cq cq.o c_queue.o $(LIBS)

cq.o: cq.c c_queue.h
	$(CC) $(CFLAGS) -c cq.c

c_queue.o: c_queue.c c_queue.h
	$(CC) $(CFLAGS) -c c_queue.c

clean:
	rm -f *.o cq