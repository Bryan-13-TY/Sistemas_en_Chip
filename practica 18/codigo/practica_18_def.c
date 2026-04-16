/*******************************************************
This program was created by the CodeWizardAVR V4.06a 
Automatic Program Generator
� Copyright 1998-2025 Pavel Haiduc, HP InfoTech S.R.L.
http://www.hpinfotech.ro

Project : Práctica 18 'Pantalla LCD 16x2'
Version : 1.0
Date    : 14/03/2026
Author  : García Escamilla Bryan Alexis
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

// Delay functions
#include <delay.h>

// Alphanumeric LCD functions
#include <alcd.h>

#define BTN_CAMBIAR_MODO PIND.0
#define BTN_AJUSTAR_HORA PIND.1
#define BTN_AJUSTAR_MIN PIND.2
#define BTN_AJUSTAR_SEG PIND.3

// Declare your global variables here

// Variables de temperatura
float temperatura_celsius;
int temperatura_entera;

// Control de tiempo del programa
int contador_reloj, contador_antirebote, tiempo_antirebote;

// Estados de botones
bit btn_presionado_anterior, btn_presionado_actual;

// Variables para mostrar temperatura
unsigned char temp_unidades, temp_decenas, temp_decimas, lectura_adc;

// Variables de reloj
unsigned char segundos = 0, minutos = 0, horas = 0;

// Variables de fecha
unsigned char dia_actual = 22, mes_actual = 8;

// Variables de año
unsigned short siglo = 19, anio = 95;

// Control de modo (editar hora / editar fecha)
unsigned char modo_edicion;

const char ASCII_CERO = 48; //codigo ascii

// ADC Voltage Reference: AVCC pin
#define ADC_VREF_TYPE ((0<<REFS1) | (1<<REFS0) | (1<<ADLAR))

// Read the 8 most significant bits
// of the AD conversion result
// Read Voltage=read_adc*(Vref/256.0)
unsigned char read_adc(unsigned char adc_input) {
    ADMUX=adc_input | ADC_VREF_TYPE;
    // Delay needed for the stabilization of the ADC input voltage
    delay_us(10);
    // Start the AD conversion
    ADCSRA|=(1<<ADSC);
    // Wait for the AD conversion to complete
    while ((ADCSRA & (1<<ADIF))==0);
    ADCSRA|=(1<<ADIF);
    return ADCH;
}

unsigned char leer_adc_promedio(unsigned char canal) {
    unsigned int suma = 0;
    unsigned char i;

    for(i = 0; i < 20; i++)
    {
        suma += read_adc(canal);
        delay_ms(2);
    }

    return suma / 20;
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
    // Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=In Bit2=Out Bit1=Out Bit0=Out 
    DDRB=(1<<DDB7) | (1<<DDB6) | (1<<DDB5) | (1<<DDB4) | (0<<DDB3) | (1<<DDB2) | (1<<DDB1) | (1<<DDB0);
    // State: Bit7=0 Bit6=0 Bit5=0 Bit4=0 Bit3=T Bit2=0 Bit1=0 Bit0=0 
    PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

    // Port C initialization
    // Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
    DDRC=(0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
    // State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
    PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

    // Port D initialization
    // Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
    DDRD=(0<<DDD7) | (0<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
    // State: Bit7=P Bit6=P Bit5=P Bit4=P Bit3=P Bit2=P Bit1=P Bit0=P 
    PORTD=(0<<PORTD7) | (0<<PORTD6) | (0<<PORTD5) | (0<<PORTD4) | (1<<PORTD3) | (1<<PORTD2) | (1<<PORTD1) | (1<<PORTD0);

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

    // ADC initialization
    // ADC Clock frequency: 500.000 kHz
    // ADC High Speed Mode: Off
    // ADC Auto Trigger Source: Software
    // Only the 8 most significant bits of
    // the AD conversion result are used
    ADMUX=ADC_VREF_TYPE;
    ADCSRA=(1<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (0<<ADPS1) | (1<<ADPS0);
    SFIOR=(1<<ADHSM) | (0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);

    // SPI initialization
    // SPI disabled
    SPCR=(0<<SPIE) | (0<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);

    // TWI initialization
    // TWI disabled
    TWCR=(0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWEN) | (0<<TWIE);

    // Alphanumeric LCD initialization
    // Connections are specified in the
    // Project|Configure|C Compiler|Libraries|Alphanumeric LCD menu:
    // RS: PORTB Bit 0
    // RD: PORTB Bit 1
    // EN: PORTB Bit 2
    // D4: PORTB Bit 4
    // D5: PORTB Bit 5
    // D6: PORTB Bit 6
    // D7: PORTB Bit 7
    // Characters/line: 16
    lcd_init(16);
    contador_reloj = 0;
    contador_antirebote = 0;
    tiempo_antirebote = 20;

    while (1) {
        //delay_ms(0.5);

        if (!BTN_CAMBIAR_MODO) {
            btn_presionado_actual = 0;
        } else {
            btn_presionado_actual = 1;
        }

        if (btn_presionado_anterior && !btn_presionado_actual) {
            if (!modo_edicion) {
                modo_edicion = 1;
            } else {
                modo_edicion = 0;
            }
        }
        btn_presionado_anterior = btn_presionado_actual;

        // Se coloca la palabra 'ESCOM' en la LCD
        lcd_gotoxy(11, 0);
        lcd_putsf("ESCOM");

        // Se coloca la temperatura
        lectura_adc = leer_adc_promedio(1); // Leemos la temperatura
        temperatura_celsius = lectura_adc * 1.95;

        if (temperatura_celsius > 99) {
            temperatura_celsius = 99;
        }

        // Dividimos y obtenemos el valor de la temperatura
        temperatura_entera = temperatura_celsius * 10;
        temp_decenas = temperatura_entera / 100;
        temperatura_entera %= 100;
        temp_decimas = temperatura_entera % 10;
        temp_unidades = temperatura_entera / 10;

        // Colocamos la temperatura en la LCD
        lcd_gotoxy(10, 1);
        lcd_putchar(temp_decenas + ASCII_CERO);
        lcd_gotoxy(11, 1);
        lcd_putchar(temp_unidades + ASCII_CERO);
        lcd_gotoxy(12, 1);
        lcd_putchar('.');
        lcd_gotoxy(13, 1);
        lcd_putchar(temp_decimas + ASCII_CERO);
        lcd_gotoxy(14, 1);
        lcd_putchar(ASCII_CERO + 175); // Carácter °
        lcd_gotoxy(15, 1);
        lcd_putchar('C');

        // Reloj en movimiento
        if (modo_edicion) {
            if (!BTN_AJUSTAR_HORA) {
                if (contador_antirebote > tiempo_antirebote) {
                    contador_antirebote = 0;
                    horas++;
                } else {
                    contador_antirebote++;
                }
            }

            if (!BTN_AJUSTAR_MIN) {
                if (contador_antirebote > tiempo_antirebote) {
                    contador_antirebote = 0;
                    minutos++;
                } else {
                    contador_antirebote++;
                }
            }

            if (!BTN_AJUSTAR_SEG) {
                if (contador_antirebote > tiempo_antirebote) {
                    contador_antirebote = 0;
                    segundos++;
                } else {
                    contador_antirebote++;
                }
            }
        } else {
            if (!BTN_AJUSTAR_HORA) {
                if (contador_antirebote > tiempo_antirebote) {
                    contador_antirebote = 0;
                    anio++;
                    if (anio > 99) {
                        siglo++;
                        anio = 0;
                    }
                } else {
                    contador_antirebote++;
                }
            }

            if (!BTN_AJUSTAR_MIN) {
                if (contador_antirebote > tiempo_antirebote) {
                    contador_antirebote = 0;
                    mes_actual++;
                } else {
                    contador_antirebote++;
                }
            }

            if (!BTN_AJUSTAR_SEG) {
                if (contador_antirebote > tiempo_antirebote) {
                    contador_antirebote = 0;
                    dia_actual++;
                } else {
                    contador_antirebote++;
                }
            }
        }

        if (contador_reloj > 49) {
            contador_reloj = 0;
            segundos++;
        } else {
            contador_reloj++;
        }

        if (segundos > 59) {
            minutos++;
            segundos = 0;
        }

        if (minutos > 59) {
            horas++;
            minutos = 0;
            segundos = 0;
        }

        if (horas > 23) {
            dia_actual++;
            horas = 0;
            segundos = 0;
            minutos = 0;
        }

        if (dia_actual > 31) {
            mes_actual++;
            dia_actual = 0;
        }

        if (mes_actual > 12) {
            anio++;
            mes_actual = 0;

            if (anio > 99) {
                siglo++;
                anio = 0;
            }
        }

        // Colocamos la hora en la LCD
        lcd_gotoxy(0, 1);
        lcd_putchar(horas / 10 + ASCII_CERO);
        lcd_gotoxy(1, 1);
        lcd_putchar(horas % 10 + ASCII_CERO);
          
        lcd_gotoxy(2, 1);
        lcd_putchar(':');
          
        lcd_gotoxy(3, 1);
        lcd_putchar(minutos / 10 + ASCII_CERO);
        lcd_gotoxy(4, 1);
        lcd_putchar(minutos % 10 + ASCII_CERO);
          
        lcd_gotoxy(5, 1);
        lcd_putchar(':');
          
        lcd_gotoxy(6, 1);
        lcd_putchar(segundos / 10 + ASCII_CERO);
        lcd_gotoxy(7, 1);
        lcd_putchar(segundos % 10 + ASCII_CERO);

        // Colocamos la fecha en la LCD
        lcd_gotoxy(0, 0);
        lcd_putchar(siglo / 10 + ASCII_CERO);
        lcd_gotoxy(1, 0);
        lcd_putchar(siglo % 10 + ASCII_CERO);
        lcd_gotoxy(2, 0);
        lcd_putchar(anio / 10 + ASCII_CERO);
        lcd_gotoxy(3, 0);
        lcd_putchar(anio % 10 + ASCII_CERO); 
        
        lcd_gotoxy(4, 0);
        lcd_putchar('-');
          
        lcd_gotoxy(5, 0);
        lcd_putchar(mes_actual / 10 + ASCII_CERO);
        lcd_gotoxy(6, 0);
        lcd_putchar(mes_actual % 10 + ASCII_CERO);
          
        lcd_gotoxy(7, 0);
        lcd_putchar('-');
        
        lcd_gotoxy(8, 0);
        lcd_putchar(dia_actual / 10 + ASCII_CERO);
        lcd_gotoxy(9, 0);
        lcd_putchar(dia_actual % 10 + ASCII_CERO);
    }
}
