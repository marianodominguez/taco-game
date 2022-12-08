#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <joystick.h>
#include <atari.h>
#include <unistd.h>
#include <time.h>
#include <peekpoke.h>

typedef unsigned char byte;
#define KBCODE 764
#define FWidth 14
#define max_y 18

byte XSize, YSize;
byte tacostr [] = "TACOT";
byte blank [] = "  ";
byte bits []  = "  ";
byte xcord,prev_x;
byte border_left;
byte border_right;
int delay;
byte line_buffer[max_y][FWidth];

void main_screen(void) {
    //grmode (0);
    screensize (&XSize, &YSize);
    border_left=XSize/2-FWidth/2;
    border_right=XSize/2+FWidth/2;
    
    /* Set screen colors */
    (void) textcolor (COLOR_BLACK);
    (void) bordercolor (COLOR_RED);
    (void) bgcolor (COLOR_GREEN);
    
    cvlinexy (border_left,2, max_y-2);
    cvlinexy (border_right,2, max_y-2);
    xcord=FWidth/2-2;
    gotoxy(border_left+1, max_y);
    chline (FWidth-1);
}

void draw_line (byte line) {
    byte key;
    int i;
    if (line>0 && line-1<max_y) {
        cputsxy(xcord+border_left+1, line-1, blank);
        line_buffer[line-1][xcord]=' ';
        line_buffer[line-1][xcord+1]=' ';
    }

    if (line==0) {
        int r = rand() % 4;
        delay=5000;
        strncpy(bits, tacostr+r, 2);
    }
    
    key=PEEK(KBCODE);
    POKE(764,255);
    if(key!=255) {
        //left
        cputsxy(xcord+border_left+1, line, blank);
        line_buffer[line][xcord]=' ';
        line_buffer[line][xcord+1]=' ';

        if(key==KEY_PLUS || key==KEY_A ||  key==KEY_LEFT) {
            if (xcord>0) 
                xcord--;
            delay=5000;
        }

        if(key==KEY_ASTERISK || key==KEY_D || key==KEY_RIGHT) {
            if (xcord<border_right-border_left-3) 
                xcord++;
            delay=5000;
        }
        if(key==KEY_DASH || key==KEY_S || key==KEY_DOWN) {
            delay=200;
        }
        if(key==KEY_EQUALS || key==KEY_W || key==KEY_UP) {
            delay=5000;
        }
        key=255;
    }
    cputsxy(xcord+border_left+1, line, bits);
    line_buffer[line][xcord]=bits[0];
    line_buffer[line][xcord+1]=bits[1];
    //sleep(0.5);
    for (i=0; i<delay; i++);
}

byte locate(byte X, byte Y) {
    return line_buffer[Y][X];
}

byte block_at(x,y) {
    char chr1;
    char chr2;

    chr1=locate(x,y+1);
    chr2=locate(x+1,y+1);
    
    if (chr1!=' ' || chr2!=' ') {
        return 1;
    }
    return 0;
}

void init(void) {
    int i,j;
    for(i=0; i< FWidth; i++) {
        for (j=0;j<max_y;j++) {
            line_buffer[j][i]=' ';
        }
    }
}

void eat_tacos() {
    int i;
    char found=1;
    byte j;
    char *idx;
    int position;
    byte cline[FWidth]; 

    for(i=max_y;i>=0;i--) {
        strncpy(cline,line_buffer[i],FWidth);
        cline[FWidth]=0;       
        while (found != 0) {
            idx = strstr(cline,"TACO");
            cputsxy(0, 0, "                       ");
            cputsxy(0, 0, cline);
            if (idx == NULL) {
                found=0;
            } 
            else {
                position=idx-cline;
                found=1;
                for(j=0;j<4;j++)
                    line_buffer[i][position+j]=' ';
                strncpy(cline,line_buffer[i],FWidth);
                cline[FWidth]=0;
                gotoxy(0,1);
                printf("%d",position);
                cputsxy(xcord+border_left+1, i, "    ");
            }
        }
    }
}

int main (void) {
    byte end=0;
    byte line=0;
    unsigned long t;
    init();
    time(&t);
    srand(t);
    main_screen();
  
    while (end==0) {
        draw_line (line);
        if (line>=max_y || block_at(xcord,line)) {
            if(line==1) end=1;
            eat_tacos();
            line=0;
        }
        else {
            line++;
        }
    }
    cputsxy(10, 10, " .... GAME OVER ....");
    cgetc();
    return EXIT_SUCCESS;
}
