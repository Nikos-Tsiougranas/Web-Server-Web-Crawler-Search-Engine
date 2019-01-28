CC = g++ 
CPPFLAGS = -g -Wall
default: webcrawler clean
webcrawler: webcrawler.o funct.o mainserver.o threads.o queuelist.o -lpthread
	$(CC) $(CFLAGS) -o ../webcrawler webcrawler.o mainserver.o funct.o threads.o queuelist.o -lpthread
funct.o: funct.cpp funct.h
	$(CC) $(CFLAGS) -c funct.cpp
mainserver.o: mainserver.cpp mainserver.h
	$(CC) $(CFLAGS) -c mainserver.cpp
threads.o: threads.cpp threads.h
	$(CC) $(CFLAGS) -c threads.cpp
queuelist.o: queuelist.cpp queuelist.h
	$(CC) $(CFLAGS) -c queuelist.cpp
webcrawler.o: webcrawler.cpp header.h
	$(CC) $(CFLAGS) -c webcrawler.cpp
clean:
	rm *.o