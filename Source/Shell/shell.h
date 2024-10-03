#define help_buffer 10

typedef void (*FuncPointer)();

char commands[][2][256] = {
    {"ARGTEST","A filler command for a testing argument parsing"},
    {"CLEAR","Clears the screen and returns to home"},
    {"EXIT","Exits the current running terminal. Returns to process."},
    {"HELP","Receive help on using commands"},
    {"TETRIS","A fully working version of tetris"},
    {"DRIVESEL","Set current ATA drive, for testing!"},
    {"VERSION","Gets current version"},
};
