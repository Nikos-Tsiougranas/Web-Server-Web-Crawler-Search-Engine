#include "mainserver.h"

using namespace std;

int serverinit(int* command_fd,int command_port)
{
    int opt=1;                                      
    *command_fd=socket(AF_INET,SOCK_STREAM,0);  //initialise command port
    struct sockaddr_in command;
    if (*command_fd==-1)
    {
        perror("Could not create socket");
        return -1;
    }
    setsockopt(*command_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt));
    struct hostent *he;
    struct in_addr **addr_list;
    he=gethostbyname(host_ip);
    addr_list=(struct in_addr **)he->h_addr_list;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(inet_ntoa(*addr_list[0]));
    server.sin_port = htons( serving_port );
    command.sin_family = AF_INET;
    command.sin_addr.s_addr = INADDR_ANY;
    command.sin_port = htons( command_port );
    if(bind(*command_fd,(struct sockaddr *)&command , sizeof(command)) < 0)
    {
        perror("bind failed. Error");
        return -1;
    }
    if (listen(*command_fd, 10) < 0)
	{
		perror("listen");
		return -1;
	}
    pthread_mutex_lock(&mtx);           
    buffer=buffer->push(st_URL);        //push the starting URL to wait buffer
    usedUrls=usedUrls->push(st_URL);    //placed it to the ones that will be used
    pthread_mutex_unlock(&mtx);
    pthread_cond_signal(&cond_nonempty);
}
int check_final_state(int numofthreads) //check if the threads are all dead cause if they are then the save_dir is constructed
{
    int flag=0;
    for(int i=0;i<numofthreads;i++)
    {    
        if(thread_doing_nothing[i]==0)
            flag=1;
    }
    return flag;
}
void initialiseProject2(int socket)
{
    FILE* file=fopen("./Project2adapted/docfile.txt","w");
    DIR* dir=opendir(save_dir);
    struct dirent *ent;
    while ((ent = readdir (dir)) != NULL)   //create the docfile.txt for Project2 to read
    {
        char buffer[256]="";
        if(!strcmp(ent->d_name,".") || !strcmp(ent->d_name,".."))
            continue;
        if(save_dir[0]=='/')
            strcat(buffer,save_dir);   
        else
        {
            strcat(buffer,"../");
            strcat(buffer,save_dir);
            if(buffer[strlen(save_dir)-1]!='/')
                strcat(buffer,"/");
        }
        strcat(buffer,ent->d_name);
        fprintf(file,"%s\n",buffer);
    }
    closedir(dir);
    fclose(file);
    char cwd[1024]="";
    getcwd(cwd, sizeof(cwd));
    mkfifo("./Project2adapted/write_read",0666);    //create read write pipes
    mkfifo("./Project2adapted/read_write",0666);
    int pid=fork();                                 //fork
    if(pid==0)
    {
        char buff[256]="";
        strcat(buff,cwd);
        strcat(buff,"/Project2adapted/jobExecutor");
        close(socket);
        char *args[] = {buff, (char*)"-d", (char*)"docfile.txt",NULL }; 
	    execv(args[0], args);   //exec
        exit(0);
    }
    char* buffer;
    int length;
    p3read=open("./Project2adapted/read_write",O_RDONLY);      //open the pipes
    read(p3read,&length,sizeof(int));
    buffer=(char*)malloc(length*sizeof(char));
    read(p3read,buffer,length);
    p3write=open("./Project2adapted/write_read",O_WRONLY);        
    free(buffer);
    return;
}

void readfromworkers(int socket)    //function to read the response from project 2 with pipes
{
    int volume=-1;
    int numofworkers=-1;
    int length=0;
    read(p3read,&numofworkers,sizeof(int));
    for(int i=0;i<numofworkers;i++)
    {
        read(p3read,&volume,sizeof(int));
        if(volume==0)
        {
            char buff[4096]="";
            read(p3read,&length,sizeof(int));
            read(p3read,buff,length);
            write(socket,buff,length);
            continue;
        }
        for(int n=0;n<volume;n++)
        {
            int counter2;
            char buffer[4096]="";
            read(p3read,&length,sizeof(int));
            read(p3read,buffer,length);
            strcat(buffer,"\r\n");
            write(socket,buffer,strlen(buffer));
            read(p3read,&counter2,sizeof(int));
            int *lines=(int*)malloc(counter2*sizeof(int));
            read(p3read,lines,counter2*sizeof(int));
            for(int k=0;k<counter2;k++)
            {
                char send_message[4102]="";
                char buff[4096]="";
                sprintf(send_message,"%d ",lines[k]);
                read(p3read,&length,sizeof(int));
                read(p3read,buff,length);
                strcat(buff,"\r\n");
                strcat(send_message,buff);
                write(socket,send_message,strlen(send_message));
            }
            free(lines);
        }
    }
}

