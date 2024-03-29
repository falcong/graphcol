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

void readConfFile(int *nRestart,int *maxIt,int *fixLong,float *propLong,int *maxItImprove,float *startTemp,float *minTemp,float *tempFactor, int *maxItConstTemp);



/**
Function that read the information stored on the instance file in DIMACS format for load the coloring graph
 */
Graph *loadGraph(char *instFile);

/**
Function that print in a "out.dot" file the resulting graph of the computation
 */
void printDotOut(NodesList *l);


int findTabu(Graph *g, int numColors, int fixLong, float propLong, int maxIt, int *stopIt, int ***adj, boolean returnBest);

void saveBestSolution(Graph *g,int numColors,int *bestSol);

void loadBestSolution(Graph *g,int numColors,int *bestSol);

int findSA(Graph *g,int numColors,int *stopIt,int ***adj, float startTemp, float minTemp,float tempFactor, int maxItImprove, int maxItConstTemp);

int findVNS(Graph *g, int numColors, int fixLong, float propLong, int maxIt, int *stopIt, int ***adj);

void shake(Graph *g,int numColors,int neigh, int **adjColors, int iVns);

void doVNSChain(Graph *g, int numColors, int **adjColors, int iVns);

void chainUpdate(Graph *g, int **adjColors,oneMove *move,int numColors,boolean *newConflicting,boolean *blacklist);

int getRandomChainNode(Graph *g,int **adjColors,int nextClass,boolean *conflicting,int nC,int numColors,boolean *blacklist);

int countColorConflicting(Graph *g,int numColors,int color,boolean *conflicting,boolean *blacklist);

void doVNSGrenade(Graph *g, int numColors, int **adjColors, int iVns);

void grenadeUpdate(Graph *g,int **adjColors,oneMove *move,int numColors,boolean *newConflicting,boolean *blacklist);

void doVNSFirework(Graph *g, int numColors, int **adjColors, int iVns);

void fireworkUpdate(Graph *g,int **adjColors,oneMove *move,int numColors,boolean *newConflicting,boolean *blacklist,int newConf,boolean *preConflicting, boolean *postConflicting);

void randomColor(Graph *g, int numColors);

boolean *getConflictingNodes(Graph *g, int numColors, int **adjColors, boolean *conflicting);

boolean *getOnlyNewConflictingNodes(boolean *preConflicting,boolean *postConflicting,int numNodes,boolean *conflicting);

int countOnlyNewConflictingNodes(boolean *newConflicting, int numNodes);

void doVNSEmptyRefill(Graph *g, int numColors, int **adjColors);

void randomConflictingColor(Graph *g, int numColors, int **adjColors);

void buildAdjacency(Graph *g,int **adjColors,int numColors);

void printAdjacency(int **adjColors,Graph *g,int numColors);

int nodesConflicting(NodesList *nl, int **adjColors, int numColors);

int adjConflicting(Node *n, int **adjColors);

boolean isConflicting(Graph *g, int id, int **adjColors);

void printConflictingNodesList(Graph *g,int **adjColors);

oneMove *findBest1Move(Graph *g, int **adjColors, int **tabuList, int numColors, oneMove *move, int fixLong, float propLong, int nIt, int nC, int bestNc);

oneMove *findBestNodeMove(Graph *g, int id,int **adjColors, int numColors, oneMove *move);

oneMove *findRandomSA1Move(Graph *g,int **adjColors, int numColors, oneMove *moves);

void updateAdjacency(Graph *g, int **adjColors, oneMove *move, int numColors);

void updateAdjacencyTabu(Graph *g, int **adjColors, int id, int color, int newColor, int numColors);

int moveProfit(int **adjColors, Node *n, int newColor, int numColors);

boolean isTabu(Graph *g,int **adjColors, int id, int color, int **tabuList, int nIt, int numColors,int nC ,int bestNc);

int setTabu(Graph *g,int **adjColors, int **tabuList, int numColors, oneMove *move, int fixLong, float propLong, int nIt);

int greedyColor(Graph *g);

int greedyInitColor(Graph *g,int numColors);

int getGreedyMaxOrder(Graph *g,pNode *orderNode);

boolean colorAdjFree(Node *n,int color);

int getNodeMaxOrder(Graph *g);

void waitEnter(char *message);

void printSeparator();
#endif
