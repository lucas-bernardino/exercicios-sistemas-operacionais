#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
  for (long int i = p->quantity; i < (p->quantity + 2500000000L); i++) {
    double sinal = i % 2 == 0 ? 1.0 : -1.0;
    sum += sinal / (2.0 * i + 1.0);
  }
  p->res = sum * 4;
}

int main() {

  pthread_t threads[2];

  long int QUANTITY = 5000000000L;

  double half = QUANTITY / 2.0;

  params *p1 = (params *)malloc(sizeof(params));
  params *p2 = (params *)malloc(sizeof(params));

  p1->quantity = 0L;
  p2->quantity = half;

  p1->res = 0L;
  p1->res = 0L;

  pthread_create(&threads[0], NULL, &calc, p1);
  pthread_create(&threads[1], NULL, &calc, p2);

  pthread_join(threads[0], NULL);
  pthread_join(threads[1], NULL);

  printf("P1 res: %.20lf\nP2 res: %.20lf\n", p1->res, p2->res);
  printf("Sum P1 + P2: %.20lf\n", p1->res + p2->res);

  free(p1);
  free(p2);
}
