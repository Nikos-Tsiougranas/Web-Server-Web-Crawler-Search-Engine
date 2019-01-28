#include "Slave_prepare_db.h"
using namespace std;

int read_sizes(int* linecounter,int *maxlength,char* docfile)   //reads the sizes to know how memory to allocate for the structs
{
    FILE* file = fopen(docfile, "r");   
    if (file == NULL)                   //checking if the file exists
    {  
        cout<<"Error opening file"<<endl;
        return -1;
    }
    char* line=NULL;
    size_t falsebuffer=0; 
    int currlength;
    while(-1!=(currlength=getline(&line,&falsebuffer,file))) //getting lines till EOF
    {
        if(*maxlength<currlength)
            *maxlength=currlength;                          //finding the biggest sentence
        (*linecounter)++;                                   //increasing linecounter
        free(line);
        line=NULL;
        falsebuffer=0;
    }
    fclose(file);
    free(line);
    if(*linecounter==0 || *maxlength < 3)                   //checking if there is a sentence
        return -1;
    return 1;
}
void preparemap(char** paths,int pathcounter,Mymap* mymap)
{
    DIR *d;
    struct dirent *dir;
    for(int i=0;i<pathcounter;i++)
    {   
        d = opendir(paths[i]);
        int numoffiles=0;
        if (d)
        {
            while ((dir = readdir(d)) != NULL)
            {
                if(!strcmp(dir->d_name,".") || !strcmp(dir->d_name,".."))
                    continue;
                numoffiles++;           //find the number of documents worker is responsible for
            }
            closedir(d);
        }
        mymap->setnumofdocuments(i,numoffiles,paths[i]); //set number of documents
    }
}
void preparelines(char** paths,int pathcounter,Mymap* mymap)    //find number of lines each doc has to allocate memory
{
    DIR *d;
    struct dirent *dir;
    FILE* file=NULL;
    for(int i=0;i<pathcounter;i++)
    {           
        d = opendir(paths[i]);
        int numoffiles=0;
        if (d)
        {
            int j=0;
            while ((dir = readdir(d)) != NULL)
            {                
                int lines=0;
                if(!strcmp(dir->d_name,".") || !strcmp(dir->d_name,".."))
                    continue;
                char* buffer=(char*)malloc(1024*sizeof(char));
                memcpy(buffer,paths[i],strlen(paths[i]));
                buffer[strlen(paths[i])]='/';
                memcpy(buffer+strlen(paths[i])+1,dir->d_name,strlen(dir->d_name)+1);
                file = fopen(buffer, "r");
                if(file==NULL)
                {
                    cout<<"Error Opening file"<<endl;
                    return;
                }
                char* line=NULL;
                size_t falsebuffer=0; 
                while(-1!=getline(&line,&falsebuffer,file)) //getting lines till EOF
                {                       
                    lines++;                                  //increasing linecounter
                    free(line);
                    line=NULL;
                    falsebuffer=0;
                }
                fclose(file);
                free(line);
                mymap->setnumoflines(i,j,lines,buffer);     //allocate memory
                j++;
                free(buffer);
            }            
            closedir(d);
        }
    }
}
void split(char* temp,int i,int j,int l,Mymap* mymap,Trienode* trie)
{
    char* token;                                                
    token=strtok(temp," \t\n");
    while(token!=NULL)
    {   
        trie->insert(token,i,j,l);                         //splitting each word and insert it to the trie
        token=strtok(NULL," \t\n");
        mymap->addword();
    }
    free(token);
}
void create_db(char** paths,int pathcounter,Mymap* mymap,Trienode* trie)
{
    preparemap(paths,pathcounter,mymap);
    preparelines(paths,pathcounter,mymap);
    DIR *d;
    struct dirent *dir;
    for(int i=0;i<pathcounter;i++)  //for each dir
    {   
        d = opendir(paths[i]);
        if (d)
        {
            int j=0;
            while ((dir = readdir(d)) != NULL)  //for each doc
            {
                if(!strcmp(dir->d_name,".") || !strcmp(dir->d_name,".."))
                    continue;
                FILE* file = fopen(mymap->get_docpath(i,j), "r");   
                char* line=NULL;
                size_t falsebuffer=0; 
                int l=0;
                while(-1!=getline(&line,&falsebuffer,file)) //getting lines till EOF
                {                      
                    char* temp2=(char*)malloc((strlen(line)+1)*sizeof(char));
                    int flag=0;
                    for(int k=0;k<strlen(line);k++)
                    {
                        if(!flag)
                        {
                            if(line[k]!='<')
                            {
                                int index=strlen(temp2);
                                temp2[index]=line[k];
                                temp2[index+1]='\0';
                            }
                            else
                                flag=1;
                        }
                        if(line[k]=='>')
                            flag=0;
                    }
                    mymap->insertline(i,j,l,temp2);          //insert line and allocate memory according to its length
                    mymap->addbytes(strlen(temp2));          //add bytes info needed for wc
                    char* temp=(char*)malloc((strlen(temp2)+1)*sizeof(char));
                    strcpy(temp,temp2);
                    split(temp,i,j,l,mymap,trie);           //insert to trie
                    free(line);
                    line=NULL;
                    falsebuffer=0;
                    l++;
                    free(temp);
                }
                fclose(file);
                free(line);
                j++;
            }
            closedir(d);
        }
    }
    return;
}
void initialise_db(int id,char* name,int* pathcounter,char*** paths,Mymap* mymap,Trienode* trie)
{
    char* buff=(char*)malloc(1024*sizeof(char));
    sprintf(name,"%dw",id);
    mkfifo(name,0666);
    fd[0][1]=open(name,O_RDONLY);           //open the read pipes
    read (fd[0][1], buff, sizeof(int));
    int pathssigned;
    memcpy(&pathssigned,buff,sizeof(int));
    *paths=(char**)malloc(pathssigned*sizeof(char*));
    int length;
    while(1)
    {
        read (fd[0][1], &length, sizeof(int));
        read(fd[0][1],buff,length*sizeof(char));
        if(!strcmp(buff,"END"))
            break;
        (*paths)[*pathcounter]=(char*)malloc(length*sizeof(char));   
        memcpy((*paths)[*pathcounter],buff,length);
        (*pathcounter)++;
    }
    mymap->setnumofdirectories(*pathcounter);       //set number of directories to allocate memory
    create_db(*paths,*pathcounter,mymap,trie);      
    sprintf(name,"%dr",id);
    mkfifo(name,0666);
    fd[0][0]=open(name,O_WRONLY);           //open write pipes
    write(fd[0][0],"DB_READY",9*sizeof(char));  //ready
    free(buff);
}
void child(int id)
{
    int pathcounter=0;
    char **paths;
    char* name=(char*)malloc(10*sizeof(char));  //name for logfile
    Mymap* mymap=new Mymap();       //mymap
    Trienode* trie=new Trienode();  //trienode      
    initialise_db(id-1,name,&pathcounter,&paths,mymap,trie);
    char logname[512];
    snprintf(logname,sizeof(logname),"log/Worker_%i.txt",getpid());
    FILE *fp=fopen(logname,"wr");
    fclose(fp);
    sigset_t mask;
    while(1)
    { 
        if(signalflag!=0)
        {
            sigfillset(&mask);          //block signals till we reach pause() for the first time
            sigprocmask(SIG_SETMASK, &mask, NULL);
        }
        if(signalflag==0)
        {
            if(read_fatherinput(id,name,mymap,trie)==0) //if 0 then exit is called free memory
            {
                delete mymap;
                delete trie;
                int numberofstrings=getnumofstrings(logname);
                write(fd[0][0],&numberofstrings,sizeof(int));   //send the number of strings in log
                for(int i=0;i<pathcounter;i++)
                    free(paths[i]);
                free(paths);
                // cout<<"Worker "<<getpid()<<" Dead"<<endl;
                close(fd[0][0]);
                close(fd[0][1]);
                sprintf(name,"%dw",id-1);
                remove(name);           //delete pipes
                sprintf(name,"%dr",id-1);
                remove(name);           //delete pipes
                free(name);
                return;
            }
            signalflag=1;
        }
        if(killed)  //sigint or sigterm is sent free memory and die
        {
            delete mymap;
            delete trie;
            int numberofstrings=getnumofstrings(logname);
            close(fd[0][0]);
            close(fd[0][1]);
            sprintf(name,"%dw",id-1);
            remove(name);           //delete pipes
            sprintf(name,"%dr",id-1);
            remove(name);           //delete pipes
            free(name);
            for(int i=0;i<pathcounter;i++)
                free(paths[i]);
            free(paths);
            return;
        }
        sigemptyset(&mask);     //unblock signals
        sigprocmask(SIG_SETMASK, &mask, NULL);
        pause();        
    }
    //queries
}
int getnumofstrings(char* logname)  //return to master the number of keywords searched successfully
{
    FILE *file=fopen(logname,"r");
    int l=0;
    char* line=NULL;
    size_t falsebuffer=0; 
    while(-1!=getline(&line,&falsebuffer,file))
        l++;
    free(line);
    fclose(file);
    if(l==0)                //line number is 0 then return 0
        return 0;
    char** strings=(char**)malloc(l*sizeof(char*));
    for(int i=0;i<l;i++)
    {    
        strings[i]=(char*)malloc(512*sizeof(char));
        strcpy(strings[i],"\0");        //pass the strings
    }
    int counter=0;
    file=fopen(logname,"r");
    char* token;
    char* token2;
    line=NULL;
    falsebuffer=0;
    for(int i=0;i<l;i++)
    {
        getline(&line,&falsebuffer,file);
        token=strtok(line,":");
        token=strtok(NULL,":");
        token=strtok(NULL,":");
        token=strtok(NULL," :");
        if(!strcmp(token,"wc") || !strcmp(token,"maxcount") || !strcmp(token,"mincount"))   //check if the type of queries is what we want else next line
        {
            free(line);
            line=NULL;
            falsebuffer=0;
            continue;
        }       
        token=strtok(NULL," :");
        int flag=1;
        for(int j=0;j<=counter;j++)
        {
            if(!strcmp(token,strings[j]))
            {    
                flag=0; 
                break;
            }
        }
        token2=strtok(NULL," :\n");
        if(flag && token2!=NULL)            //check if the keyword is found anywhere if yes increase counter
        {
            strcpy(strings[counter],token);
            counter++;
        }
        free(line);
        line=NULL;
        falsebuffer=0;
    }
    fclose(file);
    for(int i=0;i<l;i++)
        free(strings[i]);
    free(strings);
    token=NULL;
    free(token);
    free(line);
    return counter;
}