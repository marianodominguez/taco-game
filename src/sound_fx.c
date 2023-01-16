#include <string.h>
#include <peekpoke.h>
#include "byte.h"
#include "stdlib.h"
#define RTCLK 20

byte *intro1="V2T180O3(((1CCg$b))(($E$F$b$D))((FFC$F))((GGDF)))";
byte *intro2="V2O4N&&&&C$bfG";
byte *intro3="V5N9&&&&&&&&(9EGF7b5CD))";

//voice volume,pitch,distortion,duration
int voice[4][4];
voice[0]={0,0,10,0};
voice[1]={0,0,10,0};
voice[2]={0,0,10,0};
voice[3]={0,0,10,0};

//Implements music same as zx spectrum 128, except for midi strings
// http://fizyka.umk.pl/~jacek/zx/doc/man128/sp128p10.html

int play(byte* a, byte* b, byte* c, byte* d) {
    unsigned int n[4];
    unsigned int i=0;
    n[0]=strlen(a);
    n[1]=strlen(b);
    n[2]=strlen(c);
    n[3]=strlen(d);
    byte notes_to_play=1;
    while(notes_to_play) {
        notes_to_play=0;
        if( i<n[0]_) {
            notes_to_play=1;
            _play_note(0,a[i]);
        }
    }
}

void _play_note(byte v, byte* note) {
    byte b[2];
    if (note=='V') {
        strncpy(b,note,2);
        voice[v][0]=atoi(b);
    }
}