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
#include "Scorelist.h"

int read_fatherinput(int id,char* name,Mymap* mymap,Trienode* trie);