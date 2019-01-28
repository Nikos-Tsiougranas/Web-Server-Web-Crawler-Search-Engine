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
#include "Listnode.h"
#include "Map.h"
#ifndef TRIENODE_H
#define TRIENODE_H
using namespace std;
extern int** fd;
extern int searchflag;
class Trienode
{
    Trienode* sibling;  //the brotherlist
    Trienode* child;    //the child
    listnode* list;     //the postinglist
    char value;
    public:
        Trienode();
        ~Trienode();
        void insert(char* token,int i,int j,int l);
        int search(char* timeofquery,char* word,int curr,Mymap* mymap,Scorelist* score);
        void maxcount(char* timeofquery,char* word,int curr,int *maxvalue,char* maxpath,Mymap* mymap);
        void mincount(char* timeofquery,char* word,int curr,int *minvalue,char* minpath,Mymap* mymap);
};
#endif