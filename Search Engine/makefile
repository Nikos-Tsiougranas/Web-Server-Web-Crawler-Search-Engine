CC = g++ 
CPPFLAGS = -g -Wall
default: jobExecutor clean
jobExecutor: Project2.o Map.o Slave_send_response.o Map.o Slave_prepare_db.o Trienode.o Listnode.o Scorelist.o Master_send_queries.o Master_separate_childs.o
	$(CC) $(CFLAGS) -o jobExecutor Project2.o Slave_prepare_db.o Slave_send_response.o Map.o Master_send_queries.o Trienode.o Listnode.o Scorelist.o Master_separate_childs.o
Map.o: Map.cpp Map.h
	$(CC) $(CFLAGS) -c Map.cpp
Slave_send_response.o: Slave_send_response.cpp Slave_send_response.h
	$(CC) $(CFLAGS) -c Slave_send_response.cpp
Slave_prepare_db.o: Slave_prepare_db.cpp Slave_prepare_db.h
	$(CC) $(CFLAGS) -c Slave_prepare_db.cpp
Trienode.o: Trienode.cpp Trienode.h
	$(CC) $(CFLAGS) -c Trienode.cpp
Listnode.o: Listnode.cpp Listnode.h
	$(CC) $(CFLAGS) -c Listnode.cpp
Scorelist.o: Scorelist.cpp Scorelist.h
	$(CC) $(CFLAGS) -c Scorelist.cpp
Master_send_queries.o: Master_send_queries.cpp Master_send_queries.h
	$(CC) $(CFLAGS) -c Master_send_queries.cpp
Master_separate_childs.o: Master_separate_childs.cpp Master_separate_childs.h
	$(CC) $(CFLAGS) -c Master_separate_childs.cpp
Project2.o: Project2.cpp Project2.h
	$(CC) $(CFLAGS) -c Project2.cpp
clean:
	rm *.o