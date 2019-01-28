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

#ifndef THREADS_H
#define THREADS_H

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

int contentlength(char client_message[]);
void thread_send_request(int server_fd,char URL[]);
void find_the_urls(char* doc);
void save_the_file(char* doc,char sURL[]);
void* thread_fun(void *arg);

#endif