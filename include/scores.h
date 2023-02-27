#include "byte.h"

/**
 * use Thomas Cherryhomes' convention to save high scores in a fixed sector
*/
#define SCORE_SECTOR 500
//#define SCORE_SECTOR 10
#define SECTOR_SIZE 128
#define DCB 0x300
#define NSCORES 10
#define NSIZE 5


int save_scores(int scores[], byte* names[]);
int load_scores(int scores[], byte* names[]);
byte write_sector(byte* values, byte sector);
byte read_sector(byte* buffer, byte sector);