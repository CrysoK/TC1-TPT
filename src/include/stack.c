#include "stack.h"
#include <stdlib.h>
#include "sds/sds.h"
#include "debug.h"

struct StackNode *new_node(char *);

struct StackNode *new_node(char *data) {
  struct StackNode *node = NULL;
  node = (struct StackNode*)malloc(sizeof(struct StackNode));
  node->data = sdsnew(data);
  node->next = NULL;
  return node;
}

bool is_empty(struct StackNode *root) {
  return root == NULL ? true : false;
}

void push(struct StackNode **root, char *data) {
  struct StackNode *node = new_node(data);
  node->next = *root;
  *root = node;
  DBG_log("<?> PUSH \"%s\"\n", data);
}

char *pop(struct StackNode **root) {
  if(is_empty(*root)) return NULL;
  struct StackNode *temp = *root;
  *root = (*root)->next;
  DBG_log("<?> POP \"%s\"\n", temp->data);
  return temp->data;
}

char * peek(struct StackNode *root) {
  if(is_empty(root)) return NULL;
  return root->data;
}
