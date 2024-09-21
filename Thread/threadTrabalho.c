#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

int *vet;
long nThreads;
int n;

void *soma(void *param);

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
    pthread_t *tid;
    pthread_attr_t attr;
    FILE *file;
    int sum = 0;
    long *indice;
    long i;

    if (argc != 3)
    {
        printf("Use: ./threads <nomeArquivo> <numeroThreads>\n");
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
        printf("    com <numeroThreads> menor ou igual a %d\n", sizeFileInt(file));
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
    
    nThreads = atoi(argv[2]);
    tid = (pthread_t*) malloc(sizeof(pthread_t) * nThreads);
    if (!tid)
    {
        printf("Erro de alocacao do tid\n");
        free(vet);
        fclose(file);
        return -1;
    }

    indice = (long*) malloc(sizeof(long) * nThreads);
    if (!indice){
        printf("Erro de alocacao do indice\n");
        free(vet);
        free(tid);
        fclose(file);
        return -1;
    }

    for (i = 0; i < nThreads; i++)
    {
        indice[i] = i;
    }
    
    n = sizeFileInt(file);
    fread(vet, sizeof(int), n, file);
    fclose(file);
    
    pthread_attr_init(&attr);

    for (i = 0; i < nThreads; i++)
    {
        pthread_create(&tid[i], &attr, soma, &indice[i]);
    }

    for (i = 0; i < nThreads; i++)
    {
        pthread_join(tid[i], NULL);
    }

    for (i = 0; i < nThreads; i++)
    {
        sum += indice[i];
    }
    free(vet);
    free(indice);
    free(tid);
    printf("%d\n", sum);

    return 0;
}

void *soma(void *param){
    long ind = *(long*)(param);
    long nt, nn;
    nt = nThreads;
    nn = n;

    int sum = 0;

        //for (long j = indice*n/nThreads; j < (indice+1)*n/nThreads; j++)
        for (long j = ind*nn/nt; j < (ind+1)*nn/nt; j++)
        {
            sum += vet[j];
        }
        *(long*) param = sum;
    pthread_exit(0);
}