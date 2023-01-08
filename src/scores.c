#include <string.h>
#include <peekpoke.h>
#include "byte.h"
#include <stdio.h>
#include <atari.h>
#include <conio.h>

/**
 * use Thomas Cherryhomes' convention to save high scores in a fixed sector
*/
#define SCORE_SECTOR 0x1F4
//#define SCORE_SECTOR 10
#define SECTOR_SIZE 128
#define DCB 0x300
#define NSCORES 10
#define NSIZE 5

byte _buffer[SECTOR_SIZE];

int high_scores[10];
byte high_names[6][10];

byte rat1[5]={32,32,0,32,32};
byte rat2[5]={32,1,2,7,32};
byte rat3[5]={32,32,6,4,32};

/**
 * Write sector to disk, returns true if performed
*/
byte write_sector(byte* values, byte sector) {
    byte sector_hi = sector / 256;
    byte sector_lo = sector - sector_hi*256;
    byte buffer_hi = (int)(values) / 256;
    byte buffer_lo = (int)(values) - buffer_hi*256;
    //printf("%d,%d\n",buffer_hi,buffer_lo);

    POKE(DCB+1, 1);
    POKE(DCB+2, 0x50);
    POKE(DCB+10, sector_lo);
    POKE(DCB+11, sector_hi);
    POKE(DCB+4,  buffer_lo);
    POKE(DCB+5,  buffer_hi);
    __asm__ ("jsr $E453");
    return PEEK(DCB+3);
}

/*
* Get score table form sector, returns 1 if sector was loaded
*/
byte read_sector(byte* buffer, byte sector) {
    byte sector_hi = sector / 256;
    byte sector_lo = sector - sector_hi*256;
    byte buffer_hi = (int)(buffer) / 256;
    byte buffer_lo = (int)(buffer) - buffer_hi*256;
    //printf("%d,%d\n",buffer_hi,buffer_lo);

    POKE(DCB+1, 1);
    POKE(DCB+2, 0x52);
    POKE(DCB+10, sector_lo);
    POKE(DCB+11, sector_hi);
    POKE(DCB+4,  buffer_lo);
    POKE(DCB+5,  buffer_hi);
    __asm__ ("jsr $E453");
    return PEEK(DCB+3);
}

int save_scores(int scores[], byte names[NSIZE][NSCORES]) {
    byte i,j;
    j=0;
    for(i=0;i<NSCORES;i++) {
        _buffer[j]  =(byte) (scores[i]/256);
        _buffer[j+1]=(byte) (scores[i]-scores[i]*256);
        j+=2;
    }
    j=NSCORES*2;
    for(i=0;i<NSCORES;i++) {
        strncpy(&_buffer[j],names[i],NSIZE);
        j+=NSIZE+1;
    }
    return write_sector(_buffer, SCORE_SECTOR);
}

int load_scores(int scores[],byte names[NSIZE][NSCORES]) {
    byte i,j;
    int v;
    v=read_sector(_buffer, SCORE_SECTOR);
    for(i=0;i<128;i++) {
        printf("%c,", _buffer[i]);
    }
    j=0;
    for(i=0;i<NSCORES*2;i+=2) {
        scores[j]=_buffer[i]*256+_buffer[i+1];
        j+=1;
    }
    j=NSCORES*2;
    printf("\n");
    for(i=0;i<NSCORES;i++) {
        strncpy(names[i],&_buffer[j],NSIZE);
        names[i][NSIZE]='\0';
        printf("%d -[%s]\n",j,names[i] );
        j+=NSIZE+1;
    }
    return v;
}

void high_scores_screen() {
    byte i;
    _graphics(2);
    _setcolor_low(0,0x24); // background
    _setcolor_low(1,0x2E); // font1
    _setcolor_low(2,0x00); //
    _setcolor_low(3,0x25); // text window

    if(!load_scores(high_scores, high_names) ) {
        printf("Unable to load high scores ... press any key" );
        return;
    }

    for (i = 0; i < 3; i++)
    {
        cputcxy(i, 0, rat1[i]);
        cputcxy(i, 1, rat2[i]);
        cputcxy(i, 2, rat3[i]);
    }

    cputsxy(6,1," * HIGH SCORES *" );

    for(i=0; i<NSCORES; i++) {
        gotoxy(0,i+2);
        printf("%d - %s", high_scores[i], high_names[i]);
    }

}
