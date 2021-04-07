#include "BootManager.h"
#include "GlobalDefinitions.h"

/*
*	\brief
*
*/
CBootManager::CBootManager()
{
    pinMode(GPIO_RX, INPUT);

#if PRINT_LINES == 0
    pinMode(GPIO_TX, OUTPUT);
    digitalWrite(GPIO_TX, HIGH);
#endif
}



/*
*	\brief
*
*/
bool CBootManager::IsConfigBoot()
{
    bool pinState = digitalRead(GPIO_RX) == LOW ? true : false;

    Println("GPIO3 'Rx' = ");
    Println(digitalRead(GPIO_RX));

    return pinState;
}