int command_port_fun(int fd,int numofthreads)
{
    int socket=accept(fd,NULL,NULL);
    char client_message[1024]="";
    int read_size = recv(socket , client_message , sizeof(client_message)-1 , 0);
    if(read_size>0)
    {
        if(!strcmp(client_message,"STATS\r\n"))
        {
            time_t end=time(NULL);
            int hours=(end-start)/3600;
            int resttime=(end-start)%3600;
            int min=resttime/60;
            resttime=resttime%60;
            int sec=resttime;
            char output[256];
            pthread_mutex_lock(&mtx);
            sprintf(output,"Server up for %d:%d:%d, served %d pages, %d bytes\n",hours,min,sec,totalpages,totalbytes);
            pthread_mutex_unlock(&mtx);
            write(socket,output,strlen(output));
        } 
        else if(!strcmp(client_message,"SHUTDOWN\r\n"))
        {    
            close(socket);
            return 2;
        }
        else if(!strncmp(client_message,"SEARCH ",7))
        {
            pthread_mutex_lock(&mtx);
            if(check_final_state(numofthreads)==1)      //check if all threads are done if not print message to socket
            {
                pthread_mutex_unlock(&mtx);
                write(socket,"Crawling is still in progress...\r\n",strlen("Crawling is still in progress...\r\n"));
                close(socket);
                fflush(stdout);
                return 1;
            }
            pthread_mutex_unlock(&mtx);
            if(!workersdone)    //if threads are done then initialise project2
            {
                workersdone=1;
                p2initialisation=1;
                write(socket,"Wait for workers and db to finish their initialisation\r\n",strlen("Wait for workers and db to finish their initialisation\r\n"));
                initialiseProject2(socket);
            }
            write(socket,"Starting to work on your querie\r\n",strlen("Starting to work on your querie\r\n"));
            char send_message[512]="";
            strcat(send_message,"/search ");
            char* token;
            strtok(client_message," \r\n");
            token=strtok(NULL," \r\n");
            if(token==NULL || !strncmp(token,"-d",2))   //check the input given better
            {
                write(socket,"Wrong Input\r\n",strlen("Wrong Input\r\n"));
                close(socket);
                fflush(stdout);
                return 1;
            }
            int count=0;
            while(token!=NULL && strncmp(token,"-d",2) && count<10)
            {
                strcat(send_message,token);
                strcat(send_message," ");
                token=strtok(NULL," \r\n");
                count++;       
            }
            strcat(send_message,"-d 1");    //default time to wait 1 second
            int length=strlen(send_message)+1;
            write(p3write,&length,sizeof(int)); 
            write(p3write,send_message,length); //message to project2
            readfromworkers(socket);        //read project2 response
        }
        else
            write(socket,"Wrong Input\r\n",strlen("Wrong Input\r\n"));
    }
    else if(read_size==0)
        cout<<"Client disconnected"<<endl;
    else if(read_size==-1)
        perror("recv failed");
    close(socket);
    fflush(stdout);
    return 1;
}

int server_running(int command_fd,int numofthreads)
{
    while(1)
    {
        for(int i=0;i<numofthreads;i++)
        {   
            if(pthread_tryjoin_np(tid[i],NULL)==0)  //check if threads are alive if not create a new one
            {
                cout<<"A thread died trying to revive it"<<endl;
                pthread_create(&tid[i],NULL,&thread_fun,NULL);
            }
        }
        if(command_port_fun(command_fd,numofthreads)==2)
            return 1;   //close the program
    }
}