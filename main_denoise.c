/* file main_denoise.c */
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

#include "included_filter_h.h"

extern double getTime();


int main(int argc, char *argv[]) {

	if (argc < 5 )	{ /*file name, input file name, output file name, size of window, filter type*/ 
      printf("Not enough arguments\n");
      return 1;
	}
	char *input_file = argv[1];
	char *ouput_file = argv[2];
	int window_size = atoi(argv[3]);
	char *filter_type = argv[4];
	
	RGB *rgbArray, *rgbArray_new;
	
	int width = 0, height = 0, max = 0;
	double time;
	filter f;

	if (*filter_type == 'A')
		f = MEAN;
	else /*filter_type equals M*/
		f = MEDIAN;
		
	time = getTime();
	printf("\nReading file %s\n", input_file);
	rgbArray = readPPM(input_file, &width, &height, &max);	/*returns a pointer to an array (of type RGB) of size (*width)*(*height) storing the RGB values of the image.*/
	printf("***    %s read in %e seconds\n\n", input_file, getTime() - time); 

	time = getTime();
	printf("Processing %d x %d image using %d x %d window and %s filter...\n", width, height, window_size, window_size, (f == MEAN) ? "mean" : "median");
	rgbArray_new = denoiseImage(width, height, rgbArray, window_size, f);
	printf("***    image processed in %e seconds\n\n", getTime() - time);
	
	time = getTime();
	printf("Writing file %s\n", ouput_file);
	writePPM(ouput_file, width, height, max, rgbArray_new);
	printf("***    %s written in %e seconds\n\n", ouput_file, getTime() - time);
	
	free(rgbArray);
	free(rgbArray_new);
	
	return 0;
}