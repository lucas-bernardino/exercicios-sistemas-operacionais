# Descrição do problema

### Use threads para acelerar o cálculo de Pi
Considere o código abaixo que calcula PI com [Série de Leibniz](https://pt.wikipedia.org/wiki/F%C3%B3rmula_de_Leibniz_para_%CF%80) usando 5 bilhões de termos. Utilize pthreads para acelerar sua execução:

- Dica: Mantenha a função calculo_pi, apenas passe como parâmetro um novo intervalo para o for. Mantenha a variável sum local na função. Realize os joins e some o retorno de todas as funções.