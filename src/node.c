#include "../inc/node.h"

Node *createNode(int id)
{
  Node *node;
  node = (Node *)malloc(sizeof(Node));
  if(node == NULL)
  {
    printf("Not enough memory to allocate node %d!\n",id);
    exit(EXIT_MEMORY);
  }

  node->id = id;
  node->color = 0; //Default value for color
  node->next = NULL;
  node->prev = NULL;
  node->adj = (pNodesList *) createpNodesList();

  return node;
}


