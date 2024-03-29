#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <peekpoke.h>
#include "atari_lib.h"
#include "font.h"
#include "byte.h"

const byte file_header[] = {0x42, 0x4d, 0x9a, 0x1e,'\0'};

byte exit_code=0;
int screen;

void read_header(FILE* fd) {
    byte header[8][5];
    byte *id;
    int i,j;
    for(i=0;i<8;i++) {
        for(j=0;j<4;j++) {
            header[i][j]=fgetc(fd);
        }
        header[i][4]='\0';
    }

    strncpy(id,header[0],4);

    for(i=0;i<154-32;i++) {
       j=fgetc(fd);
    }

    if (strncmp(id, file_header,4)) {
        printf("%s","WRONG FILE TYPE: ");
        printf("%s",id);
        cgetc();
        exit_code=1;
    }
}

void plot_byte(byte byte, int offset) {
    POKE(screen+offset, byte);
}

int read_bitmap(char* filename) {
    byte dbyte,nbyte;
    int i=0;
    FILE* fd = fopen(filename, "r");
    if (fd==NULL)
        return EXIT_FAILURE;

    screen=PEEK(0x59)*256+PEEK(0x58);
    read_header(fd);
    //broken
    //fseek(fd, 66, SEEK_SET);
    while( exit_code!=1 && i<160*96/4) {
        dbyte=fgetc(fd);
        nbyte=fgetc(fd);
        dbyte=(dbyte&0xf0)<<2|(dbyte&0x0f)<<4;
        nbyte=(nbyte>>2)|(nbyte&0x0f);
        plot_byte(dbyte | nbyte, i);
        i++;
    }
    fclose(fd);
    return exit_code;
}

int load_font(void) {;
    POKE(756,(unsigned int) &GAME_FONT/256);
    return EXIT_SUCCESS;
}
