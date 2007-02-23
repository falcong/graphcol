#include "../inc/routines.h"
#include <time.h>

/**
Function that read from the command line the name of the instance file
*/
void readCommand(int argc,char *argv[],char *instFile, int *pcolors, int *verbosity)
{
  int colors,verb;
  if(argc != 2 && argc != 3 && argc != 4)
  {
    printf("Wrong command line!\n");
    printf("Use %s instance_file [colors][v0|v1|v2]\n",argv[0]);
    exit(EXIT_WRONGCOMMANDLINE);
  }

  strcpy(instFile,argv[1]);

  if(argc == 3)
  {
		if(sscanf(argv[2],"%d",&colors)!=1)
		{
			colors=-1;
		}
		
		if(sscanf(argv[2],"v%d",&verb)!=1)
		{
			verb=DEFAULTVERB;
		}
		
	}
	else if(argc > 3)
	{
		if(sscanf(argv[2],"%d",&colors)!=1)
		{
			printf("Wrong command line!\n");
			printf("Use %s instance_file [colors][v0|v1|v2]\n",argv[0]);
			exit(EXIT_WRONGCOMMANDLINE);
		}
		
    if(sscanf(argv[3],"v%d",&verb)!=1)
		{
      verb=DEFAULTVERB;
    }
  }
  else
  {
    colors=-1;
  }

  *pcolors = colors;
  *verbosity = verb;

#ifdef DEBUG
  printf("instance file: %s\n",instFile);
#endif

}

void readConfFile(int *nRestart,int *maxIt,int *fixLong,float *propLong,int *maxItImprove,float *startTemp,float *minTemp,float *tempFactor, int *maxItConstTemp)
{
  FILE *fConf;
  char row[LUNGHEZZA];
  char first;
  
  fConf=fopen(CONF_FILE,"r");
  if(fConf==NULL)
  {
    printf("Error in opening configuration file\n");
    exit(EXIT_OPENFILE);
  }

  while(fgets(row,LUNGHEZZA,fConf)!=NULL)
  {
    sscanf(row,"%c",&first);
//     printf("%c\n",first);
    
    switch(first)
    {
      case '#': //comment row
                break;
      default:{
// 								printf("stringa: %s\n",row);
								if(sscanf(row,"REST %d",nRestart)==1)
								{
//                   printf("Rest it:%d\n",*nRestart);
								}
               	else if(sscanf(row,"MAXIT %d",maxIt)==1)
                {
//                   printf("Max it:%d\n",*maxIt);
                }
                else if(sscanf(row,"FIXTT %d",fixLong)==1)
                {
//                   printf("Fix long:%d\n",*fixLong);
                }
                else if(sscanf(row,"PROPTT %f",propLong)==1)
                {
//                   printf("Prop long:%f\n",*propLong);
                }
								else if(sscanf(row,"TEMPSTOP %f",minTemp)==1)
								{
//                   printf("Prop long:%f\n",*propLong);
								}
								else if(sscanf(row,"TEMPSTART %f",startTemp)==1)
								{
//                   printf("Start temperature:%f\n",*startTemp);
								}
								else if(sscanf(row,"TEMPFACT %f",tempFactor)==1)
								{
//                   printf("Decrease temperature factor:%f\n",*tempFactor);
								}
								else if(sscanf(row,"ITCONSTTEMP %d",maxItImprove)==1)
								{
//                   printf("Iterations at constant temperature:%d\n",*maxItImprove);
								}
								else if(sscanf(row,"MAXITCONSTTEMP %d",maxItConstTemp)==1)
								{
//                   printf("Max iterations at constant temperature:%d\n",*maxItConstTemp);
								}
                else if(sscanf(row," ")==0)
                {
                  //empty row
                }
                else
                {
                  printf("Problem in configuration file. Command not recognized!\n");
                  printf("stringa: %s\n",row);
                  exit(EXIT_WRONGCONF);
                }
                break;
              }
    }
  }

  fclose(fConf);
}

/**
Function that read the information stored on the instance file in DIMACS format for load the coloring graph
*/
Graph *loadGraph(char *instFile)
{
  FILE *finst;
  char line[LUNGHEZZA];
  char format[LUNGHEZZA];
  int numNodes,numEdges,w,v;
  char type;

  boolean problemRead;
  Node *nW, *nV;
  Graph *g;

	g=NULL;
  problemRead = FALSE;

  finst=fopen(instFile,"r");
  if (finst == NULL)
  {
    printf("Error in opening instance file\n");
    exit(EXIT_OPENFILE);
  }

  while(fgets(line,LUNGHEZZA,finst)!=NULL)
  {
//     printf("Riga %s\n",line);
    if (sscanf(line,"%c",&type) == 0)
    {
      printf("Problem format wrong!\n");
      exit(EXIT_WRONGINPUTFORMAT);
    }

    if (type == 'c')
    {
#ifdef DEBUG
//       printf("Commento: %s \n",line);
#endif
      continue;
    }
    else
    {
      switch(type)
      {
        //Problem Line
        case 'p': if(sscanf(line,"p %s %d %d",format,&numNodes,&numEdges) !=3)
                  {
                    printf("Problem Format Wrong!\n");
                    exit(EXIT_WRONGINPUTFORMAT);
                  }
                  printf("Problem: %s (n:%d, e:%d)\n",format,numNodes,numEdges);
                  //Creazione grafo per l'analisi del problema
                  g=createGraph(numNodes,numEdges);
                  initNodes(numNodes,g);
                  problemRead = TRUE;
//                   printf("Inizializzata la lista di nodi\n");
//                   printNodesList(g->nodesList);

                  break;
        //Node lines
        case 'n': //Non servono per il problema del coloring
                  break;
        //Edge lines
        case 'e': if(sscanf(line,"e %d %d",&w,&v) != 2)
                  {
                    printf("Edge Format Wrong!\n");
                    exit(EXIT_WRONGINPUTFORMAT);
                  }
#ifdef DEBUG
//                   printf("Readed Edge: %d %d\n",w,v);
#endif
                  if(problemRead == FALSE)
                  {
                    printf("Problem Row not already Read!");
                    exit(EXIT_WRONGINPUTFORMAT);
                  }
                  
                  // Don't insert in the adjacency list loop-edges
                  if(w==v)
                    continue;
                  
                  nW=getNodeFromList(w,g->nodesList);
                  if(nW == NULL)
                  {
                    printf("Node %d not already Created!",w);
                    exit(EXIT_WRONGINPUTFORMAT);
                  }

                  nV=getNodeFromList(v,g->nodesList);
                  if(nV == NULL)
                  {
                    printf("Node %d not already Created!",v);
                    exit(EXIT_WRONGINPUTFORMAT);
                  }
//                   printf("Creating Edge (%d,%d)\n",nW->id,nV->id);

                  //Inserimento dell'edge sotto forma di liste di adiacenza fra i due nodi selezionati
                  //Verifica che non vi siano duplicati
                  if(!nodesIsInAdjList(nW->id,nV->adj))
									{
                    appendpNodesList(nW,nV->adj);
										nV->numAdj++;
									}
                  
                  if(!nodesIsInAdjList(nV->id,nW->adj))
									{
                    appendpNodesList(nV,nW->adj);
										nW->numAdj++;
									}

//                   printf("Created Edge(%d,%d)\n",nW->id,nV->id);
                  break;

        default:  printf("%s\n",line);
                  break;
      }
    }


  }

  fclose(finst);

  return g;
}

/**
Function that print in a "out.dot" file the resulting graph of the computation
*/
void printDotOut(NodesList *l)
{
  Node *n;
  pNodesList *pl;
  pNode *pn;
  FILE *pf;
  char *name;

  pf=fopen("out.dot","w");
  fprintf(pf,"strict graph G {\n");
  
  fprintf(pf,"ratio=compress;\n");

  //Print nodes
  n=firstNodesList(l);
  while(!endNodesList(n,l))
  {
    name=getColorName(n->color);
    fprintf(pf,"%d [color=%s,style=filled];\n",n->id,name);
    n=nextNodesList(n);
  }
  
  //Print edges
  n=firstNodesList(l);
  while(!endNodesList(n,l))
  {
    pl = n->adj;
//    fprintf(pf,"%d",n->color);

    if(emptypNodesList(pl))
    {
      n=nextNodesList(n);
      continue;
    }

    pn=firstpNodesList(pl);
    while(!endpNodesList(pn,pl))
    {
      fprintf(pf,"%d--%d;\n",n->id,pn->n->id);
//       printf("%d--%d;\n",n->id,pn->n->id);
      pn=nextpNodesList(pn);
    }
    n=nextNodesList(n);
  }

  fprintf(pf,"}\n");
  fclose(pf);
}

