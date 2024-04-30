#include <stdio.h>

double calculo_pi(long int n) {
  double sum = 0;
  for (long int i = 0; i < n; i++) {
    double sinal = i % 2 == 0 ? 1.0 : -1.0;
    sum += sinal / (2.0 * i + 1.0);
  }
  return (sum * 4);
}

int main() {
  double pi = calculo_pi(2500000000L);
  printf("Valor de PI: %.20lf\n", pi);
}
