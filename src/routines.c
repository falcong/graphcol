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

void readConfFile(int *nRestart,int *maxIt,int *fixLong,float *propLong)
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
                  printf("Problem: %s(n:%d, e:%d)\n",format,numNodes,numEdges);
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
boolean findTabu(Graph *g, int numColors, int fixLong, float propLong, int maxIt, int *stopIt, int ***adj)
{
  int **tabuList;
  int **adjColors;
  int i,j,nIt,nC,bestNc,oldC,tabuT,endIt;
  oneMove *move;
  Node *n;

  //Build tabu list,adjacency matrix & 1-move 
  {
    tabuList = (int **) calloc(sizeof(int *),g->numNodes);
    if(tabuList == NULL)
		{
      printf("Not enough memory to allocate tabu list\n");
      exit(EXIT_MEMORY);
    }
    
		if(*adj == NULL)
		{
			adjColors = (int **) calloc(sizeof(int *),g->numNodes);
			if(adjColors == NULL)
			{
				printf("Not enough memory to allocate adjacency matrix\n");
				exit(EXIT_MEMORY);
			}
		}
		else
		{
			adjColors = *adj;
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
  buildAdjacency(g,adjColors);

  nIt=0;
  nC=nodesConflicting(g->nodesList,adjColors,numColors);
	bestNc=nC;
  printf("Number of conflicting nodes:%d\n",nC);
	endIt=maxIt;
	
  while(nC > 0 && nIt < endIt)
  {
    nIt++;
		//Find the best 1-move 
    move=findBest1Move(g,adjColors,tabuList,numColors,move,fixLong,propLong,nIt,nC,bestNc);
    if(move->id==-1)
    {
      printf("It%d\t(conf:%d):\tNo available moves!\tSkip to next iteration!\n",nIt,nC);
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
		}
				
		printf("It%d/%d\t(conf:%d,best:%d):\tNode\t%d (%d=>%d)\tsetTabu(%d it)\n",nIt,endIt,nC,bestNc,move->id,move->color,move->bestNew,tabuT-nIt);
  }
  
//   printAdjacency(adjColors,g,numColors);
	*adj=adjColors;
  *stopIt=nIt;
  return bestNc;
}

int findSA(Graph *g,int numColors,int *stopIt,int ***adj, float startTemp, float tempFactor)
{
	int **adjColors;
	int i,j,nIt,nC,bestNc,profit;
	float currentTemp;
	double exponent,dice;
	oneMove *move;
	Node *n;
	
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
		
	//Init the adjacency matrix with the solution values
	buildAdjacency(g,adjColors);
	
	nC=nodesConflicting(g->nodesList,adjColors,numColors);
	printf("Number of conflicting nodes:%d\n",nC);
	
	nIt=0;
	currentTemp=startTemp;
	
	srand(1000);
	
	while(nC > 0 && nIt < 10)
	{
		nIt++;
		
		//Generate random move 
		move=findRandomSA1Move(g,adjColors,numColors,move);
		
		n=getNodeFromList(move->id,g->nodesList);
		
		//Check if the move is valid
		profit=moveProfit(adjColors,n,move->bestNew,numColors);
		printf("profit(%d: %d=>%d):%d ",move->id,move->color,move->bestNew,profit);
		
		if(profit<=0)
		{	//Downhill move
			//Do the move
			printf(" Ok\n");
		}
		else
		{	//Uphill move
			dice=fmod(drand48(),2);	
			exponent=-profit/currentTemp;
			
			printf("dice:%.2f < exp(%.2f):%.2f (T:%.2f)?",dice,exponent,exp(exponent),currentTemp);
			
			if(dice<exp(exponent))
			{
				printf(" Yes\n");
			}
			else
			{
				printf(" No\n");
				continue;
			}
		}

		//Do the 1-move
		move->color=n->color;
		n->color=move->bestNew;
		
// 		printf("It%d\t(conf:%d):\tNode\t%d (%d=>%d)\n",nIt,nC,move->id,move->color,move->bestNew);
		
    //Update adjacency matrix
		updateAdjacency(g,adjColors,move,numColors);
		
		//Update the obj function
		nC=nodesConflicting(g->nodesList,adjColors,numColors);
		
		//Update temperature value
		currentTemp=tempFactor*currentTemp;
				
		printf("It%d\t(conf:%d):\tNode\t%d (%d=>%d)\n",nIt,nC,move->id,move->color,move->bestNew);
	}
	
	*adj=adjColors;
	*stopIt=nIt;
	return nC;
}

/**
Random color the graph with numColors colors
*/
void randomColor(Graph *g, int numColors)
{
  Node *n;
  srand((unsigned int) time(NULL));
  
  n=firstNodesList(g->nodesList);
  
  printf("Random coloring the nodes graph\n");
  
  while(!endNodesList(n,g->nodesList))
  {
    n->color=(rand()%numColors);
//     printf("%d<=(%d)\n",n->id,n->color);
    n=nextNodesList(n);
  }
}

void randomConflictingColor(Graph *g, int numColors, int **adjColors)
{
	Node *n;
	srand((unsigned int) time(NULL));
  
	n=firstNodesList(g->nodesList);
  
	printf("Random coloring the conflicting nodes graph\n");
  
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
void buildAdjacency(Graph *g,int **adjColors)
{
  Node *n;
  pNode *pn;
  
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

oneMove *findRandomSA1Move(Graph *g,int **adjColors, int numColors, oneMove *move)
{
	int id,newColor;
	Node *n;
	
	//Select random node
	id=(rand()%(g->numNodes-1))+1;
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
*/
int moveProfit(int **adjColors, Node *n, int newColor, int numColors)
{
  int profit;
	//Subtract the profit given by leaving the current color: is the number of 
	//conflicting nodes that it will be lost.
  profit=-adjColors[n->id-1][n->color];
//   printf("profit[%d(%d)]=%d\n",n->id,n->color,profit);

	//Sum the loss given by entering in new class color: is the number of 
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
      
			  //thrown basic aspiration criterion
			
/*      {
        n=getNodeFromList(id,g->nodesList);
        tempColor=n->color;
        n->color=color;
        updateAdjacencyTabu(g,adjColors,id,tempColor,color,numColors);

        if((nC=nodesConflicting(g->nodesList,adjColors,numColors))==0)
        {
          n->color=tempColor;
          updateAdjacencyTabu(g,adjColors,id,color,tempColor,numColors);
          printf("Activated tabu aspiration criterion:(node %d: %d=>%d)->conflict:%d!\n",id,color,n->color,nC);
          return FALSE;
        }
        else
        {
          n->color=tempColor;
          updateAdjacencyTabu(g,adjColors,id,color,tempColor,numColors);
//           printf("Not Find (c:%d), roll back (%d=>%d)!\n",nC,color,n->color);
          return TRUE; 
        }
      }*/
				
			//thrown enhanced aspiration criterion
			
			{
				n=getNodeFromList(id,g->nodesList);
				profit=moveProfit(adjColors,n,color,numColors);
				if(profit+nC<bestNc)
// 				if(profit+nC==0)
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
		printf("Max:%d\n",id);
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
	
	srand((unsigned int)time(NULL));
	iMax=0;
	for(i=0;i<g->numNodes;i++)
	{
		id=getGreedyMaxOrder(g,orderNode);
		printf("%d-",id);
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
