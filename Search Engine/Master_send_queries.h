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
extern int** fd;
extern int p3write;

void search(char* input,char* token,int** workerids,int numofworkers);
void maxcount(char* token,int** workerids,int numofworkers);
void mincount(char* token,int** workerids,int numofworkers);
void wc(int** workerids,int numofworkers);
int inputmanager(char* input,int** workerids,int numofworkers);
