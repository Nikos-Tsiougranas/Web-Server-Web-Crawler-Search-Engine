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

#ifndef QUEUELIST_H
#define QUEUELIST_H

class Queuelist
{
    int fd;
    Queuelist* next;
    public:
        Queuelist(int filedescriptor,Queuelist* Next=NULL):fd(filedescriptor){next=Next;}
        void deletequeue();
        Queuelist* push(int filedescriptor);
        int pop(int length);
        int length();
};

#endif