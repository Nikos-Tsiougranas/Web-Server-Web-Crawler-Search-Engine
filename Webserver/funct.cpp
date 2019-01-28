#include "funct.h"

using namespace std;

int check_input(int argc, char** argv,int* serving_port,int* command_port,int* numofthreads) //check if the input is valid
{
    if(argc!=9)
    {
        cout<<"Wrong number of arguments"<<endl;
        return -1;
    }
    if(strcmp(argv[1],"-p") || strcmp(argv[3],"-c") || strcmp(argv[5],"-t") || strcmp(argv[7],"-d"))
    {
        cout<<"Wrong Input"<<endl;
        return -1;
    }
    for(int i=0;i<strlen(argv[2]);i++)
    {
        if(!isdigit(argv[2][i]))
        {
            cout<<"Serving port socket: wrong number"<<endl;
            return -1;
        }
    }
    *serving_port=atoi(argv[2]);
    for(int i=0;i<strlen(argv[4]);i++)
    {
        if(!isdigit(argv[4][i]))
        {
            cout<<"Command port socket: wrong number"<<endl;
            return -1;
        }
    }
    *command_port=atoi(argv[4]);
    for(int i=0;i<strlen(argv[6]);i++)
    {
        if(!isdigit(argv[6][i]))
        {
            cout<<"Thread: wrong number"<<endl;
            return -1;
        }
    }
    *numofthreads=atoi(argv[6]);
    if(access(argv[8],F_OK)==-1)
    {
        cout<<"Root Directory does not exist"<<endl;
        return -1;
    }
    directory=(char*)malloc((strlen(argv[8])+1)*sizeof(char));
    strcpy(directory,argv[8]);
} 

void closefun(int numofthreads) //free all unfreed memory
{
    deathsignal=1;
    free(directory);
    buffer->deletequeue();
    for(int i=0;i<numofthreads;i++)
        pthread_cond_signal(&cond_nonempty);
    for(int i=0;i<numofthreads;i++)
        pthread_join(tid[i],NULL);
    pthread_mutex_destroy(&mtx);
    free(tid);    
}