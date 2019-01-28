#include "queuelist.h"

using namespace std;

void Queuelist::deletequeue()   //destructor
{
    if(next!=NULL)
        next->deletequeue();
    delete this;
}

Queuelist* Queuelist::push(int filedescriptor)  //push
{
    if(fd==-1)
    {
        fd=filedescriptor;
        return this;
    }
    Queuelist* header=new Queuelist(filedescriptor,this); //change the header with the new pushed item
    return header;
}

int Queuelist::pop(int length)
{
    if(length==1)
    {
        int ret=fd;
        fd=-1;    
        return ret;
    }
    if(length==2)
    {
        int ret=next->pop(length-1);
        delete next;
        next=NULL;
        return ret;
    }
    return next->pop(length-1);
}

int Queuelist::length()
{
    if(fd==-1)
        return 0;
    if(next==NULL)
        return 1;
    return next->length()+1;
}