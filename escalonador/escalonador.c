#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct picos_cpu
{
    int pico_cpu;
    struct picos_cpu *next;
};
typedef struct picos_cpu picoCPU;

struct processo
{
    int id_proc;
    int prioridade;
    int chegada_inicial;
    int instante_submissao;
    picoCPU *picos;
    struct processo *next;
};
typedef struct processo Processo;

struct fila_processo
{
    int instante_atual;
    Processo *process;
};
typedef struct fila_processo FilaProcesso;

struct fila_prontos
{
    int instante_atual;
    Processo *ptProcesso;
    struct fila_prontos *next;
};
typedef struct fila_prontos FilaProntos;

int openFile(FILE **file, const char *path, char *op){
    if (((*file) = fopen(path, op)) == NULL)
    {
        return 0;
    }

    fseek((*file), 0, SEEK_SET);
    return 1;
}

void removePico(FilaProcesso **fila){
    picoCPU *aux, *pico;
    Processo *ptProc;
    if ((*fila))
    {
        ptProc = (*fila)->process;
        pico = ptProc->picos;
        if (ptProc->next)
        {
            if (pico->next)
            {
                aux = pico;
                pico = pico->next;
                free(aux);
                
                ptProc->instante_submissao = (*fila)->instante_atual + pico->pico_cpu;
                aux = pico;
                pico = pico->next;
                free(aux);

                ptProc->picos = pico;
            }else{
                free(pico);
                (*fila)->process = ptProc->next;
                free(ptProc);
                ptProc = NULL;
            }
        }else{
            if (pico->next)
            {
                aux = pico;
                pico = pico->next;
                free(aux);
                ptProc->picos = pico;
            }else{
                free(pico);
                free(ptProc);
                free(*fila);
                (*fila) = NULL;
            }
        }
    }    
}

void removePicoID(FilaProcesso *fila, int id){
    Processo *proc = fila->process;
    Processo *prev = NULL;

    while (proc->id_proc != id)
    {
        prev = proc;
        proc = proc->next;
    }

    picoCPU *pico = proc->picos;
    picoCPU *aux;

    if (pico->next)
    {
        aux = pico;
        pico = pico->next;
        free(aux);
        
        proc->instante_submissao = fila->instante_atual + pico->pico_cpu;
        aux = pico;
        pico = pico->next;
        free(aux);

        proc->picos = pico;
    }else{
        free(pico);
        prev->next = proc->next;
        free(proc);
        proc = NULL;
    }   
}

Processo* criarProcessos(char *buffer){
    char *pch = buffer;
    Processo *processo;
    picoCPU *picos, *previous;
    picos = NULL;

    processo = (Processo *) malloc(sizeof(Processo));
    
    pch = strtok(buffer, " ");
    processo->prioridade = atoi(pch);
    pch = strtok(NULL, " ");
    processo->chegada_inicial = atoi(pch);
    processo->instante_submissao= atoi(pch);
    pch = strtok(NULL, " ");

    while (pch)
    {   

        if (!picos)
        {
            picos = (picoCPU *) malloc(sizeof(picoCPU));
            picos->pico_cpu = atoi(pch);
            picos->next = NULL;
            processo->picos = picos;
            previous = picos;
        }else{
            picos = (picoCPU *) malloc(sizeof(picoCPU));
            picos->pico_cpu = atoi(pch);
            picos->next = NULL;
            previous->next = picos;
            previous = picos;
        }
        pch = strtok(NULL, " \0");
    }

    processo->next = NULL;
    return processo;    
}

void printProcesso(FilaProcesso *lista){
    Processo *PTprocesso = lista->process;
    while (PTprocesso)
    {
        printf("P%d:\n", PTprocesso->id_proc);
        printf("prioridade: %d\nchegada inicial: %d\n", PTprocesso->prioridade, PTprocesso->chegada_inicial);
        printf("instante de submissao: %d\n", PTprocesso->instante_submissao);
        picoCPU *PTpicos = PTprocesso->picos;
        printf("picos: ");
        while (PTpicos)
        {
            printf("%d ", PTpicos->pico_cpu);
            PTpicos = PTpicos->next;
        }
        printf("\n");
        printf("\n");
        PTprocesso = PTprocesso->next;

    }
    
}