/**
Function that perform the tabu search on given Graph with given number of colors
*/
int findTabu(Graph *g, int numColors, int fixLong, float propLong, int maxIt, int *stopIt, int ***adj)
{
  int **tabuList;
  int **adjColors;
	int *bestSol;
  int i,j,nIt,nC,bestNc,oldC,tabuT,endIt;
  oneMove *move;
  Node *n;

  //Build adjacency matrix & 1-move 
	{
    tabuList = (int **) calloc(sizeof(int *),g->numNodes);
    if(tabuList == NULL)
		{
      printf("Not enough memory to allocate tabu list\n");
      exit(EXIT_MEMORY);
    }

		adjColors = (int **) calloc(sizeof(int *),g->numNodes);
		if(adjColors == NULL)
		{
			printf("Not enough memory to allocate adjacency matrix\n");
			exit(EXIT_MEMORY);
		}

		bestSol=(int *)calloc(sizeof(int),g->numNodes);
		if(bestSol == NULL)
		{
			printf("Not enough memory to allocate best solution array\n");
			exit(EXIT_MEMORY);
		}
    
    move=(oneMove *) malloc(sizeof(oneMove));
    if(move == NULL)
    {
      printf("Not enough memory to allocate move!\n");
      exit(EXIT_MEMORY);
    }
    
    for(i=0;i<g->numNodes;i++)
    {
      tabuList[i]= (int *) calloc(sizeof(int),numColors);
			bestSol[i]=-1;
      
      if(tabuList[i] == NULL)
      {
        printf("Not enough memory to allocate tabu list\n");
        exit(EXIT_MEMORY);
      }
      
      adjColors[i] = (int *) calloc(sizeof(int),numColors);
      
      if(adjColors[i] == NULL)
      {
        printf("Not enough memory to allocate adjacency matrix\n");
        exit(EXIT_MEMORY);
      }
      
      for(j=0;j<numColors;j++)
      {
        tabuList[i][j]=0;
        adjColors[i][j]=0;
      }
    }
	}
  
  //Init the adjacency matrix with the solution values
  buildAdjacency(g,adjColors,numColors);
	
// 	printAdjacency(adjColors,g,numColors);

  nIt=0;
  nC=nodesConflicting(g->nodesList,adjColors,numColors);
	bestNc=nC;
//   printf("Number of conflicting nodes:%d\n",nC);
	endIt=maxIt;
	saveBestSolution(g,numColors,bestSol);
	
  while(nC > 0 && nIt < endIt)
  {
// 		printAdjacency(adjColors,g,numColors);
    nIt++;
		//Find the best 1-move 
    move=findBest1Move(g,adjColors,tabuList,numColors,move,fixLong,propLong,nIt,nC,bestNc);
    if(move->id==-1)
    {
//       printf("It%d\t(conf:%d):\tNo available moves!\tSkip to next iteration!\n",nIt,nC);
      continue;
    }
		
		//Do the best 1-move
    n=getNodeFromList(move->id,g->nodesList);
    oldC=n->color;
    n->color=move->bestNew;
		//Set the move tabu
    tabuT=setTabu(g,adjColors,tabuList,numColors,move,fixLong,propLong,nIt);
    //Update adjacency matrix
		updateAdjacency(g,adjColors,move,numColors);
		//Update the obj function
    nC=nodesConflicting(g->nodesList,adjColors,numColors);
		
		if(nC<bestNc)
		{
			bestNc=nC;
			endIt=nIt+maxIt;
			saveBestSolution(g,numColors,bestSol);
		}
		
		if(nC==1)
		{
			printConflictingNodesList(g,adjColors);
			printf("Error in graph struct! Impossible to have only one conflicting node!\n");
			printf("Printed graph struct\n");
			exit(EXIT_INCONSISTENCY);
		}
				
// 		printf("It%d/%d\t(conf:%3d,best:%3d): Node %3d(%3d=>%3d) setTabu(%3d it)\n",nIt,endIt,nC,bestNc,move->id,move->color,move->bestNew,tabuT-nIt);
  }
  
//   printAdjacency(adjColors,g,numColors);
	
	loadBestSolution(g,numColors,bestSol);
	buildAdjacency(g,adjColors,numColors);
	nC=nodesConflicting(g->nodesList,adjColors,numColors);
	printf("Loaded best solution: c%d\n",nC);
		
// 	printAdjacency(adjColors,g,numColors);
	
	*adj=adjColors;
  *stopIt=nIt;
	
	//Deallocation
	free(move);
	
	for(i=0;i<g->numNodes;i++)
	{
		free(tabuList[i]);
	}
	
	free(tabuList);
	free(bestSol);
	
// 	return bestNc;
  return nC; // Return the current number of conflicting nodes
}

void saveBestSolution(Graph *g,int numColors,int *bestSol)
{
	Node *n;
	
// 	printf("Saving best solution: ");
	n=firstNodesList(g->nodesList);
	
	while(!endNodesList(n,g->nodesList))
	{
// 		printf("%d ",n->id);
		bestSol[n->id-1]=n->color;	
		n=nextNodesList(n);
	}
}

void loadBestSolution(Graph *g,int numColors,int *bestSol)
{
	Node *n;
	
// 	printf("Loading best solution\n");
	n=firstNodesList(g->nodesList);
	
	while(!endNodesList(n,g->nodesList))
	{
		n->color=bestSol[n->id-1];
		n=nextNodesList(n);
	}
}

