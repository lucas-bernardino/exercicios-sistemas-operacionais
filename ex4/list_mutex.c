#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

pthread_mutex_t lock;

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
static void insert_empty_list(void *arg) {
  my_list *l = (my_list *)arg;

  if (!l->header) {
    printf("Invalid operation: head must not be null");
    exit(1);
  }
  list *new_node = (list *)malloc(sizeof(list));
  if (!new_node) {
    printf("Error allocating memory with malloc!\n");
    exit(1);
  }
  new_node->value = l->value;
  new_node->next = NULL;
  l->header->list = new_node;
  l->header->size++;
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

void insert_last(void *arg) {
  my_list *l = (my_list *)arg;

  if (!l->header) {
    printf("Invalid operation: head must not be null");
    exit(1);
  }
  list *tmp = l->header->list;
  if (!tmp) {
    insert_empty_list(l);
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
  new_node->value = l->value;
  new_node->next = NULL;
  tmp->next = new_node;
  l->header->size++;
}

void *insert_first(void *arg) {
  pthread_mutex_lock(&lock);
  my_list *l = (my_list *) arg;
  if (!l->header) {
    printf("Invalid operation: head must not be null");
    exit(1);
  }
  list *tmp = l->header->list;
  if (!tmp) {
    insert_empty_list(l);
    return NULL;
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
  pthread_mutex_unlock(&lock);
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

my_list* initialize_list(size_t size, double default_value) {
  my_list* l = (my_list *)malloc(sizeof(my_list));
  if (!l) {
    printf("Error allocating memory with malloc!\n");
    exit(1);
  }
  printf("Got here 1.");
  l->header = (head *)malloc(sizeof(head));
  printf("Got here 2.");
  for (size_t i = 0; i < size; i++) {
    insert_last(l);
  }
  return l;
}

int main() {
  pthread_t threads[3];

  my_list* list = initialize_list(4, 5);
  list->value = 10;

  pthread_mutex_init(&lock, NULL);

  pthread_create(&threads[0], NULL, &insert_first, list);
  pthread_create(&threads[1], NULL, &insert_first, list);
  pthread_create(&threads[2], NULL, &insert_first, list);
  pthread_join(threads[0], NULL);
  pthread_join(threads[1], NULL);
  pthread_join(threads[2], NULL);


  print_list(list->header);
  return 0;
}
