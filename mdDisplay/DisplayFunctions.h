#ifndef DisplayFunctions
#define DisplayFunctions

#include "Fonts5x8.h"

enum DISPLAY_FLAGS
{
    REVERS_COLOR = 1,
    ADD_BEG_COL = 2,
    ADD_END_COL = 4,
    CHAR_PLUS_COL = 8
};

void initDisplay(void);
void showStatus(void);
void turnOnDisplay(void);
void turnOffDisplay(void);
void clearDisplayBuffer(void);
void setDisplayBuffer(unsigned char ch);
void showDisplayBuffer(void);
void setBlackPage(int newPage);
void setPage(int pageNumber);
void setCol(int colNumber);
void writeByteToDisplay(unsigned char ch);
void writeChar5x8(int indxChar, int flags);
void writeString5x8(const CHAR_TYPE *indxBuf, int bufLength, int flags);
void writeStringWarnings(const CHAR_TYPE *icon, int sumByte, int flags);
void writeChar12x16(int indxChar, int flags, int pageF);
void writeString12x16(CHAR_TYPE *indxBuf, int bufLength, int flags);

void showGroup0ScreenDebug(int cmd);

void showStart0ScreenDebug(int cmd);

#endif