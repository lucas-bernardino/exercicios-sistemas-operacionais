# Descrição do problema

### Considere as seguintes funções de produzir e consumir em anexo.

- Utilize semáforos POSIX (sem_post / sem_wait) para proteger a utilização da lista (produzir somente se a lista tem espaço, consumir somente se a fila tem elementos)

- Implemente um programa que inicialize a lista com no máximo i elementos, n produtores e m consumidores, e produza no máximo x itens.

### Escreva um pequeno relatório
- Apresentado a implementação e decisões
- Mostrando a saída com fila de tamanho 4, 2 produtores, 2 consumidores, 20 itens, e tempo de execução.
- Mostrando a saída com fila de tamanho 4, 2 produtores, 3 consumidores, 20 itens, e tempo de execução.
