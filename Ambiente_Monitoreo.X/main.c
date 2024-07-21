/*
 ***********************AMBIENTE DE MONITOREO**************************
 * Descripción: Se tiene 1 ambiente de monitoreo y 2 salones. Estos salones 
 * se comunican con la sala de monitoreo mediante el estándar RS-485, la entrada
 * de personas a sus respectivos salones se da por el pase de tarjeta por el modulo lector
 * RFID, en este caso en Proteus se representa cada ingreso de persona por el Botón "ENTRADA". 
 * La salida de personas es representado por el botón "SALIDA". Además, se tiene un botón de pánico 
 * en cada salón y en la sala de monitoreo (Botón Emergencia). En el ambiente de monitoreo 
 * se podrá visualizar la cantidad de alumnos que hay por aula, para poder cambiar  
 * el aula que se mostratá en pantalla se tiene un botón en el pin RB0.
 * 
 * ::::SALONES:::
 * RC0 = Entrada de personas
 * RC1 = Salida de personas
 * RB1= Botón de Emergencia 
 * 
 * ::::AMBIENTE DE MONITOREO:::
 * RB0 = Cambio de salón que se desea mostrar en el LCD
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

#define aula1 101
#define aula2 102
#define Total_Personas 5
#define EMERGENCIA a
#define codigo_emergencia 55

char codigo_aula;
char aux;
char a;

void InitPort(void){
 ADCON1bits.PCFG = 0xFF;                         //Pines en digital
 TRISEbits.RE0=0;                                //Indicador de Emergencia
}
void Init_Int (void)
{     
    /*Interrupción Por cambio de salón*/
    RCONbits.IPEN = 1;              //Habilitando prioridades.
    INTCONbits.INT0IE = 1;          // Habilitar INT0 
    INTCONbits.INT0IF = 0;          //Bandera 0
    INTCON2bits.INTEDG0 = 0;        //Flanco descendete
    INTCONbits.GIEH=1; 
   
    /*Interrupción Emergencia*/
    INTCON3bits.INT1E = 1;        // Habilitar INT1 
    INTCON3bits.INT1F = 0;        //Bandera 0
    INTCON2bits.INTEDG2 = 0;      //Flanco descendete
    INTCON3bits.INT1IP=0;        //Low priority
      
    INTCONbits.GIEL=1;           
}

void main(void) {
    char i;                                //i= número de personas 
    char texto [10];
    char temp;                             //variable temporal para el cambio de aula
    
    InitPort();
    LCD_Init();
    Init_Int();
    USART_Init (9600);                     // a 9600 baudios
    
    codigo_aula=101;
    aux=1;
    temp=90;
    a=0;
    LCD_gotoXY(0,0);
    LCD_Cadena("*SALA MONITOREO*");
    __delay_ms(100);
    
    while(1)
    {   
       if (EMERGENCIA==0)
       {          
        i = USART_RX();
        __delay_ms (50); 
        
        if (aux==1)
        {  
            LCD_gotoXY(0,0);
            LCD_Cadena("*SALA MONITOREO*");
            __delay_ms(50);
             USART_TX(temp);
             __delay_ms (50);             
             aux=0;
             USART_TX(codigo_aula);
             __delay_ms (150); 
        }
        
        if (i<Total_Personas) 
        {   
             if(codigo_aula==101)     
             {  
                 sprintf  (texto,"salon 1: %d         ",i);   
             }    
                 else{sprintf  (texto,"salon 2: %d      ",i);}
                 LCD_gotoXY(1,0);
                 LCD_Cadena(texto);
                 __delay_ms (50);
        }

        else
        {  
             LCD_gotoXY(1,0);                         
             if(codigo_aula==101)     
            {
                LCD_Cadena("salon 1: lleno");
            }     
             else {LCD_Cadena("salon 2: lleno");}
             __delay_ms (50);            
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
           while(EMERGENCIA == 0xff)
           {
             i = USART_RX();
             __delay_ms (50); 
             LATA=i;             
             aux=1;
             USART_TX(codigo_emergencia);
             __delay_ms (150);
           }      
      }
    }    
}

/*Interrupción para el cambio de salón mostrado en pantalla*/
void __interrupt(high_priority) INT_RB0 (void)
{ 
        while(PORTBbits.RB0 == 1);               // el bit sigue siendo 1 seguirá en este bucle      
        aux = 1;    
        if (codigo_aula == aula1)
        {codigo_aula = aula2;}
        else {codigo_aula=aula1;}
        INTCONbits.INT0IF = 0;    
}

/*Interrupción para la activación o desactivación de "Emergencia"*/
 void __interrupt(low_priority) INT_RB1(void)
{ 
   if(INTCON3bits.INT1F ==1)   
   {
        EMERGENCIA = ~EMERGENCIA;
        LATEbits.LATE0=~LATEbits.LATE0;                      
        INTCON3bits.INT1F = 0; 
   }
}  
