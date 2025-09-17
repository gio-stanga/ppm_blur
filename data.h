#ifndef LEGO_LIST_H
#define LEGO_LIST_H

/* return the total numer of bricks in the csv file */
int ll_size(void);

/* given the id of a brick return it's name  */
char* ll_idtoname(int id);

/* find the most similar lego brick (rgb wise) to the given rgb values */
int ll_best_fit(unsigned char r, unsigned char g, unsigned char b);

#endif
