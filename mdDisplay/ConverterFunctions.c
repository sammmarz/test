/**
 \file
Файл содержит функции для индикации сигналов тревог на экране
*/

#include "ConverterFunctions.h"

/** \brief Массив для временного хранения индексов символов шрифта перед выводом на экран*/
CHAR_TYPE bufIndx[25];
char bufChar[25];

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция формирует массив индексов символов шрифта, соответствующих цифрам переданного числа.
 \param [in] integer число которое нужно отобразить на экране
 \param [in] sumDigits количество цифр в числе
 \param [out] bufIndx массив индексов символов шрифта соответствующих цифрам
 \param [in] positionZeroInFont позиция символа 0 в символах шрифта
 \param [in] positionSpaceInFont позиция пробела в символах шрифта
 \param [in] flags дополнительные параметры
*/
void getIndxFontFromDigits(int integer, int sumDigits, CHAR_TYPE *bufIndx, int positionZeroInFont,
                           int positionSpaceInFont, int flags)
{
    for(int i = sumDigits-1; i>=0; --i)
    {
        //берём младшую цифру
        bufIndx[i] = integer % 10;
        //отбрасываем младшую цифру
        integer /= 10;
        //учитываем позицию 0 в буффере символов символы цифр должны располагаться последовательно по возрастающей
        bufIndx[i] += positionZeroInFont;
    }

    if(flags & ZERO_TO_SPACE)
    {
        for(int i=0; i<sumDigits; ++i)
        {
            //хотя бы один 0 должен присутствовать
            if(i == (sumDigits-1))
            {
                break;
            }

            if(bufIndx[i] == positionZeroInFont)
            {
                bufIndx[i] = positionSpaceInFont;
            }
            else
            {
                //заменяем на пробелы только старшие нули в числе
                break;
            }
        }
    }
}

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция формирует массив индексов символов шрифта, соответствующих переданной строке.

 !!! Функция корректно работает только со строками с символами кодировки ASCII в диапазоне 32-127 (латиница, цифры и ряд основных символов) !!!

 \param [in] string строка которую нужно отобразить на экране
 \param [in] sumChar количество символов в строке
 \param [out] bufIndx массив индексов символов шрифта соответствующих символам в переданной строке
*/
void getIndxFontFromString(char *string, int sumChar, CHAR_TYPE *bufIndx)
{
    for(int i = sumChar-1; i>=0; --i)
    {
        //прописываем индекс символа с учетом смещения относительно кодировки ASCII
        bufIndx[i] = string[i] - OFFSET;
    }
}
