#include "header.h"

using namespace std;

int g = 0;
pthread_t* tid;
int* thread_doing_nothing;
int totalbytes=0;
int totalpages=0;
time_t start=0;
int deathsignal=0;
int finalstate=0;
int workersdone=0;
int p3write=-1;
int p3read=-1;
int p2initialisation=0;
Queuelist* buffer=new Queuelist((char*)"",NULL);
Queuelist* usedUrls=new Queuelist((char*)"",NULL);
char save_dir[256]="";
char st_URL[256]="";
char host_ip[256]="";
int serving_port=-1;
struct sockaddr_in server;
pthread_mutex_t mtx;
pthread_cond_t cond_nonempty;
int main(int argc,char** argv)
{
    start=time(NULL);
    int command_port=0;
    int numofthreads=0;
    pthread_mutex_init(&mtx, 0);
    pthread_cond_init(&cond_nonempty, 0);
    if(check_input(argc,argv,&command_port,&numofthreads)==-1)  //check if input is valid
        return -1;
    thread_doing_nothing=(int*)malloc(numofthreads*sizeof(int));    //initialise threads state
    tid=(pthread_t*)malloc(numofthreads*sizeof(pthread_t));
    for(int i=0;i<numofthreads;i++)                             //create threads
    {    
        int* arg=(int*)malloc(sizeof(*arg));
        *arg=i;
        pthread_create(&tid[i],NULL,&thread_fun,arg);
    }
    int command_fd;
    if(serverinit(&command_fd,command_port)==-1)        //crawler initialisation
    {
        closefun(numofthreads);
        return -1;
    }
    if(server_running(command_fd,numofthreads)==-1)     //crawler running
    {
        closefun(numofthreads);
        return -1;
    }
    closefun(numofthreads);
    return 1;
}