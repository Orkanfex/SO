#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

int openFile(FILE **file, char *name, char *operation){
    if (((*file) = fopen(name, operation)) == NULL)
    {
        return 0;
    }
    
    fseek((*file), 0, SEEK_SET);
    return 1;
}

void splitCommandLine(char *newArgv[256], char command[256], int *newArgc){
    int i = 0;

    newArgv[i] = strtok(command, " ");

    while (newArgv[i] != NULL){
        i++;
        newArgv[i] = strtok(NULL, " ");
    }
    
    *newArgc = i;
} 

void treeConstruct(char pid[256], DIR *pDir, struct dirent *pDirent){
    
    pDirent = readdir(pDir);

    if (pDirent != NULL)
    {
        treeConstruct(pid, pDir, pDirent);
        
        if (pDirent->d_type == 4)
        {
            struct dirent *pDirentAux;
            char path[1024] = "/proc/";
            strcat(path, pDirent->d_name);
            
            DIR *pDirAux = opendir(path);
            if (pDirAux)
            {
                
                while ((pDirentAux = readdir(pDirAux)) != NULL)
                {
                    if (!strcmp(pDirentAux->d_name, "stat"))
                    {
                        char *stat[256];
                        FILE *file;
                        char array[1024];
                        int args;

                        strcat(path, "/");
                        strcat(path, pDirentAux->d_name);

                        if (openFile(&file, path, "rb"))
                        {   
                            fread(array, sizeof(char), 1024, file);
                            splitCommandLine(stat, array, &args);

                            fclose(file);
                        }
                        if (!strcmp(stat[3], pid)){
                            
                            DIR *p = opendir("/proc");
                            printf("%s %s\n", stat[0], stat[1]);  
                            printf("    ");
                            treeConstruct(stat[0], p, NULL);
                            printf("\n");
                            
                        }
                    }
                }
            }
        }
    }
}

int isProccessPID(const char *pid, DIR *pDir, struct dirent *pDirent){
    
    while ((pDirent = readdir(pDir)) != NULL)
    {
        if (!strcmp(pDirent->d_name, pid))
            return 1;
    }
   
    return 0;
}

int main(int argc, char const *argv[])
{   
    char path[256] = {"/proc/"};

    if (argc < 2)
    {
        printf("Usage: ./tree PID\n");
    }else{
        DIR *pDir;

        pDir = opendir("/proc");
        
        if (!pDir)
        {
            printf("Nao foi possivel abrir o diretorio %s\n", path);
            return 1;
        }

        if (isProccessPID(argv[1], pDir, NULL))
        {
            char pid[256] = "";
            seekdir(pDir, SEEK_SET);
            strcpy(pid, argv[1]);
            treeConstruct(pid, pDir, NULL);
        }
    }
    
    return 0;
}
