#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define INSERT_FIRST 1
#define INSERT_LAST 2
#define REMOVE_FIRST 3
#define REMOVE_LAST 4

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
  head *header;
  double value;
  int function_type;
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
  list *tmp = list_header->list;
  while (tmp) {
    printf("Value stored: %f\n", tmp->value);
    tmp = tmp->next;
  }
}

void insert_last(void *arg) {
  pthread_mutex_lock(&lock);
  my_list *l = (my_list *)arg;
  if (!l->header) {
    printf("Invalid operation: head must not be null");
    exit(1);
  }
  list *tmp = l->header->list;
  if (!tmp) {
    insert_empty_list(l);
    pthread_mutex_unlock(&lock);
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
  pthread_mutex_unlock(&lock);
}

void *insert_first(void *arg) {
  pthread_mutex_lock(&lock);
  my_list *l = (my_list *)arg;
  if (!l->header) {
    printf("Invalid operation: head must not be null");
    exit(1);
  }
  list *tmp = l->header->list;
  if (!tmp) {
    insert_empty_list(l);
    pthread_mutex_unlock(&lock);
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

void *remove_first(void *arg) {
  pthread_mutex_lock(&lock);
  my_list *l = (my_list *)arg;
  if (!l->header) {
    printf("Invalid operation: head must not be null");
    exit(1);
  }
  list *tmp = l->header->list;
  if (!tmp) {
    printf("List already empty.\n");
    return NULL;
  }
  l->header->list = tmp->next;
  free(tmp);
  pthread_mutex_unlock(&lock);
}

void *remove_last(void *arg) {
  pthread_mutex_lock(&lock);
  my_list *l = (my_list *)arg;
  if (!l->header) {
    printf("Invalid operation: head must not be null");
    exit(1);
  }
  list *tmp = l->header->list;
  while (tmp->next->next) {
    tmp = tmp->next;
  }
  list *tmp_free = tmp->next;
  tmp->next = NULL;
  free(tmp_free);
  l->header->size--;
  pthread_mutex_unlock(&lock);
}

head *initialize_list(size_t size, double default_value) {
  my_list *l = (my_list *)malloc(sizeof(my_list));
  if (!l) {
    printf("Error allocating memory with malloc!\n");
    exit(1);
  }
  l->header = (head *)malloc(sizeof(head));
  l->value = default_value;
  for (size_t i = 0; i < size; i++) {
    insert_last(l);
  }
  return l->header;
}

void *handle_threads_function(void *arg) {
  my_list *p = (my_list *)arg;
  switch (p->function_type) {
  case INSERT_FIRST:
    insert_first(p);
    break;
  case INSERT_LAST:
    insert_last(p);
    break;
  case REMOVE_FIRST:
    remove_first(p);
    break;
  case REMOVE_LAST:
    remove_last(p);
    break;
  }
  return NULL;
}

int main() {
  pthread_t threads[6];

  pthread_mutex_init(&lock, NULL);
  head *list = initialize_list(4, 5);

  my_list *p1 = (my_list *)malloc(sizeof(my_list));
  p1->header = list;
  p1->value = 10;
  p1->function_type = INSERT_FIRST;
  my_list *p2 = (my_list *)malloc(sizeof(my_list));
  p2->header = list;
  p2->value = 20;
  p2->function_type = INSERT_FIRST;
  my_list *p3 = (my_list *)malloc(sizeof(my_list));
  p3->header = list;
  p3->value = 30;
  p3->function_type = INSERT_LAST;
  my_list *p4 = (my_list *)malloc(sizeof(my_list));
  p4->header = list;
  p4->value = 0;
  p4->function_type = REMOVE_FIRST;
  my_list *p5 = (my_list *)malloc(sizeof(my_list));
  p5->header = list;
  p5->value = 0;
  p5->function_type = REMOVE_FIRST;
  my_list *p6 = (my_list *)malloc(sizeof(my_list));
  p6->header = list;
  p6->value = 0;
  p6->function_type = REMOVE_LAST;
  pthread_create(&threads[0], NULL, &handle_threads_function, p1);
  pthread_create(&threads[1], NULL, &handle_threads_function, p2);
  pthread_create(&threads[2], NULL, &handle_threads_function, p3);
  pthread_join(threads[0], NULL);
  pthread_join(threads[1], NULL);
  pthread_join(threads[2], NULL);

  printf("Printing after insertion: \n");
  print_list(list);

  pthread_create(&threads[3], NULL, &handle_threads_function, p4);
  pthread_create(&threads[4], NULL, &handle_threads_function, p5);
  pthread_create(&threads[5], NULL, &handle_threads_function, p6);
  pthread_join(threads[3], NULL);
  pthread_join(threads[4], NULL);
  pthread_join(threads[5], NULL);
  printf("Printing all elements fater removing stuff.: \n");
  print_list(list);

  printf("Freeing memory\n");
  free(p1);
  free(p2);
  free(p3);
  free(p4);
  free(p5);
  free(p6);
  free(list);

  printf("Program exit sucessfully\n");

  return 0;
}
