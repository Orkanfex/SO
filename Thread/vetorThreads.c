#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

int *vet;
int n;
void *runner(void *param);

int main(int argc, char const *argv[])
{
    pthread_t tid[4];
    pthread_attr_t attr;
    int sum = 0;
    int indice[4] = {0, 1, 2, 3};

    if (argc != 2)
    {
        printf("Use: ./vetorThreasds N\n");
        printf("onde N é o tamanho do vetor\n");
        return -1;
    }

    if (atoi(argv[1]) < 4)
    {
        printf("O tamanho minimo do vetor é 4");
        return -1;
    }

    n = atoi(argv[1]);
    vet = (int*) malloc(sizeof(int) * (atoi(argv[1])) );
    pthread_attr_init(&attr);

    for (int i = 0; i < n; i++)
    {
        scanf("%d", &vet[i]);
    }

    for (int i = 0; i < 4; i++)
    {
        pthread_create(&tid[i], &attr, runner, &indice[i]);
    }

    for (int i = 0; i < 4; i++)
    {
        pthread_join(tid[i], NULL);
    }

    for (int i = 0; i < 4; i++)
    {
        sum += indice[i];
        printf("indice[%d] = %d\n", i, indice[i]);
    }
    

    printf("Somatoria = %d\n", sum);
    
    return 0;
}

void *runner(void *param){
    int indice = *(int*)(param);
    int sum;

    printf("indice %d\n", indice);

    if (indice == 0)
    {
        for (int i = 0; i < n/4; i++)
            sum += vet[i];
        *(int*) param = sum;
        printf("t0 = %d\n", sum);
    }else{
        if (indice == 1)
        {
            for (int i = n/4; i < n/2; i++)
                sum += vet[i];
            *(int*) param = sum;
            printf("t1 = %d\n", sum);
            
        }else{
            if (indice == 2)
            {
                for (int i = n/2; i < (n/2)+(n/4); i++)           
                    sum += vet[i];
                *(int*) param = sum;
                printf("t2 = %d\n", sum);
                
            }else{
                for (int i = (n/2)+(n/4); i < n; i++)
                    sum += vet[i];
                *(int*) param = sum;
                printf("t3 = %d\n", sum);
            }
        }
    }
    pthread_exit(0);
}
