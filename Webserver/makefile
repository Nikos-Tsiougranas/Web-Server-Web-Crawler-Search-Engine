CC = g++ 
CPPFLAGS = -g -Wall
default: webserver clean
webserver: webserver.o funct.o mainserver.o threads.o queuelist.o -lpthread
	$(CC) $(CFLAGS) -o ../webserver webserver.o mainserver.o funct.o threads.o queuelist.o -lpthread
funct.o: funct.cpp funct.h
	$(CC) $(CFLAGS) -c funct.cpp
mainserver.o: mainserver.cpp mainserver.h
	$(CC) $(CFLAGS) -c mainserver.cpp
threads.o: threads.cpp threads.h
	$(CC) $(CFLAGS) -c threads.cpp
queuelist.o: queuelist.cpp queuelist.h
	$(CC) $(CFLAGS) -c queuelist.cpp
webserver.o: webserver.cpp header.h
	$(CC) $(CFLAGS) -c webserver.cpp
clean:
	rm *.o