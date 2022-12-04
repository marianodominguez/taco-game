
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <joystick.h>
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

void main_screen(void) {


    clrscr ();
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
    
    if (line>0)
        cputsxy(xcord, line-1, blank);

    if (line==0) {
        int r = rand() % 4;
        strncpy(bits, tacostr+r, 2);
    }
    ;
    key=PEEK(KBCODE);
    POKE(764,255);
    if(key!=255) {
        //left
        cputsxy(xcord, line, blank);
        if(key==KEY_PLUS || key==KEY_A ||  key==KEY_LEFT) {
            if (xcord>border_left+1) 
                xcord--;
        }
            
        if(key==KEY_ASTERISK || key==KEY_S || key==KEY_RIGHT) {
            if (xcord<border_right-2) 
                xcord++;
        }
        key=255;   
        
    }
    cputsxy(xcord, line, bits);
    sleep(1);
}

int block_at(x,y) {
    char chr1;
    char chr2;
    gotoxy(x,y+1);
    chr1=cpeekc();
    gotoxy(x+1,y+1);
    chr2=cpeekc();
    if (chr1!=' ' || chr2 !=' ') {
        return 0;
    }
    
    return 1;
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
         line++;
         if (line>=max_y+1 || block_at(xcord,line)) 
         {
            line=0;
         }
    }

    cgetc ();
    return EXIT_SUCCESS;
}