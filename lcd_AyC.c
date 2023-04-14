#include "lcd_AyC.h"
#include <mc9s08sh8.h>

#ifndef LCD_DB4
#define LCD_DB4   PTAD_PTAD0
#endif
 
#ifndef LCD_DB5
#define LCD_DB5   PTAD_PTAD1
#endif
 
#ifndef LCD_DB6
#define LCD_DB6   PTAD_PTAD2
#endif
 
#ifndef LCD_DB7
#define LCD_DB7   PTAD_PTAD3
#endif
 
#ifndef LCD_E
#define LCD_E     PTCD_PTCD0
#endif
 
#ifndef LCD_RS
#define LCD_RS    PTCD_PTCD1
#endif
 
// Define the LCD type and details
#define lcd_type 2        // 0=5x7, 1=5x10, 2=2 lines 
#define lcd_line_two 0x40 // LCD RAM address for the 2nd line 

// Send a nibble to the display
void lcdSendNibble(char nibble) { 
	// Note:  !! converts an integer expression 
    // to a boolean (1 or 0). 
    LCD_DB4 = !!(nibble & 1); 
    LCD_DB5 = !!(nibble & 2);  
    LCD_DB6 = !!(nibble & 4);    
    LCD_DB7 = !!(nibble & 8);    
     
    delay(1); 
    LCD_E = 1; 
    delay(2); 
    LCD_E = 0; 
} 
 
 
// Send a byte to the display. 
void lcdSendByte(int address, int n) { 
	LCD_RS = 0; 
    delay(60);  
 
    if(address) {
        LCD_RS = 1;
    }   
    else { 
        LCD_RS = 0;
    }    
 
    delay(1); 
    LCD_E = 0; 
 
    lcdSendNibble(n >> 4); 
    lcdSendNibble(n & 0xf); 
} 
 
 
// Initialise the display.  This is the outline of what is
// required:
// 1 : Wait 20ms for the display to stabilise after power-up
// 2 : Set RS and ENABLE low
// 3 : Wait at least 200us
// 4 : Send nibble cmd 0x03
// 5 : Wait at least 200us 
// 6 : Send nibble cmd 0x03 again
// 7 : Wait at least 200us
// 8 : Send nibble cmd 0x03 a third time
// 9 : Wait at least 200us
// 10: Send nibble cmd 0x02 to enable 4-bit mode
// 11: Wait 5ms
// 12: Send byte cmd 0x40 (4-bit communications, 2 lines, 5x8)
// 13: Send byte cmd 0x0c (Turn the display on)
// 14: Send byte cmd 0x01 (Clear and home the display)
// 15: Send byte cmd 0x06 (Set left-to-right direction)
void initLCD (void) { 
    int i; 
	PTADD=0xF;	//Puerto A, pines de 0 a 3 como salida
	PTCDD=0x3;	//Puerto C, pines de 0 y 1 como salida
    LCD_RS = 0; 
    LCD_E = 0; 
    
 
    delay(15); 
 
    for(i = 0; i < 3; i++) { 
        lcdSendNibble(0x03); 
        delay(5); 
    } 
 
    lcdSendNibble(0x02); // Home the cursor
 
    lcdSendByte(0, 0x40); // 4-bit, 2 line, 5x8 mode
    delay(5);
    lcdSendByte(0, 0x0c); // Turn on display
    delay(5);
    lcdSendByte(0, 0x01); // Clear & home display
    delay(5);
    lcdSendByte(0, 0x06); // Left to Right
    delay(5);
} 
 
 
// Position the cursor on the display.  (1, 1) is the top left
void lcdGoto(int x, int y) { 
    int address; 
    address = (y != 1)? lcd_line_two : 0;
    address += x-1; 
    lcdSendByte(0, 0x80 | address); 
} 
 
 
// Write characters to the display
void lcdPutC(char c) { 
    switch(c) { 
        case '\f': 
            lcdSendByte(0,1);    // '\f' (que representa la instrucción de borrar la pantalla)
            delay(2);            
            break; 
     
        case '\n': 
            lcdGoto(1,2);        // '\n' (que representa la instrucción de salto de línea)
            break; 
     
        case '\b': 
            lcdSendByte(0,0x10); // '\b' (que representa la instrucción de retroceso)
            break; 
     
        default: 
            lcdSendByte(1,c); 
            break; 
    } 
} 

void printLCD(char vec[]) {  //Imprime una cadena de caracteres en el LCD
	int i;
	for (i=0; vec[i] != '\0'; i++){		
		if(vec[i] == '_'){
			lcdGoto(1,2);
			i++;
		}
		if(i == 16){
			lcdGoto(1,2);	
		}	
		lcdPutC(vec[i]);	
	}
}
