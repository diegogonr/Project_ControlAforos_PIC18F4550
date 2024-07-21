/*
  Librería de LCD para 4 bits
 * PINES DE DATOS: RD0-RD1-RD2-RD3

 */

/* DATA_PORT define el puerto de datos del LCD */
 #define DATA_PORT      		PORTD            
 #define TRIS_DATA_PORT 		TRISD

/* define los pines de control*/
 #define RW_PIN   LATDbits.LATD6   		/* PORT for RW */
 #define TRIS_RW  TRISDbits.TRISD6    	/* TRIS for RW */

 #define RS_PIN   LATDbits.LATD5   		/* PORT for RS */
 #define TRIS_RS  TRISDbits.TRISD5    	/* TRIS for RS */

 #define E_PIN    LATDbits.LATD4  		/* PORT for D  */
 #define TRIS_E   TRISDbits.TRISD4    	/* TRIS for E  */

/* Comandos*/
#define Clear_LCD       0x01
#define LINEA1          0x80       
#define LINEA2          0xC0     

/* Función  de inicialización */
void LCD_Init (void);
void LCD_Comando (unsigned char cmd);
void LCD_gotoXY(int x,int y);
void LCD_Cadena ( const char *);  // apunta a varios objetos, pero no puede modificarlos.
void LCD_Data (char);
void Clear_Linea (char x,char y);


