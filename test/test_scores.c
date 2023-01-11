#include "scores.h"
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <peekpoke.h>

int i;

int test_scores[10]={1,2,3,4,5,6,7,8,9,10};
byte names[6][10];

int r_test_scores[10];
byte r_names[7][10];
byte rbuffer[128];

int save_sectors(void)
{
    for(i=1;i<128;i++) {
        rbuffer[i]=i;
    }
    write_sector(rbuffer,500);
    read_sector(rbuffer,500);
    for(i=1;i<128;i++) {
        printf("%d,", rbuffer[i]);
    }
    return 0;
}

int save_hscores(void)
{
    byte *n;
    for(i=0;i<10;i++) {
        strncpy(names[i],"MDMZZ",5);
    }
    //cputs(" saving scores ...\n");

    //save_scores(test_scores, names);
    cputs(" loading scores ...\n");
    load_scores(r_test_scores, r_names);
    cputs("\n");

    //POKE(752,0);
    //POKE(755,2);
    for(i=0;i<10;i++) {
        gotoxy(10,i+4);
        cputs( itoa(r_test_scores[i],n,10) );
        cputs("   -   ");
        cputs(r_names[i]);
        //printf("%d - %s\n",r_test_scores[i],r_names[i] );
    }
    return 0;
}

int main() {
    // save_sectors();
    // printf(" sector working, press enter \n");
    // cgetc();

    save_hscores();
    //printf(" scores working, press enter \n");
    cgetc();
    return 0;
}
