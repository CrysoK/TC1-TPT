#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

// ESTRUCTURA //////////////////////////////////////////////////////////////////

struct StackNode {
  char * data;
  struct StackNode* next;
};

// OPERACIONES /////////////////////////////////////////////////////////////////

bool  is_empty(struct StackNode *);
void  push(struct StackNode **, char *);
char *pop(struct StackNode **);
char *peek(struct StackNode *);

// Ejemplo:
// struct StackNode* root = NULL;
// push(&root, 10);
// push(&root, 20);
// push(&root, 30);
// printf("%d popped from stack\n", pop(&root));
// printf("Top element is %d\n", peek(root));

#endif