int findSA(Graph *g,int numColors,int *stopIt,int ***adj, float startTemp, float minTemp,float tempFactor, int maxItImprove, int maxItConstTemp)
{
	int **adjColors;
	int i,j,nIt,nC,bestNc,currentNc,profit,tempColor,nItNotChange,inv,currentItImprove;
	float currentTemp;
	double exponent,dice;
	oneMove *move;
	Node *n;
	
	{
		adjColors = (int **) calloc(sizeof(int *),g->numNodes);
		if(adjColors == NULL)
		{
			printf("Not enough memory to allocate adjacency matrix\n");
			exit(EXIT_MEMORY);
		}
			
		move=(oneMove *) malloc(sizeof(oneMove));
		if(move == NULL)
		{
			printf("Not enough memory to allocate move!\n");
			exit(EXIT_MEMORY);
		}
		
		for(i=0;i<g->numNodes;i++)
		{
			adjColors[i] = (int *) calloc(sizeof(int),numColors);
			
			if(adjColors[i] == NULL)
			{
				printf("Not enough memory to allocate adjacency matrix\n");
				exit(EXIT_MEMORY);
			}
	
			for(j=0;j<numColors;j++)
			{
				adjColors[i][j]=0;
			}
		}
	}
	
	//Init the adjacency matrix with the solution values
	buildAdjacency(g,adjColors,numColors);
	
	nC=nodesConflicting(g->nodesList,adjColors,numColors);
	printf("Number of conflicting nodes:%d\n",nC);
	
	inv=0;
	nIt=0;
	bestNc=nC;
 	currentTemp=startTemp;
	currentItImprove=floor(maxItImprove*1/currentTemp);
	
// 	printf("max:%d*=%d\n",currentItImprove,inv);
	
	//Iterations at decreasing temperature: while there isn't improvement for n
	//iterations, decrease the temperature value and restart the search
	while(nC > 0 && currentTemp > minTemp)
	{
		nItNotChange=0;
		//Iterations at constant temperature: 
		//while there is an improvement every n iterations
		while(nC>0 && nItNotChange<currentItImprove)
		{
			nIt++;
			currentNc=nC;
			
			//Generate random move 
			move=findRandomSA1Move(g,adjColors,numColors,move);
			
			n=getNodeFromList(move->id,g->nodesList);
			
			//Check if the move is valid	
			n->color=move->bestNew; // Do the move
			updateAdjacency(g,adjColors,move,numColors); // Update the adjacency matrix
			nC=nodesConflicting(g->nodesList,adjColors,numColors); // get new f. obj value
			
			profit=currentNc-nC; // get the profit of the move
			
			if(profit>=0)
			{
				//Downhill move
				//The move is already done
				if(profit >0)
				{
					printf("+ It%7d(%2d/%d) (c:%4d): Node %4d (%3d=>%3d) Profit:%3d%3d=%3d \n",nIt,nItNotChange,currentItImprove,nC,move->id,move->color,move->bestNew,currentNc,-nC,profit);
				}
				else
				{
					printf("= It%7d(%2d/%d) (c:%4d): Node %4d (%3d=>%3d) Profit:%3d%3d=%3d \n",nIt,nItNotChange,currentItImprove,nC,move->id,move->color,move->bestNew,currentNc,-nC,profit);
				}
			}
			else
			{	
				//Uphill move
				dice=(double)rand()/RAND_MAX;	
				exponent=profit/currentTemp;
				
				if(dice<exp(exponent))
				{
					printf("- It%7d(%2d/%d) (c:%4d): Node %4d (%3d=>%3d) Profit:%3d%3d=%3d\t%.5f<:%.2f(T:%.2f)\tOk \n",nIt,nItNotChange,currentItImprove,nC,move->id,move->color,move->bestNew,currentNc,-nC,profit,dice,exp(exponent),currentTemp);
				}
				else
				{
					tempColor=move->color;
					
					move->color=move->bestNew;
					move->bestNew=tempColor;
					n->color=tempColor;
					
					updateAdjacency(g,adjColors,move,numColors);
					printf("x It%7d(%2d/%d) (c:%4d): Node %4d (%3d=>%3d) Profit:%3d%3d=%3d\t%.2f<:%.2f(T:%.2f)\tBack \n",nIt,nItNotChange,currentItImprove,nC,move->id,move->color,move->bestNew,currentNc,-nC,profit,dice,exp(exponent),currentTemp);
					nC=nodesConflicting(g->nodesList,adjColors,numColors); // get new f. obj value				
				}
			}
		
			if(nC<bestNc)
			{
				bestNc=nC;
				nItNotChange=0;
			}
			else
				nItNotChange++;
		}
		
		bestNc=nC;
		printf("\n");
		//Update temperature value
		currentTemp=tempFactor*currentTemp;
		currentItImprove=floor(maxItImprove*1/currentTemp);
		
		if(currentItImprove>maxItConstTemp)
			currentItImprove=maxItConstTemp;
	}
	
	free(move);
	
	*adj=adjColors;
	*stopIt=nIt;
	return nC;
}

//Da testare il salvataggio della soluzione migliore prima di muoversi:
//-Se dopo la discesa locale la soluzione raggiunta è migliore dell'ottima allora ci si muove
//-Se dopo la discesa locale la soluzione raggiunta è peggiore allora non ci si muove
int findVNS(Graph *g, int numColors, int fixLong, float propLong, int maxIt, int *stopIt, int ***adj)
{
	int result,neigh,i,j,nC,iVns,bestResult;
	int **adjColors;
	int *bestSol;
	
	neigh=0;
	iVns=0;
	adjColors=NULL;
	
	{
		adjColors = (int **) calloc(sizeof(int *),g->numNodes);
		if(adjColors == NULL)
		{
			printf("Not enough memory to allocate adjacency matrix\n");
			exit(EXIT_MEMORY);
		}
		
		for(i=0;i<g->numNodes;i++)
		{
			adjColors[i] = (int *) calloc(sizeof(int),numColors);
				
			if(adjColors[i] == NULL)
			{
				printf("Not enough memory to allocate adjacency matrix\n");
				exit(EXIT_MEMORY);
			}
		
			for(j=0;j<numColors;j++)
			{
				adjColors[i][j]=0;
			}
		}
		
		bestSol=(int *)calloc(sizeof(int),g->numNodes);
		if(bestSol == NULL)
		{
			printf("Not enough memory to allocate best solution array\n");
			exit(EXIT_MEMORY);
		}
	}
	
	buildAdjacency(g,adjColors,numColors);
	
	nC=nodesConflicting(g->nodesList,adjColors,numColors);
	result=nC;
	bestResult=result;
	saveBestSolution(g,numColors,bestSol);
	printf("Number of conflicting nodes:%d\n",nC);
	
	while(result>0 && iVns < g->numNodes)
	{
		printf("VNS: Shaking the solution...\n");
		shake(g,numColors,neigh,adjColors,iVns);
		printf("VNS: Tabu descent...\n");
		
		for(i=0;i<g->numNodes;i++)
			free(adjColors[i]);
		free(adjColors);
		
		result=findTabu(g,numColors,fixLong,propLong,maxIt,stopIt,&adjColors);
		printf("VNS: it%4d (BEST tabu:%d,global:%d) neigh:%d\n",iVns,result,bestResult,neigh);
		
		if(result<bestResult)
		{
			//Reset the iterations count if the current result is better than best
			bestResult=result;
			iVns=0;
			neigh=0;
			saveBestSolution(g,numColors,bestSol);
		}
		else
		{
// 			if(result>bestResult)
			{
				loadBestSolution(g,numColors,bestSol);
				buildAdjacency(g,adjColors,numColors);
				nC=nodesConflicting(g->nodesList,adjColors,numColors);
				printf("Number of conflicting nodes:%d\n",nC);
			}
// 			printf("\t%d/5 = %d -> %d mod %d = %d",g->numNodes,g->numNodes/5,iVns,g->numNodes/5,iVns%((g->numNodes/5)));
			if(iVns%((g->numNodes/5))==0)
			{
				neigh++;
// 				printf("\tIncreased neighborhood:%d->%d",neigh-1,neigh);
			}
		}
		iVns++;
	}
	
	free(bestSol);
	*adj=adjColors;
	return bestResult;
}

void shake(Graph *g,int numColors,int neigh, int **adjColors, int iVns)
{
	switch(neigh)
	{
		case 0:doVNSChain(g,numColors,adjColors,iVns);
					break;
		case 1:doVNSGrenade(g,numColors,adjColors,iVns);
					break;
		case 2:doVNSFirework(g,numColors,adjColors,iVns);
					break;
		case 3:doVNSEmptyRefill(g,numColors,adjColors);
					break;
		default:randomConflictingColor(g,numColors,adjColors);
						doVNSEmptyRefill(g,numColors,adjColors);
					break;
	}
}

