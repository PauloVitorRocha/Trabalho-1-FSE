all:
	cd FSE/ && \
	gcc trabalho1.c pid.c uart.c lcd.c crc_crc16.c bme280.c gpio.c -lwiringPi -lpthread -o trab1;