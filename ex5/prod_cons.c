#include <unistd.h>
int elem_id = 0;

void produzir(int thread_id, lista *l) {

  mutex_lock();
  int meu_elemento = elem_id;
  elem_id++;
  mutex_unlock();

  printf("[%d]Produzindo %d...", thread_id, meu_elemento);
  usleep(333000);
  insere_fim(l, meu_elemento);
  printf("[%d]Produzido", thread_id);
}
void consumir(int thread_id, lista *l) {
  printf("[%d]Consumindo...", thread_id);
  v = remove_inicio(l);
  usleep(500000);
  printf("[%d]Consumido %d", thread_id, v);
}
