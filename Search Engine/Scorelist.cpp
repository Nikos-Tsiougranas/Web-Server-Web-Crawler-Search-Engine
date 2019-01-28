#include "Scorelist.h"
using namespace std;

Scorelist::Scorelist(int f,int d,int l,Mymap* mymap):file(f),doc(d),linecounter(0)
{   
    line=NULL;         
    if(l!=-1)   //if new is called with parameters
    { 
        line=(int*)malloc((mymap->getnumoflines(file,doc))*sizeof(int));   //get number of lines at the current doc to allocate memory
        line[linecounter]=l;    //insert new line to score
        linecounter++;
    }
    next=NULL;
}
Scorelist::~Scorelist()
{
    if(next!=NULL)
        delete(next);
    free(line);
}
void Scorelist::add(int i,int j,int l,Mymap* mymap)
{
    if(file==-1 && doc==-1) 
    {
        //insert all infos to score
        file=i;
        doc=j;
        line=(int*)malloc((mymap->getnumoflines(file,doc))*sizeof(int));//get number of lines at the current doc to allocate memory
        line[linecounter]=l;
        linecounter++;
        return;
    }                        
    if(file==i && doc==j)
    {
        int flag=1;
        for(int k=0;k<linecounter;k++)  //check if the line is already in
        {
            if(line[k]==l)
                flag=0;
        }
        if(flag)        //if it is not insert it to array
        {
            
            line[linecounter]=l;
            linecounter++;              
        } 
    }
    else                    //else go for the next till you find it or till creating a new posting node
    {
        if(next==NULL)
            next=new Scorelist(i,j,l,mymap);
        else
            next->add(i,j,l,mymap);
    }
}
void Scorelist::getinfo(int *i,int *j,int **l,int *counter) //pass all the info to the caller
{
    *i=file;
    *j=doc;
    *l=line;
    *counter=linecounter;
}
int Scorelist::volume() //number of scores
{
    if(file==-1)
        return 0;
    if(next!=NULL)
        return 1+next->volume();
    else
        return 1;
}