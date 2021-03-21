#include <stdio.h>
#include <unistd.h>  //Used for UART
#include <fcntl.h>   //Used for UART
#include <termios.h> //Used for UART
#include "crc_crc16.h"
#include <string.h>
#include "uart.h"
typedef unsigned char uchar;

float get_temp(unsigned char command)
{
    int uart0_filestream = -1;

    uart0_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY); //Open in non blocking read/write mode
    if (uart0_filestream == -1)
    {
        // printf("Erro - Não foi possível iniciar a UART.\n");
    }
    else
    {
        // printf("UART inicializada!\n");
    }
    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD; //<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);

    unsigned char tx_buffer[20];
    unsigned char *p_tx_buffer;
    unsigned short crc;
    p_tx_buffer = &tx_buffer[0];
    *p_tx_buffer++ = 0x01;
    *p_tx_buffer++ = 0x23;
    *p_tx_buffer++ = command;
    *p_tx_buffer++ = 2;
    *p_tx_buffer++ = 4;
    *p_tx_buffer++ = 6;
    *p_tx_buffer++ = 5;

    crc = calcula_CRC(&tx_buffer[0], 7);
    unsigned char bytes[2];
    // printf("%X %X %X\n", tx_buffer[0], tx_buffer[1], tx_buffer[2]);
    bytes[0] = (crc >> 8);
    bytes[1] = crc;
    // printf("%d %d\n", bytes[0], bytes[1]);
    *p_tx_buffer++ = bytes[1];
    *p_tx_buffer++ = bytes[0];
    // printf("Buffers de memória criados!\n");

    if (uart0_filestream != -1)
    {
        // printf("Escrevendo caracteres na UART ...");
        int count = write(uart0_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
        if (count < 0)
        {
            // printf("UART TX error\n");
        }
        else
        {
            // printf("escrito.\n");
        }
    }

    usleep(1000000);

    //----- CHECK FOR ANY RX BYTES -----
    if (uart0_filestream != -1)
    {
        // Read up to 255 characters from the port if they are there
        unsigned char rx_buffer[256];
        unsigned char rx_buffer2[4];
        float f;

        int rx_length = read(uart0_filestream, (void *)rx_buffer, 255); //Filestream, buffer to store in, number of bytes to read (max)
        if (rx_length < 0)
        {
            // printf("Erro na leitura.\n"); //An error occured (will occur if there are no bytes)
        }
        else if (rx_length == 0)
        {
            // printf("Nenhum dado disponível.\n"); //No data waiting
        }
        else
        {
            //Bytes received
            rx_buffer[rx_length] = '\0';

            //Verifica CRC
            short bytesCrc;
            memcpy(&bytesCrc, &rx_buffer[rx_length - 2], 2);
            short crc_response = calcula_CRC(&rx_buffer[0],7);
            if(crc_response!=bytesCrc){
                printf("CRC Inválido\n");
                return -1;
            }

            int j = 0;
            for (int i = 0; i < rx_length; i++)
            {
                if (i >= 3 && i < 7)
                {

                    // printf("%d\n", rx_buffer[i]);
                    rx_buffer2[j] = (int)rx_buffer[i];
                    // printf("rx[%d] = %d\n", j, rx_buffer2[j]);
                    // printf("%d\n", rx_buffer2[j]);
                    j++;
                }
            }
            memcpy(&f, &rx_buffer2, sizeof(f));
        }

        close(uart0_filestream);
        return f;

    }
    return 0.0;
}