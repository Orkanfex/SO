#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/shm.h>
#include <fcntl.h> 
#include <sys/mman.h>

int *vet;
long nProcess;
int n;

void soma(void *param);

int openFile(FILE **fp, char *name, char *op){
    if (((*fp) = fopen(name, op)) == NULL)
    {
        return 0;
    }
    
    fseek((*fp), 0, SEEK_SET);
    return 1;
}

int sizeFileInt(FILE *fp){
    int size;

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    size = size/sizeof(int);
    fseek(fp, 0, SEEK_SET);
    return size;
}

int main(int argc, char const *argv[])
{
    pid_t pid;
    FILE *file;
    void *indice;
    char *name = "indice";
    int sum = 0;
    long i;
    int shm_fd;

    if (argc != 3)
    {
        printf("Use: ./processoTrabalho <nomeArquivo> <numeroProcessos>\n");
        return -1;
    }


    if (!openFile(&file, (char*)argv[1], "rb"))
    {
        printf("Nao foi possivel abrir o arquivo %s\n", argv[1]);
        return -1;
    }

    if (atoi(argv[2]) > sizeFileInt(file))
    {
        printf("Use: ./threads %s <numeroThreads>\n", argv[1]);
        printf("    com <numeroProcessos> menor ou igual a %d\n", sizeFileInt(file));
        fclose(file);
        return -1;
    }
    
    vet = malloc(sizeof(int)*sizeFileInt(file));
    if (!vet)
    {
        puts("Erro de alocacao do vetor\n");
        fclose(file);
        return -1;
    }
    
    nProcess = atoi(argv[2]);
    n = sizeFileInt(file);
    
    fread(vet, sizeof(int), n, file);
    fclose(file);
    
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);//criar area de memoria comprtilhada para leitura e escrita
    ftruncate(shm_fd, sizeof(long) * nProcess);//limita o tamanho do objeto de area compartilhada
    indice = mmap(0, sizeof(long) * nProcess, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);//mapeia a area compartilhada
    
    for (i = 0; i < nProcess; i++)
    {
        ((long*)indice)[i] = i;//preenche o vetor de indices na area compartilhada com i ate i<nProcess
    }

    fflush(stdout);// limpa o buffer da saida padrao
    
    for (i = 0; i < nProcess; i++)
    {
        printf("%ld\n", i);
        pid = fork(); // cria um novo processo
        if (pid < 0){
            printf("Erro de fork\n");
            return -1;
        }else{
            if (pid== 0)
            {
                soma(&((long*)indice)[i]); // chama a funcao para somar um pedaco do vetor
                exit(0); // encerra o processo
            }else{
                wait(NULL); // aguarda os filhos
            }
        }
    }

    for (i = 0; i < nProcess; i++)
    {   
        sum += ((long*)indice)[i]; // soma os resultados retornados por cada processo filho
    }
    
    printf("%d\n", sum); // imprime o resultado
    
    free(vet);

    return 0;
}

void soma(void *param){
    long indice = *(long*)(param); //indice recebe o conteudo de (param) com um cast para (int*)
    int sum = 0;
    long nn = n;

        for (long j = indice*nn/nProcess; j < (indice+1)*nn/nProcess; j++)
        {
            sum += vet[j];//soma um pedaco do vetor de acordo com o indice atribuido
        }
        *(int*) param = sum;// param tem um cast para (int*), e aponta para o conteudo, onde *param = sum 
}