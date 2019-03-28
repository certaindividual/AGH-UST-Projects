#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>

#define  MAX(a, b) ( ( a > b) ? a : b )

int **image, **output;
double *filter;
int N, width, height;
int c, every_thread_does_n;

void image_to_arr(char const *argv[]);
void filter_to_arr(char const *argv[]);
void print_err_ext_failure(char * message);
void* filter_procedure(void *i);
void write_output(char const *argv[]);
void exit_procedure(void);
void print_timediff(struct timespec* start, struct timespec* end);

int main(int argc, char const *argv[]) {
	/*
	Program przyjmuje w argumentach wywolania:
	>liczbe watkow,	(N)
	>nazwe pliku z wejsciowym obrazem (argv[2]),
	>nazwe pliku z definicja filtru (argv[3]),
	>nazwe pliku wynikowego	(argv[4]).
	*/
	if(argc <5) {
        printf("Niepoprawne argumenty wywolania\n");
        exit(EXIT_FAILURE);
    }
	N = atoi(argv[1]);
		
	image_to_arr(argv);				
	filter_to_arr(argv);		
	
	output =(int**)calloc(height, sizeof(int*));
	for(int i=0; i<height;i++) {
		output[i] = (int*)calloc(width, sizeof(int));
	}
	every_thread_does_n = width/N;
	
	pthread_t* threads = (pthread_t*) calloc(N, sizeof(pthread_t));

	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);	

	int column_to_start_with = 0;
	for (int i = 0; i < N; i++) {

        int *x = (int*) malloc(sizeof(int));
        x[0] = column_to_start_with;
        if (pthread_create(&threads[i], NULL, &filter_procedure, (void*)x)!=0) print_err_ext_failure("Error during thread_create");
		column_to_start_with += every_thread_does_n;
    }
	
    for (int i = 0; i<N; i++) {
		if (pthread_join(threads[i], NULL)!=0) print_err_ext_failure("Error during thread join");
	}	
	
	clock_gettime(CLOCK_MONOTONIC, &end);
	
	print_timediff(&start, &end);
	
	write_output(argv);

	free(threads);
    exit(EXIT_SUCCESS);
}

void print_err_ext_failure(char * message) {
	perror(message);
	exit(EXIT_FAILURE);
}

void image_to_arr(char const *argv[]) {
		
	FILE *file_img = fopen(argv[2], "r");	
	size_t n = 0;
	char *p2_mark = NULL;
	char *width_and_height = NULL;
	if(getline(&p2_mark, &n, file_img)<=0) print_err_ext_failure("Error reading from file");	
	if(getline(&width_and_height, &n, file_img)<=0) print_err_ext_failure("Error reading from file");	
	
	
	char *width_c = strtok(width_and_height, " \n\t\0");
	char *height_c = strtok(NULL, " \n\t\0");
	
	width = atoi(width_c);	
	height = atoi(height_c);
	
	free(p2_mark);	
	free(width_and_height);
	
	
	char *M_str = NULL;
	if(getline(&M_str, &n, file_img)<=0) print_err_ext_failure("Error reading from file");		
	int M = atoi(M_str);
	if(M > 255) {
		printf("Incorrect M (>255)\n");
		exit(EXIT_FAILURE);
	}	
	free(M_str);
	
	image =(int**)calloc(height, sizeof(int*));
	for(int i=0; i<height;i++) {
		image[i] = (int*)calloc(width, sizeof(int));
	}
	int row_count=0;
	int col_count=0;
	
	char *value = (char*)calloc(20,sizeof(char)); 
	while(row_count < height) {

		char * line = NULL;				
		if(getline(&line, &n, file_img)<=0) print_err_ext_failure("Error reading from file");
				
		int count = 0;
		while( ( value = strtok(count == 0 ? line : NULL, " \n\t\0") ) != NULL  ) {
			//printf("image[%d][%d] = %d\n", row_count, col_count, atoi(value));
			image[row_count][col_count] = atoi(value);						
			count++;		
			if(col_count == width-1) {
				col_count = 0;
				row_count++;
				if(row_count >= height) break;
			}	
			else {
				col_count++;
			}
		}		
		free(line);
	}		
	fclose(file_img);

}
void filter_to_arr(char const *argv[]){
	FILE *file_filter = fopen(argv[3], "r");

	size_t n = 0;
	char *filter_size = NULL;
	if(getline(&filter_size, &n, file_filter)<=0) print_err_ext_failure("Error reading from file");
	
	c = atoi(filter_size);
	int fsize = c*c;	
	free(filter_size);
	

	int pixel_count = 0;

	filter =(double*)calloc(fsize, sizeof(double));
	
	while(pixel_count < fsize) {

		char *line = NULL;
		if(getline(&line, &n, file_filter)<=0) print_err_ext_failure("Error reading from file");			
		filter[pixel_count] = atof(line);
		pixel_count++;		
		free(line);		
		
	}	
	fclose(file_filter);
}

double j(int x, int y) {
	double s = 0;
	
	for (int  i=0; i<c; i++) {

		for(int j=0; j<c;j++) {
			
			int x_calculated = MAX(1, x-(int)ceil(c/2)+i);
			if(x_calculated > height-1) x_calculated = height-1;

			int y_calculated = MAX(1, y-(int)ceil(c/2)+j);
			if(y_calculated > width-1) y_calculated = width-1;

			s += image[x_calculated][y_calculated] * filter[i*c +j];			
		}
	}	
	return round(s);
}
void* filter_procedure(void *i) {
		
	int x; // kolumna do wykonania
	x = *((int*)i);	
	for(int plus=0; plus <every_thread_does_n && x + plus <= width;plus++) {

		for(int row=0; row<height;row++) {
			
			output[row][x+plus] = (int)j(row, x+plus);
		}
	}	
	pthread_exit((void*) 0);
}

void print_timediff(struct timespec* start, struct timespec* end) {

    time_t sec = end->tv_sec - start->tv_sec;
    long nano = end->tv_nsec - start->tv_nsec;
    if (nano < 0) { sec--; nano += 1000000000; }
  
	printf("Rozmiar obrazu: %d x %d\n", width, height);
	printf("Rozmiar filtru (c): %d\n", c);
	printf("Czas operacji: %ld s %ld nsec\n", sec, nano);
}

void write_output(char const *argv[]) {

	FILE *out = fopen(argv[4], "w");
		
    char buff[100];
    sprintf(buff, "P2\n%d %d\n255\n", width, height);
    fwrite(buff, sizeof(char), (size_t) strlen(buff), out);

	int nbrs_count=0;

	for(int i=0; i<height;i++) {

		for(int j=0; j<width; j++) {

			if(nbrs_count %20 == 0 && nbrs_count != 0) fwrite("\n", sizeof(char), (size_t)1, out);			
			sprintf(buff, "%d ", output[i][j]);
			fwrite(buff, sizeof(char), (size_t) strlen(buff), out);
			nbrs_count++;
		}
	}
	fclose(out);  

}
void exit_procedure(void) {
	
    if (image != NULL) {
        for(int i=0;i<height;i++) free(image[i]);
        free(image);
    }

    if (filter != NULL) {        
        free(filter);
    }

    if (output != NULL) {
        for(int i=0;i<height;i++) free(output[i]);
        free(output);
    }
}