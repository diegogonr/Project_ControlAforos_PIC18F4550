/*
 * File:   MiLCD.c
 * Author: GRUPO 3 -> CURSO: INTEGRADOR 1 
 * Universidad Tecnológica del Perú  
 * N:numero de Lineas                 ---> 2
 * F:configuración de los caracteres  ---> 5x8
 * BF: bandera ocupada
 * pantalla (D), el cursor (C) y el parpadeo (B)
 * I / D = INCREMENTO(1) Y DECREMENTO (0) y S = STATIC (0).
 * 
 * Created on 1 de noviembre de 2020, 02:49 PM
 */

#include <xc.h>
#include "Config.h"
#include "MiLCD.h"

void LCD_Init (void)
{
   
   TRIS_RW =0;
   TRIS_RS =0;
   TRIS_E  =0;
   DATA_PORT &= 0xf0;         //  
   TRIS_DATA_PORT &= 0xf0;    //  salida RD0-RD3  
   RW_PIN = 0;                //escritura
   RS_PIN = 0;                // instrucción
   E_PIN = 0;
   __delay_ms (15);           //según datasheet
  
   LCD_Comando (0x30);
   __delay_ms (5);
   LCD_Comando (0x30);
   __delay_us (100);  
   LCD_Comando (0x32);                //configura a 4 bits
   __delay_us(150);       
   __delay_us(150);                   //t_espera --> bf desocupe ( teoria: > 53us )
   LCD_Comando (0x28);                // NF
   __delay_us(150);                  
   LCD_Comando (0x08);                // D, C, B = OFF
   __delay_us(150);   
   
   LCD_Comando (Clear_LCD);                // limpiar el LCD
   __delay_ms(5);    
   
   LCD_Comando (0x06);                // I(1), S(0)
   __delay_us(150);     
   LCD_Comando (0x0C);                // D=1,C=0,B=0
   __delay_us(150);     

   return;  
}

void LCD_Comando (unsigned char cmd)
{     __delay_us(150);   

    TRIS_DATA_PORT &= 0xf0;  
    DATA_PORT &= 0xf0;

  //para los 4 bits + significativos
    DATA_PORT = DATA_PORT | ((cmd>>4)& 0x0f); 
    //F0 =1111 0001 ---> 0000 1111 & 0000 1111  
   
    RW_PIN = 0;                           // modo escritura
    RS_PIN = 0;                           // modo comando
  //18 TCY  -->  18*(4/48MHZ)  -->  1.5 us     --> 15
    __delay_us (15);
    E_PIN = 1;                             //habilita el LCD
    __delay_us (15);
    E_PIN = 0;                             //deshabilita el LCD
    
    
  //para los 4 bits - significativos   
    DATA_PORT &= 0xf0;    
    DATA_PORT = DATA_PORT | (cmd & 0x0f); 
    __delay_us (15);
    E_PIN = 1;                             //habilita el LCD
    __delay_us (15);
    E_PIN = 0;                            //deshabilita el LCD
    
    TRIS_DATA_PORT |= 0x0f;              //coloco como entrada los pines de dato 
    
   return; 

}

void LCD_gotoXY (int x,int y)
{   __delay_us(150);   

    if ( x>0 )
    {
        LCD_Comando((LINEA2)+y);
    }
    else 
    {
        LCD_Comando((LINEA1)+y);
    }
    return;
}

void LCD_Cadena ( const char *dato)    //apunta a varios objetos, pero no puede modificarlos.
{
    while (*dato)
    {     
       __delay_us(150);  
        LCD_Data (*dato);
        dato++;    
    }   
        
}
       
void LCD_Data (char data)
{
   __delay_us(150);  
    DATA_PORT &= 0xf0;                   //limpio los datos
    TRIS_DATA_PORT &= 0xf0; 
    
    DATA_PORT = DATA_PORT | ((data>>4) & 0x0f);    //envía 4 MSB
    
    RS_PIN = 1;                           // modo caracter
    RW_PIN = 0;                           // modo escritura
  //18 TCY  -->  18*(4/48MHZ)  -->  1.5 us
    __delay_us (15);
    E_PIN = 1;                             //habilita el LCD
    __delay_us (15);
    E_PIN = 0;                             //deshabilita el LCD
            
    DATA_PORT &= 0xf0;                   //limpio los datos
    DATA_PORT |= (data&0x0f);            //envia los 4 LSB
     
    __delay_us (15);
    E_PIN = 1;                             //habilita el LCD
    __delay_us (15);
    E_PIN = 0;  
    
    TRIS_DATA_PORT |= 0x0f;    
    
    return ;
}

void Clear_Linea (char x,char y )
{
        if ( x>0 )
    {        
        LCD_gotoXY (1,y);
        LCD_Cadena ("                ");
        __delay_ms(150); 
    }
    else 
    {
        LCD_gotoXY (0,y);
        LCD_Cadena ("                ");
        __delay_ms(150);    }
    return;
    
}