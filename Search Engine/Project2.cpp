#include "Project2.h"

using namespace std;
int signalflag=1;   //signal if a input from father is given
int searchflag=1;   //signal used in search if deadline is gone
int killed=0;       //1-if sigint sigterm are sent
int **fd;
int p3write=-1;
int p3read=-1;
pid_t* childid;
static void killreceived(int id)
{
    killed=1;
    return;  
}
static void signalreceived(int id)
{
    signalflag=0;
    return;
}
static void signalreceived2(int id)
{
    searchflag=0;
    return;
}
static void handler(int id)
{
    int i=0;
    while(1)
    {
        if(childid[i]==0)
        {   
            childid[i] = wait(NULL);    //check all childen if they are alive
            break;
        }
        i++;
    }
    return;
}
void Registersignals()
{
    signal(SIGINT,killreceived);
    signal(SIGTERM,killreceived);
    signal(SIGUSR1,signalreceived);
    signal(SIGUSR2 ,signalreceived2);
    signal(SIGCHLD, handler);            
}
int main(int argc,char* argv[])
{
    chdir("./Project2adapted");
    Registersignals();    
    p3write=open("read_write",O_WRONLY);    
    if((argc<3 || argc==4)  || strcmp(argv[1],"-d")) //checking if the arguments are valid
    {
        cout<<"Wrong arguments"<<endl;
        return -1;
    }
    int numofworkers;
    if(argc==5)
    {    
        if(strcmp(argv[3],"-w"))
        {
            cout<<"Wrong arguments"<<endl;
            return -1;
        }
        numofworkers=atoi(argv[4]);
        if(numofworkers==0)
        {
            cout<<"Wrong arguments"<<endl;
            return -1;
        }
    }
    else
        numofworkers=3;                 //if no information is given for number of workers then default number is 3
    char** paths;
    int numofpaths;
    numofpaths=read_paths(argv[2],&paths);  //reading paths and check if the document given exists    
    if(numofpaths==-1)
    {
        cout<<"Wrong pathfile"<<endl;
        return -1;
    }
    fd=(int**)malloc(numofworkers*sizeof(int*));    //where pipe numbers are saved
    for(int i=0;i<numofworkers;i++)
        fd[i]=(int*)malloc(2*sizeof(int));
    int** workerids=(int**)malloc(numofworkers*sizeof(int*));   //where infos about path distribution and workerids are saved
    int oldnumofworkers=numofworkers;       //old number of workers keeped cause they may are too many
    for(int i=0;i<numofworkers;i++)
        workerids[i]=(int*)malloc(3*sizeof(int));
    numofworkers=separate_directories(paths,numofpaths,numofworkers,workerids);  //return the number of worker needed
    if(numofworkers==0)                     //if return 0 then it is the child and we free memory allocated
    {     
        for(int i=0;i<oldnumofworkers;i++)
        {   
            free(fd[i]); 
            free(workerids[i]);
        }
        free(workerids);
        free(fd);
        for(int i=0;i<numofpaths;i++)
            free(paths[i]);
        free(paths);
        return 1;
    }
    int length=strlen("Workers and DB ready")+1;
    write(p3write,&length,sizeof(int));
    write(p3write,"Workers and DB ready",length);
    char* input;
    int inputlength=0;
    childid=(pid_t*)malloc(numofworkers*sizeof(pid_t)); //array for storing ids of dead children to revive them
    for(int i=0;i<numofworkers;i++)
        childid[i]=0;
    p3read=open("write_read",O_RDONLY);    
    while(1)
    {
        read(p3read,&inputlength,sizeof(int));
        input=(char*)malloc(inputlength*sizeof(char));
        read(p3read,input,inputlength);
        if(killed)          //if sigint or sigterm is caught inform the workers and before die free memory
        {
            for(int i=0;i<numofworkers;i++)
                kill(workerids[i][0],SIGINT);
            free(input);
            for(int i=0;i<oldnumofworkers;i++)
            {   
                free(fd[i]);  
                free(workerids[i]);
            }
            free(fd);
            free(workerids);
            for(int i=0;i<numofpaths;i++)
                free(paths[i]);
            free(paths);
            free(childid);
            return 1;
        }
        for(int index=0;index<numofworkers;index++)  //check if children are alive from the array
        {
            if(childid[index]!=0)
            {
                int counter;
                for(counter=0;counter<numofworkers;counter++)   //find the correct place at workerids
                    if(childid[index]==workerids[counter][0])
                        break;
                if(!revive(workerids,paths,numofworkers,counter)) //if return 0 then it is the child and we free memory allocated
                {
                    free(input);
                    for(int i=0;i<oldnumofworkers;i++)
                    {   
                        free(fd[i]);  
                        free(workerids[i]);
                    }
                    free(fd);
                    free(workerids);
                    for(int i=0;i<numofpaths;i++)
                        free(paths[i]);
                    free(paths);
                    free(childid);
                    return 1;
                }
                childid[index]=0;
            }
        }
        int ret=inputmanager(input,workerids,numofworkers);
        if(ret==-1)                                     //checking if they are valid
            cout<<"Wrong Input"<<endl;
        else if(ret==2)     //exit is called free memory
        {
            write(p3write,"ok",strlen("ok"));
            free(input);
            for(int i=0;i<oldnumofworkers;i++)
            {   
                free(fd[i]);  
                free(workerids[i]);
            }
            free(fd);
            free(workerids);
            for(int i=0;i<numofpaths;i++)
                free(paths[i]);
            free(paths);
            free(childid);
            break;
        }
        free(input);
        input=NULL;
        inputlength=0; 
    }
    return 1;
}