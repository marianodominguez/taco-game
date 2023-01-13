#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <atari.h>
#include <joystick.h>
#include <unistd.h>
#include <time.h>
#include <peekpoke.h>
#include "atari_lib.h"
#include "splash.h"
#include <joystick.h>
#include "scores.c"

#define KBCODE 764
#define FWidth 13
#define MAX_Y 18
#define STRIG0 644
#define STICK0 632
#define ATTRACT 77
#define hi_T 0x11
#define hi_A 0x13
#define hi_C 0x15
#define hi_O 0x17
#define lo_T 0x19
#define lo_A 0x14
#define lo_C 0x16
#define lo_O 0x18

byte BLANK_LINE[]="            ";
//zero-terminated rows
byte xcord,prev_x;
byte border_left;
byte border_right;
byte line_buffer[MAX_Y][FWidth+1];
byte XSize, YSize;
byte tacostr [] = "TACOT";
byte blank []   = "  ";
byte bits []    = "  ";
byte lo_bits [3];
byte hi_bits [3];
int score       = 1;
int delay;
int MAX_DELAY=2000;
int high_scores[NSCORES];
byte high_names[NSIZE+2][NSCORES];

byte buffer[FWidth+1];
byte rat1[5]={32,32,0,32,32};
byte rat2[5]={32,1,2,16,32};
byte rat3[5]={32,32,5,4,32};

byte rat4[5]={32,32,32,32,32};
byte rat5[5]={32,6,7,8,32};
byte rat6[5]={32,32,9,10,32};

byte rat7[5]={32,32,0,32,32};
byte rat8[5]={32,1,2,16,32};
byte rat9[5]={32,32,12,4,32};

byte temp[FWidth+1];
byte low_str[]={lo_T, lo_A, lo_C, lo_O, lo_T,'\0'};
byte high_str[]={hi_T, hi_A, hi_C, hi_O, hi_T,'\0'};
byte key=0;


