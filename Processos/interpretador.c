#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

void splitCommandLine(char *newArgv[256], char command[256], int *newArgc){
    int i = 0;

    newArgv[i] = strtok(command, " ");

    while (newArgv[i] != NULL){
        i++;
        newArgv[i] = strtok(NULL, " ");
    }
    
    *newArgc = i;
} 

int main(int argc, char const *argv[])
{
    pid_t pid;
    char command[256], *newargv[256], *envp[] = {"PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin",NULL};
    int len, newArgc, loop=1;

    while (loop)
    {
        fflush(stdin);
        fgets(command, sizeof(command), stdin);
        
        len = strlen(command) - 1;
        command[len] = '\0';

        splitCommandLine(newargv, command, &newArgc);
        
        if (!strcmp(command, "exit")){
            loop = 0;
        }else{
            pid = fork();

            if (pid < 0){
                printf("Erro: fork falhou"); 
            }else if (pid == 0){
                if (!strcmp(newargv[newArgc-1], "&"))//caso encontre o &
                    newargv[newArgc-1] = NULL;// remova da string para passar para o execve()
                
                if(execve(newargv[0], newargv, envp)){ //caso execute nao retorna nada, caso falhe retorna -1
                    printf("Erro: execve() falhou\n");
                    exit(0);
                }
            }else{
                if (strcmp(newargv[newArgc-1], "&")){ // caso tenha nao encontre o & 
                    wait(NULL); // aguarda o filho terminar a execução
                }
            }
        }
        
    }
    
    return 0;
}
