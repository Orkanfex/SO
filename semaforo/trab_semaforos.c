#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>

#define SEM1 "/sem1"
#define SEM2 "/sem2"
#define SEM3 "/sem3"
#define SEM4 "/sem4"

struct node
{
    int value;
    struct node *next;
};

typedef struct node LIST;

LIST *l1 = NULL, *l2 = NULL, *l3 = NULL;

sem_t *sem_1, *sem_2, *sem_3, *sem_4;

void *nao_pares(void*);
void *primos(void*);
void *printPrimos(void*);

int openFile(FILE **file, const char *name, const char *op){
    if (!((*file) = fopen(name,op)))
        return 0;

    fseek(*file, 0, SEEK_SET);
    return 1;
}

void freeList(LIST **list){
    
    if ((*list))
    {
        freeList(&(*list)->next);
        free(*list);
        (*list) = NULL;
    }
    
    
}
void printSimples(LIST *list){
    while (list)
    {
        printf("|%d| -> ", list->value);
        list = list->next;
    }
    
}

int main(int argc, char const *argv[])
{
    pthread_t tid[3];
    FILE *file;
    LIST *newNode, *end;
    int val = 0;
    char ch;

    // if (argc!= 2){
    //     printf("Use: %s <arquivo>\n", argv[0]);
    //     return 1;
    // }

    if (!(openFile(&file, argv[1], "r")))
        return 1;    

    sem_unlink(SEM1);
    sem_unlink(SEM2);
    sem_unlink(SEM3);

    sem_1 = sem_open(SEM1, O_CREAT, 0644, 0); //cria o semaforo
    sem_2 = sem_open(SEM2, O_CREAT, 0644, 0); //cria o semaforo
    sem_3 = sem_open(SEM3, O_CREAT, 0644, 0); //cria o semaforo

    if (!sem_1)
    {
        printf("erro no sem");
        return 1;
    }
    
    //cria as tres threads
    pthread_create(&tid[0], NULL,nao_pares, NULL);
    pthread_create(&tid[1], NULL,primos, NULL);
    pthread_create(&tid[2], NULL,printPrimos, NULL);

    while (!feof(file))
    {
        // char bufferRead[10] = "";
        // int i = 0;
        
        // while ((ch != ' ') && (ch != '\n'))
        // {
        //     bufferRead[i] = ch;
        //     i++;
        //     ch = getc(file);
        // }
        fscanf(file, "%d", &val);
        
        if(l1 == NULL){
            l1 = malloc(sizeof(struct node));
            l1->value = val;//atoi(bufferRead);
            l1->next = NULL;          
            end = l1;
        }else{
            newNode = malloc(sizeof(struct node));
            newNode->value = val;//atoi(bufferRead);
            newNode->next = NULL;
            end->next = newNode;
            end = newNode;
        }

        sem_post(sem_1);
        //ch = getc(file);
    }

    sem_post(sem_1);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);

    freeList(&l1);
    freeList(&l2);
    freeList(&l3);

    sem_close(sem_1);
    sem_close(sem_2);
    sem_close(sem_3);

    sem_unlink(SEM1);
    sem_unlink(SEM2);
    sem_unlink(SEM3);

    fclose(file);
    return 0;
}

void *nao_pares(void *param){//devo receber uma lista como parametro?
    LIST *ptr, *newNode, *end;

    sem_wait(sem_1);
    ptr = l1;

    do
    {
        if (((ptr->value % 2) != 0) && (ptr->value > 2))
        {
            if (l2 == NULL)
            {
                l2 = malloc(sizeof(struct node));
                l2->value = ptr->value;
                l2->next = NULL;
                end = l2;
            }else{
                newNode = malloc(sizeof(struct node));
                newNode->value = ptr->value;
                newNode->next = NULL;
                end->next = newNode;
                end = newNode;
            }
            sem_post(sem_2);
        }
        sem_wait(sem_1);
        ptr= ptr->next;
    } while(ptr);

    sem_post(sem_2);
    pthread_exit(0);
}

void *primos(void *param){
    LIST *ptr, *newNode, *end;

    sem_wait(sem_2); 
    ptr = l2;

    do
    {
        int divs = 0;

        for (int i = 1; i <= ptr->value; i++)
            if (!(ptr->value % i))
                divs++;

        if (divs == 2)
        {
            if (l3 == NULL)
            {
                l3 = malloc(sizeof(struct node));
                l3->value = ptr->value;
                l3->next = NULL;
                end = l3;
            }else{
                newNode = malloc(sizeof(struct node));
                newNode->value = ptr->value;
                newNode->next = NULL;
                end->next = newNode;
                end = newNode;

            }
            sem_post(sem_3);
        }
        sem_wait(sem_2); 
        ptr = ptr->next;
        
    } while(ptr);
    
    sem_post(sem_3);
    pthread_exit(0);
}

void *printPrimos(void *param){
    LIST *ptr;

    sem_wait(sem_3);
    ptr = l3;

    do
    {
        
        printf("%d ", ptr->value);
        
        sem_wait(sem_3);
        ptr = ptr->next;

    }while(ptr);
    
    printf("\n");
    pthread_exit(0);
}