/**
 * Chain neighborhood:
 * Randomly choose a conflicting node and move it into the best possible other
 * color class. Then choose at random a new conflicting node and assign to it
 * the best possible new color (not to change the color of a node more than 
 * once). 
*/
void doVNSChain(Graph *g, int numColors, int **adjColors, int iVns)
{
	int i,id,stopIt,maxIt,delta,it,newConf,nextClass,nC;
	boolean *preConflicting,*postConflicting,*newConflicting,*blacklist;
	double d;
	oneMove *move;
	Node *n;
	
	{
		move=(oneMove *) malloc(sizeof(oneMove));
		if(move == NULL)
		{
			printf("Not enough memory to allocate move!\n");
			exit(EXIT_MEMORY);
		}
		
		preConflicting=(boolean *)calloc(sizeof(boolean),g->numNodes);
		if(preConflicting == NULL)
		{
			printf("Not enough memory to allocate conflicting nodes array\n");
			exit(EXIT_MEMORY);
		}
		
		postConflicting=(boolean *)calloc(sizeof(boolean),g->numNodes);
		if(postConflicting == NULL)
		{
			printf("Not enough memory to allocate conflicting nodes array\n");
			exit(EXIT_MEMORY);
		}
		
		newConflicting=(boolean *)calloc(sizeof(boolean),g->numNodes);
		if(newConflicting == NULL)
		{
			printf("Not enough memory to allocate conflicting nodes array\n");
			exit(EXIT_MEMORY);
		}
		
		blacklist=(boolean *)calloc(sizeof(boolean),g->numNodes);
		if(blacklist == NULL)
		{
			printf("Not enough memory to allocate conflicting nodes array\n");
			exit(EXIT_MEMORY);
		}
		
		for(i=0;i<g->numNodes;i++)
		{
			preConflicting[i]=FALSE;
			postConflicting[i]=FALSE;
			newConflicting[i]=FALSE;
			blacklist[i]=FALSE;
		}
	}

	d=(double)(iVns*5)/g->numNodes;
	delta=floor(15*d);
	maxIt=20-delta;
	stopIt=(rand()%maxIt)+1;
	nextClass=-1;
	
	//Get conflicting nodes array	
	preConflicting=getConflictingNodes(g,numColors,adjColors,preConflicting);
	nC=nodesConflicting(g->nodesList,adjColors,numColors);
	printf("VNS: CHAIN C:%d for %d (1..%d)\n",nC,stopIt,maxIt);
	
	for(it=0;it<stopIt;it++)
	{	
		//Reset history informations
		nextClass=-1;	
		for(i=0;i<g->numNodes;i++)
		{
			blacklist[i]=FALSE;
		}
	
		//Random choose conflicting node
		id=getRandomChainNode(g,adjColors,nextClass,preConflicting,nC,numColors,blacklist);
		
		if(id==-1)
		{
			break;
		}
		
		//Get best move for the current node
		move=findBestNodeMove(g,id,adjColors,numColors,move);
		
		//Do the move
		n=getNodeFromList(id,g->nodesList);
		n->color=move->bestNew;
		nextClass=n->color;
		blacklist[n->id-1]=TRUE;
		
		//Update the adjacency matrix
		updateAdjacency(g,adjColors,move,numColors);
			
		//Get current conflicting nodes array
		postConflicting=getConflictingNodes(g,numColors,adjColors,postConflicting);
		
		//Get only new conflicting nodes
		newConflicting=getOnlyNewConflictingNodes(preConflicting,postConflicting,g->numNodes,newConflicting);
		newConf=countOnlyNewConflictingNodes(newConflicting,g->numNodes);

		//Get conflicting nodes array	
		preConflicting=getConflictingNodes(g,numColors,adjColors,preConflicting);
		nC=nodesConflicting(g->nodesList,adjColors,numColors);

		if(nC==0)
			break;
		
// 		printf("\tVNS CHAIN: it %2d (%d:%d->%d) conf: %dnew /%dtotal\n",it,move->id,move->color,move->bestNew,newConf,nC);
		printf("(it%2d,C:%3d,%3dnew)%3d ->%3d",it,nC,newConf,move->color,move->bestNew);
		
		if(newConf>0)
		{
			printf(" (%2d) --",newConf);
			chainUpdate(g,adjColors,move,numColors,newConflicting,blacklist);
			preConflicting=getConflictingNodes(g,numColors,adjColors,preConflicting);
			nC=nodesConflicting(g->nodesList,adjColors,numColors);
		}
		
		printf("\n");
	}
	
	free(preConflicting);
	free(postConflicting);
	free(newConflicting);	
	free(blacklist);
	free(move);
}

void chainUpdate(Graph *g, int **adjColors,oneMove *move,int numColors,boolean *newConflicting,boolean *blacklist)
{
	int newConf,id,i;
	Node *n;
	boolean *preConflicting,*postConflicting;
	
	{
		preConflicting=(boolean *)calloc(sizeof(boolean),g->numNodes);
		if(preConflicting == NULL)
		{
			printf("Not enough memory to allocate conflicting nodes array\n");
			exit(EXIT_MEMORY);
		}
		
		postConflicting=(boolean *)calloc(sizeof(boolean),g->numNodes);
		if(postConflicting == NULL)
		{
			printf("Not enough memory to allocate conflicting nodes array\n");
			exit(EXIT_MEMORY);
		}
		
		for(i=0;i<g->numNodes;i++)
		{
			preConflicting[i]=FALSE;
			postConflicting[i]=FALSE;
		}
	}
	
	newConf=countOnlyNewConflictingNodes(newConflicting,g->numNodes);
	preConflicting=getConflictingNodes(g,numColors,adjColors,preConflicting);
	
	while(newConf>0)
	{
		//Get random node from only new conflicting move->bestNew colored node 
		id=getRandomChainNode(g,adjColors,move->bestNew,newConflicting,newConf,numColors,blacklist);
		
		if(id==-1)
		{
			break;
		}
		
		//Find bestNodeMove
		move=findBestNodeMove(g,id,adjColors,numColors,move);
		
		//Do the move
		n=getNodeFromList(id,g->nodesList);
		n->color=move->bestNew;
		blacklist[n->id-1]=TRUE;
	
		//Update the adjacency matrix
		updateAdjacency(g,adjColors,move,numColors);
	
		//Get current conflicting nodes array
		postConflicting=getConflictingNodes(g,numColors,adjColors,postConflicting);
		
		//Get only new conflicting nodes
		newConflicting=getOnlyNewConflictingNodes(preConflicting,postConflicting,g->numNodes,newConflicting);
		newConf=countOnlyNewConflictingNodes(newConflicting,g->numNodes);
		
		//Get conflicting nodes array	
		preConflicting=getConflictingNodes(g,numColors,adjColors,preConflicting);
// 		nC=nodesConflicting(g->nodesList,adjColors,numColors);
		
// 		printf("\t\tVNS UPDATE CHAIN: (%d:%d->%d) conf: %dnew\n",move->id,move->color,move->bestNew,newConf);
		printf("->%3d ",move->bestNew);
		newConf=countOnlyNewConflictingNodes(newConflicting,g->numNodes);
	}

	free(preConflicting);
	free(postConflicting);
}

/**
 * Advanced method to get a random conflicting node:
 * If nextClass color class is defined, then it returns a random conflicting 
 * node that isn't blacklisted and is colored with nextClass color.
 * If nextClass color class isn't defined, then it returns a random conflicting
 * node that isn't blacklisted with undefined color. This part require the
 * number of conflicting nodes of any color.
 * If there aren't available conflicting and not blacklisted nodes it returns
 * -1 code.
 * NB. The method require a conflicting array that defines which conflicting 
 * nodes to consider.
*/
int getRandomChainNode(Graph *g,int **adjColors,int nextClass,boolean *conflicting,int nC,int numColors,boolean *blacklist)
{
	int nth,i,j,cc;
	Node *n;
	
	j=0;
	
	//Color class defined
	if(nextClass!=-1)
	{	
		//Count conflicting nodes in nextClass color class
		cc=countColorConflicting(g,numColors,nextClass,conflicting,blacklist);
		if(cc>0)
		{	//If exist conflicting nodes in nextClass color class
			nth=(rand()%cc);
			for(i=0;i<g->numNodes;i++)
			{	//Scan all nodes
				if(blacklist[i]==FALSE)
				{	//If node isn't blacklisted
					if(conflicting[i]==TRUE)
					{	//If node is conflicting
						n=getNodeFromList(i+1,g->nodesList);
						if(n->color==nextClass)
						{	//If its color is the same of nextClass
							if(j==nth)
								return i+1; //If the node is the nth
							else
								j++; //Try next node
						}
					}
				}
			}
		}
		else
		{ //If doesn't exist conflicting nodes in nextClass color class then try to
			//find a conflicting nodes with undefined color
			nextClass=-1;
		}
	}
	//If the search end with unsuccess then try to find both conflicting and not
	//blacklisted node with undefined color
	
	j=0;
	//Color class not defined
	if(nextClass==-1)
	{
		nth=(rand()%nC);
		for(i=0;i<g->numNodes;i++)
		{	
			if(blacklist[i]==FALSE)
			{	
				if(conflicting[i]==TRUE)
				{
					if(j==nth)
						return i+1;
					else
						j++;
				}
			}
		}
	}
	//There aren't available nodes return to the caller that the chain can't 
	//continue
	return -1;	
}

/**
 * Return the number of conflicting nodes that are colored with a certain color
 * and that aren't blacklisted.
 * This method require a nodes array that defines what nodes to consider
*/
int countColorConflicting(Graph *g,int numColors,int color,boolean *conflicting,boolean *blacklist)
{
	int count;
	Node *n;
	
	count=0;
	
	n=firstNodesList(g->nodesList);
	while(!endNodesList(n,g->nodesList))
	{
		if(blacklist[n->id-1]==FALSE) //A node must to not be blacklisted 
		{
			if(conflicting[n->id-1]==TRUE) //A node must to be conflicting
			{
				if(n->color==color) //A node must to be colored with a certain color
					count++;
			}
		}
		n=nextNodesList(n);
	}
	return count;	//Number of conflicting nodes in a certain color class
}

