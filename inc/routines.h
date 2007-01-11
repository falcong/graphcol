#include "../inc/defs.h"
#include "../inc/graph.h"

void readCommand(int argc,char *argv[],char *instFile, int *colors);

Graph *loadGraph(char *instFile);

void printDotOut(NodesList *l);