void createFilaProcess(FilaProcesso **lista, char *buffer){
    if (!(*lista))
    {
        
        (*lista) = (FilaProcesso *) malloc(sizeof(FilaProcesso));   
        (*lista)->process = criarProcessos(buffer);
        (*lista)->process->id_proc = 1;
        (*lista)->instante_atual = 0;
    }else{
        int id_proc = 2;
        Processo *PTprocesso = (*lista)->process;
        Processo *newProcesso;
        
        while (PTprocesso->next)
        {
            PTprocesso = PTprocesso->next;
            id_proc++;
        }
        
        newProcesso = criarProcessos(buffer);
        newProcesso->id_proc = id_proc;
        PTprocesso->next = newProcesso;
        
    }
    
}
//Insercao em lista simples para representar uma lista de processos
void inserirListaProcesso(Processo **lista, char *buffer){
    if (!(*lista))
    {
        
        (*lista) = (Processo *) malloc(sizeof(Processo));   
        (*lista) = criarProcessos(buffer);
        (*lista)->id_proc = 1;
    }else{
        int id_proc = 2;
        Processo *PTprocesso = (*lista);
        Processo *newProcesso;
        
        while (PTprocesso->next)
        {
            PTprocesso = PTprocesso->next;
            id_proc++;
        }
        
        newProcesso = criarProcessos(buffer);
        newProcesso->id_proc = id_proc;
        PTprocesso->next = newProcesso;   
    } 
}

void buscarProntos(FilaProntos **fila, Processo *lista){
    
}

void inserirProntos(FilaProntos **fila, Processo *lista){
    if(!(*fila)){
        (*fila) = (FilaProntos *) malloc(sizeof(FilaProntos));
        (*fila)->instante_atual = lista->instante_submissao;
        (*fila)->ptProcesso = lista;
        (*fila)->next = NULL;

        FilaProntos *filaP = (*fila);
        lista = lista->next;
        while(lista){
            FilaProntos *novoProcessoPronto;
            if (lista->instante_submissao <= (*fila)->instante_atual)
            {
                novoProcessoPronto = (FilaProntos *) malloc(sizeof(FilaProntos));
                novoProcessoPronto->instante_atual = (*fila)->instante_atual;
                novoProcessoPronto->ptProcesso = lista;
                novoProcessoPronto->next = NULL;

                filaP->next = novoProcessoPronto;
                filaP = filaP->next;
            }
            
            lista = lista->next;
        }
    }else{

        FilaProntos *filaP = (*fila);
        lista = filaP->ptProcesso->next;
        (*fila)->instante_atual = lista->instante_submissao;
        while(lista){
            FilaProntos *novoProcessoPronto;
            if (lista->instante_submissao <= (*fila)->instante_atual)
            {
                novoProcessoPronto = (FilaProntos *) malloc(sizeof(FilaProntos));
                novoProcessoPronto->instante_atual = (*fila)->instante_atual;
                novoProcessoPronto->ptProcesso = lista;
                novoProcessoPronto->next = NULL;

                filaP->next = novoProcessoPronto;
                filaP = filaP->next;
            }
            
            lista = lista->next;
        }
    
    }
}

void readLine(FILE *arquivo, char *buffer, int sizeBuffer, char *ch){
    int i = 0;
    (*ch) = getc(arquivo);
    while (((*ch) != '\n') && ((*ch) != EOF) && (i < (sizeBuffer-1)))
    {
        buffer[i] = *ch;
        (*ch) = getc(arquivo);
        i++;
    }
    buffer[i] = '\0';
    
}

void reordenaFila(FilaProcesso *fila){
    if (fila)
    {
        Processo *ptProc = fila->process;
        Processo *ptNextProc = ptProc->next;
        Processo *ptPreviousProc = NULL;

        while ((ptNextProc) && (ptProc->instante_submissao > ptNextProc->instante_submissao))
        {
            ptPreviousProc = ptNextProc;
            ptNextProc = ptNextProc->next;
        }    
        
        if (ptPreviousProc)
        {
            fila->process = ptProc->next;
            if (!ptNextProc)
            {
                ptPreviousProc->next = ptProc;
                ptProc->next = NULL;
            }else{
                ptPreviousProc->next = ptProc;
                ptProc->next = ptNextProc;
            }
        }
    }
}

