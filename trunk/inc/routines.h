#ifndef _routines_h
#define _routines_h

typedef struct _onemove oneMove;

#include "../inc/defs.h"
#include "../inc/graph.h"

struct _onemove{
  
  int id;
  int color;
};


/**
Function that read from the command line the name of the instance file
 */
void readCommand(int argc,char *argv[],char *instFile, int *colors);

/**
Function that read the information stored on the instance file in DIMACS format for load the coloring graph
 */
Graph *loadGraph(char *instFile);

/**
Function that print in a "out.dot" file the resulting graph of the computation
 */
void printDotOut(NodesList *l);


boolean findTabu(Graph *g, int numColors, int fixLong, float propLong);

void randomColor(Graph *g, int numColors);

void buildAdjacency(Graph *g,int **adjColors);

void printAdjacency(int **adjColors,int numNodes,int numColors);

int nodesConflicting(NodesList *nl, int **adjColors, int numColors);

int adjConflicting(int node, int **adjColors, int numColors);

oneMove *findBest1Move(int **adjColors,int numNodes, int numColors);
#endif
