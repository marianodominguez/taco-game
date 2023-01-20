#include <string.h>
#include <peekpoke.h>
#include "byte.h"
#include <stdlib.h>
#include <stdio.h>

#define RTCLK 20
#define PITCH 1
#define VOL 0
#define DISTORTION 2
#define DURATION 3

// byte *intro1="V2T180O3(((1CCg$b))(($E$F$b$D))((FFC$F))((GGDF)))";
// byte *intro2="V2O4N&&&&C$bfG";
// byte *intro3="V5N9&&&&&&&&(9EGF7b5CD))";

// byte *intro1="A2bbccc";
// byte *intro2="V3&&&";
// byte *intro3="F7FFFF&abcd";

//voice volume,pitch,distortion,duration
int voice[4][4] = {
{0,0,10,1},
{0,0,10,1},
{0,0,10,1},
{0,0,10,1}
};


//TODO: Implement music same as zx spectrum 128, except for midi strings
// http://fizyka.umk.pl/~jacek/zx/doc/man128/sp128p10.html

byte _parse_note(byte v, byte* note) {
    byte *b;
    byte command, value, idx=1;
    int presult;

    presult=strncpy(b,note,2);
    sscanf(b,"%c%2d", command, &value);

    if(presult==EOF) {
        command='_';
        presult=sscanf(b,"%d", &value);
    }
    if(presult==EOF) {
        value=0;
        command=b[0];
    }
    if (command=='V') {
        voice[v][VOL]=value;
        idx=3;
    }
    if (command>='a' || command<='g') {
        //TODO calculate freq
        voice[v][PITCH]=10+command;
        value=0;
        idx=1;
    }
    if (command>='A' || command<='G') {
        //TODO calculate freq
        voice[v][PITCH]=100+command;
        value=0;
        idx=1;
    }
    if (command=='_') {
        //TODO: calculate timer
        voice[v][DURATION]=100*value;
        idx=1;
    }
    printf("[%c],%d\n", command, value);
    return idx;
}

int play(byte* a, byte* b, byte* c, byte* d) {
    unsigned int n[4];
    unsigned int i=0;
    byte notes_to_play=1;

    n[0]=strlen(a);
    n[1]=strlen(b);
    n[2]=strlen(c);
    n[3]=strlen(d);

    while(notes_to_play) {
        notes_to_play=0;
        if( i < n[0] ) {
            notes_to_play=1;
            i+=_parse_note(0,&a[i]);
        }
    }
    return 0;
}
