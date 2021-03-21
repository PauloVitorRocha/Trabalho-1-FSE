#ifndef LCD_H_
#define LCD_H_

// float to string
void typeFloat(float myFloat);

// int to string
void typeInt(int i);

// clr lcd go home loc 0x80
void ClrLcd(void);

// go to location on LCD
void lcdLoc(int line);

// out char to LCD at current position
void typeChar(char val);

// this allows use of any size string
void typeln(const char *s);

void lcd_byte(int bits, int mode);

void lcd_toggle_enable(int bits);

void lcd_init();

#endif /* LCD_H_ */