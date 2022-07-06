/**
 \file
Файл обеспечивает навигацию в меню экрана
*/
#include "DisplayFunctions.h"
#include "mdDisplay.h"

void showGroup0Screen0(int cmd);
void showGroup0Screen1(int cmd);
void showGroup0Screen2(int cmd);
void showGroup1Screen0(int cmd);
void showGroup1Screen1(int cmd);
void showGroup1Screen2(int cmd);
void showGroup2Screen0(int cmd);
void showGroup2Screen1(int cmd);
void showGroup3Screen0(int cmd);
void showGroup4Screen0(int cmd);
void showGroup4Screen1(int cmd);
void showGroup4Screen2(int cmd);

extern bool acceptNewTourBat;
extern uint8_t accessStatus;

static int screenNum;
static int screenNumMax;
static int screenGroupNum;
static int screenGroupNumMax = 3; // 0..3 - 4 screens
int firstScreen;

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция осуществляет переход на отображение того или иного экрана в зависимости
от выбранных групп экранов и номера экрана.
*/
void showScreen(int cmd)
{
    switch (screenGroupNum)
    {
        //состояние
        case 0:
            switch (screenNum)
            {
                //текущее показание
                case 0:
                    showGroup0Screen0(cmd);
                    break;

                //тревоги
                case 1:
                    showGroup0Screen1(cmd);
                    break;
                    
                //черный экран
                case 2:
                    showGroup0Screen2(cmd);
                    break;                   
            }

            break;

        //мгновенное состояние
        case 1:
            switch (screenNum)
            {
                //объемы
                case 0:
                    showGroup1Screen0(cmd);
                    break;

                //Подст. коэф.
                case 1:
                    showGroup1Screen1(cmd);
                    break;

                //Расход
                case 2:
                    showGroup1Screen2(cmd);
                    break;
            }

            break;

        //сервис
        case 2:
            switch (screenNum)
            {
                //прошивка
                case 0:
                    showGroup2Screen0(cmd);
                    break;

                //батарея
                case 1:
                    showGroup2Screen1(cmd);
                    break;
            }

            break;

        /*
                //клапан
                case 3:
                    switch (screenNum)
                    {
                        //клапан
                        case 0:
                            showGroup3Screen0(cmd);
                            break;
                    }

                    break;
        */
        //интерфейсы
        case 3:
            switch (screenNum)
            {
                //Оптический порт
                case 0:
                    showGroup4Screen0(cmd);
                    break;

                //GPRS
                case 1:
                    showGroup4Screen1(cmd);
                    break;

                //Статус модема
                case 2:
                    showGroup4Screen2(cmd);
                    break;
            }

            break;
    }

    // включаем дисплей и выводим буфер на экран
    taskENTER_CRITICAL();
    showDisplayBuffer();
    turnOnDisplay();
    taskEXIT_CRITICAL();
}

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief Функция устанавливаем максимальный номер экрана внутри группы (нумерация с 0).
*/
void setScreenNumMax(void)
{
    //устанавливаем максимальный номер экрана внутри группы (нумерация с 0)
    switch (screenGroupNum)
    {
        //состояние
        case 0:
            screenNumMax = 1;
            // если открыт замок производителя или калибр. разрешаем отображать черный экран
            if(accessStatus & 0xf)
            {
                screenNumMax = 2;
            }
            break;

        //мгновенное состояние
        case 1:
            screenNumMax = 2;
            break;

        //сервис
        case 2:
            screenNumMax = 1;
            break;

        /*
                //клапан
                case 3:
                    screenNumMax = 0;
                    break;
        */
        //интерфейсы
        case 3:
            screenNumMax = 2;
            break;
    }
}

/**
 \ingroup mdDisplayGroup
 \author Shuvalov Sergey
 \brief В зависимости от действий пользователя функция осуществляет переходы в меню навигации,
либо передает команду на обработчик отображенияя экрана.
*/
void navigate(int codeBtn, int cmd)
{
    // экран после включения дисплея
    if (firstScreen == 1)
    {
        firstScreen = 0;
        screenGroupNum = 0;
        screenNum = 0;
        setScreenNumMax();
        showScreen(cmd);
        return;
    }

    //оставляем тот же экран, но режим редактирования
    if (cmd > 0)
    {
        showScreen(cmd);
        return;
    }

    switch (codeBtn)
    {
        case LEFT_BTN:
            if(acceptNewTourBat)
            {
                break;
            }
            //если экран "Тревоги"
            if((screenGroupNum==0) && (screenNum == 1))
            {
                cmd = LEFT_BTN;
                break;
            }

            if(acceptNewTourBat)
                break;
            
            screenNum = 0;
            screenGroupNum--;

            if (screenGroupNum < 0)
            {
                screenGroupNum = screenGroupNumMax;
            }

            setScreenNumMax();
            break;

        case CENTER_BTN:
            if(acceptNewTourBat)
            {
                break;
            }
            
            if (cmd == 0) // не команда
            {
                screenNum++;
            }

            if (screenNum > screenNumMax)
            {
                screenNum = 0;
            }

            break;

        case RIGHT_BTN:
            if(acceptNewTourBat)
            {
                break;
            }
            //если экран "Тревоги"
            if((screenGroupNum==0) && (screenNum == 1))
            {
                cmd = RIGHT_BTN;
                break;
            }

            screenNum = 0;
            screenGroupNum++;

            if (screenGroupNum > screenGroupNumMax)
            {
                screenGroupNum = 0;
            }

            setScreenNumMax();
            break;
    }

    showScreen(cmd);
}

