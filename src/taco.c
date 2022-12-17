#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <joystick.h>
#include <atari.h>
#include <unistd.h>
#include <time.h>
#include <peekpoke.h>
#include "atari_lib.h"
#include "splash.h"

#define KBCODE 764
#define FWidth 14
#define max_y 18

typedef unsigned char byte;

byte XSize, YSize;
byte tacostr [] = "TACOT";
byte blank [] = "  ";
byte bits []  = "  ";
int score=1;
byte xcord,prev_x;
byte border_left;
byte border_right;
int delay;
const byte BLANK_LINE[]="             ";
int MAX_DELAY=5000;

//zero terminate rows
byte line_buffer[max_y][FWidth+1];

void main_screen(void) {
    grmode (0);
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
    cputcxy(border_left, max_y, CH_LLCORNER);
    cputcxy(border_right, max_y, CH_LRCORNER);
}

byte locate(byte X, byte Y) {
    return line_buffer[Y][X];
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
        delay=MAX_DELAY;
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
            if (xcord>0 && locate(xcord-1,line)==' ') 
                xcord--;
            delay=MAX_DELAY;
        }

        if(key==KEY_ASTERISK || key==KEY_D || key==KEY_RIGHT) {
            if (xcord<border_right-border_left-3 && locate(xcord+2,line)==' ') 
                xcord++;
            delay=MAX_DELAY;
        }
        if(key==KEY_DASH || key==KEY_S || key==KEY_DOWN) {
            delay=200;
        }
        if(key==KEY_EQUALS || key==KEY_W || key==KEY_UP) {
            delay=MAX_DELAY;
        }
        key=255;
    }
    cputsxy(xcord+border_left+1, line, bits);
    line_buffer[line][xcord]=bits[0];
    line_buffer[line][xcord+1]=bits[1];
    //sleep(0.5);
    for (i=0; i<delay; i++);
}

void splash_screen(void) {
    if (read_sunraster("TACOBOT.BMP") ==1) {
        grmode(2);
        (void) bordercolor (COLOR_BLUE);
        cputsxy(6,2, "TACOBOT");
        printf("%s","         Press any key to start");
    }
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
    for (j=0;j<max_y;j++) {
        for(i=0; i< FWidth; i++) {
            line_buffer[j][i]=' ';
        }
    }
    for (j=0;j<max_y;j++) {
            line_buffer[j][FWidth]='\0';
    }
}

void eat_tacos() {
    int i;
    char found=1;
    byte j;
    char *idx;
    int position;
    byte cline[FWidth+1]; 

    for(i=max_y;i>=0;i--) {
        strcpy(cline,line_buffer[i]);
        found=1;   
        while (found != 0) {
            idx = strstr(cline,"TACO");
            if (idx == NULL) {
                found=0;
            } 
            else {
                position=idx-cline;
                for(j=0;j<4;j++)
                    line_buffer[i][position+j]=' ';
                strcpy(cline,line_buffer[i]);
                gotoxy(0,2);
                printf("Tacos: %d ",score);
                score=score+1*found;
                found++;
                MAX_DELAY=MAX_DELAY-score*20;
                if(MAX_DELAY<=200) MAX_DELAY=200;
                cputsxy(position+border_left+1,i, "    ");
            }
        }
        found=0;
    }
}

int main (void) {
    byte end=0;
    byte line=0;
    unsigned long t;
    while(1) {
        line=0;
        end=0;
        init();
        time(&t);
        srand(t);
        splash_screen();
        cgetc();
        main_screen();
        while (end==0) {
            draw_line (line);
            if (line>=max_y || block_at(xcord,line)) {
                eat_tacos();
                xcord=FWidth/2-2;
                if(line==1) end=1;
                line=0;
            }
            else {
                line++;
            }
        }
        cputsxy(10, 9,  " ...................");
        cputsxy(10, 10, " .... GAME OVER ....");
        cputsxy(10, 11, " ...................");
        cgetc();
    }
    return EXIT_SUCCESS;

}
