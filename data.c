#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define NUM_BRICKS 261

// this struct defines a single brick
typedef struct{
	int id;
	char *name;
	unsigned char r, g, b;
} Brick;

// set to 1 when CSV is parsed and struct is filled
int init = 0;

// static array that contains each brick
// not flexible at all but so far so good
Brick data[NUM_BRICKS];


// put all the data of the csv in RAM, fast but not scalable,
// so far so good but if this scales up i'd have to change this
void initialize(void){
	if (init) {
		printf("ll_ERROR: table already initialized\n");
		exit(1);
	}

	FILE* fp = fopen("list.csv", "r");
	if(!fp){
		printf("ll_ERROR: could not open lsit.csv file");
		exit(1);
	}

	// skip firt line
	int ch;
	do{ ch = fgetc(fp); }while(ch != '\n');
	

	char buffer[256]; // more than enough for those lines
	for(int i = 0; i < NUM_BRICKS; i++){
		if(!fgets(buffer, sizeof(buffer), fp)){
			printf("ll_ERROR\n");
			fclose(fp);
			exit(1);
		}

		strtok(buffer, ",");

		data[i].id = i+1;

		char *tok = strtok(NULL, ",");
		if(!tok){
			printf("ll_ERROR: name missing in csv file\n");
			fclose(fp);
			exit(1);
		}

		data[i].name = strdup(tok);

		data[i].r = (unsigned char)atoi(strtok(NULL, ","));
		data[i].g = (unsigned char)atoi(strtok(NULL, ","));
		data[i].b = (unsigned char)atoi(strtok(NULL, ","));
	}

	fclose(fp);
	init = 1;
}

// ll stands for 'lego list'
// this acronymus will be used as a header in all teh functions of the API
int ll_size(void){
	if(!init) { initialize(); }

	return NUM_BRICKS;
}

// given an id return it's name
char* ll_idtoname(int id){
	if(!init) { initialize(); }

	return data[id-1].name;
}

// computes the euclidian distance between a lego brick and a pixel 
int rgb_distance(Brick ref, int r1, int g1, int b1){
	int dr = r1-ref.r;
	int dg = g1-ref.g;
	int db = b1-ref.b;
	return dr*dr + dg*dg + db*db;
}

// find the best fitting brick for a specific colour and return it's id
int ll_best_fit(unsigned char r, unsigned char g, unsigned char b){	
	if(!init) { initialize(); }

	int best_dist = 0x7fffffff;
	int best_idx = 0;

	for(int i = 0; i < NUM_BRICKS; i++){
		int d = rgb_distance(data[i], r, g, b);
		if(d < best_dist){
		       	best_dist = d;
				best_idx = i;
		}
	}
	return data[best_idx].id;
}

