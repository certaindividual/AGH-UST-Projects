#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/times.h>


int sortSys(char* fileName, int recNum, int recSize) {

    int file = open(fileName, O_RDWR);
    if(file <0) perror("Wystapil blad przy otwieraniu pliku:\n");
    long recordLen = (long) ((recSize+1)*sizeof(char));

    for(int i=1;i<recNum;i++){
        
        char* record_i = (char*) malloc((recSize+1)*sizeof(char));
        //read record on i position
        lseek(file, i*recordLen, SEEK_SET); //SEEK_SET - from the beginning        
        if (read(file, record_i, sizeof(char) * (recSize+1)) != (recSize + 1)) { 
            perror("Blad wczytywania:\n");
            return 1;
        }

        int j = i-1;
        char* record_j = (char*) malloc(recSize*sizeof(char));
        //read record on j position
        lseek(file, j*recordLen, SEEK_SET);
        if (read(file, record_j, sizeof(char) * (recSize+1)) != (recSize + 1)) { 
            perror("Blad wczytywania:\n");
            return 1;
        }        
        
        while(record_i[0] < record_j[0] && j>=0) {

            //write j on j+1 pos
            lseek(file, (j+1)*recordLen,SEEK_SET);
            if (write(file, record_j, sizeof(char) * (recSize + 1)) != (recSize + 1)) { 
                perror("Blad przy zapisywaniu:\n");
                return 1;
            }
            j--;
            if(j>=0) {
                //read record on j position
                lseek(file, j*recordLen,SEEK_SET);
                if (read(file, record_j, sizeof(char) * (recSize+1)) != (recSize + 1)) { 
                    perror("Blad wczytywania:\n");
                    return 1;
                }
            }            
        }
        //write record on i to current j+1 pos
        lseek(file, (j+1)*recordLen,SEEK_SET);
        if (write(file, record_i, sizeof(char) * (recSize + 1)) != (recSize + 1)) { 
            perror("Blad przy zapisywaniu:\n");
            return 1;
        }
        free(record_i);
        free(record_j);
    }
    close(file);      
    return 0;
}
int sortLib(char* fileName, int recNum, int recSize) {

    FILE *file = fopen(fileName, "r+");
    long recordLen = (long) ((recSize+1)*sizeof(char));

    for(int i=1;i<recNum;i++){
        
        char* record_i = (char*) malloc((recSize+1)*sizeof(char));
        //read record on i position
        fseek(file, i*recordLen, 0); //mode - 0 - from the beginning         
        if (fread(record_i, sizeof(char), (size_t)(recSize+1), file) != (recSize + 1)) { 
            printf("Blad wczytywania 1, zmienna i = %d\n", i);
            return 1;
        }

        int j = i-1;
        char* record_j = (char*) malloc(recSize*sizeof(char));
        //read record on j position
        fseek(file, j*recordLen,0);
        if (fread(record_j, sizeof(char), (size_t)recSize+1, file) != (recSize + 1)) { 
            printf("Blad wczytywania\n");
            return 1;
        }        
        
        while(record_i[0] < record_j[0] && j>=0) {

            //write j on j+1 pos
            fseek(file, (j+1)*recordLen,0);
            if (fwrite(record_j, sizeof(char), (size_t)(recSize + 1), file) != (recSize + 1)) { 
                printf("Blad przy zapisywaniu\n");
                return 1;
            }
            j--;
            if(j>=0) {
                //read record on j position
                fseek(file, j*recordLen,0);
                if (fread(record_j, sizeof(char), (size_t)recSize+1, file) != (recSize + 1)) { 
                    printf("Blad wczytywania\n");
                    return 1;
                }
            }            
        }
        //write record on i to current j+1 pos
        fseek(file, (j+1)*recordLen,0);
        if (fwrite(record_i, sizeof(char), (size_t)(recSize + 1), file) != (recSize + 1)) { 
            printf("Blad przy zapisywaniu\n");
            return 1;
        }
        free(record_i);
        free(record_j);
    }
    fclose(file);    
    return 0;
}

