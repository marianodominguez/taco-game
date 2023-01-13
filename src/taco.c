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

#define HIGH_LEVEL_SCORE 15 // increase difficulty at this score
#define HS_COL 15  //High scores display column
#define KBCODE 764 // Keyboard address
#define FWidth 13  //Playfield width
#define MAX_Y 18   // maximum line on playfield
#define STRIG0 644  //Trigger buttoe
#define STICK0 632  //Joystick address
#define ATTRACT 77  // Atari attract mode
#define hi_T 0x11   // Atascii values for the half scroll letter
#define hi_A 0x13   // top part
#define hi_C 0x15
#define hi_O 0x17
#define lo_T 0x19   // bottom part
#define lo_A 0x14
#define lo_C 0x16
#define lo_O 0x18

byte BLANK_LINE[]="            "; // length=Fwidth, use for delete rows
// use zero-terminated rows
byte xcord;            //current piece x coordinate
byte border_left;      //calculated borders for field
byte border_right;
byte line_buffer[MAX_Y][FWidth+1]; // reserve 1 extra byte for termnate strings with zero
byte XSize, YSize;                  // screen size
const byte tacostr [] = "TACOT";          // Letters for pieces
const byte blank []   = "  ";             // delete curreent piece
byte bits []    = "  ";                   //falling piece
byte lo_bits [3];                         // space for middle frames of piece
byte hi_bits [3];
int score       = 1;                      // current player score
int delay;                                // delay for falling speed
int MAX_DELAY=2000;                       // Initial delay level 1
int high_scores[NSCORES];                 // array of high scores
byte high_names[NSIZE+2][NSCORES];        // high score names

byte buffer[FWidth+1];                    //buffer for reading/writing rows
byte rat1[5]={32,32,0,32,32};             // ascii character for rat frames
byte rat2[5]={32,1,2,16,32};
byte rat3[5]={32,32,5,4,32};

byte rat4[5]={32,32,32,32,32};
byte rat5[5]={32,6,7,8,32};
byte rat6[5]={32,32,9,10,32};

byte rat7[5]={32,32,0,32,32};
byte rat8[5]={32,1,2,16,32};
byte rat9[5]={32,32,12,4,32};

byte temp[FWidth+1];                    //Temporary value to copy arrays to string
byte low_str[]={lo_T, lo_A, lo_C, lo_O, lo_T,'\0'};  //lower part letters
byte high_str[]={hi_T, hi_A, hi_C, hi_O, hi_T,'\0'}; //upper part letters
byte key=0;                         //pressed key

/**
 * game screen, initializes graphics 0 after splash screen, draws border and
 * game background
*/
void main_screen(void) {
    int i,j;
    _graphics(0);
    clrscr();  // graphics implementation does not always clears screen
    POKE(ATTRACT,0);  //Disable attect mode (for joystick play)
    load_font();
    cursor(0);         //hide cursor, this is a global value
    screensize (&XSize, &YSize);    //calculates screen size and borders
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

    //Draw scene, use graphics character for drawing box
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

/**
 * Load scores form disk secto
 * show high scores
*/
void high_scores_screen() {
    byte i;
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
        gotoxy(HS_COL,2*i+3);
        printf("%d  %s",high_scores[i], high_names[i]);
    }
}
/**
 * Get character at coordinates, x,y. in array columns are inverted
*/
byte locate(byte X, byte Y) {
    return line_buffer[Y][X];
}
/**
 * Rat animation sound effect
*/
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
/**
 * Rat eats taco animation
*/
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

/**
 * When making a word, rat eats the last line
*/
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
/**
 * Piece falling animation, use half-characters to simulate smooth movement
*/
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

/**
 * Draw the falling piece in screen, add a random X for high levels
 * get user's input and move pieace
 */

