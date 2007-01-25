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

  if(argc > 2)
  {
    colors=atoi(argv[2]);
    
    if(argc > 3)
    {
      if(sscanf(argv[3],"v%d",&verb)!=1)
        verb=DEFAULTVERB;
    }
    else
    {
      verb=DEFAULTVERB;
    }
  }
  else
  {
    colors=MAXCOLORS;
  }

  *pcolors = colors;
  *verbosity = verb;

#ifdef DEBUG
  printf("instance file: %s\n",instFile);
#endif

}

void readConfFile(int *maxIt,int *fixLong,float *propLong)
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
//                 printf("stringa: %s\n",row);
                if(sscanf(row,"MAXIT %d",maxIt)==1)
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
                    appendpNodesList(nW,nV->adj);
                  
                  if(!nodesIsInAdjList(nV->id,nW->adj))
                    appendpNodesList(nV,nW->adj);

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


boolean findTabu(Graph *g, int numColors, int fixLong, float propLong, int maxIt)
{
  int **tabuList;
  int **adjColors;
  int i,j,nIt,nC,oldC,tabuT;
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
  printAdjacency(adjColors,g,numColors);
  
  nIt=0;
  nC=nodesConflicting(g->nodesList,adjColors,numColors);
  printf("Number of conflicting nodes:%d\n",nC);
  
  while(nC > 0 && nIt < maxIt)
  {
    nIt++;
    
    move=findBest1Move(g,adjColors,tabuList,numColors,move,fixLong,propLong,nIt);
    
    n=getNodeFromList(move->id,g->nodesList);
    oldC=n->color;
    n->color=move->bestNew;

    tabuT=setTabu(g,adjColors,tabuList,numColors,move,fixLong,propLong,nIt);
    updateAdjacency(g,adjColors,move,numColors);
//     printAdjacency(adjColors,g,numColors);

    nC=nodesConflicting(g->nodesList,adjColors,numColors);
    
    if(nC==1)
    {
      printAdjacency(adjColors,g,numColors);
      exit(0);
    }
    
    printf("It%d\t(conf:%d):\tNode\t%d (%d=>%d)\tsetTabu(%d it)\n",nIt,nC,move->id,move->color,move->bestNew,tabuT-nIt);
  }
  
//   printAdjacency(adjColors,g,numColors);
  
  return nC;
}

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

void buildAdjacency(Graph *g,int **adjColors)
{
  Node *n;
  pNode *pn;
  
  n=firstNodesList(g->nodesList);
  
  while(!endNodesList(n,g->nodesList))
  {
    pn=firstpNodesList(n->adj);
    
    while(!endpNodesList(pn,n->adj))
    {
      adjColors[n->id-1][pn->n->color]++;
      pn=nextpNodesList(pn);
    } 
    n=nextNodesList(n);
  }
}

void updateAdjacency(Graph *g, int **adjColors, oneMove *move, int numColors)
{
  Node *n;
  pNode *pn;
  
  n=getNodeFromList(move->id,g->nodesList);
  
  pn=firstpNodesList(n->adj);
  
  while(!endpNodesList(pn,n->adj))
  {
    adjColors[(pn->n->id)-1][move->color]--;
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

int nodesConflicting(NodesList *nl, int **adjColors, int numColors)
{
  Node *n;
  int cc; //Count conflicts
  
  cc=0;
  n=firstNodesList(nl);

  while(!endNodesList(n,nl))
  {
    if(adjConflicting(n,adjColors)>0)
      cc++;
    n=nextNodesList(n);
  }
  
  return cc;
}

int adjConflicting(Node *n, int **adjColors)
{
  int acn; //Adjacent conflicting nodes
  
  acn=adjColors[n->id-1][n->color];

  return acn;
}

boolean isConflicting(Graph *g, int id, int **adjColors)
{
  Node *n;
  
  n=getNodeFromList(id,g->nodesList);

  if(adjColors[n->id-1][n->color]>0)
    return TRUE;
  else
    return FALSE;
}

oneMove *findBest1Move(Graph *g, int **adjColors, int **tabuList, int numColors, oneMove *move, int fixLong, float propLong, int nIt)
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
  
  while(!endNodesList(n,g->nodesList))
  {
//     printf("%d ",n->id);
    if(isConflicting(g,n->id,adjColors))
    {
//       printf("%d ",n->id);
      for(i=0;i<numColors;i++)
      {
        if(i!=n->color)
        {
          if(!isTabu(g,adjColors,n->id,i,tabuList,nIt,numColors))
          {
            if((profit=moveProfit(adjColors,n,i,numColors))<best)
            {
  //             printf("%d(%d=>%d):%d\n",n->id,n->color,i,profit);
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
  {
    move->id=bestMove.id;
    move->color=bestMove.color;
    move->bestNew=bestMove.bestNew;
  }
  else
  {
    move->id=-1;
    move->color=bestMove.color;
    move->bestNew=bestMove.bestNew;
  }
  
  return move;
}

int moveProfit(int **adjColors, Node *n, int newColor, int numColors)
{
  int profit;
  
  profit=-adjColors[n->id-1][n->color];
//   printf("profit[%d(%d)]=%d\n",n->id,n->color,profit);
  
  profit+=adjColors[n->id-1][newColor];
//   printf("profit[%d(%d)]= +%d\n",n->id,n->color,adjColors[n->id-1][newColor]);
  
  return profit;
}

boolean isTabu(Graph *g,int **adjColors, int id, int color, int **tabuList, int nIt, int numColors)
{
  int tempColor,nC;
  Node *n;
  
  if(tabuList[id-1][color] != 0)
  {
    if(nIt>tabuList[id-1][color])
    {
      // Tabu expired
      return FALSE;
    }
    else
    {
      // Tabu enable
      
      //thrown aspiration criterion
      {
        n=getNodeFromList(id,g->nodesList);
        tempColor=n->color;
        
//         printf("Check aspiration criterion (%d=>%d ?): ",tempColor,color);
        
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

int setTabu(Graph *g,int **adjColors, int **tabuList, int numColors, oneMove *move, int fixLong, float propLong, int nIt)
{
  int propValue;
  
  propValue = floor(propLong*(nodesConflicting(g->nodesList,adjColors,numColors)));
  
//   tabuList[move->id-1][move->bestNew]=nIt+(rand()%fixLong)+propValue;
  tabuList[move->id-1][move->bestNew]=nIt+fixLong+propValue;
//   printf("SetT(%d,%d)=%d+%d+%d=%d\n",move->id,move->bestNew,nIt,fixLong,propValue,tabuList[move->id-1][move->bestNew]);
  return tabuList[move->id-1][move->bestNew];
}

