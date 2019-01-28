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
#include "Trienode.h"
#include "Slave_send_response.h"
extern int signalflag;
extern int killed;

int read_sizes(int* linecounter,int *maxlength,char* docfile);   //reads the sizes to know how memory to allocate for the structs
void preparemap(char** paths,int pathcounter,Mymap* mymap);
void preparelines(char** paths,int pathcounter,Mymap* mymap);
void split(char* temp,int i,int j,int l,Mymap* mymap,Trienode* trie);
void create_db(char** paths,int pathcounter,Mymap* mymap,Trienode* trie);
void initialise_db(int id,char* name,int* pathcounter,char*** paths,Mymap* mymap,Trienode* trie);
void child(int id);
int getnumofstrings(char* logname);