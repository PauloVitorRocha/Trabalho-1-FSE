
#include <wiringPi.h>
#include <stdio.h>
#include <softPwm.h>

#define ON 100
#define OFF 0

#define RESISTORPWM 4
#define VENTOINHAPWM 5

#ifndef GPIO_H_
#define GPIO_H_

void gpio_init();
void gpio_controle(double controle);
void toggle(int componente, int status);

#endif /* GPIO_H_ */