/**
 * Grenade neighborhood:
 * Randomly choose a conflicting node and move it into the best possible other
 * color class. Move each new conflicting node from the arriving class into the
 * best possible other color class. This process is repeated with I different
 * grenade.
*/
void doVNSGrenade(Graph *g, int numColors, int **adjColors, int iVns)
{
	int i,id,stopIt,maxIt,delta,it,newConf,nextClass,nC;
	boolean *preConflicting,*postConflicting,*newConflicting,*blacklist;
	double d;
	oneMove *move;
	Node *n;
	
	{
		move=(oneMove *) malloc(sizeof(oneMove));
		if(move == NULL)
		{
			printf("Not enough memory to allocate move!\n");
			exit(EXIT_MEMORY);
		}
		
		preConflicting=(boolean *)calloc(sizeof(boolean),g->numNodes);
		if(preConflicting == NULL)
		{
			printf("Not enough memory to allocate conflicting nodes array\n");
			exit(EXIT_MEMORY);
		}
		
		postConflicting=(boolean *)calloc(sizeof(boolean),g->numNodes);
		if(postConflicting == NULL)
		{
			printf("Not enough memory to allocate conflicting nodes array\n");
			exit(EXIT_MEMORY);
		}
		
		newConflicting=(boolean *)calloc(sizeof(boolean),g->numNodes);
		if(newConflicting == NULL)
		{
			printf("Not enough memory to allocate conflicting nodes array\n");
			exit(EXIT_MEMORY);
		}
		
		blacklist=(boolean *)calloc(sizeof(boolean),g->numNodes);
		if(blacklist == NULL)
		{
			printf("Not enough memory to allocate conflicting nodes array\n");
			exit(EXIT_MEMORY);
		}
		
		for(i=0;i<g->numNodes;i++)
		{
			preConflicting[i]=FALSE;
			postConflicting[i]=FALSE;
			newConflicting[i]=FALSE;
			blacklist[i]=FALSE;
		}
	}

	d=(double)((iVns-(g->numNodes/5))*5)/g->numNodes;
	delta=floor(39*d);
	maxIt=40-delta;
	stopIt=(rand()%maxIt)+1;
	nextClass=-1;
	
	//Get conflicting nodes array	
	preConflicting=getConflictingNodes(g,numColors,adjColors,preConflicting);
	nC=nodesConflicting(g->nodesList,adjColors,numColors);
	printf("VNS: GRENADE C:%d for %d (1..%d)\n",nC,stopIt,maxIt);
	
	for(it=0;it<stopIt;it++)
	{	
		//Random choose conflicting node
		id=getRandomChainNode(g,adjColors,-1,preConflicting,nC,numColors,blacklist);
		
		if(id==-1)
		{ //There isn't available nodes
			break;
		}
		
		//Get best move for the current node
		move=findBestNodeMove(g,id,adjColors,numColors,move);
		
		//Do the move
		n=getNodeFromList(id,g->nodesList);
		n->color=move->bestNew;
		
		//Update the adjacency matrix
		updateAdjacency(g,adjColors,move,numColors);
			
		//Get current conflicting nodes array
		postConflicting=getConflictingNodes(g,numColors,adjColors,postConflicting);
		
		//Get only new conflicting nodes
		newConflicting=getOnlyNewConflictingNodes(preConflicting,postConflicting,g->numNodes,newConflicting);
		newConf=countOnlyNewConflictingNodes(newConflicting,g->numNodes);
				
		//Get conflicting nodes array	
		preConflicting=getConflictingNodes(g,numColors,adjColors,preConflicting);
		nC=nodesConflicting(g->nodesList,adjColors,numColors);
		
		if(nC==0)
			break;
		
		printf("(it%2d,C:%3d) %3d ->%3d",it,nC,move->color,move->bestNew);
		
		if(newConf>0)
		{
			printf(" (%2d) --*",newConf);
			grenadeUpdate(g,adjColors,move,numColors,newConflicting,blacklist);
			preConflicting=getConflictingNodes(g,numColors,adjColors,preConflicting);
			nC=nodesConflicting(g->nodesList,adjColors,numColors);
		}
		printf("\n");
	}
	
	free(preConflicting);
	free(postConflicting);
	free(newConflicting);	
	free(blacklist);
	free(move);
}

void grenadeUpdate(Graph *g,int **adjColors,oneMove *move,int numColors,boolean *newConflicting,boolean *blacklist)
{
	int i;
	Node *n;
		
	for(i=0;i<g->numNodes;i++)
	{
		if(newConflicting[i]==TRUE)
		{
			move=findBestNodeMove(g,i+1,adjColors,numColors,move);
			
			//Do the move
			n=getNodeFromList(i+1,g->nodesList);
			n->color=move->bestNew;
			
			printf("%2d ->%2d; ",move->color,move->bestNew);
		
			//Update the adjacency matrix
			updateAdjacency(g,adjColors,move,numColors);
		}
	}
}

/**
 * Firework neighborhood:
 * Randomly choose a conflicting node and move it into the best possible other
 * color class. Then consider every nyew conflicting node as a grenade.
*/
void doVNSFirework(Graph *g, int numColors, int **adjColors, int iVns)
{
	int i,id,stopIt,maxIt,delta,it,newConf,nC;
	boolean *preConflicting,*postConflicting,*newConflicting,*blacklist;
	double d;
	oneMove *move;
	Node *n;
	
	{
		move=(oneMove *) malloc(sizeof(oneMove));
		if(move == NULL)
		{
			printf("Not enough memory to allocate move!\n");
			exit(EXIT_MEMORY);
		}
		
		preConflicting=(boolean *)calloc(sizeof(boolean),g->numNodes);
		if(preConflicting == NULL)
		{
			printf("Not enough memory to allocate conflicting nodes array\n");
			exit(EXIT_MEMORY);
		}
		
		postConflicting=(boolean *)calloc(sizeof(boolean),g->numNodes);
		if(postConflicting == NULL)
		{
			printf("Not enough memory to allocate conflicting nodes array\n");
			exit(EXIT_MEMORY);
		}
		
		newConflicting=(boolean *)calloc(sizeof(boolean),g->numNodes);
		if(newConflicting == NULL)
		{
			printf("Not enough memory to allocate conflicting nodes array\n");
			exit(EXIT_MEMORY);
		}
		
		blacklist=(boolean *)calloc(sizeof(boolean),g->numNodes);
		if(blacklist == NULL)
		{
			printf("Not enough memory to allocate conflicting nodes array\n");
			exit(EXIT_MEMORY);
		}
		
		for(i=0;i<g->numNodes;i++)
		{
			preConflicting[i]=FALSE;
			postConflicting[i]=FALSE;
			newConflicting[i]=FALSE;
			blacklist[i]=FALSE;
		}
	}

	d=(double)((iVns-(2*g->numNodes/5))*5)/g->numNodes;
	delta=floor(29*d);
	maxIt=30-delta;
	stopIt=(rand()%maxIt)+1;
	
	//Get conflicting nodes array	
	preConflicting=getConflictingNodes(g,numColors,adjColors,preConflicting);
	nC=nodesConflicting(g->nodesList,adjColors,numColors);
	printf("VNS: FIREWORK C:%d for %d (1..%d)\n",nC,stopIt,maxIt);
	
	for(it=0;it<stopIt;it++)
	{	
		//Random choose conflicting node
		id=getRandomChainNode(g,adjColors,-1,preConflicting,nC,numColors,blacklist);
		
		if(id==-1)
		{ //There isn't available nodes
// 			printf("Return NO AVAILABLE, Conflicts:%d\n",nC);
			break;
		}
		
		//Get best move for the current node
		move=findBestNodeMove(g,id,adjColors,numColors,move);
		
		//Do the move
		n=getNodeFromList(id,g->nodesList);
		n->color=move->bestNew;
		
		//Update the adjacency matrix
		updateAdjacency(g,adjColors,move,numColors);
			
		//Get current conflicting nodes array
		postConflicting=getConflictingNodes(g,numColors,adjColors,postConflicting);
		
		//Get only new conflicting nodes
		newConflicting=getOnlyNewConflictingNodes(preConflicting,postConflicting,g->numNodes,newConflicting);
		newConf=countOnlyNewConflictingNodes(newConflicting,g->numNodes);
				
		//Get conflicting nodes array	
		preConflicting=getConflictingNodes(g,numColors,adjColors,preConflicting);
		nC=nodesConflicting(g->nodesList,adjColors,numColors);
		
		if(nC==0)
		{
			break;
		}
			
		printf("(it%2d,C:%3d) %2d ->%2d",it,nC,move->color,move->bestNew);
		
		if(newConf>0)
		{
			printf(" (%2d) --*",newConf);
			fireworkUpdate(g,adjColors,move,numColors,newConflicting,blacklist,newConf,preConflicting,postConflicting);
			preConflicting=getConflictingNodes(g,numColors,adjColors,preConflicting);
			nC=nodesConflicting(g->nodesList,adjColors,numColors);
		}
		else
		{
			printf("\n");
		}
	}
	
	free(preConflicting);
	free(postConflicting);
	free(newConflicting);	
	free(blacklist);
	free(move);
}

