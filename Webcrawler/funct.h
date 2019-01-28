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

#ifndef FUNCT_H
#define FUNCT_H
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

int check_input(int argc, char** argv,int* command_port,int* numofthreads);
void closefun(int numofthreads);

#endif