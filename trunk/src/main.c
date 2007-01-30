#include "../inc/defs.h"
#include "../inc/routines.h"

void printProcessInfo(Graph *g, int type, int result, char *instFile, int colors, int execTime, int maxIt, int fixLong, float propLong);


int main(int argc, char *argv[])
{
  char instFile[LUNGHEZZA+1];
  Graph *g;
  int colors,verbosity;
  int fixLong,maxIt;
  float propLong;
  boolean result;
  int startTime,stopTime,execTime;

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
	
	if(colors==-1)
	{
		colors=greedyColor(g);
		printf("Greedy Colors:%d\n",colors);
		
		result=FALSE;
		while(!result)
		{
			//Build the random initial solution
			randomColor(g,colors);
			
			startTime=time(NULL);
			result=findTabu(g,colors,fixLong,propLong,maxIt);
			stopTime=time(NULL);
			execTime=stopTime-startTime;
			
			if(result==0)
				printf("Find %d-coloring for the current graph\n",colors);
			else
			{
				printf("\nFailed to find %d-coloring for the current graph\n",colors);
				printf("Remaining %d conflicting nodes\n\n",result);
			}
			printProcessInfo(g,verbosity,result,instFile,colors,execTime,maxIt,fixLong,propLong);
			
			colors--;
		}
	}
	else
	{
  	//Build the random initial solution
  	randomColor(g,colors);
  
  	startTime=time(NULL);
  	result=findTabu(g,colors,fixLong,propLong,maxIt);
		stopTime=time(NULL);
		execTime=stopTime-startTime;
  
		if(result==0)
			printf("Find %d-coloring for the current graph\n",colors);
		else
		{
			printf("\nFailed to find %d-coloring for the current graph\n",colors);
			printf("Remaining %d conflicting nodes\n\n",result);
		}
		
		printProcessInfo(g,verbosity,result,instFile,colors,execTime,maxIt,fixLong,propLong);
	}
  
  return 0;
}

void printProcessInfo(Graph *g, int type, int result, char *instFile, int colors, int execTime, int maxIt, int fixLong, float propLong)
{
  
  FILE *fResults;
	FILE *fLegalColoring;
	char *solfilename,filename[LUNGHEZZA];
	Node *n;
  
  fResults=fopen("results.txt","a");
	if(fResults==NULL)
	{
		printf("Error in opening results file\n");
		exit(EXIT_OPENFILE);
	}
	
  fprintf(fResults,"\n%s\t(%d,%d)\t%d\t",instFile,g->numNodes,g->numEdges,colors);
  
  if(result==0)
    fprintf(fResults,"COLORED(C:%d)\t",result);
  else
    fprintf(fResults,"FAILED(C:%d)\t",result);
  
  fprintf(fResults,"%dsec\t%dit\t%dfix\t%.2fprop",execTime,maxIt,fixLong,propLong);
  fclose(fResults);
  
  switch(type)
  {
    case 0: 
            break;
		case 1: printf("Printing graph image\n");
						printDotOut(g->nodesList);
            system("dot out.dot -Tpng > out.png");
            break;
    case 2: printf("Printing adjacency list\n");
						printNodesList(g->nodesList);
            break;
    default:break;
  }
	
	if(result==0)
	{
		if(sscanf(instFile,"instances/%s.col",filename)!=1)
		{
			printf("Error in opening coloring solution file\n");
			exit(EXIT_OPENFILE);
		}
		
		sprintf(solfilename,"instances_colored/%s(%d).sol",filename,colors);
		
		fLegalColoring=fopen(solfilename,"w");
		
		if(fLegalColoring==NULL)
		{
			printf("Error in opening coloring solution file\n");
			exit(EXIT_OPENFILE);
		}
			
		fprintf(fLegalColoring,"NODES\tCOLOR\n");
		
		n=firstNodesList(g->nodesList);
		while(!endNodesList(n,g->nodesList))
		{
			fprintf(fLegalColoring,"%d\t%d\n",n->id,n->color);
			n=nextNodesList(n);
		}
	}	
}
