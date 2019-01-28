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
#include "Map.h"
#ifndef SCORELIST_H
#define SCORELIST_H
class Scorelist
{   
    int file;   //file id
    int doc;    //doc id
    int linecounter;    //number of lines
    int* line;  //line array
    Scorelist* next;
    public:
        Scorelist(int f=-1,int d=-1,int l=-1,Mymap* mymap=NULL);
        ~Scorelist();
        void add(int i,int j,int l,Mymap* mymap);
        void getinfo(int *i,int *j,int **l,int *counter);
        Scorelist* getnext(){return next;}
        int volume();
};
#endif