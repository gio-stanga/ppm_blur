#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "data.h"

/* define the list node  */
typedef struct Lego_brick{
	int num;	// needed amount
	char* name;	// name of the brick
	struct Lego_brick* next; 
} Lego_brick;

// root of the list
Lego_brick* root = NULL;

/* adds a brick to the list
 * if a node with the same name is altready present jsut increas it's num */
void add_brick(char* brick_name){

	// if there is no root build node, set it as root and num as 1 and return
	if(!root){
		Lego_brick* brick = malloc(sizeof(Lego_brick));
		if(!brick){
			printf("ERROR: could not allocate memory\n");
			exit(1);
		}
		root = brick;
		brick->num = 1;
		brick->name = strdup(brick_name);
		if(!brick->name) {
			printf("ERROR: could not copy string\n");
			exit(1);
		}
		brick->next = NULL;
		return;
	}

	Lego_brick* prec = NULL;
	for(Lego_brick* i = root; i != NULL; prec = i, i = i->next){

		/* if a node with that name already exists
		 * just increas num */
		if(strcmp(i->name, brick_name) == 0){
			i->num = i->num+1;
			return;
		}
	}

	/* if there is no node with that name create one 
	 * and set num as 1 */
	Lego_brick* brick = malloc(sizeof(Lego_brick));
	prec->next = brick;
	brick->num = 1;
	brick->name = strdup(brick_name);
	brick->next = NULL;
}

/* just print the list of bricks needed and the relative amount */
void print_bricks(void){
	printf("\n\n\n");

	for(Lego_brick* i = root; i != NULL; i = i->next)
		printf("|| brick name: %s, quantity: %d\n", i->name, i->num);

	printf("\n\nNOTE: all bricks are #3024 lego bricks, "
			"\nnow it's up to you to find them :)\n\n");
}

/* find the best diameter size for the blocks */
int find_min(int height, int width, int block_size){
	int lo = (block_size * 9)/10;
	int hi = (block_size *11)/10; 

	if(lo < 1) lo = 1;

	int min = block_size;
	int best_score = 0x3f3f3f3f;
	
	for(int i = lo; i <= hi; i++){
		if(height % i + width % i < best_score){
			best_score = height % i + width % i;
			min = i;
		}
	}
	return min;
}

/* calculates the number of needed blocks */
int calc_blocks(int height, int width, int *row_blocks,
		int *column_blocks, int *diameter, int block_size){
	
	int d = find_min(height, width, block_size);

	*diameter = d;
	*row_blocks = width/d;
	*column_blocks = height/d;
	
	return (*column_blocks) * (*row_blocks);
}

/* calculates the average value of each byte
 * for every pixel in the whole block */
int read_block(FILE *fp, unsigned char avg[3], int width, int d) {
    const int bpp = 3;
    unsigned char rgb[3];
    unsigned long totR = 0, totG = 0, totB = 0; 
    const unsigned long denom = (unsigned long)d * (unsigned long)d;

    for (int j = 0; j < d; j++) {
        for (int i = 0; i < d; i++) {
            if (fread(rgb, 1, 3, fp) != 3) {
		printf("ERROR: could not read file correctly");
                exit(1);
            }
            totR += rgb[0];
            totG += rgb[1];
            totB += rgb[2];
        }
        
        if (j != d - 1) {
            if (fseek(fp, (long)(width - d) * bpp, SEEK_CUR) != 0) {
                exit(1);	
            }
        }
    }

    avg[0] = (unsigned char)(totR / denom);
    avg[1] = (unsigned char)(totG / denom);
    avg[2] = (unsigned char)(totB / denom);
    return 0;
}

/* paints a NxN block with the RGB color specified in avg[3] */
int write_block(FILE* fp, unsigned char* avg, int width, int d){
	const int bpp = 3;
	for(int j = 0; j<d; j++){
		for(int i = 0; i<d; i++){
			if(fwrite(avg, 1, 3, fp) != 3){
				printf("ERROR: could not write in file\n");
				exit(1);
			}	
		}
		if(j != d - 1){
			if(fseek(fp, (long)(width - d) * bpp, SEEK_CUR) != 0){
				printf("ERROR: could not move file pointer correctly\n");     
				exit(1);			
			}
		}
	}
	return 0;
}

int main(int argc, char *argv[]){
	FILE *fp;

	if(argc != 3){
		printf("invalid arguments\n");
		exit(1);
	}

	int block_size = atoi(argv[1]);
	if (block_size <= 0){
		printf("ERROR: invalid argument, block_size must be greater than zero\n");
		exit(1);
	}

	int height, width, maxval;
	char magic[3];

	char *fname = argv[2];

	fp = fopen(fname, "r+b"); // open file
	
	if(!fp){
		printf("ERROR: could not open image file.\n");
		exit(1);
	}

	// get file format
	if(fscanf(fp, "%2s", magic) != 1){
		printf("ERROR while reading the file\n");
		exit(1);
	}

	if(magic[0] != 'P' || magic[1] != '6'){
		printf("ERROR: unknown file format, please use a .ppm image\n");
		exit(1);
	}

	// get width and height
	if(fscanf(fp, "%d %d", &width, &height) != 2){
		printf("ERROR while reading the file\n");
		exit(1);
	}
	
	// get maxval 
	// IMPORTANT!! so far we don't manage files with maxval > 255
	// so if that is the case -> unexpected behaviour
	if(fscanf(fp, "%d", &maxval) != 1){
		printf("ERROR while reading the file\n");
		exit(1);
	}

	if(maxval > 255){
		printf("ERROR: maxval < 255 is unsupported\n");
		exit(1);
	}
	
	// discard useless chars
	int ch;
	do{
		ch = fgetc(fp);
	}while(ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');
	ungetc(ch, fp);


	// 3 bytes for a pixel (RGB)
	const int bpp = 3;

	// reference to first byte of the first pixel
	long data0 = ftell(fp);


	int row_blocks, column_blocks, d;
        
	calc_blocks(height, width, &row_blocks, &column_blocks, &d, block_size);

	unsigned char avg[3];

	for(int by = 0; by < column_blocks; by++){
		for(int bx = 0; bx < row_blocks; bx++){

			
			long offset = data0 + (long)((by * d * width) + (bx * d)) * bpp;			
			
			if(fseek(fp, offset, SEEK_SET) != 0){
				printf("ERROR: could not move file pointer correctly\n");
				exit(1);
			}
			// read and compute avg
			read_block(fp, avg, width, d);

			// use ll library to find best fitting brick
			int brick_id = ll_best_fit(avg[0], avg[1], avg[2]);

			// add each brick to a linked list with 2 keys (brick_name, how_many)
			add_brick(ll_idtoname(brick_id));
	
			if(fseek(fp, offset, SEEK_SET) != 0){
				printf("ERROR: could not move file pointer correctly\n");
				exit(1);
			}
	
			// paint the block with avg
			write_block(fp, avg, width, d);
	
		}
	}

	print_bricks();

	return 0;
}
