
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <joystick.h>
#include <atari.h>
#include "atari_lib.h"
#include <unistd.h>
#include <time.h>
#include <peekpoke.h>

//#define KBCODE 0xD209

#define KBCODE 764
#define byte unsigned char

byte XSize, YSize, FWidth=14;
byte tacostr [] = "TACOT";
byte blank [] = "  ";
byte bits []  = "  ";
byte max_y;
byte xcord,prev_x;
byte border_left;
byte border_right;
int delay;

void main_screen(void) {
    //grmode (0);
    screensize (&XSize, &YSize);
    border_left=XSize/2-FWidth/2;
    border_right=XSize/2+FWidth/2;
    max_y=YSize - 5;

    /* Set screen colors */
    (void) textcolor (COLOR_BLACK);
    (void) bordercolor (COLOR_RED);
    (void) bgcolor (COLOR_GREEN);
    
    cvlinexy (border_left,2, max_y);
    cvlinexy (border_right,2, max_y);
    xcord=XSize-20;
    gotoxy(border_left+1, max_y+1);
    chline (FWidth-1);
}

void draw_line (int line) {
    byte key;
    int i;
    if (line>0)
        cputsxy(xcord, line-1, blank);

    if (line==0) {
        int r = rand() % 4;
        delay=5000;
        strncpy(bits, tacostr+r, 2);
    }
    
    key=PEEK(KBCODE);
    POKE(764,255);
    if(key!=255) {
        //left
        cputsxy(xcord, line, blank);
        if(key==KEY_PLUS || key==KEY_A ||  key==KEY_LEFT) {
            if (xcord>border_left+1) 
                xcord--;
            delay=5000;
        }
            
        if(key==KEY_ASTERISK || key==KEY_D || key==KEY_RIGHT) {
            if (xcord<border_right-2) 
                xcord++;
            delay=5000;
        }
        if(key==KEY_DASH || key==KEY_S || key==KEY_DOWN) {
            delay=100;
        }
        key=255;   
        
    }
    cputsxy(xcord, line, bits);
    //sleep(0.5);
    for (i=0; i<delay; i++);
}

//Hack 
//cpeekc returns always 0

char locate(unsigned char X, unsigned char Y) 
{   unsigned int screen =0;
    screen=PEEK(0x59)*256+PEEK(0x58);

    return PEEK(screen+(Y*40)+X); 
};


char block_at(x,y) {
    char chr1;
    char chr2;
    // gotoxy(x,y+1);
    // chr1=cpeekc();
    // gotoxy(x+1,y+1);
    // chr2=cpeekc();

    chr1=locate(x,y+1);
    chr2=locate(x+1,y+1);
    
    cputcxy(0, 0, chr1);
    cputcxy(1, 0, chr2);
    if (chr1!=0 || chr2 !=0) {
        return 1;
    }
    return 0;
}

int main (void)
{
    byte end=0;
    byte line=0;
    unsigned long t;
    time(&t);
    srand(t);
    main_screen();     
  
    while (end==0)
    {
        draw_line (line);
        if (line>=max_y || block_at(xcord,line)) 
        {
            if(line==1) end=1;
            line=0;
        }
        else {
            line++;
        }

    }
    cputsxy(10, 10, " .... GAME OVER ....");
    cgetc ();
    return EXIT_SUCCESS;
}
