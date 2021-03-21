#include <stdio.h>
#include <unistd.h>  //Used for UART
#include <fcntl.h>   //Used for UART
#include <termios.h> //Used for UART
#include <wiringPiI2C.h>
#include <wiringPi.h>
#include <softPwm.h> /* include header file for software PWM */
#include <stdlib.h>
#include <string.h>
#include "crc_crc16.h"
#include "lcd.h"
#include "uart.h"
#include "bme280.h"
#include "gpio.h"
#include "pid.h"
#include <pthread.h>
#include <signal.h>
#include <time.h>

typedef unsigned char uchar;

// Define some device parameters
#define I2C_ADDR 0x27  // I2C device address
#define LINE1 0x80     // 1st line
#define LINE2 0xC0     // 2nd line
#define RESISTOR 23    //GPIO 23
#define VENTOINHA 24   //GPIO 24
#define RESISTORPWM 4  //PWM 4
#define VENTOINHAPWM 5 //PWM 5

int fd; // seen by all subroutines
int saida = 1;
int input_user = 0;
double tempRef, userRef;
pthread_t threads[2];
FILE *ptr;

//BME280
#define DELAY 1000000
// Width of the graph (128 - 40)
#define WIDTH 88

char buffer[10];

void time_generate()
{
    time_t current_time;
    struct tm *time_info;

    time(&current_time);
    time_info = localtime(&current_time);

    strftime(buffer, 10, "%H:%M:%S", time_info);
}

void *get_all_temp()
{
    while (saida)
    {
        int i;
        int T, P, H; // calibrated values

        for (i = 0; i < 120; i++) // read values twice a second for 1 minute
        {
            bme280ReadValues(&T, &P, &H);
            T -= 150; // for some reason, the sensor reports temperatures too high
        }

        // printf("BME280 device successfully opened.\n");
        double tempInt, tempAmb;
        if (!input_user)
        {
            tempRef = get_temp(0xC2);
        }
        else{
            tempRef=userRef;
        }
        tempInt = get_temp(0xC1);
        tempAmb = (double)T / 100.0;
        if (tempInt == -1 || tempRef == -1 || tempInt < tempAmb || tempInt > 100.0 || tempRef < tempAmb || tempRef > 100.0)
        {
            continue;
        }
        pid_atualiza_referencia(tempRef);

        double controle = pid_controle(tempInt);
        // printf("controle = %lf\n", controle);
        gpio_controle(controle);

        lcdLoc(LINE1);
        typeln("TI:");
        typeFloat(tempInt);
        typeln(" TR:");
        typeFloat(tempRef);
        lcdLoc(LINE2);
        typeln("TA:");
        typeFloat(tempAmb);
        delay(500);
        // printf("TR = %3.2f\n", tempRef);
        // printf("TI = %3.2f\n", tempInt);
        // printf("TA = %3.2f\n", tempAmb);

        time_generate();
        ptr = fopen("temp.csv", "a");
        if (ptr == NULL)
        {
            printf("Error ao abrir arquivo!");
            exit(1);
        }
        fprintf(ptr, "%s,", buffer);
        fprintf(ptr, "%3.2f,", tempRef);
        fprintf(ptr, "%3.2f,", tempInt);
        fprintf(ptr, "%3.2f,", tempAmb);
        fprintf(ptr, "%3.2f\n", controle);
        fclose(ptr);
    }
    return NULL;
}

void *get_input()
{
    while (saida)
    {
        printf("1 - Mudar TR\n");
        printf("2 - Retornar a TR do potenciometro\n");
        int opcao;
        scanf("%d", &opcao);
        if (opcao == 1)
        {
            printf("Enviar uma nova TR: ");
            scanf("%lf", &userRef);
            input_user = 1;
        }
        else if (opcao == 2)
        {
            printf("Retornar a TR do potenciometro: ");
            input_user = 0;
        }
        usleep(1000000);
        system("clear");
    }
    return NULL;
}

void sig_handler(int signum)
{
    saida = 0;
    ClrLcd(); // clear LCD
    toggle(RESISTORPWM, 0);
    toggle(VENTOINHAPWM, 0);
    printf("\nEncerrando\n");
    exit(0);
}

int main()
{

    lcd_init(); // setup LCD
    gpio_init();
    int i = bme280Init(1, 0x76);
    if (i != 0)
    {
        return 0; // problem - quit
    }

    signal(SIGINT, sig_handler);

    usleep(1000000); // wait for data to settle for first read
    ptr = fopen("temp.csv", "w");
    if (ptr == NULL)
    {
        printf("Error ao abrir arquivo!");
        exit(1);
    }
    fprintf(ptr, "DATETIME, TR, TI, TA, ATUADORES\n");
    fclose(ptr);
    pid_configura_constantes(5, 1, 5);
    pthread_create(&threads[0], NULL, get_all_temp, NULL);
    pthread_create(&threads[1], NULL, get_input, NULL);
    pthread_join(threads[0], NULL);
    pthread_join(threads[1], NULL);

    return 0;
}
