#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>
#include "string.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/poll.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/stat.h>
#include <cerrno>
#include <fcntl.h>
#include "queuelist.h"
#include "threads.h"

#ifndef MAINSERVER_H
#define MAINSERVER_H

extern char host_ip[256];
extern int serving_port;
extern int p3write;
extern int p3read;
extern int p2initialisation;
extern char save_dir[256];
extern char st_URL[256];
extern int deathsignal;
extern Queuelist* buffer;
extern Queuelist* usedUrls;
extern pthread_mutex_t mtx;
extern pthread_cond_t cond_nonempty;
extern pthread_t* tid;
extern int* thread_doing_nothing;
extern int workersdone;
extern struct sockaddr_in server;
extern int totalbytes;
extern int totalpages;
extern time_t start;


int serverinit(int* command_fd,int command_port);
int check_final_state(int numofthreads);
void initialiseProject2(int socket);
void readfromworkers(int socket);
int command_port_fun(int fd,int numofthreads);
int server_running(int command_fd,int numofthreads);

#endif