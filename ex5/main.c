#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define INSERT_FIRST 1
#define INSERT_LAST 2
#define REMOVE_FIRST 3
#define REMOVE_LAST 4

#define CONSUMIDORAS 3

pthread_mutex_t lock;

sem_t sem_prod;
sem_t sem_cons;

int elem_id = 0;
int count_consm = 0;

bool canFinish = false;
bool lastThread = false;

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
  int thread_id;
} my_list;

// Inserts a value if the list is empty;
static void insert_empty_list(void *arg) {
  my_list *l = (my_list *)arg;

  if (!l->header) {
    printf("Invalid operation: head must not be null");
    exit(1);
  }
  list *new_node = (list *)calloc(1, sizeof(list));
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
  list *new_node = (list *)calloc(1, sizeof(list));
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
  list *node = (list *)calloc(1, sizeof(list));
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
  if (l->header->size == 0) {
    l->header->list = NULL;
  }
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
  my_list *l = (my_list *)calloc(1, sizeof(my_list));
  if (!l) {
    printf("Error allocating memory with malloc!\n");
    exit(1);
  }
  l->header = (head *)calloc(1, sizeof(head));
  l->value = default_value;
  l->header->list = NULL;
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

void *produzir(void *arg) {

  for (;;) {
    my_list *p = (my_list *)arg;
    pthread_mutex_lock(&lock);
    if (elem_id == 20) {
      printf("Alcançou limite de producao. Thread [%d] produtora saindo.\n",
             p->thread_id);
      canFinish = true;
      pthread_mutex_unlock(&lock);
      return NULL;
    }
    int meu_elemento = elem_id;
    elem_id++;
    pthread_mutex_unlock(&lock);
    sem_wait(&sem_prod);
    printf("[%d]Produzindo %d...\n", p->thread_id, meu_elemento);
    usleep(500000);
    p->value = meu_elemento;
    insert_last(p);
    printf("[%d]Produzido\n", p->thread_id);
    sem_post(&sem_cons);
  }
}

void *consumir(void *arg) {
  for (;;) {
    my_list *p = (my_list *)arg;
    pthread_mutex_lock(&lock);
    if (canFinish) {
      printf("Alcançou limite de producao. Thread [%d] consumidora saindo.\n",
             p->thread_id);
      count_consm++;
      pthread_mutex_unlock(&lock);
      break;
    }
    pthread_mutex_unlock(&lock);
    printf("[%d]Consumindo...\n", p->thread_id);
    sem_wait(&sem_cons);
    pthread_mutex_lock(&lock);
    if (lastThread) {
      printf("Alcançou limite de producao. Thread [%d] consumidora saindo.\n",
             p->thread_id);
      return NULL;
    }
    pthread_mutex_unlock(&lock);
    usleep(333000);
    remove_first(p);
    printf("[%d]Consumido %f\n", p->thread_id, p->value);
    sem_post(&sem_prod);
  }
  pthread_mutex_lock(&lock);
  if (count_consm == (CONSUMIDORAS - 1)) {
    sem_post(&sem_cons);
    lastThread = true;
  }
  pthread_mutex_unlock(&lock);
  return NULL;
}

int main() {
  pthread_t threads[5];

  pthread_mutex_init(&lock, NULL);
  head *list = initialize_list(0, 0);

  sem_init(&sem_prod, 0, 4);
  sem_init(&sem_cons, 0, 0);

  my_list *p0 = (my_list *)calloc(1, sizeof(my_list));
  p0->header = list;
  p0->value = 10;
  p0->thread_id = 0;

  my_list *p1 = (my_list *)calloc(1, sizeof(my_list));
  p1->header = list;
  p1->value = 10;
  p1->thread_id = 1;

  my_list *p2 = (my_list *)calloc(1, sizeof(my_list));
  p2->header = list;
  p2->value = 10;
  p2->thread_id = 2;

  my_list *p3 = (my_list *)calloc(1, sizeof(my_list));
  p3->header = list;
  p3->value = 10;
  p3->thread_id = 3;

  my_list *p4 = (my_list *)calloc(1, sizeof(my_list));
  p4->header = list;
  p4->value = 10;
  p4->thread_id = 4;

  pthread_create(&threads[0], NULL, &produzir, p0);
  pthread_create(&threads[1], NULL, &produzir, p1);

  pthread_create(&threads[2], NULL, &consumir, p2);
  pthread_create(&threads[3], NULL, &consumir, p3);
  pthread_create(&threads[4], NULL, &consumir, p4);

  pthread_join(threads[0], NULL);
  pthread_join(threads[1], NULL);
  pthread_join(threads[2], NULL);
  pthread_join(threads[3], NULL);
  pthread_join(threads[4], NULL);

  printf("Fim.\n");

  return 0;
}
