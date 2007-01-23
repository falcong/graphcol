#include "../inc/routines.h"
#include <time.h>

/**
Function that read from the command line the name of the instance file
*/
void readCommand(int argc,char *argv[],char *instFile, int *pcolors)
{
  int colors;
  if(argc != 2 && argc != 3)
  {
    printf("Wrong command line!\n");
    printf("Use %s instance_file\n",argv[0]);
    exit(EXIT_WRONGCOMMANDLINE);
  }

  strcpy(instFile,argv[1]);

  if(argc > 2)
  {
    colors=atoi(argv[2]);
  }
  else
  {
    colors=MAXCOLORS;
  }

  *pcolors = colors;

#ifdef DEBUG
  printf("instance file: %s\n",instFile);
#endif

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
    printf("Impossibile aprire il file istanza\n");
    exit(EXIT_OPENFILE);
  }

  while(fgets(line,LUNGHEZZA,finst)!=NULL)
  {
//     printf("Riga %s\n",line);
    if (sscanf(line,"%c",&type) == 0)
    {
      printf("Formato del problema errato!\n");
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

  pf=fopen("out.dot","w");
  fprintf(pf,"strict graph G {\n");
  
  fprintf(pf,"ratio=1;\n");

  //Print nodes
  n=firstNodesList(l);
  while(!endNodesList(n,l))
  {
    fprintf(pf,"%d [color=",n->id);
    
    switch(n->color)
    {
      case 0:  fprintf(pf,"blue");
          break;
      case 1:  fprintf(pf,"red");
          break;
      case 2:  fprintf(pf,"green");
          break;
      case 3:  fprintf(pf,"yellow");
          break;
      default:fprintf(pf,"white");
    }
    
    fprintf(pf,"];\n");
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


boolean findTabu(Graph *g, int numColors, int fixLong, float propLong)
{
  int **tabuList;
  int **adjColors;
  int i,j,nIt,nC;
  
  //
  //Build tabu list & adjacency matrix
  //
  
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
  
  
  //   printAdjacency(adjColors,g->numNodes,numColors);
  //
  //Build the random initial solution
  //
  randomColor(g,numColors);
  
  //
  //Init the adjacency matrix with the solution values
  //
  buildAdjacency(g,adjColors);
  printAdjacency(adjColors,g->numNodes,numColors);
  
  nIt=0;
  nC=nodesConflicting(g->nodesList,adjColors,numColors);
  printf("%d\n",nC);
  
  while(nC > 0 || nIt < 1000)
  {
    nIt++;
  
    findBest1Move(adjColors,g->numNodes,numColors);
    nC=nodesConflicting(g->nodesList,adjColors,numColors);
  }
  
  if(nC==0)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

void randomColor(Graph *g, int numColors)
{
  Node *n;
  srand((unsigned int) time(NULL));
  
  n=firstNodesList(g->nodesList);
  
  printf("Random coloring the edges graph\n");
  
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

void printAdjacency(int **adjColors,int numNodes,int numColors)
{
  int i,j;
  
  for(i=0;i<numNodes;i++)
  {
    printf("%d: ",i+1);
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
  
  n=firstNodesList(nl);

  while(!endNodesList(n,nl))
  {
    cc+=adjConflicting(n->id,adjColors,numColors);
    n=nextNodesList(n);
  }
  
  return cc;
}

int adjConflicting(int node, int **adjColors, int numColors)
{
  int i,cac; //Count adjacent nodes conflicts
  
  cac=0;
  for(i=0;i<numColors;i++)
  {
    cac+=adjColors[node-1][i];
  }
  
  return cac;
}

oneMove *findBest1Move(int **adjColors,int numNodes, int numColors)
{
  oneMove *move;
  int i,j;
  
  move=(oneMove *) malloc(sizeof(oneMove));
  if(move == NULL)
  {
    printf("Not enough memory to allocate move!\n");
    exit(EXIT_MEMORY);
  }
  
  move->id=0;
  move->color=0;
  
  for(i=0;i<numNodes;i++)
  {
    for(j=0;j<numColors;j++)
    {
      if (adjColors[i][j])
    }
  }
  
  return move;
}
