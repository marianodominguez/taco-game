#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <peekpoke.h>
#include "atari_lib.h"

const char file_header[] = {0x42, 0x4d, 0x46, 0x1e,'\0'};
char exit_code=0;
int screen;

void read_header(FILE* fd) {
    unsigned char header[8][5];
    unsigned char *id;
    int i,j;
    for(i=0;i<8;i++) {
        for(j=0;j<4;j++) {
            header[i][j]=fgetc(fd);
        }
        header[i][4]='\0';
    }
    
    strncpy(id,header[0],4);

    for(i=0;i<70-32;i++) {
       j=fgetc(fd);     
    }

    if (strncmp(id, file_header,4)) {
        printf("%s","WRONG FILE TYPE: ");
        printf("%x",id);
        cgetc();
        exit_code=1;
    }
}

void plot_byte(unsigned char byte, int offset){
    POKE(screen+offset, byte);
}

int read_sunraster(char* filename) {
    int rbyte;
    unsigned char dbyte,nbyte;
    int i=0;
    FILE* fd = fopen(filename, "r");
    grmode(7+16);
    screen=PEEK(0x59)*256+PEEK(0x58);
    read_header(fd);
    //broken
    //fseek(fd, 66, SEEK_SET);
    while( rbyte!=EOF || exit_code==1 ) {
        rbyte=fgetc(fd);
        dbyte=rbyte<<2;
        rbyte=fgetc(fd);
        nbyte=rbyte;
        plot_byte(nbyte | dbyte,i);
        i++;
    }
    fclose(fd);
    return exit_code;
}
