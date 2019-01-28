#include <cstdio>
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include <cmath>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <csignal>
#include <ctime>
#include <sys/wait.h>
#include "Scorelist.h"
#include "Map.h"
#ifndef LISTNODE_H
#define LISTNODE_H

class listnode
{
    listnode* next; //next list item
    int dir;         //id of dir
    int file;        //id of file
    int line;       //id of line
    int times;      //how many times at the document
    public:
        listnode(int i,int j,int l):dir(i),file(j),line(l),times(0){next=NULL;}
        ~listnode();
        void add(int i,int j,int l);
        void search(Mymap* mymap,Scorelist* score);
        int volumeperdocument(int d,int f);
        int getminimum(char* timeofquery,char* word,char* name,Mymap* mymap);
        int getmaximum(char* timeofquery,char* word,char* name,Mymap* mymap);
};
#endif