#include "threads.h"

using namespace std;

int contentlength(char client_message[])    //get the length of the message accordint to http get request
{
    int length=0;
    char* token=strtok_r(client_message,"\r\n",&client_message);
    for(int i=0;i<3;i++)
        token=strtok_r(client_message,"\r\n",&client_message);
    length=atoi(token+16);
    return length;
}
void thread_send_request(int server_fd,char URL[])  //send the URL to web server with the correct http get form
{
    char send_message[256]="";
    char* token=strtok_r(URL,"/",&URL);
    token=strtok_r(URL,"/",&URL);
    token=strtok_r(URL,"\r\n",&URL);
    strcpy(send_message,"GET /");
    strcat(send_message,token);
    strcat(send_message," HTTP/1.1\r\nHost: localhost\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate\r\nConnection: Keep-Alive\r\n\r\n");
    write(server_fd,send_message,strlen(send_message)); 
}
void find_the_urls(char* doc)   //parse the document for URLs
{   
    char* token;
    while((doc=strstr(doc,"<a href=\""))!=NULL)
    {
        char URL[256]="";
        strcat(URL,"http://");
        strcat(URL,host_ip);
        strcat(URL,":");
        char port[10]="";
        sprintf(port,"%d",serving_port);
        strcat(URL,port);
        token=strtok_r(doc,"\"",&doc);
        token=strtok_r(doc,"\"",&doc);
        strcat(URL,token);
        pthread_mutex_lock(&mtx);
        if(!usedUrls->find(URL))    //check if it has been used
        {
            buffer=buffer->push(URL);       //push the valid URLS inside the buffer for other threads to use
            usedUrls=usedUrls->push(URL);   //place the URL that have been parsed
            pthread_mutex_unlock(&mtx);
            pthread_cond_signal(&cond_nonempty);
        }
        else
            pthread_mutex_unlock(&mtx);
    }
}
void save_the_file(char* doc,char sURL[])   
{
    char* token=strtok_r(sURL,"/",&sURL);
    for(int i=0;i<2;i++)
        token=strtok_r(sURL,"/",&sURL);
    char save_folder[128]="";
    strcat(save_folder,save_dir);   //constract the path to save_dir
    strcat(save_folder,"/");
    strcat(save_folder,token);
    pthread_mutex_lock(&mtx);
    DIR* dir = opendir(save_folder);    //open dir if it doesnot exist make a new one
    if (!dir)
        mkdir(save_folder, 0700);
    closedir(dir);
    pthread_mutex_unlock(&mtx);
    token=strtok_r(sURL,"\0",&sURL);
    char save_doc[256]="";
    strcat(save_doc,save_folder);
    strcat(save_doc,"/");
    strcat(save_doc,token);
    FILE *file;
    file = fopen(save_doc, "wr");   //open the page and place all the html page inside and save
    fprintf(file,"%s",doc);
    fclose(file);
}

void* thread_fun(void *arg)
{   
    int opt=1;
    int id=*(int*)arg;
    free(arg);
    thread_doing_nothing[id]=0;     //initialise thread state
    while(1)
    {
        pthread_mutex_lock(&mtx);
        while(buffer->length()==0)
        {    
            thread_doing_nothing[id]=1;     //inform your thread state that you are doing nothing
            pthread_cond_wait(&cond_nonempty, &mtx);
            if(deathsignal)
            {
                pthread_mutex_unlock(&mtx);
                pthread_exit(0);
            }
        }
        thread_doing_nothing[id]=0;         //inform your thread state that you are about to ask a URL
        char client_message[4096]="";
        char* URL=(char*)malloc(256*sizeof(char));
        buffer->pop(buffer->length(),&URL);
        pthread_mutex_unlock(&mtx);
        char sURL[256]="";
        strcpy(sURL,URL);
        int server_fd=socket(AF_INET,SOCK_STREAM,0);    //every time create a new socket
        if(server_fd==-1)
        {
            perror("Could not create socket");
            pthread_exit(0);
        }
        setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt));   //make it bind
        connect(server_fd,(struct sockaddr *)&server, sizeof(struct sockaddr));             //connect
        thread_send_request(server_fd,URL);
        recv(server_fd,client_message,sizeof(client_message),0);    //receive the client_message(the "header")
        int length=contentlength(client_message);   //get length of doc from header
        pthread_mutex_lock(&mtx);
        totalbytes+=length;                         //increase bytes and length for STATS
        totalpages+=1;
        pthread_mutex_unlock(&mtx);
        memset(client_message,'\0',4096);
        char* doc=(char*)malloc((length+1)*sizeof(char));
        memset(doc,'\0',length);
        int ind=0;
        while(recv(server_fd,client_message,sizeof(client_message)-1,0)!=0) //receive messages until all are read
        {
            memcpy(doc+ind,client_message,strlen(client_message));
            ind+=strlen(client_message);
            memset(client_message,'\0',4096);
        }
        doc[length]='\0';
        save_the_file(doc,sURL);    //save the file 
        find_the_urls(doc);         //find new URLS
        close(server_fd);
        free(URL);
        free(doc);
    }
}