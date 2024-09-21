#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

int openFile(FILE **file, char *name, char *op){
    if(!((*file) = fopen(name, op)))
        return 0;

    fseek((*file), 0, SEEK_SET);
    return 1;
}

void gerar(FILE *file, long long n_elements, int limit){
    
    srand(time(NULL));
    
    for (int i = 0; i < n_elements; i++)
    {
        fprintf(file, "%d", rand() % limit);
        fprintf(file, "%s", " ");
    }
    
}


int main(int argc, char const *argv[])
{
    FILE *file = NULL;
    char *filename;
    int size;

    if (argc < 4 )
    {
        printf("Use: ./%s <quantidade_valores> <extensao> <limite_superior>\n", argv[0]);
        return 1;
    }

    size = strlen(argv[1]) + strlen(argv[1]) + 1;

    filename = malloc(sizeof(char) * size);
    strcpy(filename, argv[1]);
    strcat(filename, ".");
    strcat(filename, argv[2]);

    if (!openFile(&file, filename, "wr"))
    {
        printf("erro ao criar/abrir arquivo");
        return 1;
    }

    gerar(file, atoi(argv[1]), atoll(argv[3]));

    free(filename);
    fclose(file);
    return 0;
}
