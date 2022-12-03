
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <joystick.h>
#include <atari.h>

unsigned char XSize, YSize;

int main_screen(void) {
    clrscr ();
    screensize (&XSize, &YSize);
    /* Set screen colors */
    (void) textcolor (COLOR_BLACK);
    (void) bordercolor (COLOR_RED);
    (void) bgcolor (COLOR_GREEN);
    
    cvlinexy (XSize-10,2, YSize - 5);
    cvlinexy (XSize-30,2, YSize - 5);
    chline (21);
}

int main (void)
{

    int end=0;
    main_screen();     

    // while (end=0)
    // {
    //     cgetc ();
    // }
    cgetc ();
    return EXIT_SUCCESS;
}