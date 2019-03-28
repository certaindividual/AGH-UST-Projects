//for PATH_MAX and strptime warnings
#define __USE_XOPEN
#define _GNU_SOURCE

#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <pwd.h>
#include <stdbool.h>
#include <ftw.h>

//Globals for nftw
time_t nftwDate;
char *nftwOpe;

void buildPath(char * absPath, char * filePath, struct dirent *fileInfo) {
	strcpy(filePath, absPath);
	strcat(filePath, "/");
	strcat(filePath, fileInfo->d_name);
}

void printStats(const char *absPath, const struct stat *fileStat) {
	printf("%s\t\t", absPath);
	printf(" %ld\t\t", fileStat->st_size);
	char* ls_l_string = malloc(sizeof(char) * 11);
    ls_l_string[0] = S_ISDIR(fileStat->st_mode) ? 'd' : '-';
    ls_l_string[1] = (fileStat->st_mode & S_IRUSR) ? 'r' : '-';
    ls_l_string[2] = (fileStat->st_mode & S_IWUSR) ? 'w' : '-';
    ls_l_string[3] = (fileStat->st_mode & S_IXUSR) ? 'x' : '-';
    ls_l_string[4] = (fileStat->st_mode & S_IRGRP) ? 'r' : '-';
    ls_l_string[5] = (fileStat->st_mode & S_IWGRP) ? 'w' : '-';
    ls_l_string[6] = (fileStat->st_mode & S_IXGRP) ? 'x' : '-';
    ls_l_string[7] = (fileStat->st_mode & S_IROTH) ? 'r' : '-';
    ls_l_string[8] = (fileStat->st_mode & S_IWOTH) ? 'w' : '-';
    ls_l_string[9] = (fileStat->st_mode & S_IXOTH) ? 'x' : '-';
	ls_l_string[10] = '\0';
	printf("%s\t\t", ls_l_string);
	char * timeFormatted = (char*) malloc(20 * sizeof(char));
	strftime(timeFormatted, 20, "%d.%m.%Y %H:%M:%S", localtime(&(fileStat->st_mtime)));
    printf(" %s", timeFormatted);
	printf("\n");    
}

bool fileFulfilsTimeCondition(char* ope, struct stat* fileStat, time_t date) {
	return (
		(strcmp(ope, "=") == 0 && difftime(fileStat->st_mtime, date)) == 0 ||
		(strcmp(ope, "<") == 0 && difftime(fileStat->st_mtime, date)) < 0 ||
		(strcmp(ope, ">") == 0 && difftime(fileStat->st_mtime, date)) > 0
	);
}

int nftwFun (const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
	if (typeflag != FTW_F) return 0;
	if(!fileFulfilsTimeCondition(nftwOpe, (struct stat*)sb, nftwDate)) return 0;
	printStats(fpath, sb);
	return 0;
}

int nftwProc(char * absPath, char* ope, time_t date) {
	DIR *dir = opendir(absPath);
	if(dir == NULL) {
		perror("Blad przy otwieraniu katalogu:\n");		 
		return 1;
	}
	nftwDate = date;
	nftwOpe = ope;
	nftw(absPath, nftwFun, 25, FTW_PHYS); //FTW_PHYS - do not follow symbolic links
	closedir(dir);
	return 0;
}

int statProc(char * absPath, char* ope, time_t date) {

	 DIR *dir = opendir(absPath);
	 if(dir == NULL) {
		 perror("Blad przy otwieraniu katalogu:\n");		 
		 return 1;
	 }
	struct dirent *fileInfo = readdir(dir);
    struct stat* fileStat = (struct stat*) malloc(sizeof(struct stat));	
	char * filePath = (char*) malloc(PATH_MAX);
	
	while (fileInfo != NULL) {

		buildPath(absPath, filePath, fileInfo);		
		lstat(filePath, fileStat);	
	

		if (strcmp(fileInfo->d_name, ".") != 0 && strcmp(fileInfo->d_name, "..") != 0) {
			if (S_ISDIR(fileStat->st_mode)) {
				pid_t child_pid = fork();
				if (child_pid == 0) {
                    statProc(filePath, ope, date);
                    exit(EXIT_FAILURE);
                }				
			}   
			if (S_ISREG(fileStat->st_mode)) {				
				if (fileFulfilsTimeCondition(ope, fileStat, date)) {
					printStats(filePath, fileStat);
				}
			}		          
		}
		fileInfo = readdir(dir);
	}
	closedir(dir);	
	return 0;	
}

int main(int argc, char *argv[]) {

	if(argc <5) {
        printf("Zbyt mala liczba argumentow\n");
        return 1;
    }

	char* path = malloc(strlen(argv[1]) * sizeof(char*));
    strcpy(path, argv[1]);
	char* ope = malloc(strlen(argv[2]) * sizeof(char*)); 
	strcpy(ope, argv[2]);
	char* dateGiven = malloc(strlen(argv[3]) * sizeof(char*));
	strcpy(dateGiven, argv[3]);
	char* mode = malloc(strlen(argv[4]) * sizeof(char*));
	strcpy(mode, argv[4]);
    
	struct tm* timeStructure = malloc(sizeof(struct tm));
	if(strptime(dateGiven, "%d.%m.%Y %H:%M:%S", timeStructure) == NULL){
		printf("Niepoprawna data\n");
		return 1;
	}    
    time_t date = mktime(timeStructure);

	char * absPath = (char*) malloc(PATH_MAX * sizeof(char));
	if(realpath(path, absPath) == NULL) {
		perror("Niepoprawna sciezka:\n");
		return 1;
	}
	if(strcmp(ope, "<")!=0 && strcmp(ope, ">")!=0 && strcmp(ope, "=")!=0){
		printf("Niepoprawny drugi argument\n");
		return 1;
	}
	if(strcmp(mode, "nftw")!=0 && strcmp(mode, "stat")!=0) {
		printf("Niepoprawny ostatni argument\n");
		return 1;
	}
	if(strcmp(mode, "nftw") == 0){
		printf("Przeszukiwanie folderow wykonuje funkcja nftw\n");
		return nftwProc(absPath, ope, date);
	}
	else {
		printf("Przeszukiwanie w każdym z odnalezionych katalogow odbywa sie w osobnym procesie\n");
		return statProc(absPath, ope, date);
	}

}
