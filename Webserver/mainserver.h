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
#include "threads.h"
#include "queuelist.h"

#ifndef MAINSERVER_H
#define MAINSERVER_H

extern time_t start;
extern int totalbytes;
extern int totalpages;
extern pthread_t* tid;
extern pthread_cond_t cond_nonempty;
extern pthread_mutex_t mtx;
extern Queuelist* buffer;

int serverinit(int* server_fd,int* command_fd,int serving_port,int command_port);
int command_port_fun(int fd);
int server_running(int server_fd,int command_fd,int numofthreads);

#endif