#include <stdio.h>
#include <conio.h>
#include <atari.h>
#include "sound_fx.h"
#include <peekpoke.h>

int main(void)
{
    int i;
    byte v1[]="V10abcfedgV8ABCDEFG";
    printf("parsing sound string \n");
    play(v1,"","","");
    for(i=0;i<1000;i++);
    play("", v1,"","");
    cgetc();
    return 0;
}