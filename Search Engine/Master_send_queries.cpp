#include "Master_send_queries.h"
using namespace std;

void search(char* input,char* token,int** workerids,int numofworkers)
{
    token=strtok(NULL," \t\n");
    if(token==NULL)                         //check the validity of the input
    {
        cout<<"Wrong Input"<<endl;
        return;
    }
    int counter=0;
    while(strcmp(token,"-d"))
    {
        counter++;
        token=strtok(NULL," \t\n");
        if(token==NULL)
        {
            cout<<"Wrong Input"<<endl;
            return;
        }
    }
    if(counter==0)
    {
        cout<<"Wrong Input"<<endl;
        return;
    }
    token=strtok(NULL," \t\n");
    if(token==NULL)
    {
        cout<<"Wrong Input"<<endl;
        return;
    }
    for(int i=0;i<strlen(token);i++)
    {
        if(!isdigit(token[i]))
        {
            cout<<"Wrong Input"<<endl;
            return;
        }
    }
    int timer=atoi(token);
    if(timer==0)
    {
        cout<<"Invalid deadline set"<<endl;     //if deadline=0 then invalid
        return;
    }
    char** warray=(char**)malloc(counter*sizeof(char*));
    for(int i=0;i<counter;i++)
        warray[i]=(char*)malloc(256*sizeof(char));
    for(int i=0;i<numofworkers;i++)
    {
        kill(workerids[i][0],SIGUSR1);          //send signal to inform them for querie
        int length;
        length=strlen("/search")+1;
        write(fd[i][0],&length,sizeof(int));
        write(fd[i][0],"/search",length*sizeof(char));  //write all the infos
        write(fd[i][0],&counter,sizeof(int));
        length=strlen(input)+1;
        write(fd[i][0],&length,sizeof(int));
        write(fd[i][0],input,length*sizeof(char));
    }
    for(int i=0;i<numofworkers;i++)                 //wait for child to reach and be ready
        read(fd[i][1],&input,6*sizeof(char));
    time_t s=time(NULL)+timer;
    while(s - time(NULL)>0);
    for(int i=0;i<numofworkers;i++)                 //inform the time is over
        kill(workerids[i][0],SIGUSR2);
    char* buffer=(char*)malloc(10000*sizeof(char));
    write(p3write,&numofworkers,sizeof(int));
    for(int i=0;i<numofworkers;i++)
    {
        int j=i+1;
        char* name=(char*)malloc(sizeof(int)*sizeof(char));
        memcpy(name,&j,sizeof(int));
        int code=mkfifo(name,0666);
        int tempfd=open(name,O_WRONLY | O_NONBLOCK);        //check if the child reached hear with tempfile
        if(tempfd==-1)
        {
            char temp[512]="";
            int volume2=0;
            read(p3write,&volume2,sizeof(int));
            sprintf(temp,"Worker %d did not respond on time\r\n",workerids[i][0]);
            int length2=strlen(temp)+1;
            write(p3write,&length2,sizeof(int));
            write(p3write,temp,length2);
            remove(name);
            free(name);       
            continue;
        }
        close(tempfd);
        remove(name);   
        free(name);            
        int length;
        int volume=-1;
        read(fd[i][1],&volume,sizeof(int)); //read all the values and print them
        write(p3write,&volume,sizeof(int));
        if(volume==0)
        {    
            char temp[512]="";
            sprintf(temp,"Worker %d found no match\r\n",workerids[i][0]);
            int length2=strlen(temp)+1;
            write(p3write,&length2,sizeof(int));
            write(p3write,temp,length2);
            continue;
        }
        for(int n=0;n<volume;n++)
        {
            int counter2;
            read(fd[i][1],&length,sizeof(int));
            write(p3write,&length,sizeof(int));
            read(fd[i][1],buffer,length*sizeof(char));
            write(p3write,buffer,length);
            read(fd[i][1],&counter2,sizeof(int));
            write(p3write,&counter2,sizeof(int));
            int *lines=(int*)malloc(counter2*sizeof(int));
            read(fd[i][1],lines,counter2*sizeof(int));
            write(p3write,lines,counter2*sizeof(int));
            for(int k=0;k<counter2;k++)
            {    
                read(fd[i][1],&length,sizeof(int));
                write(p3write,&length,sizeof(int));
                read(fd[i][1],buffer,length);
                write(p3write,buffer,length);
            }
            free(lines);
        }
    }
    free(buffer);
    for(int i=0;i<counter;i++)
        free(warray[i]);
    free(warray);
}
void maxcount(char* token,int** workerids,int numofworkers)
{
    token=strtok(NULL," \t\n");
    if(token==NULL)                 //validity of input
    {
        cout<<"Wrong Input"<<endl;
        return;
    }
    if(strtok(NULL," \t\n")!=NULL)
    {
        cout<<"Wrong Input"<<endl;
        return;
    }
    int j;
    char* buffer=(char*)malloc(1024*sizeof(char));
    int maximum=0;
    char* maximumbuffer=(char*)malloc(1024*sizeof(char));
    for(int i=0;i<numofworkers;i++)
    {
        kill(workerids[i][0],SIGUSR1);                  //send the querie
        int length=strlen("/maxcount")+1;
        write(fd[i][0],&length,sizeof(int));
        write(fd[i][0],"/maxcount",length*sizeof(char));
        length=strlen(token)+1;
        write(fd[i][0],&length,sizeof(int));
        write(fd[i][0],token,length*sizeof(char));
        read(fd[i][1],&length,sizeof(int));                 //wait and get the answers
        read(fd[i][1],buffer,length*sizeof(char));
        int tempvalue=0;
        read(fd[i][1],&tempvalue,sizeof(int));
        if(tempvalue>maximum && tempvalue!=0)               //sort all the answers
        {
            maximum=tempvalue;
            strcpy(maximumbuffer,buffer);
        }   
        else if(tempvalue==maximum && tempvalue!=0)         //if the values are the same compare the paths
        {
            if(strcmp(maximumbuffer,buffer)>0)
                strcpy(maximumbuffer,buffer);
        }
    }
    if(maximum!=0)
        cout<<token<<" found at path:"<<maximumbuffer<<" "<<maximum<<" times"<<endl;
    else
        cout<<"Word "<<token<<" not found"<<endl;
    free(maximumbuffer);
    free(buffer); 
}
void mincount(char* token,int** workerids,int numofworkers) //same as maxcount
{
    token=strtok(NULL," \t\n");
    if(token==NULL)
    {
        cout<<"Wrong Input"<<endl;
        return;
    }
    if(strtok(NULL," \t\n")!=NULL)
    {
        cout<<"Wrong Input"<<endl;
        return;
    }
    int j;
    char* buffer=(char*)malloc(1024*sizeof(char));
    int minimum=1000000000;
    char* minimumbuffer=(char*)malloc(1024*sizeof(char));
    for(int i=0;i<numofworkers;i++)
    {
        kill(workerids[i][0],SIGUSR1);
        int length=strlen("/mincount")+1;
        write(fd[i][0],&length,sizeof(int));
        write(fd[i][0],"/mincount",length*sizeof(char));
        length=strlen(token)+1;
        write(fd[i][0],&length,sizeof(int));
        write(fd[i][0],token,length*sizeof(char));
        read(fd[i][1],&length,sizeof(int));
        read(fd[i][1],buffer,length*sizeof(char));
        int tempvalue=0;
        read(fd[i][1],&tempvalue,sizeof(int));
        if(tempvalue<minimum && tempvalue!=0)
        {
            minimum=tempvalue;
            strcpy(minimumbuffer,buffer);
        }
        else if(tempvalue==minimum && tempvalue!=0)
        {
            if(strcmp(minimumbuffer,buffer)>0)
                strcpy(minimumbuffer,buffer);
        }
    }
    if(minimum!=1000000000)
        cout<<token<<" found at path:"<<minimumbuffer<<" "<<minimum<<" times"<<endl;
    else
        cout<<"Word "<<token<<" not found"<<endl;
    free(minimumbuffer);
    free(buffer);
}
void wc(int** workerids,int numofworkers)
{
    if(strtok(NULL," \t\n")!=NULL)
    {
        cout<<"Wrong Input"<<endl;
        return;
    }
    int totalwords=0;
    int totalbytes=0;
    int totallines=0;
    int workerwords,workerbytes,workerlines;
    char* buffer=(char*)malloc(sizeof(int)*sizeof(char));
    for(int i=0;i<numofworkers;i++)
    {
        kill(workerids[i][0],SIGUSR1);      //send signal and wait for answers
        int length=strlen("/wc")+1;
        write(fd[i][0],&length,sizeof(int));
        write(fd[i][0],"/wc",length*sizeof(char));
        read(fd[i][1],&workerbytes,sizeof(int));
        read(fd[i][1],&workerwords,sizeof(int));
        read(fd[i][1],&workerlines,sizeof(int));
        totalbytes+=workerbytes;          //add to the total number
        totalwords+=workerwords;
        totallines+=workerlines;
    }
    cout<<"Total bytes: "<<totalbytes<<endl<<"Total words: "<< totalwords<<endl<<"Total lines: "<< totallines<<endl; //print
    free(buffer);
}
int inputmanager(char* input,int** workerids,int numofworkers)
{
    char* temp=(char*)malloc((strlen(input)+1)*sizeof(char));
    strcpy(temp,input);         //save input before cutting it with strtok
    char* token=strtok(temp," \t\n");
    if(token==NULL)                 //check validity
    {
        cout<<"Wrong Input"<<endl;
        free(temp);
        free(token);
        return 1;
    }
    if(!strcmp(token,"/search"))
        search(input,token,workerids,numofworkers);
    else if(!strcmp(token,"/maxcount"))
        maxcount(token,workerids,numofworkers);
    else if(!strcmp(token,"/mincount"))
        mincount(token,workerids,numofworkers);
    else if(!strcmp(token,"/wc"))
        wc(workerids,numofworkers);
    else if(!strcmp(token,"/exit"))
    {
        // cout<<"Exiting..."<<endl;   //exiting
        for(int i=0;i<numofworkers;i++)
        {
            int id=i+1;
            kill(workerids[i][0],SIGUSR1);      //send signals to all workers
            char* name=(char*)malloc(sizeof(int)*sizeof(char));     //exit them
            memcpy(name,&id,sizeof(int));
            int length=strlen(token)+1;
            write(fd[i][0],&length,sizeof(int));
            write(fd[i][0],token,length);
            int numberofstrings=0;
            read(fd[i][1],&numberofstrings,sizeof(int));        //wait for numberofstring info
            // cout<<"Worker "<<workerids[i][0]<<" found "<<numberofstrings<<" strings"<<endl;
            free(name);            
        }
        token=NULL;
        free(token);
        free(temp);
        return 2;
    }
    else
    {
        cout<<"Wrong Input"<<endl;
    }
    free(temp);
    token=NULL;
    free(token);
    return 1;
}