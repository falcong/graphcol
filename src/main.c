#include "../inc/defs.h"
#include "../inc/routines.h"

void printProcessInfo(Graph *g, int type);


int main(int argc, char *argv[])
{
  char instFile[LUNGHEZZA+1];
  Graph *g;
  int colors;
  boolean result;

  //Lettura del file istanza
  readCommand(argc,argv,instFile,&colors);

  //Caricamento dell'istanza
  g=loadGraph(instFile);
  
  result=findTabu(g,colors,5,0.5);
  
  if(result)
    printf("Find %d-coloring for the current graph\n",colors);
  else
    printf("Failed to find %d-coloring for the current graph\n",colors);
  
  printProcessInfo(g,1);
  
  return 0;
}

void printProcessInfo(Graph *g, int type)
{
  switch(type)
  {
    case 1: printDotOut(g->nodesList);
            system("dot out.dot -Tpng > out.png");
            break;
    case 2: printNodesList(g->nodesList);
            break;
    default:break;
  }
}
