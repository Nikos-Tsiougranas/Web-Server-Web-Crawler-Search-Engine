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
#include "Slave_prepare_db.h"

extern int** fd;
int read_paths(char* docfile,char*** paths);
int separate_directories(char** paths,int pathstoseparate,int numofworkers,int** workerids);
int revive(int** workerids,char** paths,int numofworkers,int i);