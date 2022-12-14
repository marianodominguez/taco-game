#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <peekpoke.h>
#include "atari_lib.h"

const char file_header[] = {0x59,0xA6,0x6A,0x95,'\0'};
char exit_code=0;


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
    if (strncmp(id, file_header,4)) {
        printf("%s","WRONG FILE TYPE: ");
        printf("%x",id);
        cgetc();
        exit_code=1;
    }
}

void plot_byte(unsigned char byte, int offset){
    int screen=PEEK(0x59)*256+PEEK(0x58);
    POKE(screen+offset, byte);
}

int read_sunraster(char* filename) {
    int rbyte;
    unsigned char dbyte;
    int i=0;
    FILE* fd = fopen(filename, "r");
    read_header(fd);
    while( rbyte!=EOF || exit_code==1) {
        rbyte=fgetc(fd);
        dbyte=rbyte;
        plot_byte(dbyte,i);
        i++;
    }
    return exit_code;
}
