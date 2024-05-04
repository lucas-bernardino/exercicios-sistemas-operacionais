#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct list_t {
  double value;
  struct list_t *next;
} list;

typedef struct head_t {
  size_t size;
  list *list;
} head;

// Inserts a value if the list is empty;
static void insert_empty_list(head *list_header, double val) {
  list *new_node = (list *)malloc(sizeof(list));
  if (!new_node) {
    printf("Error allocating memory with malloc!\n");
    exit(1);
  }
  new_node->value = val;
  new_node->next = NULL;
  list_header->list = new_node;
  list_header->size++;
  return;
}

void insert_last(head *list_header, double val) {
  list *tmp = list_header->list;
  if (!tmp) {
    insert_empty_list(list_header, val);
    return;
  }
  while (tmp->next) {
    tmp = tmp->next;
  }
  list *new_node = (list *)malloc(sizeof(list));
  if (!new_node) {
    printf("Error allocating memory with malloc!\n");
    exit(1);
  }
  new_node->value = val;
  new_node->next = NULL;
  tmp->next = new_node;
  list_header->size++;
}

void insert_first(head *list_header, double val) {
  list *tmp = list_header->list;
  if (!tmp) {
    insert_empty_list(list_header, val);
    return;
  }
  list *node = (list *)malloc(sizeof(list));
  if (!node) {
    printf("Error allocating memory with malloc!\n");
    exit(1);
  }
  node->value = val;
  list_header->list = node;
  node->next = tmp;
  list_header->size++;
}

void remove_first(head *list_header) {
  list *tmp = list_header->list;
  if (!tmp) {
    printf("List already empty.\n");
    return;
  }
  list_header->list = tmp->next;
  free(tmp);
}

head *initialize_list(size_t size, double default_value) {
  head *list_header = (head *)malloc(sizeof(head));
  if (!list_header) {
    printf("Error allocating memory with malloc!\n");
    exit(1);
  }
  list_header->list = NULL;
  for (size_t i = 0; i < size; i++) {
    insert_last(list_header, default_value);
  }
  return list_header;
}

int main() {
  printf("Will it work?\n");
  head *header = initialize_list(1, 69);
  insert_last(header, 9);
  insert_first(header, 1);
  printf("Value 1: %f\n", header->list->value);
  printf("Value 2: %f\n", header->list->next->value);
  printf("Value 3: %f\n", header->list->next->next->value);
  // printf("Size: %lru\n", header->size);
  remove_first(header);
  printf("After remove_first\n");
  printf("Value 1: %f\n", header->list->value);
  printf("Value 2: %f\n", header->list->next->value);
  head *header2 = initialize_list(0, 0);
  remove_first(header2);
  /* OUTPUT:
  Value 1: 1.000000
  Value 2: 69.000000
  Value 3: 9.000000
  Size: 3
   */
  return 0;
}
