#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char* argv[], char* envp[]) {

  for (;;) {
    char argument[15];
    printf("Your argument: ");
    fgets(argument, sizeof(argument), stdin);
    
    argument[strcspn(argument, "\n")] = 0; // usado para remover o \n do argument

    if (argument == NULL || strlen(argument) == 1) {
      printf("ERROR: missing process argument\n");
      return 1;
    }
  
    if (strcmp(argument, "exit") == 0) {
      printf("Exiting...\n");
      return 0;
    }

    FILE* process = fopen(argument, "r");

    if (!process) {
      printf("ERROR: could not open process\n");
      return 1;
    }

    pid_t PID = fork();

    if (PID == 0) { 
      execle(argument, NULL, envp);
    }

    int status;
    waitpid(PID, &status, 0);

  }
}