void fireworkUpdate(Graph *g,int **adjColors,oneMove *move,int numColors,boolean *grenadesConflicting,boolean *blacklist,int newConf,boolean *preConflicting, boolean *postConflicting)
{
	int i,id,nC,nG;
	boolean *newConflicting;
	Node *n;
	
	newConflicting=(boolean *)calloc(sizeof(boolean),g->numNodes);
	if(newConflicting == NULL)
	{
		printf("Not enough memory to allocate conflicting nodes array\n");
		exit(EXIT_MEMORY);
	}		
	
	for(i=0;i<g->numNodes;i++)
	{
		newConflicting[i]=FALSE;
	}
	
	nG=0;
	for(i=0;i<g->numNodes;i++)
	{
		if(grenadesConflicting[i]==TRUE)
		{
			id=i+1;	
			//Get best move for the current node
			move=findBestNodeMove(g,id,adjColors,numColors,move);
		
			//Do the move
			n=getNodeFromList(id,g->nodesList);
			n->color=move->bestNew;
		
			//Update the adjacency matrix
			updateAdjacency(g,adjColors,move,numColors);
			
			//Get current conflicting nodes array
			postConflicting=getConflictingNodes(g,numColors,adjColors,postConflicting);
		
			//Get only new conflicting nodes
			newConflicting=getOnlyNewConflictingNodes(preConflicting,postConflicting,g->numNodes,newConflicting);
			newConf=countOnlyNewConflictingNodes(newConflicting,g->numNodes);
				
			//Get conflicting nodes array	
			preConflicting=getConflictingNodes(g,numColors,adjColors,preConflicting);
			nC=nodesConflicting(g->nodesList,adjColors,numColors);
		
			if(nC==0)
				return;
			
			if(nG==0)
				printf("\tGRENADE-%2d: %2d ->%2d",nG+1,move->color,move->bestNew);
			else
				printf("\t\t\t\tGRENADE-%2d: %2d ->%2d",nG+1,move->color,move->bestNew);
		
			if(newConf>0)
			{
				printf(" (%2d) --*",newConf);
				grenadeUpdate(g,adjColors,move,numColors,newConflicting,blacklist);
				preConflicting=getConflictingNodes(g,numColors,adjColors,preConflicting);
				nC=nodesConflicting(g->nodesList,adjColors,numColors);
			}
			
			nG++;
			printf("\n");
		}
	}

	free(newConflicting);
}

/**
 * First empty the color class wich contains the largest number of conflicting 
 * nodes by moving each of its nodes into the best possible other color class, 
 * then refill the class by successively choosing an equal number of others 
 * nodes (new conflicting nodes when possible).
*/
void doVNSEmptyRefill(Graph *g, int numColors, int **adjColors)
{
	int nC,i,maxColor,maxConflict,cC,newConf;
	boolean *preConflicting,*postConflicting,*newConflicting,*blacklist;
	Node *n;
	oneMove *move;
		
	{
		move=(oneMove *) malloc(sizeof(oneMove));
		if(move == NULL)
		{
			printf("Not enough memory to allocate move!\n");
			exit(EXIT_MEMORY);
		}
		
		preConflicting=(boolean *)calloc(sizeof(boolean),g->numNodes);
		if(preConflicting == NULL)
		{
			printf("Not enough memory to allocate conflicting nodes array\n");
			exit(EXIT_MEMORY);
		}
		
		postConflicting=(boolean *)calloc(sizeof(boolean),g->numNodes);
		if(postConflicting == NULL)
		{
			printf("Not enough memory to allocate conflicting nodes array\n");
			exit(EXIT_MEMORY);
		}
		
		newConflicting=(boolean *)calloc(sizeof(boolean),g->numNodes);
		if(newConflicting == NULL)
		{
			printf("Not enough memory to allocate conflicting nodes array\n");
			exit(EXIT_MEMORY);
		}
		
		blacklist=(boolean *)calloc(sizeof(boolean),g->numNodes);
		if(blacklist == NULL)
		{
			printf("Not enough memory to allocate conflicting nodes array\n");
			exit(EXIT_MEMORY);
		}
		
		for(i=0;i<g->numNodes;i++)
		{
			preConflicting[i]=FALSE;
			postConflicting[i]=FALSE;
			newConflicting[i]=FALSE;
			blacklist[i]=FALSE;
		}
	}
	
	preConflicting=getConflictingNodes(g,numColors,adjColors,preConflicting);
	nC=nodesConflicting(g->nodesList,adjColors,numColors);
	
	maxColor=-1;
	maxConflict=-1;
	
	//Find the color class with largest number of conflicting nodes
	for(i=0;i<numColors;i++)
	{
		cC=countColorConflicting(g,numColors,i,preConflicting,blacklist);
		
		if(cC>maxConflict)
		{
			maxColor=i;
			maxConflict=cC;
		}
	}
	
// 	printf("VNS EMPTY-REFILL: Color class %d with largest number of conflicting nodes: %d\n",maxColor,maxConflict);
// 	printAdjacency(adjColors,g,numColors);
	
	//Empty the color class
	n=firstNodesList(g->nodesList);
	while(!endNodesList(n,g->nodesList))
	{		
		if(n->color==maxColor)
		{
			//Get best move for the current node
			move=findBestNodeMove(g,n->id,adjColors,numColors,move);
	
			//Do the move
			n->color=move->bestNew;
		
			//Update the adjacency matrix
			updateAdjacency(g,adjColors,move,numColors);
			
			nC=nodesConflicting(g->nodesList,adjColors,numColors);

			if(nC==0)
				return;
		}
		
		n=nextNodesList(n);
	}
	
// 	printf("VNS: EMPTY-REFILL: Emptying Color class %d (%d->0)\n",maxColor,maxConflict);
// 	printAdjacency(adjColors,g,numColors);
	
	postConflicting=getConflictingNodes(g,numColors,adjColors,postConflicting);
	
	newConflicting=getOnlyNewConflictingNodes(preConflicting,postConflicting,g->numNodes,newConflicting);
	newConf=countOnlyNewConflictingNodes(newConflicting,g->numNodes);

	if(newConf>0)
	{
		printf("VNS: EMPTY-REFILL: New conflicts: %2d->%2d emptying class %d\n",maxConflict,newConf,maxColor);
		
		for(i=0;i<g->numNodes;i++)
		{
			if(newConflicting[i]==TRUE)
			{
				move=findBestNodeMove(g,i+1,adjColors,numColors,move);
			
			  //Do the move
				n=getNodeFromList(i+1,g->nodesList);
				n->color=move->bestNew;
			
				printf("n:%2d updating (%2d ->%2d)\n",move->id,move->color,move->bestNew);
		
			  //Update the adjacency matrix
				updateAdjacency(g,adjColors,move,numColors);
				
				nC=nodesConflicting(g->nodesList,adjColors,numColors);

				if(nC==0)
					return;
			}
		}
	}
// 	printAdjacency(adjColors,g,numColors);
	
	free(preConflicting);
	free(postConflicting);
	free(newConflicting);	
	free(blacklist);
	free(move);
}

/**
Random color the graph with numColors colors
*/
void randomColor(Graph *g, int numColors)
{
  Node *n;
//   srand((unsigned int) time(NULL));
  
  n=firstNodesList(g->nodesList);
  
  printf("Random coloring the nodes graph\n");
  
  while(!endNodesList(n,g->nodesList))
  {
    n->color=(rand()%numColors);
//     printf("%d<=(%d)\n",n->id,n->color);
    n=nextNodesList(n);
  }
}

