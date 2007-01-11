#include "../inc/defs.h"
#include "../inc/routines.h"

int main(int argc, char *argv[])
{
  char instFile[LUNGHEZZA+1];
  Graph *g;
  int colors;

  //Lettura del file istanza
  readCommand(argc,argv,instFile,&colors);

  //Caricamento dell'istanza
  g=loadGraph(instFile);

//   printNodesList(g->nodesList);

  printDotOut(g->nodesList);
  system("dot out.dot -Tps > out.ps");
  system("kghostview out.ps");

  return 0;
}
