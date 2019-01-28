#include "funct.h"

using namespace std;

int check_input(int argc, char** argv,int* command_port,int* numofthreads)  //check if the input is valid and the docs accessible
{
    if(argc!=12)
    {
        cout<<"Wrong number of arguments"<<endl;
        return -1;
    }
    if(strcmp(argv[1],"-h") || strcmp(argv[3],"-p") || strcmp(argv[5],"-c") || strcmp(argv[7],"-t") || strcmp(argv[9],"-d"))
    {
        cout<<"Wrong Input"<<endl;
        return -1;
    }
    strcpy(host_ip,argv[2]);
    for(int i=0;i<strlen(argv[4]);i++)
    {
        if(!isdigit(argv[4][i]))
        {
            cout<<"Serving port socket: wrong number"<<endl;
            return -1;
        }
    }
    serving_port=atoi(argv[4]);
    for(int i=0;i<strlen(argv[6]);i++)
    {
        if(!isdigit(argv[6][i]))
        {
            cout<<"Command port socket: wrong number"<<endl;
            return -1;
        }
    }
    *command_port=atoi(argv[6]);
    for(int i=0;i<strlen(argv[8]);i++)
    {
        if(!isdigit(argv[8][i]))
        {
            cout<<"Thread: wrong number"<<endl;
            return -1;
        }
    }
    *numofthreads=atoi(argv[8]);
    DIR* dir = opendir(argv[10]);
    if(!dir)
    {
        cout<<"Save directory does not exist but I created a new one"<<endl;
        mkdir(argv[10],0700);
    }
    closedir(dir);
    strcpy(save_dir,argv[10]);
    strcpy(st_URL,argv[11]);
}
void closefun(int numofthreads)     
{
    deathsignal=1;
    usedUrls->deletequeue();
    buffer->deletequeue();
    for(int i=0;i<numofthreads;i++)
        pthread_cond_signal(&cond_nonempty);
    for(int i=0;i<numofthreads;i++)
        pthread_join(tid[i],NULL);
    pthread_mutex_destroy(&mtx);
    free(tid);
    free(thread_doing_nothing);
    if(!p2initialisation)   //if the project2 has not been initialise then return here else /exit the project 2 and remove the read-write pipes from filesystem
        return;
    int length=strlen("/exit ");
    write(p3write,&length,sizeof(int));
    write(p3write,"/exit ",length); //send exit
    char buf[3];
    read(p3read,buf,strlen("ok"));  //wait till it is almost dead
    close(p3read);
    close(p3write);
    remove("./Project2adapted/write_read");
    remove("./Project2adapted/read_write");
}