/*******************************************************
This program was created by the CodeWizardAVR V4.06a 
Automatic Program Generator
� Copyright 1998-2025 Pavel Haiduc, HP InfoTech S.R.L.
http://www.hpinfotech.ro

Project : Práctica 17 'Matriz de led 7x5'
Version : 1.1
Date    : 12/03/2026
Author  : García Escamilla Bryan Alexos
Company : 
Comments: 


Chip type               : ATmega8535
Program type            : Application
AVR Core Clock frequency: 1.000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 128
*******************************************************/

// I/O Registers definitions
#include <mega8535.h>
#include <delay.h>

// Declare your global variables here
unsigned char modo;
char columnas = 0x01;
char filas = 0;
char indice = 0, numero = 0, letra = 0;
char repetir = 0, renglon = 0;
char desplazamiento = 0;
char buffer_desplazamiento[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

char modo_1[] = {0x7F, 0x7F, 0x7F, 0x7F, 0x7F};

char modo2_3[7][5] = {
    {0x01, 0x01, 0x01, 0x01, 0x01},
    {0x02, 0x02, 0x02, 0x02, 0x02},
    {0x04, 0x04, 0x04, 0x04, 0x04},
    {0x08, 0x08, 0x08, 0x08, 0x08},
    {0x10, 0x10, 0x10, 0x10, 0x10},
    {0x20, 0x20, 0x20, 0x20, 0x20},
    {0x40, 0x40, 0x40, 0x40, 0x40}
};

// Matriz para programar la real
char modo_numero[10][5] = {
    {0x45, 0x49, 0x51, 0x3e, 0x3e}, // 0
    {0x42, 0x7f, 0x40, 0x00, 0x00}, // 1
    {0x61, 0x51, 0x49, 0x46, 0x42}, // 2
    {0x41, 0x49, 0x49, 0x36, 0x22}, // 3
    {0x14, 0x12, 0x7f, 0x10, 0x18}, // 4
    {0x49, 0x49, 0x49, 0x31, 0x4f}, // 5
    {0x49, 0x49, 0x49, 0x32, 0x3e}, // 6
    {0x01, 0x79, 0x05, 0x03, 0x01}, // 7
    {0x49, 0x49, 0x49, 0x36, 0x36}, // 8
    {0x49, 0x49, 0x49, 0x3e, 0x26} // 9
};


char modo_nombre[19][5] = {    
    // Letra G real
    {0x00, 0x00, 0x00, 0x3e, 0x00},
    {0x00, 0x00, 0x3e, 0x41, 0x00},
    {0x00, 0x3e, 0x41, 0x49, 0x00},
    {0x3e, 0x41, 0x49, 0x49, 0x00},
    {0x41, 0x49, 0x49, 0x3a, 0x3e},
    {0x49, 0x49, 0x3a ,0x00, 0x41},
    {0x49, 0x3a, 0x00, 0x00, 0x49},
    {0x3a, 0x00 ,0x00 ,0x00, 0x49},
    {0x00, 0x00, 0x00, 0x00, 0x3a},
    
    // Letra M real
    {0x00, 0x00, 0x00, 0x7f, 0x00},
    {0x00, 0x00, 0x7f, 0x02, 0x00},
    {0x00, 0x7f, 0x02, 0x04, 0x00},
    {0x7f, 0x02, 0x04, 0x02, 0x00},
    {0x02, 0x04, 0x02, 0x7f, 0x7f},
    {0x04, 0x02, 0x7f, 0x00, 0x02},
    {0x02, 0x7f, 0x00, 0x00, 0x04},
    {0x7f, 0x00, 0x00, 0x00, 0x02},
    {0x00, 0x00, 0x00, 0x00, 0x7f}      
};

// Función para obtener la columna actual del número en desplazamiento
char obtener_columna_desplazamiento(char num_index, char col_index) {
    // Si el �ndice de columna esté fuera del rango del n�mero, retornar 0 (columna vacía)
    if (col_index < 0 || col_index >= 5) {
        return 0x00;
    }
    return modo_numero[num_index][col_index];
}

void main(void) {
    // Declare your local variables here
    
    // Input/Output Ports initialization
    // Port A initialization
    // Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
    DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (0<<DDA3) | (0<<DDA2) | (0<<DDA1) | (0<<DDA0);
    // State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
    PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);
    
    // Port B initialization
    // Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
    DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (0<<DDB4) | (0<<DDB3) | (0<<DDB2) | (0<<DDB1) | (0<<DDB0);
    // State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=P Bit1=P Bit0=P 
    PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (1<<PORTB2) | (1<<PORTB1) | (1<<PORTB0);

    // Port C initialization
    // Function: Bit7=In Bit6=In Bit5=In Bit4=Out Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
    DDRC=(0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (1<<DDC4) | (1<<DDC3) | (1<<DDC2) | (1<<DDC1) | (1<<DDC0);
    // State: Bit7=T Bit6=T Bit5=T Bit4=0 Bit3=0 Bit2=0 Bit1=0 Bit0=0 
    PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

    // Port D initialization
    // Function: Bit7=In Bit6=Out Bit5=Out Bit4=Out Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
    DDRD=(0<<DDD7) | (1<<DDD6) | (1<<DDD5) | (1<<DDD4) | (1<<DDD3) | (1<<DDD2) | (1<<DDD1) | (1<<DDD0);
    // State: Bit7=T Bit6=0 Bit5=0 Bit4=0 Bit3=0 Bit2=0 Bit1=0 Bit0=0 
    PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (0<<PORTD3) | (0<<PORTD2) | (0<<PORTD1) | (0<<PORTD0);

    // Timer/Counter 0 initialization
    // Clock source: System Clock
    // Clock value: Timer 0 Stopped
    // Mode: Normal top=0xFF
    // OC0 output: Disconnected
    TCCR0=(0<<WGM00) | (0<<COM01) | (0<<COM00) | (0<<WGM01) | (0<<CS02) | (0<<CS01) | (0<<CS00);
    TCNT0=0x00;
    OCR0=0x00;

    // Timer/Counter 1 initialization
    // Clock source: System Clock
    // Clock value: Timer1 Stopped
    // Mode: Normal top=0xFFFF
    // OC1A output: Disconnected
    // OC1B output: Disconnected
    // Noise Canceler: Off
    // Input Capture on Falling Edge
    // Timer1 Overflow Interrupt: Off
    // Input Capture Interrupt: Off
    // Compare A Match Interrupt: Off
    // Compare B Match Interrupt: Off
    TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<WGM11) | (0<<WGM10);
    TCCR1B=(0<<ICNC1) | (0<<ICES1) | (0<<WGM13) | (0<<WGM12) | (0<<CS12) | (0<<CS11) | (0<<CS10);
    TCNT1H=0x00;
    TCNT1L=0x00;
    ICR1H=0x00;
    ICR1L=0x00;
    OCR1AH=0x00;
    OCR1AL=0x00;
    OCR1BH=0x00;
    OCR1BL=0x00;

    // Timer/Counter 2 initialization
    // Clock source: System Clock
    // Clock value: Timer2 Stopped
    // Mode: Normal top=0xFF
    // OC2 output: Disconnected
    ASSR=0<<AS2;
    TCCR2=(0<<WGM20) | (0<<COM21) | (0<<COM20) | (0<<WGM21) | (0<<CS22) | (0<<CS21) | (0<<CS20);
    TCNT2=0x00;
    OCR2=0x00;

    // Timer(s)/Counter(s) Interrupt(s) initialization
    TIMSK=(0<<OCIE2) | (0<<TOIE2) | (0<<TICIE1) | (0<<OCIE1A) | (0<<OCIE1B) | (0<<TOIE1) | (0<<OCIE0) | (0<<TOIE0);

    // External Interrupt(s) initialization
    // INT0: Off
    // INT1: Off
    // INT2: Off
    MCUCR=(0<<ISC11) | (0<<ISC10) | (0<<ISC01) | (0<<ISC00);
    MCUCSR=(0<<ISC2);

    // USART initialization
    // USART disabled
    UCSRB=(0<<RXCIE) | (0<<TXCIE) | (0<<UDRIE) | (0<<RXEN) | (0<<TXEN) | (0<<UCSZ2) | (0<<RXB8) | (0<<TXB8);

    // Analog Comparator initialization
    // Analog Comparator: Off
    // The Analog Comparator's positive input is
    // connected to the AIN0 pin
    // The Analog Comparator's negative input is
    // connected to the AIN1 pin
    ACSR=(1<<ACD) | (0<<ACBG) | (0<<ACO) | (0<<ACI) | (0<<ACIE) | (0<<ACIC) | (0<<ACIS1) | (0<<ACIS0);
    SFIOR=(0<<ACME);

    // ADC initialization
    // ADC disabled
    ADCSRA=(0<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (0<<ADPS1) | (0<<ADPS0);

    // SPI initialization
    // SPI disabled
    SPCR=(0<<SPIE) | (0<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);

    // TWI initialization
    // TWI disabled
    TWCR=(0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWEN) | (0<<TWIE);

    while (1) {
        // Place your code here
        modo = PINB & 0x07;
        switch (modo) {
            case 0: // LINEAS HORIZONTALES HACIA LA IZQUIERDA
                filas = ~modo_1[indice];
                delay_ms(100);
                break;
            case 1: // LINEAS VERTICALES HACIA ABAJO
                filas = ~modo2_3[renglon][indice];
                repetir++;
                if (repetir == 5) {
                    renglon++;
                    repetir = 0;
                }
                if (renglon == 7) renglon = 0;
                break;
            case 2: // PUNTITO HACIA LA IZQUIERDA-ABAJO
                filas = ~modo2_3[renglon][indice];
                delay_ms(100);
                repetir++;
                if (repetir == 5) {
                    renglon++;
                    repetir = 0;
                }
                if (renglon == 7) renglon = 0;
                break;
            case 3: // NÚMEROS DEL 0 AL 9
                {
                    char num_actual = buffer_desplazamiento[desplazamiento / 5];
                    char col_actual = indice;
                    
                    if (col_actual >= 0 && col_actual < 5) {
                        filas = ~obtener_columna_desplazamiento(num_actual, col_actual);
                    } else {
                        filas = 0xFF; // Columna vac�a
                    }
                    
                    repetir++;
                    if (repetir >= 100) {
                        repetir = 0;
                        desplazamiento++;
                        if (desplazamiento >= 50) {
                            desplazamiento = 0;
                        }
                    }
                }
                break;
            default:
                filas = ~modo_nombre[letra][indice];
                repetir++;

                if (repetir == 60) {
                    repetir = 0;
                    letra++;
                }
                if (letra == 19) {
                    letra = 0;
                }
        }

        // Neceario para hacer los corrimientos
        switch (columnas) {
            case 0x01: 
                columnas = 0x02;
                break;
            case 0x02:
                columnas = 0x04;
                break;
            case 0x04:
                columnas = 0x08;
                break;
            case 0x08:
                columnas = 0x10;
                break;
            default:
                columnas = 0x01;
        }

        indice++;
        if (indice == 5) {
            indice = 0;
        }

        PORTC = columnas;
        PORTD = filas;
        delay_ms(2);
    }
}