int copySys(char* fileName1, char* fileName2, int recNum, int recSize) {
    int fromFile = open(fileName1, O_RDONLY);
    if(fromFile == -1) {
        perror("Wystapil blad przy otwieraniu pliku:\n");
    }
    int toFile = open(fileName2, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
    //S_IRWXU  user (file owner) has read, write, and execute permission
    if(toFile == -1) {
        perror("Wystapil blad przy otwieraniu pliku:\n");
    }
    
    char *buf = malloc(recSize * sizeof(char));

    for (int i = 0; i < recNum; i++){
        if(read(fromFile, buf, sizeof(char) * (recSize + 1)) != (recSize + 1)) {
            perror("Blad przy odczytywaniu:\n");
            return 1;
        }
        if(write(toFile, buf, sizeof(char) * (recSize + 1)) != (recSize + 1)) {
            perror("Blad przy wpisywaniu:\n");
            return 1;
        }
    }
    free(buf);
    close(fromFile);
    close(toFile);
    
    return 0;
}

int copyLib(char* fileName1, char* fileName2, int recNum, int recSize) {
    FILE *fromFile = fopen(fileName1, "r");
    FILE *toFile = fopen(fileName2, "w+");
    char *buf = malloc(recSize * sizeof(char));

    for (int i = 0; i < recNum; i++){
        if(fread(buf, sizeof(char), (size_t)(recSize + 1), fromFile) != (recSize + 1)) {
            printf("Blad przy odczytywaniu\n");
            return 1;
        }
        if(fwrite(buf, sizeof(char), (size_t)(recSize + 1), toFile) != (recSize + 1)) {
            printf("Blad przy wpisywaniu\n");
            return 1;
        }
    }
    free(buf);
    fclose(fromFile);
    fclose(toFile);
    
    return 0;
}

double calculateTime(clock_t start, clock_t end) {
    return (double) (end - start) / sysconf(_SC_CLK_TCK);
}

int generate(int argc, char *argv[]) {
    char* fileName = malloc(strlen(argv[2]) * sizeof(char*));
    strcpy(fileName, argv[2]);    
    int recNum = (int) strtol(argv[3], NULL, 10);
    int recSize = (int) strtol(argv[4],NULL,10);

    FILE *file = fopen(fileName, "w+");
    FILE *randomize = fopen("/dev/urandom", "r");
    char* buf = (char*) malloc(recSize*sizeof(char));
    
    for(int i=0;i<recNum;i++) {
        if (fread(buf, sizeof(char), (size_t) recSize , randomize) != recSize) {
            printf("Nie wczytano odpowiedniej ilosci znakow\n");
            return 1;
        }

        for (int j = 0; j < recSize; j++) {
            buf[j] = (char) ('A' + abs(buf[j]) % 25);
        }
        buf[recSize] = '\n';

        if (fwrite(buf, sizeof(char), (size_t) recSize + 1, file) != recSize + 1) {
            printf("Nie zapisano odpowiedniej ilosci znakow\n");
            return 1;
        }
    }
    free(buf);
    fclose(file); 
    fclose(randomize); 
    
    return 0;
}
int sort(int argc, char *argv[]) {
    if(argc <6) {
        printf("Zbyt mala liczba argumentow\n");
        return 1;
    }
    
    char* fileName = malloc(strlen(argv[2]) * sizeof(char*));
    strcpy(fileName, argv[2]);
    int recNum = (int) strtol(argv[3], NULL, 10);
    int recSize = (int) strtol(argv[4],NULL,10);

    if(strcmp(argv[5], "sys")!=0  &&  strcmp(argv[5], "lib")!=0){
        printf("Niepoprawny ostatni argument");
        return 1;
    }
    bool sys = (strcmp(argv[5], "sys")==0);

    if(sys)
        return sortSys(fileName, recNum, recSize);
    else
        return sortLib(fileName, recNum, recSize);       
}
int copy(int argc, char *argv[]) {
    if(argc <7) {
        printf("Zbyt mala liczba argumentow\n");
        return 1;
    }
    char* fileName1 = malloc(strlen(argv[2]) * sizeof(char*));
    strcpy(fileName1, argv[2]);
    char* fileName2 = malloc(strlen(argv[3]) * sizeof(char*));
    strcpy(fileName2, argv[3]);
    int recNum = (int) strtol(argv[4], NULL, 10);
    int recSize = (int) strtol(argv[5], NULL, 10);
    if(strcmp(argv[6], "sys")!=0  &&  strcmp(argv[6], "lib")!=0){
        printf("Niepoprawny ostatni argument");
        return 1;
    }
    bool sys = (strcmp(argv[6], "sys")==0);

    if(sys)
        return copySys(fileName1, fileName2, recNum, recSize);
    else
        return copyLib(fileName1, fileName2, recNum, recSize);      
}

int main(int argc, char *argv[]) {

    if(argc <5) {
        printf("Zbyt mala liczba argumentow\n");
        return 1;
    }

    struct tms **tmsTime = (struct tms **) malloc(2 * sizeof(struct tms *));
    clock_t timeDiff[2];
    for (int i = 0; i < 2; i++) tmsTime[i] = (struct tms *) malloc(sizeof(struct tms *));

    printf("   Real time [seconds]      User time [seconds]      System time [seconds]\n");
    timeDiff[0] = times(tmsTime[0]);


    if(strcmp(argv[1],"generate")==0) { 
        int returnVal = generate(argc, argv);
        if(returnVal != 0) return returnVal;
    }  
    else if (strcmp(argv[1],"sort") == 0) { 
        int returnVal = sort(argc, argv); 
        if(returnVal != 0) return returnVal; 
    }
    else if (strcmp(argv[1],"copy")==0) { 
        int returnVal = copy(argc, argv); 
        if(returnVal != 0) return returnVal;
    }
    else {
        printf("Niepoprawny pierwszy argument\n");
        return 1;
    }    


    timeDiff[1] = times(tmsTime[1]);
    printf("       %lfs",  calculateTime(timeDiff[0], timeDiff[1]));
    printf("                %lfs   ",  calculateTime(tmsTime[0]->tms_utime, tmsTime[1]->tms_utime));
    printf("              %lfs ",  calculateTime(tmsTime[0]->tms_stime, tmsTime[1]->tms_stime));
    printf("\n");
}
