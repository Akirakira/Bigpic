#include <p18f14k50.h>
#include <delays.h>
#include <usart.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _XTAL_FREQ 4000000

//-------------- CONFIG ------------------------
#pragma config PWRTEN = OFF, BOREN  = OFF, BORV   = 30
#pragma config WDTEN  = ON, WDTPS  = 1024
#pragma config STVREN = ON
#pragma config FOSC   = IRC  //  ??????
#pragma config PLLEN  = ON,  CPUDIV = NOCLKDIV, USBDIV = OFF
#pragma config FCMEN  = OFF, IESO   = OFF, HFOFST = OFF
#pragma config LVP    = OFF, XINST  = OFF, BBSIZ  = OFF
#pragma config CP0    = OFF, CP1  = OFF, CPB  = OFF, CPD  = OFF
#pragma config WRT0   = OFF, WRT1 = OFF
#pragma config WRTB   = OFF, WRTC = OFF, WRTD = OFF
#pragma config EBTR0  = OFF, EBTR1 = OFF, EBTRB = OFF

#pragma code

unsigned char rx_data,i,count=0,sleepCount=0,adCount=0;
unsigned char adH=0,adL=0;
unsigned char sends[12] = "you send : ";
unsigned char aok[4] = "AOK";
unsigned char adstart[3] = "AD";
unsigned char slee[6] = "SLEEP";

unsigned char receiv[] = "";
int v = 0;
//******************************
void main(void){
    char i;
    OSCCON = 0b01010010;   // IRC->4Mhz
    PORTA = 0x00;
    //PORTB = 0x00;
    PORTC = 0x00;
    //TRISA = 0x00;   // PortA
    //TRISB = 0x00;   // PortB
    TRISC = 0b00001000;   // PortC

    ANSEL  = 0;                // Port ... if this nothing, do no RX.
    ANSELH = 0;
    // AD setting
    ADCON2 = 0b10101100;
    ADCON1 = 0x00;
    ADCON0 = 0b00011101;
    ANSELbits.ANS7 = 1;

    baudUSART (               //baudRate
        BAUD_IDLE_RX_PIN_STATE_HIGH &
        BAUD_IDLE_TX_PIN_STATE_HIGH &
        BAUD_16_BIT_RATE &
        BAUD_WAKEUP_OFF &
        BAUD_AUTO_OFF);
    OpenUSART(               //UART???
        USART_TX_INT_OFF &
        USART_RX_INT_OFF &
        USART_ASYNCH_MODE &
        USART_EIGHT_BIT &
        USART_CONT_RX &
        USART_BRGH_HIGH,
        103);
    
    PORTCbits.RC4 = 0;
    PORTCbits.RC5 = 0;
    PORTCbits.RC7 = 0;

    //IDLE mode
    OSCCONbits.IDLEN = 1;
    
    WriteUSART('W');

    //SSPADD = 9;                 // Board Rate 100kHz
    while(1){
        PORTCbits.RC4 = 0;
        PORTCbits.RC5 = 0;
        PORTCbits.RC7 = 0;
        while (!DataRdyUSART());

            //RCIF = 0;
            //putsUSART("you send :\r\n");
            i=ReadUSART();
            PORTCbits.RC4 = 1;
            PORTCbits.RC5 = 1;
        if(i!='\n'){
            receiv[v] = i;
            v++;

        }else{
            
            ClrWdt();

            // -> send
            for(char i=0;i<=strlen(sends)-1;i++){
                WriteUSART(sends[i]);
                 __delay_ms(3);
            }

            //recive = AOK? SLEEP?
            for(char i=0;i<=strlen(receiv)-1;i++){
                WriteUSART(receiv[i]);
                if(aok[i] == receiv[i]){
                    count++;
                }
                if(slee[i] == receiv[i]){
                    sleepCount++;
                }
                if(adstart[i] == receiv[i]){
                    adCount++;
                }
                 __delay_ms(3);
            }

            if(count >= 3){
                WriteUSART('#');
            }
            if(sleepCount >= 5){
                WriteUSART('S');
                SLEEP();
                ClrWdt();
                WriteUSART('!');
            }
            if(adCount >= 2){
                char i = 1;
                while(i){
                    ADCON0bits.GO = 1;
                    while(ADCON0bits.GO);
                    adH = ADRESH;
                    adL = ADRESL;
                    if(adH >= 2){
                        PORTCbits.RC7 = 1;
                        for(int n=0;n>100;n++){
                            __delay_ms(10);
                        }
                    }
                }
            }
              v = 0;
              count = 0;
              sleepCount = 0;
              adCount = 0;
              __delay_ms(3);
        }
    }
    
}
