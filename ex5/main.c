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
  int thread_id;
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

void *produzir(void *arg) {

  my_list *p = (my_list *)arg;
  pthread_mutex_lock(&lock);
  if (elem_id == 20) {
    printf("Alcançou limite de producao. \nSaindo...\n");
    exit(0);
  }
  int meu_elemento = elem_id;
  elem_id++;
  pthread_mutex_unlock(&lock);

  sem_wait(&sem_prod);
  printf("[%d]Produzindo %d...\n", p->thread_id, meu_elemento);
  usleep(333000);
  insert_last(p);
  printf("[%d]Produzido\n", p->thread_id);

  sem_post(&sem_cons);
}

void *consumir(void *arg) {
  my_list *p = (my_list *)arg;
  printf("[%d]Consumindo...\n", p->thread_id);
  sem_wait(&sem_cons);

  remove_first(p);
  usleep(500000);
  printf("[%d]Consumido %f\n", p->thread_id, p->value);

  sem_post(&sem_prod);
}

/*
 - Utilize semáforos POSIX (sem_post / sem_wait) para proteger a utilização da
lista (produzir somente se a lista tem espaço, consumir somente se a fila tem
elementos)

- Implemente um programa que inicialize a lista com no máximo i elementos, n
produtores e m consumidores, e produza no máximo x itens.

Escreva um pequeno relatório
- Apresentado a implementação e decisões
- Mostrando a saída com fila de tamanho 4, 2 produtores, 2 consumidores, 20
itens, e tempo de execução.
*/

int main() {
  pthread_t threads[50];

  pthread_mutex_init(&lock, NULL);
  head *list = initialize_list(4, 0);

  sem_init(&sem_prod, 0, 2);
  sem_init(&sem_cons, 0, 2);

  for (int i = 0; i < 50; i++) {
    my_list *p0 = (my_list *)malloc(sizeof(my_list));
    p0->header = list;
    p0->value = 10;
    p0->thread_id = i;

    my_list *p1 = (my_list *)malloc(sizeof(my_list));
    p1->header = list;
    p1->value = 10;
    p1->thread_id = i;

    pthread_create(&threads[0], NULL, &produzir, p0);
    pthread_create(&threads[1], NULL, &consumir, p1);
  }

  return 0;
}
