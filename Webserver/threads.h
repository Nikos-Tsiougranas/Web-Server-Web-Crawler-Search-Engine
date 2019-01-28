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
#include "queuelist.h"

#ifndef THREADS_H
#define THREADS_H

extern char* directory;
extern int deathsignal;
extern Queuelist* buffer;
extern pthread_t* tid;
extern pthread_cond_t cond_nonempty;
extern pthread_mutex_t mtx;
extern int totalbytes;
extern int totalpages;

int checkinputmessage(char** filename,char client_message[]);
void server_thread_response(int socket,char client_message[]);
void* thread_fun(void *arg);

#endif