void reordenaSJF(FilaProcesso *fila){
    Processo *ptProcesso;
    if (fila)
    {
        ptProcesso = fila->process;
        Processo *ptNextProcesso = ptProcesso->next;
        Processo *ptPreviousProcesso = NULL;
        while ((ptNextProcesso))
        {
            if (ptProcesso->picos->pico_cpu < ptNextProcesso->picos->pico_cpu)
            {
                ptPreviousProcesso = ptNextProcesso;
                ptNextProcesso = ptNextProcesso->next;
            }else{
                if (ptPreviousProcesso)
                {
                    ptPreviousProcesso->next = ptNextProcesso->next;
                    ptNextProcesso->next = ptProcesso;
                    fila->process = ptNextProcesso;
                }
                
                ptNextProcesso = NULL;
            }
            
        }
        
    }
        
}

void reordenaSJFPorSubmicao(FilaProcesso *fila){
    Processo *ptProcesso;
    if (fila)
    {
        ptProcesso = fila->process;
        Processo *ptNextProcesso = ptProcesso->next;
        Processo *ptPrevious = ptProcesso;
        Processo *ptPreviousAtual = NULL;

        int chute = ptProcesso->instante_submissao;
        Processo *ptProcessoAtual = ptProcesso;

        while ((ptNextProcesso))
        {
            if (ptNextProcesso->instante_submissao < chute)
            {
                chute = ptNextProcesso->instante_submissao;
                ptPreviousAtual = ptPrevious;
                ptProcessoAtual = ptNextProcesso;
            }
            ptPrevious = ptNextProcesso;
            ptNextProcesso = ptNextProcesso->next;
        }

        fila->process = ptProcessoAtual;
        ptPreviousAtual->next = ptProcessoAtual->next;
        ptProcessoAtual->next = ptProcesso;
        
    }
        
}

void FCFS(FILE *arquivo, FILE *output){
    FilaProcesso *filaProcessos = NULL;
    Processo *PTprocesso = NULL;
    char buffer[101];
    char ch; 
    
    do
    {
        readLine(arquivo, buffer, 101, &ch);
        createFilaProcess(&filaProcessos, buffer);
        
    }while (ch != EOF);
    
    //printProcesso(filaProcessos);
    filaProcessos->instante_atual = filaProcessos->process->instante_submissao;

    fprintf(output, "%d[", filaProcessos->instante_atual);
    while (filaProcessos)
    {
        PTprocesso = filaProcessos->process;
        
        if (PTprocesso->instante_submissao <= filaProcessos->instante_atual)
        {
            fprintf(output, "P%d ", PTprocesso->id_proc);
            PTprocesso->instante_submissao = filaProcessos->instante_atual + PTprocesso->picos->pico_cpu;
            filaProcessos->instante_atual = PTprocesso->instante_submissao;
            fprintf(output, "%d", PTprocesso->instante_submissao);
            
            if (PTprocesso->next)
                fprintf(output, "|");
            else
                fprintf(output, "]");

            removePico(&filaProcessos);
            
            reordenaFila(filaProcessos);
        }else{
            fprintf(output, "*** ");
            fprintf(output, "%d|", PTprocesso->instante_submissao);
            filaProcessos->instante_atual = PTprocesso->instante_submissao;
        }   
    }

    fclose(arquivo);
}

void SJF(FILE *arquivo, FILE *output){
    FilaProcesso *filaProcessos = NULL;
    Processo *PTprocesso = NULL;
    char buffer[101];
    char ch; 
    
    do
    {
        readLine(arquivo, buffer, 101, &ch);
        createFilaProcess(&filaProcessos, buffer);
        
    // }while (i <= 0);
    }while (ch != EOF);
    
    //printProcesso(filaProcessos);
    filaProcessos->instante_atual = filaProcessos->process->instante_submissao;

    fprintf(output, "%d[", filaProcessos->instante_atual);
    while (filaProcessos)
    {
        PTprocesso = filaProcessos->process;
        
        if (PTprocesso->instante_submissao <= filaProcessos->instante_atual)
        {
            fprintf(output, "P%d ", PTprocesso->id_proc);
            PTprocesso->instante_submissao = filaProcessos->instante_atual + PTprocesso->picos->pico_cpu;
            filaProcessos->instante_atual = PTprocesso->instante_submissao;
            fprintf(output, "%d", PTprocesso->instante_submissao);
            
            if (!(PTprocesso->next) && (filaProcessos->process == PTprocesso))
                fprintf(output, "]");
            else
                fprintf(output, "|");

            removePico(&filaProcessos);
            
            reordenaSJF(filaProcessos);
        }else{
            Processo *ptProcNext = PTprocesso->next;
            while ((ptProcNext) && (ptProcNext->instante_submissao > filaProcessos->instante_atual))
                ptProcNext = ptProcNext->next;
            
            if (ptProcNext)
            {
                fprintf(output, "P%d ", ptProcNext->id_proc);
                ptProcNext->instante_submissao = filaProcessos->instante_atual + ptProcNext->picos->pico_cpu;
                filaProcessos->instante_atual = ptProcNext->instante_submissao;
                fprintf(output, "%d", ptProcNext->instante_submissao);
                
                if (!(PTprocesso->next) && (filaProcessos->process == PTprocesso))
                    fprintf(output, "]");
                else
                    fprintf(output, "|");

                removePicoID(filaProcessos, ptProcNext->id_proc);
                
                reordenaSJF(filaProcessos);
            }else{
                reordenaSJFPorSubmicao(filaProcessos);
                fprintf(output, "*** ");
                fprintf(output, "%d|", filaProcessos->process->instante_submissao);
                filaProcessos->instante_atual = filaProcessos->process->instante_submissao;
            }       
        }
    }
    fclose(arquivo);
}

