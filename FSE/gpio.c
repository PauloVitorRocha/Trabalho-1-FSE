#include "gpio.h"

void gpio_init()
{
    wiringPiSetup();
}

void toggle(int componente, int status)
{
    pinMode(componente, OUTPUT);
    softPwmCreate(componente, 0, 100);
    softPwmWrite(componente, status);
}

void gpio_controle(double controle)
{
    // Controle > 0
    if (controle > 0)
    {
        toggle(VENTOINHAPWM, 0);
        toggle(RESISTORPWM, controle);
        // Controle < -40
    }
    else if (controle < -40)
    {
        toggle(RESISTORPWM, 0);
        toggle(VENTOINHAPWM, controle * -1);
        // Controle < 0 && Controle > -40
    }
    else{
        toggle(RESISTORPWM, 0);
        toggle(VENTOINHAPWM, 0);
    }
}
