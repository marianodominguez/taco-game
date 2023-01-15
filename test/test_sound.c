/*
** Test program for _sound for atari
**
**  January 6 2023 Mariano Domínguez
*/

#include <stdio.h>
#include <conio.h>
#include <atari.h>
#include "atari_lib.h"
#include <peekpoke.h>

int main(void)
{
    int i=0;
    unsigned char j;
    while(PEEK(0xD01f)!=6) {
        printf("playing sound \n");

        // sound(0,121,10,15); //voice, pitch, distortion, volume
        // for(i=0;i<9000;i++);
        // sound(0,0,0,0);
        for(i=0;i<1000;i++);

        for (j=0; j<144; j++) {
            sound(1,(144-j),10,8);
            for (i=0; i<50; i++);
        }
        sound(0,20,10,10);
        for (i=0; i<500; i++);
        sound(0,0,0,0);
        sound(0,100,10,8);
        for (i=0; i<500; i++);
        sound(0,0,0,0);
    }
    return 0;
}
