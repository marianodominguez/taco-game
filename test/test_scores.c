#include "scores.h"
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int i;

int r_test_scores[10];
byte r_names[6][10];

int load_arrays() {
    load_scores(r_test_scores, r_names);
    for (i = 0; i < 10; i++)
    {
        printf("%d - %s", r_test_scores[i], r_names[i] );
    }
    printf("\n");
    return 1;
}

int main() {
    //this works
    //i=save_sectors();
    //save_arrays();
    load_arrays();
    cgetc();
    return i;
}
