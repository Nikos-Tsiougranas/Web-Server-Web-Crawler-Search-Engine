#include "queuelist.h"

using namespace std;

Queuelist::Queuelist(char* GURL,Queuelist* Next=NULL)   //constructor
{
    next=Next;
    strcpy(URL,GURL);
}

void Queuelist::deletequeue()   //destructor
{
    if(next!=NULL)
        next->deletequeue();
    delete this;    
}

Queuelist* Queuelist::push(char* GURL)  //push
{
    if(!strcmp(URL,""))
    {
        strcpy(URL,GURL);
        return this;
    }
    Queuelist* header=new Queuelist(GURL,this); //change the header with the new item
    return header;
}

void Queuelist::pop(int length,char** returl)
{
    if(length==1)
    {
        strcpy(*returl,URL);
        strcpy(URL,"");
        return;    
    }
    if(length==2)
    {
        next->pop(length-1,returl);
        delete next;
        next=NULL;
        return;
    }
    next->pop(length-1,returl);
}

int Queuelist::find(char* Gurl)
{
    if(!strcmp(Gurl,URL))
        return 1;
    if(next!=NULL)
        return next->find(Gurl);
    return 0;
}

int Queuelist::length()
{
    if(!strcmp(URL,""))
        return 0;
    if(next==NULL)
        return 1;
    return next->length()+1;
}
