#ifndef _routines_h
#define _routines_h

#include "../inc/defs.h"
#include "../inc/graph.h"

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


boolean findTabu(Graph *g, int numColors);
#endif
