#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <atari.h>
#include <peekpoke.h>

int main(void) {
    int j;
    int screen;
    _graphics(7+16);
    screen=PEEK(0x59)*256+PEEK(0x58);
    for(j=0; j<=255;j++) {
    POKE(screen+160+j, j);
    }
    cgetc();
    return EXIT_SUCCESS; 
}

