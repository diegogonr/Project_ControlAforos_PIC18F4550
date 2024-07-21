/*
 * File:   lib_USART.c
 * Author: 
 *
 */


#include <xc.h>
#include "Config.h"
#include "lib_USART.h"


void USART_Init (long BAUD) {
    /*Configuraci�n de puertos del pic18f4550 */
    TRISCbits.RC6 = 0;            //TX (salida)
    TRISCbits.RC7 = 1;            //RX (entrada)
            
    /*Config para los BAUDIOS*/
    SPBRG = (unsigned char)(((_XTAL_FREQ)/BAUD)/64-1);   //f�rmula de datasheet  
    
    /*Config USART*/
    TXSTAbits.BRGH = 0;           //velocidad baja    
    TXSTAbits.SYNC = 0;           //asincrono 
    RCSTAbits.SPEN = 1;           //habilitar los pines de USART 
    
    /*Tx*/
    TXSTAbits.TX9 = 0;            //8 bits de dato
    TXSTAbits.TXEN = 1;           //transmisi�n: ON
    
    /*RX*/
    RCSTAbits.RC9 = 0;            //8 bits de dato
    RCSTAbits.CREN = 1;           //recepci�n: ON
    
}

void USART_TX (char data){
    while (TXSTAbits.TRMT==0);                  //�el registro TSR se encuentra a�n lleno?
    TXREG = data;                 //env�a la data    
}

char USART_RX (){
       if(((~RCSTAbits.FERR) & (~RCSTAbits.OERR))== 0)
    {
     /*reinicializaci�n del sistema por error */
     RCSTAbits.SPEN = 0;            
     __delay_ms(1);
      RCSTAbits.SPEN = 1;                
    }
    return RCREG;                //recibe la data
}
