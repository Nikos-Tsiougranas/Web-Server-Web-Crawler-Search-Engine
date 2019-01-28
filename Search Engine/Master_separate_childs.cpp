#include "Master_separate_childs.h"
using namespace std;

int read_paths(char* docfile,char*** paths)
{
    FILE* file = fopen(docfile, "r");   
    if (file == NULL)                   //checking if the file exists
    {  
        cout<<"Error opening file"<<endl;
        return -1;
    }
    char* line=NULL;
    size_t falsebuffer=0; 
    int linecounter=0;
    while(-1!=(getline(&line,&falsebuffer,file))) //getting lines till EOF
    {
        linecounter++;                                   //increasing linecounter
        free(line);
        line=NULL;
        falsebuffer=0;
    }
    fclose(file);
    free(line);
    if(linecounter==0)                   //checking if there is a sentence
        return -1;
    *paths=(char**)malloc(linecounter*sizeof(char*));
    for(int i=0;i<linecounter;i++)
        (*paths)[i]=(char*)malloc(1024*sizeof(char));
    FILE* file2 = fopen(docfile, "r");   
    char* line2=NULL;
    size_t falsebuffer2=0; 
    int i=0;
    while(-1!=(getline(&line2,&falsebuffer2,file2))) //getting lines till EOF
    {
        char* token=strtok(line2,"\n");
        strcpy((*paths)[i],token);
        i++;
        free(line2);
        line2=NULL;
        falsebuffer2=0;
    }    
    fclose(file2);
    free(line2);
    return linecounter;
}
int separate_directories(char** paths,int pathstoseparate,int numofworkers,int** workerids)
{
    int currentpath=0;
    int pid;
    int i=0;
    char* name=(char*)malloc(10*sizeof(char));
    while(pathstoseparate>0)
    {
        pid=fork();     //fork child
        if(pid==0)
        {    
            child(i+1); //if child call child function
            free(name);
            return 0;
        }
        else
        {
            workerids[i][0]=pid;    //pass pid to workerids
            int pathssigned=0;
            pathssigned=ceil((double)pathstoseparate/((double)numofworkers-(double)i)); //pass the number of paths signed to table
            workerids[i][1]=currentpath;        //pass the number of path we are now        
            workerids[i][2]=pathssigned;                    
            sprintf(name,"%dw",i);
            int code=mkfifo(name,0666);         //create pipe
            fd[i][0] = open (name,O_WRONLY);    //open
            int n;
            write(fd[i][0],&pathssigned,sizeof(int));   //let the worker know which paths is signed
            for(int j=0;j<pathssigned;j++)
            {
                n=strlen(paths[currentpath+j])+1;
                write(fd[i][0],&n,sizeof(int));
                write(fd[i][0],paths[currentpath+j],n*sizeof(char));
            }
            n=4;
            write(fd[i][0],&n,sizeof(int));
            write(fd[i][0],"END",n*sizeof(char));   //inform him we reached the end
            pathstoseparate-=pathssigned;
            currentpath+=pathssigned;
            i++;            
        }
        if(i==numofworkers)
            break;
    }
    
    char* buffer=(char*)malloc(9*sizeof(char));
    for(int j=0;j<i;j++)
    {        
        sprintf(name,"%dr",j);
        int code=mkfifo(name,0666);         //create and open read pipe and wait till it answers
        fd[j][1] = open (name,O_RDONLY);        
        read(fd[j][1],buffer,9*sizeof(char));
        if(strcmp(buffer,"DB_READY"))
        {    
            cout<<"Error"<<endl;
            free(name);
            free(buffer);
            return -1;
        }
    }
    free(name);
    free(buffer);
    return i;
}
int revive(int** workerids,char** paths,int numofworkers,int i) //we follow the same path as above(separate_directories) but we already know which the paths info we need
{
    int l=i+1;  
    char* name1=(char*)malloc(10*sizeof(char));
    char* name2=(char*)malloc(10*sizeof(char));
    sprintf(name1,"%dw",i);
    sprintf(name2,"%dr",i);
    int pid=fork();
    if(pid==0)
    {    
        child(l);
        free(name1);
        free(name2);
        return 0;
    }
    else
    {
        workerids[i][0]=pid;
        int code=mkfifo(name1,0666);
        fd[i][0] = open (name1,O_WRONLY);
        int n;
        write(fd[i][0],&workerids[i][2],sizeof(int));
        for(int j=0;j<workerids[i][2];j++)
        {
            n=strlen(paths[workerids[i][1]+j])+1;
            write(fd[i][0],&n,sizeof(int));
            write(fd[i][0],paths[workerids[i][1]+j],n*sizeof(char));
        }
        n=4;
        write(fd[i][0],&n,sizeof(int));
        write(fd[i][0],"END",n*sizeof(char));
    }
    char* buffer=(char*)malloc(9*sizeof(char));
    int code=mkfifo(name2,0666);
    fd[i][1] = open (name2,O_RDONLY);     
    read(fd[i][1],buffer,9*sizeof(char));
    if(strcmp(buffer,"DB_READY"))
    {    
        cout<<"Error"<<endl;
        free(buffer);
        free(name1);
        free(name2);
        return -1;
    }
    free(buffer);
    free(name1);
    free(name2);
    return 1;
}