void main_screen(void) {
    int i,j;
    _graphics(0);
    clrscr();
    POKE(ATTRACT,0);
    load_font();
    cursor(0);         //hide cursor
    screensize (&XSize, &YSize);
    border_left=XSize/2-FWidth/2;
    border_right=XSize/2+FWidth/2;

    // background decorations
    for(i=0;i<XSize;i=i+1) {
        for (j=0;j<MAX_Y;j=j+4){
            cputsxy(i,j, "[" );
            cputsxy(i,j+1,">" );
            cputsxy(i,j+2,"[" );
            cputsxy(i,j+3," ");
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

void high_scores_screen() {
    byte i;
    byte col=6;
    _graphics(0);
    clrscr();
    load_font();

    _setcolor_low(0,0x24); //
    _setcolor_low(1,0x2E); // font1
    _setcolor_low(2,0x24); // background
    _setcolor_low(3,0x25); // text window

    if(!load_scores(high_scores, high_names) ) {
        printf("Unable to load high scores ... press any key" );
        cgetc();
        return;
    }

    for (i = 0; i < 5; i++)
    {
        cputcxy(i, 0, rat1[i]);
        cputcxy(i, 1, rat2[i]);
        cputcxy(i, 2, rat3[i]);
    }
    cputsxy(10,0,"    High scores:    " );

    for(i=0; i<NSCORES; i++) {
        gotoxy(col,2*i+3);
        printf("%d - %s",high_scores[i],high_names[i]);
    }
}

byte locate(byte X, byte Y) {
    return line_buffer[Y][X];
}

void play_sound_rat(void) {
    int j,i=0;
    for (i=0; i<144; i++) {
        sound(0,144-i,10,8);
        for (j=0; j<50; j++);
    }
    sound(0,0,0,0);
    _setcolor_low(1,0xFF);
    sound(0,20,10,10);
    for (i=0; i<500; i++); //change this for timer delay
    sound(0,0,0,0);
    sound(0,144,23,8);
    for (i=0; i<500; i++); //change this for timer delay
    sound(0,0,0,0);
}

void rat_anim() {
    byte x,i;
    int d;
    x=0;
    while(x<XSize-4) {
        for(i=0;i<5;i++){
            cputcxy(x+i, MAX_Y+1, rat1[i]);
            cputcxy(x+i, MAX_Y+2, rat2[i]);
            cputcxy(x+i, MAX_Y+3, rat3[i]);
        }
        x++;
        for(d=0; d<700; d++);
        for(i=0;i<5;i++){
            cputcxy(x+i, MAX_Y+1, rat4[i]);
            cputcxy(x+i, MAX_Y+2, rat5[i]);
            cputcxy(x+i, MAX_Y+3, rat6[i]);
        }
        x++;
        for(d=0; d<700; d++);
        for(i=0;i<5;i++){
            cputcxy(x+i, MAX_Y+1, rat7[i]);
            cputcxy(x+i, MAX_Y+2, rat8[i]);
            cputcxy(x+i, MAX_Y+3, rat9[i]);
        }
        x++;
        for(d=0; d<700; d++);
    }
    cputsxy(XSize-4, MAX_Y+1, "     ");
    cputsxy(XSize-4, MAX_Y+2, "     ");
    cputsxy(XSize-4, MAX_Y+3, "     ");
}

// When make a word, rat eats the last line
void rat_routine() {
    byte i,x;
    strncpy(buffer,line_buffer[MAX_Y-1],FWidth-2);
    for(i=MAX_Y-1; i>1; i--) {
        strncpy(temp,line_buffer[i-1],FWidth-2);
        //clean current line
        strncpy(line_buffer[i],temp,FWidth-2);
        //restore line
        cputsxy(border_left+1, i,temp);
    }
    //cvlinexy (border_right,2, MAX_Y-2);
    //drop bottom line
    buffer[FWidth-2]='\0';
    cputsxy(border_left+1, MAX_Y+1 ,buffer);
    sleep(1);
    for (x=0; x<border_left; x++)
        cputsxy(x, MAX_Y+2, " Rat ate the taco" );
    sleep(1);
    play_sound_rat();
    rat_anim();
     for (x=0; x<border_left+10; x++)
        cputsxy(x, MAX_Y+2,BLANK_LINE );
    cputsxy( border_left+1, MAX_Y+1, BLANK_LINE );
}

void print_taco(int line,int tdelay) {
    int i;
    if (line>=1) {
        cputsxy(xcord+border_left+1, line-1, blank);
        cputsxy(xcord+border_left+1, line-1, lo_bits);
        cputsxy(xcord+border_left+1, line, hi_bits);
    }
    for (i=0; i<tdelay/2; i++) {
    }
    if (line>=1) cputsxy(xcord+border_left+1, line-1, blank);
    cputsxy(xcord+border_left+1, line, bits);
    for (i=0; i<delay/2; i++) {
    }
}

/*Draw the falling piece in screen, add X for high levels*/
void draw_line (byte line) {
    int i;
    unsigned char J=0;
    if (line>0 && line<MAX_Y+1) {
        cputsxy(xcord+border_left+1, line-1, blank);
        line_buffer[line-1][xcord]=' ';
        line_buffer[line-1][xcord+1]=' ';
    }

    if (line==0) {
        int r = rand() % 4;
        //tacobot sends the occasional X
        delay=MAX_DELAY;
        strncpy(bits, tacostr+r, 2);
        strncpy(lo_bits, low_str+r, 2);
        strncpy(hi_bits, high_str+r, 2);
        if (score>=20) {
            if (rand()%10<=2) {
                i=rand()%2;
                bits[i]='X';
                lo_bits[i]=' ';
                hi_bits[i]=' ';
            }
        }
    }

    key=PEEK(KBCODE);
    J=PEEK(STICK0);

    line_buffer[line][xcord]=' ';
    line_buffer[line][xcord+1]=' ';
    if(key!=255) {
        POKE(KBCODE,255);
        cputsxy(xcord+border_left+1, line, blank);
        if (line>1) cputsxy(xcord+border_left+1, line-1, blank);
        if(key==KEY_PLUS || key==KEY_A ||  key==KEY_LEFT) {
            if (xcord>0 && locate(xcord-1,line)==' ')  {
                xcord--;
            }
            delay=MAX_DELAY;
        }

        if(key==KEY_ASTERISK || key==KEY_D || key==KEY_RIGHT) {
            if (xcord<border_right-border_left-3 && locate(xcord+2,line)==' ')  {
                xcord++;
            }
            delay=MAX_DELAY;
        }
        if(key==KEY_EQUALS || key==KEY_S || key==KEY_DOWN ) {
            delay=200;
        }
        if(key==KEY_DASH || key==KEY_W || key==KEY_UP) {
            delay=MAX_DELAY;
        }
        key=255;
    }
    print_taco(line,delay);
    line_buffer[line][xcord]=bits[0];
    line_buffer[line][xcord+1]=bits[1];
}

void splash_screen(void) {
    _graphics(7+16);
    _setcolor_low(0,0x24); //mouse face
    _setcolor_low(1,0x2E); // tortilla,
    _setcolor_low(2,0xEA); //border, letters
    if (read_sunraster("TACOBOT.BMP") ==1) {
        grmode(2);
        (void) bordercolor (COLOR_BLUE);
        cputsxy(6,2, "TACOBOT");
        printf("%s","         Press START key");
    } else {
        sound(0, 100, 11, 12);
        sleep(1);
        sound(0,0,0,0);
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

void play_sound_taco(void) {
    int i=0;
    sound(0,144,10,8);
    for (i=0; i<1000; i++) {
        _setcolor_low(1,(unsigned char) i);
    }
    sound(0,0,0,0);
    _setcolor_low(1,0xFF);
    sound(0,20,20,10);
    for (i=0; i<500; i++); //TODO: change this for timer delay
    sound(0,0,0,0);
    sound(0,144,23,8);
    for (i=0; i<500; i++);
    sound(0,0,0,0);
}

void print_score() {
    cputsxy(border_right+1,0,"<<<<<<<<<<");
    gotoxy(border_right+1,1);
    printf("Tacos: %d ",score);
    cputsxy(border_right+1,2,"<<<<<<<<<<");
    if (score>=15) {
        _setcolor_low(2,0x36);
    }
}

/*
Remove the word "taco" from grid
Set score if word was formed

*/
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
                strncpy(cline,line_buffer[i],FWidth);
                print_score();
                score=score+1*found;
                found++;
                //for high scores, increase speed
                MAX_DELAY=MAX_DELAY-score*5;
                if(MAX_DELAY<=400) MAX_DELAY=400;
                cputsxy(position+border_left+1,i,"    ");
            }
            if (found!=0) {
                play_sound_taco();
                rat_routine();
            }
        }
        found=0;
    }
}

void wait_start() {
    int i;
    int key=0;
    while (key!=6 || PEEK(STRIG0)==0) {
        key = PEEK(0xD01f);
        for (i=0; i<500; i++);
    }
}

void game_over_screen() {
    byte i,c,ch,cchar=0;
    byte name[NSIZE+1];
    byte col=6;
    cputsxy(7, 9,  "============================");
    cputsxy(7, 10, "=                          =");
    cputsxy(7, 11, "=        GAME OVER         =");
    cputsxy(7, 12, "=                          =");
    cputsxy(7, 13, "= PRESS ENTER TO TRY AGAIN =");
    cputsxy(7, 14, "=                          =");
    cputsxy(7, 15, "============================");
    cgetc();
    i=NSCORES-1;
    while (score>high_scores[i])
    {
        i--;
    }
    if (i>=NSCORES-1) return;
    high_scores_screen();
    cursor(1);         //show cursor
    cputsxy(4,0," YOU MADE IT TO THE HALL OF FAME !!" );
    cputsxy(4,1," Use keyboard to enter your name " );
    //name=scanf();
    strncpy(name,"     ",NSIZE);
    name[NSIZE]='\0';
    while(ch<5) {
        gotoxy(col,2*i+3);
        printf("%d - %s",score,name);
        c=255;
        while(--c) {
            _setcolor_low(1,0xc); // font1
        }
        _setcolor_low(1,0x2E); // font1
        cchar=cgetc();
        if(cchar<='z' && cchar>='!') {
            name[ch]=cchar;
            if (ch<4) ch++;
        }
        if(cchar==155) {
            ch=5;
        }
        if(cchar==126) {
            name[ch]=' ';
            cputcxy(' ',col+ch,2*i+3);
            ch--;
            if (ch>=5) ch=0;
        }
        cputcxy(name[ch],col+ch,2*i+3);
        gotoxy(col+ch,2*i+3);
    }
    for(c=NSCORES-1;c<i;c--) {
        high_scores[c]=high_scores[c-1];
        strncpy(high_names[c],high_names[c-1],NSIZE);
    }
    strncpy(high_names[i],name,NSIZE);
    high_scores[i]=score;
    cursor(0);         //hide cursor
    save_scores(high_scores, high_names);
}

int main (void) {
    byte end=0;
    byte line=0;
    splash_screen();
    wait_start();
    while(1) {
        line=0;
        end=0;
        init();
        high_scores_screen();
        sleep(5);
        _randomize();
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
        game_over_screen();
    }
    return EXIT_SUCCESS;
}
