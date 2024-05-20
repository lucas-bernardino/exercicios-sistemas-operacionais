#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define INSERT_FIRST 1
#define INSERT_LAST 2
#define REMOVE_FIRST 3
#define REMOVE_LAST 4

pthread_mutex_t lock;

sem_t sem_prod;
sem_t sem_cons;

int elem_id = 0;

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
  l->value = l->header->list->value;
  l->header->list = tmp->next;
  free(tmp);
  l->header->size--;
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
  if (!tmp) {
    printf("List already empty.\n");
    return NULL;
  }
  while (tmp->next->next) {
    tmp = tmp->next;
  }
  list *tmp_free = tmp->next;
  tmp->next = NULL;
  l->value = tmp_free->value;
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

void produzir(int thread_id, my_list *l) {

  pthread_mutex_lock(&lock);
  int meu_elemento = elem_id;
  elem_id++;
  pthread_mutex_unlock(&lock);

  sem_wait(&sem_prod);

  printf("[%d]Produzindo %d...", thread_id, meu_elemento);
  usleep(333000);
  insert_last(l);
  printf("[%d]Produzido", thread_id);

  sem_post(&sem_cons);
}

void consumir(int thread_id, my_list *l) {
  printf("[%d]Consumindo...", thread_id);
  sem_wait(&sem_cons);

  remove_first(l);
  usleep(500000);
  printf("[%d]Consumido %f", thread_id, l->value);

  sem_post(&sem_prod);
}

int main() {
  pthread_t threads[6];

  pthread_mutex_init(&lock, NULL);
  head *list = initialize_list(4, 5);

  sem_init(&sem_prod, 0, 2);
  sem_init(&sem_cons, 0, 2);

  my_list *p1 = (my_list *)malloc(sizeof(my_list));
  p1->header = list;
  p1->value = 10;
  p1->function_type = INSERT_FIRST;

  pthread_create(&threads[0], NULL, &handle_threads_function, p1);
  pthread_join(threads[1], NULL);

  printf("Printing after insertion: \n");
  print_list(list);

  free(p1);

  printf("Program exit sucessfully\n");

  return 0;
}
