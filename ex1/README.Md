
# Descrição do problema

### Crie seu próprio terminal
1. Seu terminal aceita uma string como entrada.
2. Se for a string "exit" termine o programa.
3. Para qualquer outra string, considere que é um programa que o usuário deseja executar.
4. Verifique se o programa existe (fopen retorna nulo se o arquivo não exite), caso não exista, imprima um erro e peça ao usuário um novo comando.
5. Caso exista, você deve executar este programa.
    1. Para realizar isso com a interface posix, é necessário abrir um novo processo com fork(),
    2. e mudar somente o conteúdo do processo filho com execle.
    * execle recebe o caminho do executável, argumentos (null), e ambiente (utilize o envp do seu main).
    * Utilize o seguinte main: int main(int argc, char *argv[], char *envp[])
6. Faça o seu terminal (processo original) esperar o processo filho com waitpid(pid, &status, 0);
*    Sendo pid o retorno do fork, e status um novo inteiro.
7. Após a execução do comando, peça um novo comando ao usuário.