void draw_line (byte line) {
    int i,xprob;
    unsigned char J=0; //joystick
    //clear previous piece location
    if (line>0 && line<MAX_Y+1) {
        cputsxy(xcord+border_left+1, line-1, blank);
        line_buffer[line-1][xcord]=' ';
        line_buffer[line-1][xcord+1]=' ';
    }
    // generate piece randomly
    if (line==0) {
        int r = rand() % 4;
        //tacobot sends the occasional X, more after 30
        delay=MAX_DELAY;
        strncpy(bits, tacostr+r, 2);
        strncpy(lo_bits, low_str+r, 2);
        strncpy(hi_bits, high_str+r, 2);
        if (score>HIGH_LEVEL_SCORE) {
            xprob=22;
            if(score >30) xprob=35;
            if (rand()%100<xprob) {
                i=rand()%2;
                bits[i]='X';
                lo_bits[i]=' ';
                hi_bits[i]=' ';
            }
        }
    }

    key=PEEK(KBCODE); // pressed key
    J=PEEK(STICK0);

    line_buffer[line][xcord]=' ';
    line_buffer[line][xcord+1]=' ';
    if(key!=255) {
        POKE(KBCODE,255); //clear keyboard buffer
        cputsxy(xcord+border_left+1, line, blank);
        if (line>1) cputsxy(xcord+border_left+1, line-1, blank);
        if(key==KEY_PLUS || key==KEY_A ||  key==KEY_LEFT || J==11) {
            if (xcord>0 && locate(xcord-1,line)==' ')  {
                xcord--;
            }
            delay=MAX_DELAY;
        }
        if(key==KEY_ASTERISK || key==KEY_D || key==KEY_RIGHT || J==7) {
            if (xcord<border_right-border_left-3 && locate(xcord+2,line)==' ')  {
                xcord++;
            }
            delay=MAX_DELAY;
        }
        if(key==KEY_EQUALS || key==KEY_S || key==KEY_DOWN || J==13) {
            delay=200;
        }
        if(key==KEY_DASH || key==KEY_W || key==KEY_UP || J==14 ) {
            delay=MAX_DELAY;
        }
        key=255;
    }
    print_taco(line,delay); // draw piece animation
    line_buffer[line][xcord]=bits[0];
    line_buffer[line][xcord+1]=bits[1];
}


/**
 * Call splash screen, uses routine to load a bmp (no RLE, color table, 8bpp)
*/
void splash_screen(void) {
    _graphics(7+16);
    _setcolor_low(0,0x24); //mouse face
    _setcolor_low(1,0x2E); // tortilla,
    _setcolor_low(2,0xEA); //border, letters

    if (read_bitmap("TACOBOT.BMP") ==1) {
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
/**
 * Find outif there is a character block under the coordinates, it needs
 * 2 blanks to move piece
 */
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

/**
 * Initialize game values , clear character buffer
*/
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

/**
 * Taco found sound effect
*/
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

/**
 * Print current player score. do not use printf
*/
void print_score() {
    cputsxy(border_right+1,0,"<<<<<<<<<<");
    cputsxy(border_right+1,1, "Tacos: ");
    cputs(itoa(score,temp,10));
    cputsxy(border_right+1,2,"<<<<<<<<<<");
    //for higher level score, change to red screen
    if (score>=HIGH_LEVEL_SCORE) {
        _setcolor_low(2,0x36);
    }
}

/*
* Remove the word "taco" from grid
* Set score if word was formed
*/
void eat_tacos() {
    int i,speedinc=3;
    char found=1;
    byte j;
    char *idx;
    int position;
    byte cline[FWidth+1];

    //for each row detect and erase the word "TACO"
    for(i=MAX_Y;i>=0;i--) {
        strcpy(cline,line_buffer[i]);
        found=1;  // flag if a taco has been found
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
                //for higher scores, increase speed
                if (score>20) speedinc=4;
                MAX_DELAY=MAX_DELAY-score*speedinc;
                if(MAX_DELAY<=500) MAX_DELAY=500;  //Do not go too fast
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

/**
 * wait for start key to be pressed or joystick button
*/
void wait_start() {
    int i;
    int key=0;
    while (key!=6 || PEEK(STRIG0)==0) {
        key = PEEK(0xD01f);
        for (i=0; i<500; i++);
    }
}

/**
 * Game over, if user is a high score, prompt and save them to disk sector
*/
void game_over_screen() {
    byte i,c,ch,cchar=0;
    byte name[NSIZE+1];
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
        gotoxy(HS_COL,2*i+3);
        printf("%d <<%s<<",score,name);
        gotoxy(HS_COL,2*i+3);
        c=255;
        while(--c) {
            _setcolor_low(1,0xc); // font1
        }
        _setcolor_low(1,0x2E); // font1
        cchar=cgetc();
        //it is a printable character
        if(cchar<='z' && cchar>='!') {
            name[ch]=cchar;
            if (ch<4) ch++;
        }
        //user pressed enter, exit
        if(cchar==155) {
            ch=5;
        }
        //user pressed delete
        if(cchar==126) {
            name[ch]=' ';
            cputcxy(' ',HS_COL+4+ch,2*i+3);
            ch--;
            if (ch>=5) ch=0;
        }
        cputcxy(name[ch],HS_COL+4+ch,2*i+3);
        gotoxy(HS_COL+4+ch,2*i+3);
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

/**
 * Main game loop
*/
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
