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
  bool is_main_thread;
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
  if (!l->is_main_thread) {
    pthread_mutex_lock(&lock);
  }
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
  if (!l->is_main_thread) {
    pthread_mutex_unlock(&lock);
  }
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

void *remove_first(void *arg) {
  pthread_mutex_lock(&lock);
  my_list *l = (my_list *) arg;
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
  l->header = (head *)malloc(sizeof(head));
  l->value = default_value;
  l->is_main_thread = true;
  for (size_t i = 0; i < size; i++) {
    insert_last(l);
  }
  l->is_main_thread = false;
  return l;
}

int main() {
  pthread_t threads[6];

  my_list* list = initialize_list(4, 5);
  pthread_mutex_init(&lock, NULL);

  list->value = 10;
  pthread_create(&threads[0], NULL, &insert_first, list);
  pthread_join(threads[0], NULL);
  list->value = 20;
  pthread_create(&threads[1], NULL, &insert_first, list);
  pthread_join(threads[1], NULL);
  list->value= 30;
  pthread_create(&threads[2], NULL, &insert_first, list);
  pthread_join(threads[2], NULL);

  
  print_list(list->header);

  pthread_create(&threads[3], NULL, &remove_first, list);
  pthread_join(threads[3], NULL);
  
  printf("After removing the first element: \n");
  print_list(list->header);

  pthread_create(&threads[4], NULL, &remove_first, list);
  pthread_join(threads[4], NULL);

  printf("After removing the second element: \n");
  print_list(list->header);

  return 0;
}
