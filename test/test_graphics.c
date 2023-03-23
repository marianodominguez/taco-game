#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <atari.h>
#include <peekpoke.h>
#include "splash.h"

int main(void) {
    _graphics(7+16);
    _setcolor_low(0,0x24); //mouse face
    _setcolor_low(1,0x2E); // tortilla,
    _setcolor_low(2,0xEA); //border, letters

    if (read_bitmap("TEST.BMP") ==1) {
        _graphics(2);
        (void) bordercolor(COLOR_BLUE);
        cputsxy(6,2, "TACOBOT");
        printf("%s","         Press START key");
    } else {
        _graphics(0);
    }
    return 0;
}

