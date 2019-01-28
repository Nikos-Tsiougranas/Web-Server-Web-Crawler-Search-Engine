#include "Listnode.h"
using namespace std;

listnode::~listnode()
{
    if(next!=NULL)          //delete all listnodes of posting list
        delete(next);
}
void listnode::add(int i,int j,int l)
{
    if(dir==i && file==j && line==l)             //if id is found again times++
        times++;
    else                    //else go for the next till you find it or till creating a new posting node
    {
        if(next==NULL)
            next=new listnode(i,j,l);
        next->add(i,j,l);
    }
}
void listnode::search(Mymap* mymap,Scorelist* score)//search recursively all the nodes of the list and add them to the score
{
    score->add(dir,file,line,mymap);    
    if(next!=NULL)
        next->search(mymap,score);
}
int listnode::volumeperdocument(int d,int f)//find the number of times a word is inside a specific document
{
    listnode* temp=this;
    int volume=0;
    while(temp!=NULL)
    {
        if(temp->dir==d && temp->file==f)
            volume+=temp->times;
        temp=temp->next;
    }
    return volume;
}
int listnode::getminimum(char* timeofquery,char* word,char* name,Mymap* mymap)
{
    int minimum=10000000;
    int tempval=0;
    for(int i=0;i<mymap->getnumofdirectories();i++) //for every directory
    {
        for(int j=0;j<mymap->getnumofdocuments(i);j++)  //for every document
        {
            tempval=volumeperdocument(i,j);     //find the number of times the word is in it
            if(tempval!=0 && tempval<minimum)   //if tempval!=0 and less than minimum get the name(maybe it is needed for sorting)
            {
                minimum=tempval;
                mymap->getdocpath(name,i,j);
            }
            else if(tempval!=0 && tempval==minimum) //if it is equal to minimum sort accordint to paths
            {
                char* temppath=(char*)malloc(1024*sizeof(char));
                mymap->getdocpath(temppath,i,j);
                if(strcmp(name,temppath)>0)
                    strcpy(name,temppath);
                free(temppath);
            }
        }
    }
    return minimum;
}
int listnode::getmaximum(char* timeofquery,char* word,char* name,Mymap* mymap) //same as above but for maximum
{
    int maximum=0;
    int tempval=0;
    for(int i=0;i<mymap->getnumofdirectories();i++)
    {
        for(int j=0;j<mymap->getnumofdocuments(i);j++)
        {
            tempval=volumeperdocument(i,j);
            if(tempval!=0 && tempval>maximum)
            {
                maximum=tempval;
                mymap->getdocpath(name,i,j);
            }
            else if(tempval!=0 && tempval==maximum)
            {
                char* temppath=(char*)malloc(1024*sizeof(char));
                mymap->getdocpath(temppath,i,j);
                if(strcmp(name,temppath)>0)
                    strcpy(name,temppath);
                free(temppath);
            }
        }
    }
    return maximum;
}