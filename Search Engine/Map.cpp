#include "Map.h"
using namespace std;

Mymap::Mymap():totalbytes(0),totalwords(0),numofdirectories(0),totallines(0)    //initialise map
{
    lines=NULL; 
    documents=NULL;
    numofdocuments=NULL;
    nameofdocuments=NULL;
    pathnames=NULL;
}
Mymap::~Mymap()     //destroy all structs of map
{
    for(int i=0;i<numofdirectories;i++)
    {    
        for(int j=0;j<numofdocuments[i];j++)
        {    
            for(int l=0;l<lines[i][j];l++)
                free(documents[i][j][l]);
            free(documents[i][j]);
            free(nameofdocuments[i][j]);
        }
        free(documents[i]);
        free(lines[i]);
        free(nameofdocuments[i]);
        free(pathnames[i]);
    }        
    free(lines);
    free(documents);
    free(numofdocuments);
    free(nameofdocuments);
    free(pathnames);
}
void Mymap::setnumofdirectories(int num)    //allocate memory for all the structs
{
    numofdirectories=num;
    pathnames=(char**)malloc(numofdirectories*sizeof(char*));
    numofdocuments=(int*)malloc(numofdirectories*sizeof(int));
    lines=(int**)malloc(numofdirectories*sizeof(int*));
    nameofdocuments=(char***)malloc(numofdirectories*sizeof(char**));
    documents=(char****)malloc(numofdirectories*sizeof(char***));
}
void Mymap::setnumofdocuments(int index,int num,char* name)//allocate memory for all the structs
{
    numofdocuments[index]=num;
    pathnames[index]=(char*)malloc((strlen(name)+1)*sizeof(char));
    strcpy(pathnames[index],name);
    lines[index]=(int*)malloc(numofdocuments[index]*sizeof(int));
    nameofdocuments[index]=(char**)malloc(numofdocuments[index]*sizeof(char*));
    documents[index]=(char***)malloc(numofdocuments[index]*sizeof(char**));
}
void Mymap::setnumoflines(int dirindex,int docindex,int num,char* name)//allocate memory for all the structs
{
    nameofdocuments[dirindex][docindex]=(char*)malloc((strlen(name)+1)*sizeof(char));
    strcpy(nameofdocuments[dirindex][docindex],name);
    lines[dirindex][docindex]=num;
    documents[dirindex][docindex]=(char**)malloc(lines[dirindex][docindex]*sizeof(char*));
}
void Mymap::insertline(int i,int j,int l,char* line) //insert a line and increase the counter
{
    documents[i][j][l]=(char*)malloc((strlen(line)+1)*sizeof(char));
    strcpy(documents[i][j][l],line);
    totallines++;
}
void Mymap::get_all() //print all the map of the worker
{
    cout<<"Numofdirectories:"<<numofdirectories<<endl;
    for(int i=0;i<numofdirectories;i++)
    {
        cout<<"Pathname"<<pathnames[i];
        cout<<" Numofdocuments"<<numofdocuments[i]<<endl;
        for(int j=0;j<numofdocuments[i];j++)
        {
            cout<<"Nameofdocuments"<<nameofdocuments[i][j]<<" lines "<<lines[i][j]<<endl;
            for(int l=0;l<lines[i][j];l++)
                cout<<documents[i][j][l]<<endl;
        }
    }
}