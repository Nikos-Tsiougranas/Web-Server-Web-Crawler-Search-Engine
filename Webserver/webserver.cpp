#include "header.h"

using namespace std;

int deathsignal=0;
pthread_t* tid;
int totalbytes=0;
int totalpages=0;
time_t start=0;
Queuelist* buffer=new Queuelist(-1,NULL);
char* directory;
pthread_mutex_t mtx;
pthread_cond_t cond_nonempty;

int main(int argc,char** argv)
{
    start=time(NULL);
    int serving_port=0;
    int command_port=0;
    int numofthreads=0;
    pthread_mutex_init(&mtx, 0);    //initialising mutex
    pthread_cond_init(&cond_nonempty, 0);
    if(check_input(argc,argv,&serving_port,&command_port,&numofthreads)==-1)    //check if input is valid and accessible
        return -1;
    tid=(pthread_t*)malloc(numofthreads*sizeof(pthread_t));
    for(int i=0;i<numofthreads;i++)
        pthread_create(&tid[i],NULL,&thread_fun,NULL);  //create threads
    int server_fd,command_fd;
    if(serverinit(&server_fd,&command_fd,serving_port,command_port)==-1)    //initialise the server
    {
        closefun(numofthreads);
        return -1;
    }
    if(server_running(server_fd,command_fd,numofthreads)==-1) //function which is used while the server is up and running
    {
        closefun(numofthreads);
        return -1;
    }
    closefun(numofthreads);
    return 1;
}