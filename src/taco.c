
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <joystick.h>
#include <atari.h>
#include <unistd.h>
#include <time.h>
#include <peekpoke.h>

//#define KBCODE 0xD209

#define KBCODE 764

unsigned char XSize, YSize;
char tacostr [] = "TACOT";
char blank [] = "  ";
char bits []  = "  ";
unsigned char xcord;

void main_screen(void) {
    clrscr ();
    screensize (&XSize, &YSize);
    /* Set screen colors */
    (void) textcolor (COLOR_BLACK);
    (void) bordercolor (COLOR_RED);
    (void) bgcolor (COLOR_GREEN);
    
    cvlinexy (XSize-10+2,2, YSize - 5);
    cvlinexy (XSize-30-1,2, YSize - 5);
    xcord=XSize-20;
    chline (22);
}

void draw_line (int line) {
    char key;
    cputsxy(xcord, line, blank);

    if (line==0) {
        int r = rand() % 4;
        strncpy(bits, tacostr+r, 2);
    }
    cputsxy(xcord, line, bits);
    key=PEEK(KBCODE);
    POKE(764,255);
    if(key!=255) {
        //left
        cputsxy(xcord, line, blank);
        if(key==KEY_PLUS || key==KEY_A ||  key==KEY_LEFT) {
            if (xcord>XSize-30) 
                xcord--;
        }
            
        if(key==KEY_ASTERISK || key==KEY_S || key==KEY_RIGHT) {
            if (xcord<XSize-10) 
                xcord++;
        }
        key=255;   
        cputsxy(xcord, line, bits);
    }
    sleep(1);
}

int main (void)
{

    unsigned int end=0;
    unsigned int line=0;
    unsigned long t;
    time(&t);
    srand(t);
    main_screen();     
  

    while (end==0)
    {

         draw_line (line);
         line++;
         if (line==20) 
         {
            line=0;
         }
    }

    cgetc ();
    return EXIT_SUCCESS;
}