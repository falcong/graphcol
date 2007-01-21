#include "../inc/defs.h"
#include "../inc/routines.h"

void printProcessInfo(Graph *g, int type);


int main(int argc, char *argv[])
{
  char instFile[LUNGHEZZA+1];
  Graph *g;
  int colors;

  //Lettura del file istanza
  readCommand(argc,argv,instFile,&colors);

  //Caricamento dell'istanza
  g=loadGraph(instFile);
  
  printProcessInfo(g,2);
  
  return 0;
}

void printProcessInfo(Graph *g, int type)
{
  switch(type)
  {
    case 1: printDotOut(g->nodesList);
            system("dot out.dot -Tjpg > out.jpg");
            break;
    case 2: printNodesList(g->nodesList);
            break;
    default:break;
  }
}
