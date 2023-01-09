#include <string.h>
#include <peekpoke.h>
#include "byte.h"
#include <stdio.h>
#include <conio.h>

/**
 * use Thomas Cherryhomes' convention to save high scores in a fixed sector
*/
#define SCORE_SECTOR 500
//#define SCORE_SECTOR 10
#define SECTOR_SIZE 128
#define DCB 0x300
#define NSCORES 10
#define NSIZE 5

byte _buffer[SECTOR_SIZE];

/**
 * Write sector to disk, returns true if performed
*/
byte write_sector(byte* values, byte sector) {
    byte sector_hi = sector / 256;
    byte sector_lo = sector - sector_hi*256;
    byte buffer_hi = (int)(values) / 256;
    byte buffer_lo = (int)(values) - buffer_hi*256;
    printf("%x,%x=%x\n",buffer_hi,buffer_lo,values);

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
    printf("%x,%x=%x\n",buffer_hi,buffer_lo,buffer);

    POKE(DCB+1, 1);
    POKE(DCB+2, 0x52);
    POKE(DCB+10, sector_lo);
    POKE(DCB+11, sector_hi);
    POKE(DCB+4,  buffer_lo);
    POKE(DCB+5,  buffer_hi);
    __asm__ ("jsr $E453");
    return PEEK(DCB+3);
}

int save_scores(int scores[], byte names[NSIZE+1][NSCORES]) {
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

int load_scores(int scores[],byte names[NSIZE+1][NSCORES]) {
    byte i,j;
    int v;
    v=read_sector(_buffer, SCORE_SECTOR);
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

