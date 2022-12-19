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
#define MAX_Y 18

typedef unsigned char byte;

byte XSize, YSize;
byte tacostr [] = "TACOT";
byte blank []   = "  ";
byte bits []    = "  ";
int score       = 1;
byte xcord,prev_x;
byte border_left;
byte border_right;
int delay;
const byte BLANK_LINE[]="             ";
int MAX_DELAY=5000;

//zero-terminated rows
byte line_buffer[MAX_Y][FWidth+1];

void main_screen(void) {
    int i,j;
    grmode (0);
    load_font();

    screensize (&XSize, &YSize);
    border_left=XSize/2-FWidth/2;
    border_right=XSize/2+FWidth/2;
    
    // background decorations  
    for(i=0;i<XSize;i=i+1) {
        for (j=0;j<MAX_Y;j=j+3){  
            cputsxy(i,j, "[" );
            cputsxy(i,j+1,">" );
            cputsxy(i,j+2,"[" );
        }
    }
    /* Set screen colors */
    (void) textcolor (COLOR_BLACK);
    (void) bordercolor (COLOR_RED);
    (void) bgcolor (COLOR_GREEN);
    
    //Draw scene 
    cvlinexy (border_left,2, MAX_Y-2);
    cvlinexy (border_right,2, MAX_Y-2);
    xcord=FWidth/2-2;
    gotoxy(border_left+1, MAX_Y);
    chline (FWidth-1);
    cputcxy(border_left,  MAX_Y, CH_LLCORNER);
    cputcxy(border_right, MAX_Y, CH_LRCORNER);

    //clean play area
    for(i=border_left+1;i<border_right;i++) {
        for (j=0;j< MAX_Y ;j++){  
            cputsxy(i,j," ");
        }
    }
}

byte locate(byte X, byte Y) {
    return line_buffer[Y][X];
}

void rat_anim() {
    byte x;
    for(x=0; x<XSize-4; x++) {
        cputsxy(x, MAX_Y+1, "    & ");
        cputsxy(x, MAX_Y+2, " #!%\" ");
        cputsxy(x, MAX_Y+3, "  $ '  ");       
    }
    cputsxy(XSize-4, MAX_Y+1, "     ");
    cputsxy(XSize-4, MAX_Y+2, "     ");
    cputsxy(XSize-4, MAX_Y+3, "     ");  
}


// When make a word, rat eats the last line
void rat_routine() {
    byte i,x;
    byte buffer[FWidth+1];
    byte temp[FWidth+1];
    strcpy(buffer,line_buffer[MAX_Y-1]);
    buffer[FWidth]='\0' ;

    for(i=MAX_Y-1; i>1; i--) {
        strcpy(temp,line_buffer[i-1]);
        temp[FWidth]='\0'; 
        cputsxy(border_left+1, i,"xxxxxxxxxxxxxx");
        strcpy(line_buffer[i],temp);
        cputsxy(border_left+1, i,temp);
    }
    cvlinexy (border_right,2, MAX_Y-2);
    cputsxy(border_left+1, MAX_Y+1 ,buffer);
    sleep(1);
    for (x=0; x<border_left; x++)
        cputsxy(x, MAX_Y+2, " Rat ate the taco" );

    sleep(1);
    rat_anim();
     for (x=0; x<border_left+10; x++)
        cputsxy(x, MAX_Y+2,BLANK_LINE );
    cputsxy( border_left+1, MAX_Y+1, BLANK_LINE );
}


void draw_line (byte line) {
    byte key;
    int i;
    if (line>0 && line-1<MAX_Y) {
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
            if (xcord>0 && locate(xcord-1,line)==' ')  {
                cputsxy(xcord+border_left+1, line, "  ");
                xcord--;
                cputsxy(xcord+border_left+1, line, bits);
            }
            delay=MAX_DELAY;
        }

        if(key==KEY_ASTERISK || key==KEY_D || key==KEY_RIGHT) {
            if (xcord<border_right-border_left-3 && locate(xcord+2,line)==' ')  {
                cputsxy(xcord+border_left+1, line," ");
                xcord++;
                cputsxy(xcord+border_left+1, line, bits);
            }
            delay=MAX_DELAY;
        }
        if(key==KEY_EQUALS || key==KEY_S || key==KEY_DOWN) {
            delay=200;
        }
        if(key==KEY_DASH || key==KEY_W || key==KEY_UP) {
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
    grmode(7+16);
    _setcolor_low(0,0x1C); //shade
    _setcolor_low(1,0x9C);
    _setcolor_low(2,0x34); //border
    
    sound(0, 100, 0xC0, 12);
    sleep(1);
    sound(0,0,0,0);
    
    if (read_sunraster("TACOBOT.BMP") ==1) {
        grmode(2);
        (void) bordercolor (COLOR_BLUE);
        cputsxy(6,2, "TACOBOT");
        printf("%s","         Press START key");
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
    for (j=0;j<MAX_Y;j++) {
        for(i=0; i< FWidth; i++) {
            line_buffer[j][i]=' ';
        }
    }
    for (j=0;j<MAX_Y;j++) {
            line_buffer[j][FWidth]='\0';
    }
    score=1;
    delay=5000;
}

void eat_tacos() {
    int i;
    char found=1;
    byte j;
    char *idx;
    int position;
    byte cline[FWidth+1]; 

    for(i=MAX_Y;i>=0;i--) {
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
                cputsxy(0,0,"<<<<<<<<<<");
                gotoxy(0,1);
                printf("Tacos: %d ",score);
                cputsxy(0,2,"<<<<<<<<<<");
                score=score+1*found;
                found++;
                MAX_DELAY=MAX_DELAY-score*10;
                if(MAX_DELAY<=300) MAX_DELAY=300;
                cputsxy(position+border_left+1,i, "    ");
            }
            if (found!=0) rat_routine();
        }            
        found=0;
    }
}

void wait_start() {
    int i;
    int key=0;
    while (key!=6) {
        key = PEEK(0xD01f);
        for (i=0; i<500; i++);
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
        wait_start();
        main_screen();
        while (end==0) {
            draw_line (line);
            if (line>=MAX_Y || block_at(xcord,line)) {
                eat_tacos();
                xcord=FWidth/2-2;
                if(line==1) end=1;
                line=0;
            }
            else {
                line++;
            }
        }
        cputsxy(7, 9,  "============================");
        cputsxy(7, 10, "=                          =");
        cputsxy(7, 11, "=        GAME OVER         =");
        cputsxy(7, 12, "=                          =");
        cputsxy(7, 13, "= PRESS ENTER TO TRY AGAIN =");
        cputsxy(7, 14, "=                          =");
        cputsxy(7, 15, "============================");
        cgetc();
    }
    return EXIT_SUCCESS;

}