boolean *getConflictingNodes(Graph *g, int numColors, int **adjColors, boolean *conflicting)
{
	int i;
	 
	for(i=0;i<g->numNodes;i++)
	{
		if(isConflicting(g,i+1,adjColors))
		{
			conflicting[i]=TRUE;
// 			printf("c%d ",i+1);
		}
		else
		{
			conflicting[i]=FALSE;
		}
	}
	
	return conflicting;
}

boolean *getOnlyNewConflictingNodes(boolean *preConflicting,boolean *postConflicting,int numNodes,boolean *conflicting)
{
	int i;

	for(i=0;i<numNodes;i++)
	{
		if(preConflicting[i]==FALSE && postConflicting[i]==TRUE)
		{
			conflicting[i]=TRUE;
// 			printf("n%d ",i+1);
		}
		else
		{
			conflicting[i]=FALSE;
		}
	}
	return conflicting;
}

int countOnlyNewConflictingNodes(boolean *newConflicting, int numNodes)
{
	int i,count;
	
	count=0;
	for(i=0;i<numNodes;i++)
	{
		if(newConflicting[i]==TRUE)
			count++;
	}
	
	return count;
}

void randomConflictingColor(Graph *g, int numColors, int **adjColors)
{
	Node *n;
// 	srand((unsigned int) time(NULL));
  
	n=firstNodesList(g->nodesList);
  
// 	printf("Random coloring the conflicting nodes graph\n");
  
	while(!endNodesList(n,g->nodesList))
	{
		if(isConflicting(g,n->id,adjColors))
			n->color=(rand()%numColors);
//     printf("%d<=(%d)\n",n->id,n->color);
		n=nextNodesList(n);
	}
}

/**
Build the adjacency matrix from the Graph starting status
*/
void buildAdjacency(Graph *g,int **adjColors,int numColors)
{
	int i,j;
  Node *n;
  pNode *pn;
  
	
	for(i=0;i<g->numNodes;i++)
		for(j=0;j<numColors;j++)
			adjColors[i][j]=0;
	
	
  n=firstNodesList(g->nodesList);
  
	//For each node
  while(!endNodesList(n,g->nodesList))
  {
    pn=firstpNodesList(n->adj);
    
		//For each adjacent node
    while(!endpNodesList(pn,n->adj))
    {
			//Increment column with same color of current visiting node
      adjColors[n->id-1][pn->n->color]++;
      pn=nextpNodesList(pn);
    } 
    n=nextNodesList(n);
  }
}

/**
Update the adjacency matrix from the current move and Graph status 
*/
void updateAdjacency(Graph *g, int **adjColors, oneMove *move, int numColors)
{
  Node *n;
  pNode *pn;
  
  n=getNodeFromList(move->id,g->nodesList);
  
  pn=firstpNodesList(n->adj);
  
	//For each adjacent node update the adjacency matrix
  while(!endpNodesList(pn,n->adj))
  {
		//decrement column with same old color
    adjColors[(pn->n->id)-1][move->color]--; 
		//increment column with same new color
    adjColors[(pn->n->id)-1][move->bestNew]++;
    pn=nextpNodesList(pn);
  }
}

void updateAdjacencyTabu(Graph *g, int **adjColors, int id, int color, int newColor, int numColors)
{
  Node *n;
  pNode *pn;
  
  n=getNodeFromList(id,g->nodesList);
  
  pn=firstpNodesList(n->adj);
  
  while(!endpNodesList(pn,n->adj))
  {
    adjColors[(pn->n->id)-1][color]--;
    adjColors[(pn->n->id)-1][newColor]++;
    pn=nextpNodesList(pn);
  }
}

void printAdjacency(int **adjColors,Graph *g,int numColors)
{
  Node *n;
  int i,j;
  
  for(i=0;i<g->numNodes;i++)
  {
    n=getNodeFromList(i+1,g->nodesList);
    printf("%d(%d,c:%d): ",n->id,n->color,adjColors[i][n->color]);
    for(j=0;j<numColors;j++)
    {
      printf("%d ",adjColors[i][j]);
    }
    printf("\n");
  }
}

/**
Obj Function: Return the number of conflicting nodes in the Graph
*/
int nodesConflicting(NodesList *nl, int **adjColors, int numColors)
{
  Node *n;
  int cc; //Count conflicts
  
  cc=0;
  n=firstNodesList(nl);

	//For each node
  while(!endNodesList(n,nl))
  {	
		//Increment the obj func if there are almost
		//one adjacent node of the same color
    if(adjConflicting(n,adjColors)>0)
      cc++;
    n=nextNodesList(n);
  }
  
  return cc;
}

/**
Return the conflicting number of nodes for the node
*/
int adjConflicting(Node *n, int **adjColors)
{
	return adjColors[n->id-1][n->color];
}

/**
Return boolean value, if the node is ajacent to a node with the same color
*/
boolean isConflicting(Graph *g, int id, int **adjColors)
{
  Node *n;
  
  n=getNodeFromList(id,g->nodesList);

  if(adjColors[n->id-1][n->color]>0)	
		return TRUE; //if the adjacency for the node at his color is greater than 0
  else
    return FALSE;
}

void printConflictingNodesList(Graph *g,int **adjColors)
{
	Node *n;

	n=firstNodesList(g->nodesList);

	while(!endNodesList(n,g->nodesList))
	{
		if(isConflicting(g,n->id,adjColors))
		{
			printf("%d(%d)\n",n->id,n->color);
// 			printpNodesList(n->adj);
		}
		n=nextNodesList(n);
	}
}

oneMove *findBest1Move(Graph *g, int **adjColors, int **tabuList, int numColors, oneMove *move, int fixLong, float propLong, int nIt, int nC, int bestNc)
{
  int i,best,profit;
  oneMove bestMove;
  boolean chosen;
  Node *n;
  
  best=+1000;
  bestMove.id=1;
  bestMove.color=0;
  bestMove.bestNew=1;
  
  chosen=FALSE;
  
  n=firstNodesList(g->nodesList);
	//For each node
  while(!endNodesList(n,g->nodesList))
  {	//If the node is conflicting
    if(isConflicting(g,n->id,adjColors))
    {	//For each possible coloring
      for(i=0;i<numColors;i++)
      {	//Except current color
        if(i!=n->color)
        {	//If the move isn't tabu
          if(!isTabu(g,adjColors,n->id,i,tabuList,nIt,numColors,nC,bestNc))
          {	//Save the best 1-move
            if((profit=moveProfit(adjColors,n,i,numColors))<best)
            {
//               printf("%d(%d=>%d):%d\n",n->id,n->color,i,profit);
              chosen=TRUE;
              best=profit;
              bestMove.id=n->id;
              bestMove.color=n->color;
              bestMove.bestNew=i;
            }
          }
        }
      }
    }
    n=nextNodesList(n);
  }
	
  if(chosen==TRUE)
  {	//Return the best 1-move
    move->id=bestMove.id;
    move->color=bestMove.color;
    move->bestNew=bestMove.bestNew;
  }
  else
  {	//Return information that no valid move is finded
    move->id=-1;
    move->color=bestMove.color;
    move->bestNew=bestMove.bestNew;
  }
  return move;
}

oneMove *findBestNodeMove(Graph *g, int id,int **adjColors, int numColors, oneMove *move)
{
	int i,best,profit;
	Node *n;
	
	best=10000;
	n=getNodeFromList(id,g->nodesList);
	
// 	printf("Find best for node %d\n",id);
	
	for(i=0;i<numColors;i++)
	{	//Except current color
		if(i!=n->color)
		{	//Save the best 1-move
			if((profit=moveProfit(adjColors,n,i,numColors))<best)
			{
//         printf("%d(%d=>%d):%d\n",n->id,n->color,i,profit);
				best=profit;
				move->id=n->id;
				move->color=n->color;
				move->bestNew=i;
			}
		}
	}
	
	return move;
}

