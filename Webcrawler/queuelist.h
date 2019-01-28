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

#ifndef QUEUELIST_H
#define QUEUELIST_H

class Queuelist
{
    char URL[256];
    Queuelist* next;
    public:
        Queuelist(char* GURL,Queuelist* Next);
        void deletequeue();
        Queuelist* push(char* GURL);
        void pop(int length,char** returl);
        int find(char* Gurl);
        int length();
};

#endif