void atualizarProcesso(FilaProntos *fila){
    Processo *atual = fila->ptProcesso;
    Processo *novo = fila->next->ptProcesso;
    atual->picos->pico_cpu -= (novo->instante_submissao - atual->instante_submissao);
    atual->instante_submissao = novo->instante_submissao;

}

int isPreempcao(FilaProntos *fila){
    FilaProntos *pAux = fila->next;

    while (pAux)
    {
        if (pAux->ptProcesso->picos->pico_cpu < fila->ptProcesso->picos->pico_cpu)
            return 1;
    }
    
    return 0;
}

void searchPreempcao(FilaProntos *fila, FilaProntos **interrupcao, FilaProntos **antIterrupcao){
    FilaProntos *pAux = fila->next;
    FilaProntos *antIterrupcao = NULL;
    FilaProntos *iterrupcao = NULL;
    FilaProntos *anterior = fila;


    while (pAux)
    {
        if (pAux->ptProcesso->picos->pico_cpu < fila->ptProcesso->picos->pico_cpu)
        {
            *interrupcao = pAux;
            *antIterrupcao = anterior;
            
        }
        anterior = anterior->next;
        pAux = pAux->next;
    }
}

void SRTF(FILE *arquivo, FILE *output){
    Processo *listaProcesso = NULL;
    FilaProntos *fila = NULL;
    char buffer[101];
    char ch; 
    
    do
    {
        readLine(arquivo, buffer, 101, &ch);
        inserirListaProcesso(&listaProcesso, buffer);
    }while (ch != EOF);
    
    inserirProntos(&fila, listaProcesso);
    while (listaProcesso)
    {
        if (fila)
        {
            Processo *nextProceso = fila->ptProcesso->next;
            int tempoExecucao = fila->instante_atual+fila->ptProcesso->picos->pico_cpu;

            if (tempoExecucao > nextProceso->instante_submissao){
                inserirProntos(&fila, listaProcesso);
                atualizarProcesso(fila);
                
                if(isPreempcao(fila)){

                    fprintf(output, "P%d ", fila->ptProcesso->id_proc);
                    fprintf(output, "%d|", fila->instante_atual);

                    FilaProntos *interrupcao, *antInterrupcao, *inicio;
                    searchPreempcao(fila, &interrupcao, &antInterrupcao);

                    inicio = fila;
                    antInterrupcao->next = interrupcao->next;
                    interrupcao->next = fila;
                    fila = interrupcao;
                }
            }
        }else{
            fprintf(output,"***");
        }
        
    }
    
    
}

int main(int argc, char const *argv[])
{
    FILE *arquivo, *output;

    if (argc == 4)
    {
        puts("-seq");
    }else{
        if (!openFile(&arquivo, "processos-t2.1.in", "rb"))
            return -1;

        if (!openFile(&output, "processos-t21.in.out.test", "w"))
            return -1;

        fprintf(output, "FCFS: ");
        FCFS(arquivo, output);
        
        if (!openFile(&arquivo, "processos-t2.1.in", "rb"))
            return -1;

        fprintf(output, "\n\n");
        fprintf(output, "SJF: ");
        SJF(arquivo, output);

        if (!openFile(&arquivo, "processos-t2.1.in", "rb"))
            return -1;

        fprintf(output, "\n\n");
        fprintf(output, "SRTF: ");
        SRTF(arquivo, output);

        fclose(output);
    }



        
    return 0;
}
