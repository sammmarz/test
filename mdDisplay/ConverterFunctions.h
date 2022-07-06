#ifndef CONVERTERFUNCTIONS_H_INCLUDED
#define CONVERTERFUNCTIONS_H_INCLUDED

#include "Fonts5x8.h"

enum CONVERTER_FLAGS
{
    ZERO_TO_SPACE = 1,
    OFFSET = 32
};
extern CHAR_TYPE bufIndx[25];
extern char bufChar[25];

void getIndxFontFromDigits(int integer, int sumDigits, CHAR_TYPE *bufIndx, int positionZeroInFont,
                           int positionSpaceInFont, int flags);
void getIndxFontFromString(char *string, int sumChar, CHAR_TYPE *bufIndx);
#endif // CONVERTERFUNCTIONS_H_INCLUDED
