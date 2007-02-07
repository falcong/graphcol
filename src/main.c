#include "../inc/defs.h"
#include "../inc/routines.h"

void doTabu(int colors,Graph *g, int fixLong, float propLong, int maxIt, int verbosity, char *instFile, int nRestart);

void doSA(int colors, Graph *g, int verbosity, char *instFile, int maxIt);

void printTabuProcessInfo(Graph *g, int type, int result, char *instFile, int colors, int execTime, int maxIt, int fixLong, float propLong, int stopIt, int nRestart);

void printSAProcessInfo(Graph *g,int verbosity,int result,char *instFile,int colors,int execTime);

int main(int argc, char *argv[])
{
  char instFile[LUNGHEZZA+1];
  Graph *g;
  int colors,verbosity;
  int fixLong,maxIt,nRestart;
  float propLong;
	
  //Reading instance file
  readCommand(argc,argv,instFile,&colors,&verbosity);
  //Loading instance file & building graph struct
  g=loadGraph(instFile);
  
	//Set default tabu search values
  maxIt=1000;
  fixLong=g->numNodes/2;
  propLong=0.5;
	nRestart=0;
	
	//Reading configuration file
  readConfFile(&nRestart,&maxIt,&fixLong,&propLong);
	 
// 	doTabu(colors,g,fixLong,propLong,maxIt,verbosity,instFile,nRestart);
	doSA(colors,g,verbosity,instFile,maxIt);
	
  return 0;
}

void doTabu(int colors,Graph *g, int fixLong, float propLong, int maxIt, int verbosity, char *instFile, int nRestart)
{
	boolean findmin;
	int startTime,stopTime,execTime;
	int result,stopIt,restIt,nR;
	int **adjColors;
	
	adjColors=NULL;
	nR=0;
	
	if(colors==-1)
	{	//Number of colors not chosen: find upperbound value
		colors=greedyColor(g);
		printf("Greedy Colors:%d\n",colors);
			
		findmin=FALSE;
		while(!findmin)
		{
			//Build the random initial solution
			randomColor(g,colors);
			startTime=time(NULL);
			result=findTabu(g,colors,fixLong,propLong,maxIt,&stopIt,&adjColors);
			stopTime=time(NULL);
			execTime=stopTime-startTime;
			
			if(result==0)
				printf("Find %d-coloring for the current graph with Tabucol\n",colors);
			else
			{
				printf("\nFailed to find %d-coloring for the current graph with Tabucol\n",colors);
				printf("Remaining %d conflicting nodes\n\n",result);
				findmin=TRUE;
			}
			printTabuProcessInfo(g,verbosity,result,instFile,colors,execTime,maxIt,fixLong,propLong,stopIt,nR);
			colors--;
		}
	}
	else
	{
		//Number of colors chosen
  	//Build the random initial solution
		randomColor(g,colors);

		startTime=time(NULL);
		result=findTabu(g,colors,fixLong,propLong,maxIt,&stopIt,&adjColors);
		restIt=stopIt;
  
		if(result==0)
			printf("Find %d-coloring for the current graph with Tabucol\n",colors);
		else
		{
			printf("\nFailed to find %d-coloring for the current graph with Tabucol\n",colors);
			printf("Remaining %d conflicting nodes\n\n",result);
			
			while(result!=0 && nR<nRestart)
			{
				nR++;
				randomConflictingColor(g,colors,adjColors);
				result=findTabu(g,colors,fixLong,propLong,maxIt,&stopIt,&adjColors);
				restIt+=stopIt;
				
				if(result==0)
				{
					printf("Find %d-coloring for the current graph with Tabucol(r:%d)\n",colors,nRestart);
					break;
				}
				else
				{
					printf("\nFailed to find %d-coloring for the current graph with Tabucol(r:%d)\n",colors,nRestart);
					printf("Remaining %d conflicting nodes\n\n",result);
				}
				
			}
		}
		
		stopTime=time(NULL);
		execTime=stopTime-startTime;
		printTabuProcessInfo(g,verbosity,result,instFile,colors,execTime,maxIt,fixLong,propLong,restIt,nR);
	}
}

void doSA(int colors, Graph *g, int verbosity, char *instFile, int maxIt)
{
	int startTime,stopTime,execTime;
	int startTemp;
	int result,stopIt;
	int **adjColors;
	boolean findmin;
	
	adjColors=NULL;
	startTemp=10;

	colors=greedyColor(g);
	printf("Greedy Colors:%d\n",colors);
			
	findmin=FALSE;
// 	while(!findmin)
	{
		//Build the random initial solution
		randomColor(g,colors);
		
		startTime=time(NULL);
		
		result=findSA(g,colors,&stopIt,&adjColors,startTemp);
		
		stopTime=time(NULL);
		execTime=stopTime-startTime;
			
		if(result==0)
			printf("Find %d-coloring for the current graph with Simulated Annealing\n",colors);
		else
		{
			printf("\nFailed to find %d-coloring for the current graph Simulated Annealing\n",colors);
			printf("Remaining %d conflicting nodes\n\n",result);
			findmin=TRUE;
		}
		
		printSAProcessInfo(g,verbosity,result,instFile,colors,execTime);
		colors--;
	}
}


void printTabuProcessInfo(Graph *g, int type, int result, char *instFile, int colors, int execTime, int maxIt, int fixLong, float propLong, int stopIt, int nRestart)
{
  
	FILE *fResults,*fLegalColoring;
	char solfilename[LUNGHEZZA],filename[LUNGHEZZA];
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
  
  fprintf(fResults,"%dsec\t%d/%dit\t%dfix\t%.2fprop\tr:%d",execTime,stopIt,maxIt,fixLong,propLong,nRestart);
  fclose(fResults);
  
  switch(type)
  {
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
		
		printf("Printing %d-coloring solution for the current graph: %s(%d).sol\n",colors,filename,colors);
		
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
		
		fclose(fLegalColoring);
	}	
	else
	{
		printf("Not Printing %d-coloring solution for the current graph\n",colors);	
	}
}

void printSAProcessInfo(Graph *g,int verbosity,int result,char *instFile,int colors,int execTime)
{

}
