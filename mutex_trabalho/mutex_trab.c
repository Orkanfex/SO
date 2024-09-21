#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct node
{
    int num;
    pthread_mutex_t mutex;
    struct node *next;
};

typedef struct node LISTA;

LISTA *minha_lista = NULL;

pthread_mutex_t inicio;
pthread_mutex_t naoPrimo;
pthread_mutex_t inicioPrim;

void *remove_pares(void*);
void *remove_Nao_Primo(void*);
void *thread_print(void*);

int openFile(FILE **file, const char *name, const char *op){
    if (((*file) = fopen(name, op)) == NULL)
        return 0;
    
    fseek(*file, 0, SEEK_SET);
    return 1;    
}

void remover(LISTA *node){
    if (node)
    {
        pthread_mutex_destroy(&node->mutex);
        free(node);
    }    
}

int isPrime(int number){
    int i, isPrime = 1;

    if (number <= 1)
        isPrime = 0;
    else if (number <= 3)
        isPrime = 1;
    else if (number % 2 == 0 || number % 3 == 0)
        isPrime = 0;

    for (i = 5; i * i <= number; i += 6)
    {
        if (number % i == 0 || number % (i + 2) == 0)
        {
            isPrime = 0;
            break;
        }
    }

    return isPrime;
}

int main(int argc, char const *argv[])
{
    pthread_t tid[3];
    FILE *file = NULL;
    int valor;

    LISTA *end;

    

    pthread_mutex_init(&inicio, NULL);
    pthread_mutex_lock(&inicio);
    pthread_mutex_init(&naoPrimo, NULL);
    pthread_mutex_lock(&naoPrimo);
    pthread_mutex_init(&inicioPrim, NULL);
    pthread_mutex_lock(&inicioPrim);

    if (argc != 2)
    {
        printf("Use: ./%s <arquivo.txt>", argv[0]);
    }
    
    if (!(openFile(&file, argv[1], "r")))
        return 1;
    
    pthread_create(&tid[0], NULL, remove_pares, NULL);
    pthread_create(&tid[1], NULL, remove_Nao_Primo, NULL);
    pthread_create(&tid[2], NULL, thread_print, NULL);

    while (!feof(file))
    {
        fscanf(file,"%d", &valor);

        if (!minha_lista)
        {
            minha_lista = malloc(sizeof(LISTA));
            minha_lista->num = valor;
            minha_lista->next = NULL;
            pthread_mutex_init(&minha_lista->mutex, NULL);
            pthread_mutex_lock(&minha_lista->mutex);

            end = minha_lista;
            
            pthread_mutex_unlock(&inicio);
        }else{
            LISTA *newNode, *pAux;
            newNode = malloc(sizeof(LISTA));
            newNode->num = valor;
            newNode->next = NULL;
            pthread_mutex_init(&newNode->mutex, NULL);
            pthread_mutex_lock(&newNode->mutex);
            
            pAux = end;
            end->next = newNode;
            end = end->next;

            pthread_mutex_unlock(&pAux->mutex);

        }
        
    }

    pthread_mutex_unlock(&end->mutex);
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);

    pthread_mutex_destroy(&inicio);
    pthread_mutex_destroy(&naoPrimo);
    pthread_mutex_destroy(&inicioPrim);
    
    return 0;
}

void *remove_pares(void *param){

    LISTA *anterior, *atual;
    anterior = NULL;

    pthread_mutex_lock(&inicio);
     
    atual = minha_lista;
    int i = 0;

    while (atual)
    {
        pthread_mutex_lock(&atual->mutex);
        
        if(!(atual->num %2) && atual->num > 2){
            if(anterior == NULL){
                minha_lista = atual->next;
                remover(atual);
                atual = minha_lista;
            }else{
                anterior->next = atual->next;
                remover(atual);
                atual = anterior->next;
            }
        }else{
            LISTA *pAux;
            
            if(anterior == NULL){
                anterior = atual;
                atual = atual->next;
                pthread_mutex_unlock(&naoPrimo);
            }else{
                pAux = anterior;
                anterior = atual;
                atual = atual->next;
                
                pthread_mutex_unlock(&pAux->mutex);                            
            }            
        }
    }

    pthread_mutex_unlock(&inicio);
    pthread_mutex_unlock(&anterior->mutex);
    pthread_exit(0);
}

void *remove_Nao_Primo(void *param){

    LISTA *anterior, *atual;
    anterior = NULL;
    
    pthread_mutex_lock(&naoPrimo);
    atual = minha_lista;

    while(atual){
        pthread_mutex_lock(&atual->mutex);
        
        if(!isPrime(atual->num)){
            if(anterior == NULL){
                minha_lista = atual->next;
                remover(atual);
                atual = minha_lista;
            }else{
                anterior->next = atual->next;
                remover(atual);
                atual = anterior->next;
            }
        }else{
            LISTA *pAux;
            if (anterior == NULL){
                anterior = atual;
                atual = atual->next;

                pthread_mutex_unlock(&inicioPrim);
            }else{
                pAux = anterior;
                anterior = atual;
                atual = atual->next;

                pthread_mutex_unlock(&pAux->mutex);
            }
        }
    }
    // printf("primos terminou/ ");

    pthread_mutex_unlock(&naoPrimo);
    pthread_mutex_unlock(&anterior->mutex);
    pthread_exit(0);
}

void *thread_print(void *param){

    pthread_mutex_lock(&inicioPrim);
    LISTA *printLista = minha_lista;
    
    while (printLista)
    {
        pthread_mutex_lock(&printLista->mutex);
        printf("%d ", printLista->num);
        pthread_mutex_unlock(&printLista->mutex);
        printLista = printLista->next;
    }

    pthread_mutex_unlock(&inicioPrim);
    pthread_exit(0);
}