

//Temp cpeekc in the meantime it is suported by atari
#if !defined(__ATARI__)
#  error This module may only be used when compiling for the Atari!
#endif

extern void __fastcall__ _cpeekc (void); 
extern void __fastcall__ grmode (unsigned char mode);  