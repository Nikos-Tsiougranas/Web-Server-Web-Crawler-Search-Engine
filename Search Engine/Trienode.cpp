#include "Trienode.h"
using namespace std;

Trienode::Trienode():value(-1)
{
    sibling=NULL;
    child=NULL;
    list=NULL;
}
Trienode::~Trienode() 
{
    if(list!=NULL)      //deleting recursively all the nodes of the trie
        delete(list);
    if(child!=NULL)
        delete(child);
    if(sibling!=NULL)
        delete(sibling);
}
void Trienode::insert(char* token,int i,int j,int l)   //recursive insertion
{
    if(value==-1 || value==token[0])        //if the value is the same or the value is not occupied
    {
        value=token[0];
        if(strlen(token)==1)                //if we are at the last letter
        {
            if(list==NULL)
                list=new listnode(i,j,l);      //if the posting list is NULL create
            list->add(i,j,l);                  //add
        }   
        else                                //else go to the child
        {
            if(child==NULL)
                child=new Trienode();
            child->insert(token+1,i,j,l);      //if it is a child go fot the next letter
        }
    }
    else
    {
        if(sibling==NULL)
            sibling=new Trienode();         
        sibling->insert(token,i,j,l);          //if the value is not the same go for the sibling
    }
    return;
}

int Trienode::search(char* timeofquery,char* word,int curr,Mymap* mymap,Scorelist* score) //same as above
{
    if(word[curr]==value)
    {
        if(curr==strlen(word)-1)
        {
            if(list!=NULL)          //when we find it we inform the log
            {    
                char logname[1024];
                snprintf(logname,sizeof(logname),"log/Worker_%i.txt",getpid());
                FILE* fp=fopen(logname,"a");
                int flag=1;
                int tempval=0;
                for(int i=0;i<mymap->getnumofdirectories();i++)
                {
                    for(int j=0;j<mymap->getnumofdocuments(i);j++)
                    {
                        tempval=list->volumeperdocument(i,j);
                        if(tempval!=0)
                        {
                            if(flag)
                            {
                                fprintf(fp,"%s",timeofquery);       //for every document we find that has the word inform log
                                fprintf(fp," : ");
                                fprintf(fp,"search :");
                                fprintf(fp,"%s",word);
                                flag=0;
                            }
                            char name[512];
                            mymap->getdocpath(name,i,j);
                            fprintf(fp,":");
                            fprintf(fp,"%s",name);
                        }
                    }
                }
                fprintf(fp,"\n");
                fclose(fp);                        
                list->search(mymap,score);          //inform scoreboard(we need it to send the results to master)
                return 1;   //identifier we found the word
            }
            else
                return 0;
        }
        else
        {
            if(child!=NULL)
                return child->search(timeofquery,word,curr+1,mymap,score);
            else
                return 0;
        }
    }
    else
    {
        if(sibling!=NULL)
            return sibling->search(timeofquery,word,curr,mymap,score);
        else
            return 0;
    }
}
void Trienode::maxcount(char* timeofquery,char* word,int curr,int *maxvalue,char* maxpath,Mymap* mymap) //same as above call maximum to return maxvalue
{
    if(word[curr]==value)
    {
        if(curr==strlen(word)-1)
        {
            if(list!=NULL)
            {
                *maxvalue=list->getmaximum(timeofquery,word,maxpath,mymap);
                return;
            }
            else
                return ;
        }
        else
        {
            if(child!=NULL)
                child->maxcount(timeofquery,word,curr+1,maxvalue,maxpath,mymap);
            else
                return ;
        }
    }
    else
    {
        if(sibling!=NULL)
            sibling->maxcount(timeofquery,word,curr,maxvalue,maxpath,mymap);
        else
            return ;
    }
}
void Trienode::mincount(char* timeofquery,char* word,int curr,int *minvalue,char* minpath,Mymap* mymap)//same as above call maximum to return minvalue
{
    if(word[curr]==value)
    {
        if(curr==strlen(word)-1)
        {
            if(list!=NULL)
            {
                *minvalue=list->getminimum(timeofquery,word,minpath,mymap);
                return;
            }
            else
                return ;
        }
        else
        {
            if(child!=NULL)
                child->mincount(timeofquery,word,curr+1,minvalue,minpath,mymap);
            else
                return ;
        }
    }
    else
    {
        if(sibling!=NULL)
            sibling->mincount(timeofquery,word,curr,minvalue,minpath,mymap);
        else
            return ;
    }
}