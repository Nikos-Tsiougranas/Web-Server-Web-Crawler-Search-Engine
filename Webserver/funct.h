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

#ifndef FUNCT_H
#define FUNCT_H

extern char* directory;
extern int deathsignal;
extern Queuelist* buffer;
extern pthread_t* tid;
extern pthread_cond_t cond_nonempty;
extern pthread_mutex_t mtx;

int check_input(int argc, char** argv,int* serving_port,int* command_port,int* numofthreads);
void closefun(int numofthreads);

#endif