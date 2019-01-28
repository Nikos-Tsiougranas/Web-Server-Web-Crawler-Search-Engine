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
#ifndef MAP_H
#define MAP_H
class Mymap
{
    int numofdirectories;       //number of directories
    char** pathnames;           //all the pathnames of dirs
    int*  numofdocuments;        //the number of documents
    char*** nameofdocuments;    //all the pathnames of doc
    int** lines;                //numberoflines at each document
    char**** documents;          //each document
    int totalbytes;             //totalbytes till now needed for wc
    int totalwords;             //totalwords till now needed for wc
    int totallines;             //totallines till now needed for wc
    public:
        Mymap();
        ~Mymap();
        void setnumofdirectories(int num);
        void setnumofdocuments(int index,int num,char* name);
        void setnumoflines(int dirindex,int docindex,int num,char* name);
        void insertline(int i,int j,int l,char* line);
        void addbytes(int add){totalbytes+=add;}
        void addword(){totalwords++;}
        char* get_docpath(int i,int j){return nameofdocuments[i][j];}
        void get_all();
        int getnumoflines(int i,int j){return lines[i][j];}
        int gettotalbytes(){return totalbytes;}
        int gettotalwords(){return totalwords;}
        int gettotallines(){return totallines;}
        int getnumofdirectories(){return numofdirectories;}
        int getnumofdocuments(int i){return numofdocuments[i];}
        void getdocpath(char* name,int i,int j){strcpy(name,nameofdocuments[i][j]);}
        char* getdocline(int i,int j,int l){return documents[i][j][l];}
};
#endif