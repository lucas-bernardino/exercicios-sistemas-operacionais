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

typedef struct my_list_t {
  head* header;
  double value;
} my_list;

// Inserts a value if the list is empty;
static void insert_empty_list(head *list_header, double val) {
  if (!list_header) {
    printf("Invalid operation: head must not be null");
    exit(1);
  }
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

static void print_list(head *list_header) {
  if (!list_header) {
    printf("Invalid operation: head must not be null");
    exit(1);
  }
  list* tmp = list_header->list;
  while (tmp) {
    printf("Value stored: %f\n", tmp->value);
    tmp = tmp->next;
  }
}

void insert_last(head *list_header, double val) {
  if (!list_header) {
    printf("Invalid operation: head must not be null");
    exit(1);
  }
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

void *insert_first(void *arg) {
  my_list *l = (my_list *) arg;
  if (!l->header) {
    printf("Invalid operation: head must not be null");
    exit(1);
  }
  list *tmp = l->header->list;
  if (!tmp) {
    insert_empty_list(l->header, l->value);
    return;
  }
  list *node = (list *)malloc(sizeof(list));
  if (!node) {
    printf("Error allocating memory with malloc!\n");
    exit(1);
  }
  node->value = l->value;
  l->header->list = node;
  node->next = tmp;
  l->header->size++;

  return NULL;
}

void remove_first(head *list_header) {
  if (!list_header) {
    printf("Invalid operation: head must not be null");
    exit(1);
  }
  list *tmp = list_header->list;
  if (!tmp) {
    printf("List already empty.\n");
    return;
  }
  list_header->list = tmp->next;
  free(tmp);
}

void remove_last(head *list_header) {
  if (!list_header) {
    printf("Invalid operation: head must not be null");
    exit(1);
  }
  list *tmp = list_header->list;
  while (tmp->next->next) {
    tmp = tmp->next;
  }
  list* tmp_free = tmp->next;
  tmp->next = NULL;
  free(tmp_free);
  list_header->size--;
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
  pthread_t threads[1];

  my_list* list = (my_list *)malloc(sizeof(my_list));
  list->header = initialize_list(4, 5);
  list->value = 10;

  pthread_create(&threads[0], NULL, &insert_first, list);

  pthread_join(threads[0], NULL);

  print_list(list->header);  
  return 0;
}
