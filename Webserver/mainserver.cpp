#include "mainserver.h"

using namespace std;

int serverinit(int* server_fd,int* command_fd,int serving_port,int command_port)
{
    int opt=1;
    *server_fd=socket(AF_INET,SOCK_STREAM,0);   //create sockets
    *command_fd=socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in server,command;
    if (*server_fd == -1 || *command_fd==-1)
    {
        perror("Could not create socket");
        return -1;
    }
    setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt));  //make both the sockets to connect
    setsockopt(*command_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt));
    server.sin_family = AF_INET;            //initialise the server socket and the command socket
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( serving_port );
    command.sin_family = AF_INET;
    command.sin_addr.s_addr = INADDR_ANY;
    command.sin_port = htons( command_port );
    if( (bind(*server_fd,(struct sockaddr *)&server , sizeof(server)) < 0) || (bind(*command_fd,(struct sockaddr *)&command , sizeof(command)) < 0)) //bind them both
    {
        perror("bind failed. Error");
        return -1;
    }
    if ((listen(*server_fd, 10) < 0) || (listen(*command_fd, 10) < 0))  //listen for both of them
	{
		perror("listen");
		return -1;
	}
}

int command_port_fun(int fd)
{
    int socket=accept(fd,NULL,NULL);    //accept the connection
    char client_message[1024]="";
    int read_size = recv(socket , client_message , sizeof(client_message)-1 , 0);
    if(read_size>0)
    {
        if(!strcmp(client_message,"STATS\r\n")) //if STATS then print the time which server runs and all the infos
        {   
            time_t end=time(NULL);
            int hours=(end-start)/3600;
            int resttime=(end-start)%3600;
            int min=resttime/60;
            resttime=resttime%60;
            int sec=resttime;
            char output[256];
            sprintf(output,"Server up for %d:%d:%d, served %d pages, %d bytes\n",hours,min,sec,totalpages,totalbytes);
            write(socket,output,strlen(output));
        } 
        else if(!strcmp(client_message,"SHUTDOWN\r\n")) //inform the program that it has to free memory and remove locks
        {    
            close(socket);
            return 2;
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

int server_running(int server_fd,int command_fd,int numofthreads)
{
    struct pollfd fds[2];
    int numoffd=2;
    fds[0].fd = server_fd;     //intialise poll utility
    fds[0].events = POLLIN;
    fds[1].fd = command_fd;
    fds[1].events = POLLIN;
    while(1)
    {
        int rc=poll(fds,numoffd,-1);
        for(int i=0;i<numofthreads;i++) //check if a thread is dead if it is then create a new one
        {
            if(pthread_tryjoin_np(tid[i],NULL)==0)
            {
                cout<<"A thread died trying to revive it"<<endl;
                pthread_create(&tid[i],NULL,&thread_fun,NULL);
            }
        }
        if(rc<0)
        {
            perror("poll() failed");
            return -1;
        }
        if(fds[0].revents & POLLIN)     //then a request is called from server port and a URL is pushed
        {    
            pthread_mutex_lock(&mtx);
            buffer=buffer->push(accept(fds[0].fd,NULL,NULL));
            pthread_mutex_unlock(&mtx);
            pthread_cond_signal(&cond_nonempty);
        }
        else if(fds[1].revents & POLLIN)    //then a request is called from command port and a command function is called
            if(command_port_fun(fds[1].fd)==2)
                return 1;    
    }
}