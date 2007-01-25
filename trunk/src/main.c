#include "../inc/defs.h"
#include "../inc/routines.h"

void printProcessInfo(Graph *g, int type);


int main(int argc, char *argv[])
{
  char instFile[LUNGHEZZA+1];
  Graph *g;
  int colors,verbosity;
  int fixLong,maxIt;
  float propLong;
  boolean result;

  //Reading instance file
  readCommand(argc,argv,instFile,&colors,&verbosity);
  
  //Loading instance file & building graph struct
  g=loadGraph(instFile);
  
  //Set default tabu search values
  maxIt=1000;
  fixLong=g->numNodes/2;
  propLong=0.5;
  
  //Reading configuration file
  readConfFile(&maxIt,&fixLong,&propLong);
  
  //Build the random initial solution
  randomColor(g,colors);
  
  result=findTabu(g,colors,fixLong,propLong,maxIt);
  
  if(result==0)
    printf("Find %d-coloring for the current graph\n",colors);
  else
  {
    printf("\nFailed to find %d-coloring for the current graph\n",colors);
    printf("Remaining %d conflicting nodes\n\n",result);
  }
  
  printProcessInfo(g,verbosity);
  
  return 0;
}

void printProcessInfo(Graph *g, int type)
{
  switch(type)
  {
    case 0: 
            break;
    case 1: printDotOut(g->nodesList);
            system("dot out.dot -Tpng > out.png");
            break;
    case 2: printNodesList(g->nodesList);
            break;
    default:break;
  }
}