oneMove *findRandomSA1Move(Graph *g,int **adjColors, int numColors, oneMove *move)
{
	int id,newColor;
	Node *n;
	
	//Select random node
	id=(rand()%(g->numNodes-1))+1;
	//The selected node must be conflicting
	while(!isConflicting(g,id,adjColors))
	{
		id=(rand()%(g->numNodes-1))+1;	
	}
		
	n=getNodeFromList(id,g->nodesList);

	//Select random color not equal from the current color's node
	newColor=rand()%numColors;
	while(newColor==n->color)
	{
		newColor=rand()%numColors;
	}

	move->id=id;
	move->bestNew=newColor;
	move->color=n->color;
	return move;
}

/**
Function that returns the profit given by a 1-move
 
Note. in the current implementation the profit is not equal to the number of
conflicting nodes gained
*/
int moveProfit(int **adjColors, Node *n, int newColor, int numColors)
{
  int profit;
	
	profit=0;
	//Subtract the profit given by leaving the current color: is the number of 
	//conflicting nodes that can be lost.
  profit=-adjColors[n->id-1][n->color];
//   printf("profit[%d(%d)]=%d\n",n->id,n->color,profit);

	//Sum the loss given by entering in new class color: is the number of new
	//conflicting nodes that it will be owned
  profit+=adjColors[n->id-1][newColor];
//   printf("profit[%d(%d)]= +%d\n",n->id,newColor,adjColors[n->id-1][newColor]);
  
	return profit;
}

boolean isTabu(Graph *g,int **adjColors, int id, int color, int **tabuList, int nIt, int numColors, int nC, int bestNc)
{
  int profit;
// 	int tempColor;
  Node *n;
  
  if(tabuList[id-1][color] != 0)
  {
    if(nIt>tabuList[id-1][color])
		{	// Tabu expired
      return FALSE;
    }
    else
    {	// Tabu enable
      
			//thrown enhanced aspiration criterion: first version that work on 
			//the number of real conflicting nodes lost
      {
//         n=getNodeFromList(id,g->nodesList);
//         tempColor=n->color;
//         n->color=color;
//         updateAdjacencyTabu(g,adjColors,id,tempColor,color,numColors);
// 				nC=nodesConflicting(g->nodesList,adjColors,numColors);
// 
//         if(nC<bestNc)
//         {
//           n->color=tempColor;
//           updateAdjacencyTabu(g,adjColors,id,color,tempColor,numColors);
// //           printf("Activated tabu aspiration criterion:(node %d: %d=>%d)->conflict:%d!\n",id,color,n->color,nC);
//           return FALSE;
//         }
//         else
//         {
//           n->color=tempColor;
//           updateAdjacencyTabu(g,adjColors,id,color,tempColor,numColors);
// //           printf("Not Find (c:%d), roll back (%d=>%d)!\n",nC,color,n->color);
//           return TRUE; 
//         }
      }
				
			//thrown enhanced aspiration criterion: second version that work on
			//the profit and not on the number of real conflicting nodes lost
			
			//Seem to works better the second implementation
			
			{
				n=getNodeFromList(id,g->nodesList);
				profit=moveProfit(adjColors,n,color,numColors);
				if(profit+nC<bestNc)
				{
// 					printf("Activated tabu aspiration criterion:(node %d: %d=>%d) -> conflict:%d+%d=%d<%d!\n",id,n->color,color,profit,nC,profit+nC,bestNc);
					return FALSE; 
				}
				else
				{
					return TRUE;
				}
			}
			
			//       printf("tabu(it%d):id%d(%d)=tabu since %d\n",nIt,id,color,tabuList[id-1][color]);
      return TRUE;
    }
  }
  else
  { //NEVER done
    return FALSE;
  }
}

/**
Set the tabu time for a given move on couple node-Color
*/
int setTabu(Graph *g,int **adjColors, int **tabuList, int numColors, oneMove *move, int fixLong, float propLong, int nIt)
{
  int propValue;
  
  propValue = floor(propLong*(nodesConflicting(g->nodesList,adjColors,numColors)));
  
	tabuList[move->id-1][move->color]=nIt+fixLong+propValue;
	
//   printf("SetT(%d,%d)=%d+%d+%d=%d\n",move->id,move->bestNew,nIt,fixLong,propValue,tabuList[move->id-1][move->bestNew]);
	return tabuList[move->id-1][move->color];
	
}

int greedyColor(Graph *g)
{
	int i,id,iMax,minColor;
	boolean colored;
	pNode *orderNode;
	Node *n;
	
	orderNode=(pNode *)calloc(g->numNodes,sizeof(pNode));
	if(orderNode==NULL)
	{
		printf("Not enough memory to allocate greedy list\n");
		exit(EXIT_MEMORY);	
	}
	
	n=firstNodesList(g->nodesList);
	
	while(!endNodesList(n,g->nodesList))
	{	
		orderNode[n->id-1].n=n;
// 		printf("Greedy n %d: %dadj\n",orderNode[n->id-1].n->id,orderNode[n->id-1].n->numAdj);
		n=nextNodesList(n);
	}
	
	iMax=0;
	for(i=0;i<g->numNodes;i++)
	{
		id=getGreedyMaxOrder(g,orderNode);
// 		printf("Max:%d\n",id);
		colored=FALSE;
		minColor=0;
		
		while(!colored)
		{
			if(colorAdjFree(orderNode[id-1].n,minColor))
			{
				orderNode[id-1].n->color=minColor;
				colored= TRUE;
			}
			else
			{
				minColor++;
			}
		}
		
		if(minColor>iMax)
			iMax=minColor;
	}
		
	return iMax+1;
}

int greedyInitColor(Graph *g,int numColors)
{
	int i,id,iMax,minColor;
	boolean colored;
	pNode *orderNode;
	Node *n;
	
	orderNode=(pNode *)calloc(g->numNodes,sizeof(pNode));
	if(orderNode==NULL)
	{
		printf("Not enough memory to allocate greedy list\n");
		exit(EXIT_MEMORY);	
	}
	
	n=firstNodesList(g->nodesList);
	
	while(!endNodesList(n,g->nodesList))
	{	
		orderNode[n->id-1].n=n;
// 		printf("Greedy n %d: %dadj\n",orderNode[n->id-1].n->id,orderNode[n->id-1].n->numAdj);
		n=nextNodesList(n);
	}
	
// 	srand((unsigned int)time(NULL));
	iMax=0;
	for(i=0;i<g->numNodes;i++)
	{
		id=getGreedyMaxOrder(g,orderNode);
// 		printf("%d-",id);
		colored=FALSE;
		minColor=0;
		
		while(!colored && minColor <=numColors)
		{
			if(colorAdjFree(orderNode[id-1].n,minColor))
			{
				orderNode[id-1].n->color=minColor;
				colored= TRUE;
			}
			else
			{
				minColor++;
			}
			
			if(minColor==numColors)
			{
				orderNode[id-1].n->color=rand()%numColors;
				colored= TRUE;
			}
		}
	}
		
	return 0;
}

int getGreedyMaxOrder(Graph *g,pNode *orderNode)
{
	int i,ordMax,idOrdMax;
	
	ordMax=0;
	idOrdMax=1;
	
	for(i=0;i<g->numNodes;i++)
	{
		if(orderNode[i].n->color ==-1)
		{
			if(orderNode[i].n->numAdj > ordMax)
			{
// 				printf("Node %d\n",orderNode[i].n->id);
				ordMax=orderNode[i].n->numAdj;
				idOrdMax=i+1;
			}
		
		}
	}
	
	return idOrdMax;
}

boolean colorAdjFree(Node *n,int color)
{
	pNode *pn;
	
	pn=firstpNodesList(n->adj);

	while(!endpNodesList(pn,n->adj))
	{
		if(pn->n->color == color)
			return FALSE;
		pn=nextpNodesList(pn);
	}

	return TRUE;
}

int getNodeMaxOrder(Graph *g)
{
	int max;
	Node *n;
	
	n=firstNodesList(g->nodesList);
	max=0;
	
	while(!endNodesList(n,g->nodesList))
	{
		if(n->numAdj > max)
		{
			max=n->numAdj;
		}
		n=nextNodesList(n);
	}
	
	return max;
}

void waitEnter(char *message)
{
	char c;
	
	printf("%s",message);
	while((c=getchar())!='\n')
	{
		printf("%c ",c);
	}
}

void printSeparator()
{
	printf("---------------------------------------------------\n");
}

