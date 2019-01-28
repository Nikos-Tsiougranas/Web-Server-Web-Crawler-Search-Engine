#include "Slave_send_response.h"
using namespace std;

int read_fatherinput(int id,char* name,Mymap* mymap,Trienode* trie)
{
    char* buff=(char*)malloc(1024*sizeof(char));
    int length;
    read (fd[0][1], &length, sizeof(int));
    read(fd[0][1],buff,length);
    time_t curtime;                     //time of query arrival
    time(&curtime);
    struct tm* timeofarrival=localtime(&curtime);
    char* timeofquery=(char*)malloc((strlen(asctime(timeofarrival)))*sizeof(char));
    strcpy(timeofquery,strtok(asctime(timeofarrival),"\n"));
    char* token=strtok(buff," \t\n");   //find which input is given
    if(!strcmp(token,"/search"))
    {
        searchflag=1;
        int counter;
        read (fd[0][1], &counter, sizeof(int));   
        int length; 
        read (fd[0][1], &length, sizeof(int));
        read(fd[0][1],buff,length);
        write(fd[0][0],"ready",6*sizeof(char));     //"ready to start searching" sychronise in order to start all the processes around the same time to execute the query
        char** warray=(char**)malloc(counter*sizeof(char*));
        for(int i=0;i<counter;i++)
            warray[i]=(char*)malloc(256*sizeof(char));
        token=strtok(buff," \t\n");
        Scorelist* score=new Scorelist();
        for(int i=0;i<counter;i++)              //search all the words who are given
        {
            token=strtok(NULL," \t\n");
            memcpy(warray[i],token,strlen(token)+1);
            if(trie->search(timeofquery,warray[i],0,mymap,score)==0)
            {
                char logname[1024];                 //if word not found then just write it to log
                snprintf(logname,sizeof(logname),"log/Worker_%i.txt",getpid());
                FILE* fp=fopen(logname,"a");
                fprintf(fp,"%s",timeofquery);
                fprintf(fp," : ");
                fprintf(fp,"search :");
                fprintf(fp,"%s\n",warray[i]);
                fclose(fp);
            }
        }
        int volume=0;
        if(score!=NULL)
            volume=score->volume();
        char* temp=(char*)malloc(sizeof(int)*sizeof(char));
        memcpy(temp,&id,sizeof(int));       //temp file for checking if the worker managed in time
        sigset_t mask;              
        sigfillset(&mask);                  //block all signals when we are inside searchflag
        if(searchflag)
        {    
            sigprocmask(SIG_SETMASK, &mask, NULL);  
            int code=mkfifo(temp,0666);
            int fd2=open(temp,O_RDONLY);   //wait for father to answer
            close(fd2);                    
            remove(temp);
            free(temp);                     //remove the temp file
            write(fd[0][0],&volume,sizeof(int));    //pass him all the values, paths and documents
            Scorelist* temp=score; 
            if(volume!=0)
            {  
                for(int k=0;k<volume;k++)
                {
                    int i,j,counter;
                    int *lines;
                    temp->getinfo(&i,&j,&lines,&counter);
                    int length=strlen(mymap->get_docpath(i,j))+1;
                    write(fd[0][0],&length,sizeof(int));
                    write(fd[0][0],mymap->get_docpath(i,j),length); //pass path
                    write(fd[0][0],&counter,sizeof(int));
                    write(fd[0][0],lines,counter*sizeof(int));  //pass line ids
                    for(int k=0;k<counter;k++)
                    {
                        length=strlen(mymap->getdocline(i,j,lines[k]))+1;
                        write(fd[0][0],&length,sizeof(int));
                        write(fd[0][0],mymap->getdocline(i,j,lines[k]),length); //pass line of doc
                    }
                    temp=temp->getnext();         
                }
            }
            sigemptyset(&mask);
            sigprocmask(SIG_SETMASK, &mask, NULL);  //unblock signals
        }
        else
            free(temp);
        for(int i=0;i<counter;i++)
            free(warray[i]);
        free(warray);
        delete(score);

    }
    else if(!strcmp(token,"/maxcount"))
    {
        char* path=(char*)malloc(1024*sizeof(char));
        int maxvalue=0;
        read (fd[0][1], &length, sizeof(int));    
        read(fd[0][1],buff,length);
        strcpy(path,buff);
        trie->maxcount(timeofquery,buff,0,&maxvalue,path,mymap);    //search maxcount and return maxvalue
        length=strlen(path)+1;
        write(fd[0][0],&length,sizeof(int));
        write(fd[0][0],path,strlen(path)+1);
        write(fd[0][0],&maxvalue,sizeof(int));
        char logname[1024];
        snprintf(logname,sizeof(logname),"log/Worker_%i.txt",getpid());     //pass the winner file to the log
        FILE* fp=fopen(logname,"a");
        if(maxvalue==0)             //if no winner then just "-" after keyword
            strcpy(path,"-");            
        fprintf(fp,"%s : maxcount : %s :%s (%d)\n",timeofquery,buff,path,maxvalue);
        fclose(fp);
        free(path);        
    }
    else if(!strcmp(token,"/mincount")) //same as above
    {
        char* path=(char*)malloc(1024*sizeof(char));
        int minvalue=0;
        read (fd[0][1], &length, sizeof(int));    
        read(fd[0][1],buff,length);
        strcpy(path,buff);
        trie->mincount(timeofquery,buff,0,&minvalue,path,mymap);
        length=strlen(path)+1;
        write(fd[0][0],&length,sizeof(int));
        write(fd[0][0],path,strlen(path)+1);
        write(fd[0][0],&minvalue,sizeof(int));
        char logname[1024];
        snprintf(logname,sizeof(logname),"log/Worker_%i.txt",getpid());
        FILE* fp=fopen(logname,"a");
        if(minvalue==0)
            strcpy(path,"-");
        fprintf(fp,"%s : mincount : %s : %s (%d)\n",timeofquery,buff,path,minvalue);
        fclose(fp);
        free(path);
    }
    else if(!strcmp(token,"/wc"))
    {   
        int i=mymap->gettotalbytes();   
        write(fd[0][0],&i,sizeof(int));
        i=mymap->gettotalwords();
        write(fd[0][0],&i,sizeof(int));
        i=mymap->gettotallines();
        write(fd[0][0],&i,sizeof(int));
        char logname[1024];
        snprintf(logname,sizeof(logname),"log/Worker_%i.txt",getpid()); //write all the info to log
        FILE* fp=fopen(logname,"a");
        fprintf(fp,"%s : wc : total bytes = %d : total words = %d : total lines = %d\n",timeofquery,mymap->gettotalbytes(),mymap->gettotalwords(),mymap->gettotallines());
        fclose(fp);
    }
    else if(!strcmp(token,"/exit")) //free memomy and return 0 to know the child to die
    {
        token=NULL;
        free(token);
        free(buff);
        free(timeofquery);
        return 0;
    }
    else
    {   
        token=NULL;
        free(token);
        free(buff);
        free(timeofquery);
        return -1;
    }
    token=NULL;
    free(token);
    free(buff);
    free(timeofquery);
    return 1;
}