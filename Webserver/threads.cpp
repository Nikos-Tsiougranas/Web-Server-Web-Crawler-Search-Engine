#include "threads.h"

using namespace std;

int checkinputmessage(char** filename,char client_message[])    //check if the http request is valid
{
    if((client_message[strlen(client_message)-1]!='\n') || client_message[strlen(client_message)-2]!='\r')
        return -1;
    char* token;
    token=strtok_r(client_message," ",&client_message);
    if(strcmp(token,"GET"))
        return -1;
    token=strtok_r(client_message," ",&client_message);
    char buffer[256];
    strcpy(buffer,directory);
    strcat(buffer,token);
    if(strcmp((buffer)+(strlen(buffer)-5),".html"))
        return -1;
    if(access(buffer,F_OK)==-1)
        return -1;
    FILE* fp=fopen(buffer,"r");
    if(fp==NULL)
        return -2;
    fclose(fp);
    token=strtok_r(client_message,"\r\n",&client_message);
    if(strcmp(token,"HTTP/1.1"))
        return -1;
    token=strtok_r(client_message,"\r\n",&client_message);
    if(strncmp(token,"Host:",5))
        return -1;
    strcpy(*filename,buffer);
    return 1;           //return 1 if the page exists
}

void server_thread_response(int socket,char client_message[])
{
    char* filename=(char*)malloc(256*sizeof(char));
    int flag=0;
    int retval;
    retval=checkinputmessage(&filename,client_message); //check if the http get request is valid
    char response[4096]="";
    char res[4096]="";
    if(retval==-1)      //doesnot exist
    {
        strcpy(res,"<html>Sorry dude, couldn't find this file.<\html>");
        strcpy(response,"HTTP/1.1 404 Not Found\r\n");
    }
    else if(retval==1)  //exists
    {    
        strcpy(response,"HTTP/1.1 200 OK\r\n");
        flag=1;
    }
    else    //no rights to read
    {
        strcpy(res,"<html>Trying to access this file but don't think I can make it.<\html>");
        strcpy(response,"HTTP/1.1 403 Forbidden\r\n");
    }
    time_t current_time;
    char* time_string;
    current_time = time(NULL);
    time_string = ctime(&current_time);
    strcat(response,"Date: ");  //construct http response
    strcat(response,time_string);
    strcat(response,"Server: myhttpd/1.0.0 (Ubuntu64)\r\n");
    strcat(response,"Content-Length: ");
    char length[10];
    if(flag==0)
        sprintf(length,"%d",(int)strlen(res));
    else
    {
        FILE *fp = fopen(filename, "rb");
        fseek(fp, 0, SEEK_END);
        long filelength = ftell(fp);
        totalbytes+=filelength;     //find the length of the document and add it to totalbytes
        fclose(fp);
        sprintf(length,"%ld",filelength);
    }
    strcat(response,length);
    strcat(response,"\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n");
    if(flag==0)
        strcat(response,res);
    send(socket,response,strlen(response),0);
    if(flag==1)
    {
        FILE* fp;
        fp=fopen(filename,"r");
        int n;
        while ((n = fread(res, 1, sizeof(res), fp)) > 0)    //send the message to the server
            write(socket,res,n);
        totalpages++;       //increase totalpages num
        fclose(fp);
    }
    free(filename);
}

void* thread_fun(void *arg)
{   
    while(1)
    {
        char client_message[2048]="";   //message to send to the client
        pthread_mutex_lock(&mtx);
        while(buffer->length()==0)  //check if a URL is accessible if it is not then wait
        {    
            pthread_cond_wait(&cond_nonempty, &mtx);
            if(deathsignal)                     //check if the threads must be killed
            {
                pthread_mutex_unlock(&mtx); 
                pthread_exit(0);
            }
        }
        int new_socket=buffer->pop(buffer->length());   //take the next URL in line
        pthread_mutex_unlock(&mtx);
        int read_size = recv(new_socket , client_message , sizeof(client_message)-1 , 0);   //receive client request
        if(read_size>0)
            server_thread_response(new_socket,client_message);  //send the message to client
        if(read_size==0)
        {
            cout<<"Client disconnected"<<endl;
            fflush(stdout);
        }
        else if(read_size==-1)
        {    
            perror("recv failed");
            pthread_exit(0);
        }
        close(new_socket);
    }
    pthread_exit(0);
}