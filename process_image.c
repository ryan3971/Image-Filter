/* file process_image.c */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


#include "included_filter_h.h"

typedef int Number;
int compare(const void *a, const void *b);

/* readPPM reads a PPM image from a file.
   Input: file is a pointer to a character string specifying a file name.
   Output:
     *width stores the width of the image
     *height stores its height
     *max stores the largest RGB value in the image

   readPPM returns a pointer to an array of size (*width)*(*height)
   storing the RGB values of the image.
*/

/*
	Each element of the returned array points to the structure RGB
*/
RGB *readPPM(const char *file, int *width, int *height, int *max)	{	

	RGB *rgbArray;
	
	FILE *fp;
	int ch;
	
	
	fp = fopen(file, "rb");
	
	if (fp == NULL)	{
		printf("%s can't be opened\n", file);
		exit(EXIT_FAILURE);
	}
	
	
	/*
	Checks the first character of each line to see if it is a comment or non-digit
	*/
    ch = getc(fp);
    while (ch == '#' || !isdigit(ch)) {
		while (getc(fp) != '\n');
			ch = getc(fp);
    }
    ungetc(ch, fp);
	
	/*
	read image size information
	Note: the != 3 tests the return value of fscanf
	*/
    if (fscanf(fp, "%d %d %d", width, height, max) != 3) {
         fprintf(stderr, "Invalid image size (error loading '%s')\n", file);
         exit(1);
    } 
	
	int index;
	
	rgbArray = (RGB*)malloc((*height) * (*width) * sizeof(RGB));
	
	for (index = 0; index < (*height)*(*width); index++)	{
			
		if (fscanf(fp, "%hhu %hhu %hhu", &rgbArray[index].r, &rgbArray[index].g, &rgbArray[index].b) != 3)	{
			fprintf(stderr, "Invalid image size (error loading '%s')\n", file);
			exit(1);
		}
	}
	
    fclose(fp);
	return rgbArray;
}


/* writePPM writes an image into a file.
   Input:
      file   name of the file
      width  width of the image
      height height of the image
      max    largest RGB value
      image  pointer to an array of size width*height with RGB values
*/
void writePPM(const char *file, int width, int height, int max, const RGB *image)	{
	
	int index = 0;
	FILE *fp;
	
	int x, y;
	

    fp = fopen(file, "wb");
	
    if (!fp) {
         fprintf(stderr, "Unable to open file '%s'\n", file);
         exit(1);
    }


    fprintf(fp, "P3 %d %d %d ", width, height, max);

   for (y = 0; y < height; y++) {
		for (x = 0; x < width; x++) {
			
			fprintf(fp, "%hhu %hhu %hhu ", image[index].r, image[index].g, image[index].b);

			index++;
    }
  }
	

    fclose(fp);
	
}


/* denoiseImage applies filtering to an image.
   Input:
      width  width of the image
      height height of the image
      max    largest RGB value
      image  pointer to an array of size width*height with RGB values
      n      size of filtering window
      f      type of filtering: MEAN or MEDIAN

   denoiseImage returns a pointer to an arrary of size width*height containing the new image.
*/
RGB *denoiseImage(int width, int height, const RGB *image, int n, filter f)		{
	
	RGB *image_new;
	
	int row, col, x, y, window_start_x, window_start_y, temp_x, temp_y, index;
	
	/*
	Could have done in one line but would have been kinda trivky to follow
	Alloctate all memory to one pointer and siple shift what part of the memory the holders point to
	*/
	
	Number *holder_r = malloc(sizeof(Number)*n*n+1);
	Number *holder_g = malloc(sizeof(Number)*n*n+1);
	Number *holder_b = malloc(sizeof(Number)*n*n+1);

	image_new = (RGB*)malloc(height * width * sizeof(RGB));

	for (row = 0; row < height; row++)	{
		for (col = 0; col < width; col++)	{
										
			index = 0;
					
			window_start_y = row - (n / 2);
			window_start_x = col - (n / 2);

			/*
			Extract the values within the window size and put them into an array
			*/
			
			for (y = window_start_y; y < window_start_y + n; y++)	{
				
				temp_y = y;

				/*
				Only bother checking x if row y is within the boundaries
				*/
				if (temp_y >= 0 && temp_y < height)	{
				
					for (x = window_start_x; x < window_start_x + n; x++)	{
							
						temp_x = x;
						/*
						Only x values within boundary are used
						*/
						if (temp_x >= 0 && temp_x < width)	{
							/*
							temp_y * width) + temp_x is the location of a pixel within the window to extract
							*/
							
							
							/*
							int k = (temp_y * width) + temp_x
							Much better
							*/
							
							holder_r[index] = image[(temp_y * width) + temp_x].r;
							holder_g[index] = image[(temp_y * width) + temp_x].g;
							holder_b[index] = image[(temp_y * width) + temp_x].b;
							index++;
						}
					}
				}
			}

		/*
		Caculate either the mean or median of the array, depending on the filter type
		*/
		
		/*
		Better to not have switch in loop.
		*/
			
			int w, new_r = 0, new_g = 0, new_b = 0;
			
			switch (f)	{
				case MEAN:
					for (w = 0; w < index; w++)	{
						new_r += holder_r[w];
						new_g += holder_g[w];
						new_b += holder_b[w];
					}

					new_r = new_r / index;
					new_g = new_g / index;
					new_b = new_b / index;

					break;
				case MEDIAN:
				
					qsort(holder_r, index, sizeof(Number), compare);
					qsort(holder_g, index, sizeof(Number), compare);
					qsort(holder_b, index, sizeof(Number), compare);
					/*
					If the total size of the window is an even number, take the middle value. 
					Note: (index + 1) to account for it starting at zero
					
					If it is an odd number take the average of the two middle values
					*/
					if (((index + 1) / 2) % 2 == 0)	{
						new_r = holder_r[(index / 2) - 1];
						new_g = holder_g[(index / 2) - 1];
						new_b = holder_b[(index / 2) - 1];
					}else{
						new_r = (holder_r[(index / 2) - 1] + holder_r[(index / 2)]) / 2;
						new_g = (holder_g[(index / 2) - 1] + holder_g[(index / 2)]) / 2;
						new_b = (holder_b[(index / 2) - 1] + holder_b[(index / 2)]) / 2;
					}
			}
			image_new[(row * width) + col].r = new_r;
			image_new[(row * width) + col].g = new_g;
			image_new[(row * width) + col].b = new_b;
			
		}
	}
	
	free(holder_r);
	free(holder_g);
	free(holder_b);

	return image_new;
}


int compare(const void *a, const void *b)
{
  Number x = *((Number *)a);
  Number y = *((Number *)b);
  if (x < y) return -1;
  if (x>y) return 1;
  return 0;
}
  

