/*
 **********************************SALÓN 2***********************************
 * Descripción: AULA 1 que tiene por código identificador el # 101. El salón
 * hará el conteo de ingreso y salida de las personas, estos datos serán mostrados 
 * en una pantalla LCD 16X2, cada vez que se ingrese se activará el torniquete girando
 * en un sentido, así mismo a la salida de cada persona girará en el otro sentido. El
 * torniquete será simulado mediante un motor paso a paso. 
 * El salón cuenta con un botón de pánico y un indicador led de este mismo. 
 * 
 * ::::SALÓN 2:::
 * RC0 = Entrada de personas
 * RC1 = Salida de personas
 * RB1= Botón de Emergencia 
 * 
 */

#include <xc.h>
#include "Config.h"
#include "lib_USART.h"
#include "MiLCD.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#define codigo 102
#define Total_Personas 5                         // número de personas totales por salón 
#define a EMERGENCIA
#define codigo_emergencia 55   
#define OFF 0
#define ON 0xff

void Tx_i (void);
void Giro_IN (void);
void Giro_OUT (void);

char i=0;                                          //i= número de personas 
char codigo_aula=0;
char aux=0;

unsigned long cod1=0x125a;                    //codigo de referencia de cada tarjeta 
char texto [32];                              //10 registros x 8 bits
char cont [10];

char motor=0;
char a=0;                                     //variable de emergencia 
char const HORARIO[4] = {  0b10001000,
                           0b11000000,
                           0b10010000,
                           0b10100000};


    
    
void InitPort(void)
{
    ADCON1bits.PCFG = 0xFF;                         //Pines en digital
    TRISCbits.RC0 = 1;                              //Entrada de personas                       
    TRISCbits.RC1 = 1;                              //Salida de personas
    TRISCbits.RC2=0;                                //Control de envío de datos
    TRISEbits.RE0=0;                                //Indicador de Emergencia
    TRISB=0b00000111; 
}

void Init_Int (void)
{     
    INTCON3bits.INT1E = 1;        // Habilitar INT1 
    INTCON3bits.INT1F = 0;        //Bandera 0
    INTCON2bits.INTEDG2 = 0;      //Flanco descendete
    INTCONbits.GIE = 1;   
}

void main(void) {
  InitPort();
  Init_Int();
  LCD_Init();
  USART_Init (9600);                             // USART a 9600 baudios
    
  LCD_gotoXY(0,0);                   
  LCD_Cadena("***Bienvenido***");
  __delay_ms(100);
  LCD_gotoXY(1,0);                   
  LCD_Cadena("Pase su tarjeta");
  __delay_ms(100);
  while (1)
  {     
    codigo_aula = USART_RX();
    __delay_ms (50);
    Tx_i();
                                 
    if (codigo_aula==codigo_emergencia) 
        {EMERGENCIA=0xff;
         LATEbits.LATE0=1;}
    
        if(EMERGENCIA==OFF)
        {
            if((PORTCbits.RC0 == 1) && (i<Total_Personas))   //se ejecuta cuando detecta el button 1 (entrada de persona)
                {                              
                    __delay_ms (50);                         //efecto rebote
                    while(PORTCbits.RC0 == 1);               //el bit sigue siendo 0, seguirá en este bucle  
                    LCD_gotoXY(0,0);                   
                    LCD_Cadena("****SALON-1****");
                    __delay_ms(100); 
                    sprintf  (texto,"UID:%x    ",cod1);        // cod1 convierte en una cadena
                    LCD_gotoXY(1,0);
                    LCD_Cadena(texto);
                    i++;                                     //aumento en 1   
                    Giro_IN();                               //Giro de entrada
                    sprintf(cont, "   # %d ", i);               //conversión de binario a decimal     
                    LCD_gotoXY(1,10); 
                    LCD_Cadena(cont);
                    __delay_ms(100);
                }

            else if ((PORTCbits.RC1 == 1) && (i>0) )         //se ejecuta cuando detecta el button 2 (salida de persona)
                {
                    __delay_ms (50);                         //efecto rebote
                    while(PORTCbits.RC1 == 1);               // el bit sigue siendo 0, seguirá en este bucle
                    LCD_gotoXY(0,0);                   
                    LCD_Cadena("****SALON-1****");
                    __delay_ms(100);
                    sprintf  (texto,"UID:%x    ",cod1);        // cod1 convierte en una cadena
                    LCD_gotoXY(1,0);                     
                    LCD_Cadena(texto);
                    i--;                                     //disminución en 1  
                    Giro_OUT ();                             //Giro de salida
                    sprintf(cont, "   # %d ", i);               //conversión de binario a decimal     
                    LCD_gotoXY(1,10); 
                    LCD_Cadena(cont);
                    __delay_ms(100); 
                }
            
            if (i==Total_Personas) 
                {                   
                    LCD_gotoXY(1,0);
                    LCD_Cadena("***AULA LLENA***");
                    __delay_ms(100); 
                }
        }    
        else 
        {
          LCD_Comando (Clear_LCD); 
          LCD_gotoXY(0,0);
          LCD_Cadena("***EMERGENCIA***");
          __delay_ms (100);
          LCD_gotoXY(1,0);
          LCD_Cadena("*Puerta abierta*");
          __delay_ms (100);
           while(EMERGENCIA == ON)
           {
               codigo_aula = USART_RX();
              __delay_ms (50);

            if ((codigo_aula!=codigo_emergencia)&& (aux==0)) 
                {EMERGENCIA=OFF;
                LATEbits.LATE0=0;}
               if (i>0)
               {
                Giro_OUT ();
                i--;
                Tx_i();                      
                __delay_ms (150);
               }
           }
        }         
  } 
}

void __interrupt() INT_isr(void)
{
    EMERGENCIA = ~EMERGENCIA;
    aux=~aux;
    LATEbits.LATE0=~LATEbits.LATE0;                      
    INTCON3bits.INT1F = 0; 
}

/*Verificador de código de aula y envío de dato*/
void Tx_i (void)
{
    if(codigo_aula==codigo)
    {
        LATCbits.LATC2=1;
        USART_TX(i);
        __delay_ms (150); 
    }

    else 
    {
        LATCbits.LATC2=0;
    }
 
}   

/*Giro de motor en sentido de entrada*/
void Giro_IN (void)
{
    if (motor<3)
    {
        motor++;
        LATB=HORARIO[motor];
    }
    else 
    {
        motor=0;
        LATB=HORARIO[motor];                        
    }

}

/*Giro de motor en sentido de salida*/
void Giro_OUT (void)
{
    if (motor>0)
    {
        motor--;             
        LATB=HORARIO[motor];
    }
    else 
    {
        motor=3;
        LATB=HORARIO[motor];                        
    }
}
