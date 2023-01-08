#include "byte.h"

int save_scores(int scores[], byte* names[]);
int load_scores(int scores[], byte* names[]);
byte write_sector(byte* values, byte sector);
byte read_sector(byte* buffer, byte sector);