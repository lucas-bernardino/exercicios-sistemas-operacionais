#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const long int QUANTITY = 5000000000L;
const double half = QUANTITY / 2.0;

pthread_mutex_t lock;

typedef struct params {
  long int quantity;
  double res;
} params;

double calculo_pi(long int n) {
  double sum = 0;
  for (long int i = 0; i < n; i++) {
    double sinal = i % 2 == 0 ? 1.0 : -1.0;
    sum += sinal / (2.0 * i + 1.0);
  }
  return (sum * 4);
}

void *calc(void *arg) {

  params *p = (params *)arg;

  double sum = 0;
  for (long int i = p->quantity; i < (p->quantity + half); i++) {
    pthread_mutex_lock(&lock);
    double sinal = i % 2 == 0 ? 1.0 : -1.0;
    sum += sinal / (2.0 * i + 1.0);
    p->res = sum;
    pthread_mutex_unlock(&lock);
  }
  p->res = sum * 4;
}

int main() {

  pthread_t threads[2];

  double half = QUANTITY / 2.0;

  params *p = (params *)malloc(sizeof(params));

  p->quantity = 0L;

  pthread_mutex_init(&lock, NULL);

  pthread_create(&threads[0], NULL, &calc, p);
  pthread_create(&threads[1], NULL, &calc, p);

  pthread_join(threads[0], NULL);
  pthread_join(threads[1], NULL);

  printf("Result: %.20lf\n", p->res);

  free(p);
}
