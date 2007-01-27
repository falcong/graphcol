#ifndef _routines_h
#define _routines_h

typedef struct _onemove oneMove;

#include "../inc/defs.h"
#include "../inc/graph.h"

struct _onemove{
  
  int id;
  int color;
  int bestNew;
};


/**
Function that read from the command line the name of the instance file
 */
void readCommand(int argc,char *argv[],char *instFile, int *colors, int *verbosity);

void readConfFile(int *maxIt,int *fixLong,float *propLong);



/**
Function that read the information stored on the instance file in DIMACS format for load the coloring graph
 */
Graph *loadGraph(char *instFile);

/**
Function that print in a "out.dot" file the resulting graph of the computation
 */
void printDotOut(NodesList *l);


boolean findTabu(Graph *g, int numColors, int fixLong, float propLong, int maxIt);

void randomColor(Graph *g, int numColors);

void buildAdjacency(Graph *g,int **adjColors);

void printAdjacency(int **adjColors,Graph *g,int numColors);

int nodesConflicting(NodesList *nl, int **adjColors, int numColors);

int adjConflicting(Node *n, int **adjColors);

boolean isConflicting(Graph *g, int id, int **adjColors);

oneMove *findBest1Move(Graph *g, int **adjColors, int **tabuList, int numColors, oneMove *move, int fixLong, float propLong, int nIt);

void updateAdjacency(Graph *g, int **adjColors, oneMove *move, int numColors);

void updateAdjacencyTabu(Graph *g, int **adjColors, int id, int color, int newColor, int numColors);

int moveProfit(int **adjColors, Node *n, int newColor, int numColors);

boolean isTabu(Graph *g,int **adjColors, int id, int color, int **tabuList, int nIt, int numColors);

int setTabu(Graph *g,int **adjColors, int **tabuList, int numColors, oneMove *move, int fixLong, float propLong, int nIt);

int greedyColor(Graph *g);

int getGreedyMaxOrder(Graph *g,pNode *orderNode);

boolean colorAdjFree(Node *n,int color);
#endif
