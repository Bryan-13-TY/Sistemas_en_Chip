/*******************************************************
This program was created by the CodeWizardAVR V4.06a 
Automatic Program Generator
� Copyright 1998-2025 Pavel Haiduc, HP InfoTech S.R.L.
http://www.hpinfotech.ro

Project : Práctica 14 'Vúmetro'
Version : 1.0
Date    : 25/03/2026
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

// Declare your global variables here
unsigned char peak_hold[2] = {0, 0};  // Hold peak for each microphone
unsigned char hold_counter[2] = {0, 0}; // Counter for hold duration

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

// Function to convert ADC value to progressive LED pattern
unsigned char get_bar_pattern(unsigned char adc_value) {
    // Define threshold levels for each LED
    // Ajusta estos valores según la sensibilidad del micrófono
    if (adc_value < 5) return 0x00; // Sin sonido (todos apagados)
    else if (adc_value < 15) return 0x01; // LED 1 (PB0)
    else if (adc_value < 30) return 0x03; // LED 1-2 (PB0-1)
    else if (adc_value < 50) return 0x07; // LED 1-3 (PB0-2)
    else if (adc_value < 70) return 0x0F; // LED 1-4 (PB0-3)
    else if (adc_value < 90) return 0x1F; // LED 1-5 (PB0-4)
    else if (adc_value < 120) return 0x3F; // LED 1-6 (PB0-5)
    else if (adc_value < 160) return 0x7F; // LED 1-7 (PB0-6)
    else return 0xFF; // Todos los LEDs (PB0-7)
}

// Function to convert ADC value to single LED position
unsigned char get_point_pattern(unsigned char adc_value) {
    // Define threshold levels for each individual LED
    if (adc_value < 10) return 0x00; // Sin sonido (todos apagados)
    else if (adc_value < 25) return 0x01; // Solo LED 1 (PB0)
    else if (adc_value < 40) return 0x02; // Solo LED 2 (PB1)
    else if (adc_value < 55) return 0x04; // Solo LED 3 (PB2)
    else if (adc_value < 70) return 0x08; // Solo LED 4 (PB3)
    else if (adc_value < 85) return 0x10; // Solo LED 5 (PB4)
    else if (adc_value < 100) return 0x20; // Solo LED 6 (PB5)
    else if (adc_value < 115) return 0x40; // Solo LED 7 (PB6)
    else return 0x80; // Solo LED 8 (PB7)
}

// Function to get hold pattern based on current value and peak hold
unsigned char get_hold_pattern(unsigned char adc_value, unsigned char mic_index) {
    unsigned char bar = get_bar_pattern(adc_value);              // barra normal
    unsigned char peak = get_point_pattern(peak_hold[mic_index]); // pico como punto
    return bar | peak; // mezcla barra + punto (HOLD visible)
}

// Function to update hold peak
void update_hold_peak(unsigned char adc_value, unsigned char mic_index) {
    if (adc_value > peak_hold[mic_index]) {
        peak_hold[mic_index] = adc_value;
        hold_counter[mic_index] = 0;
    }

    if (++hold_counter[mic_index] >= 20) {
        if (peak_hold[mic_index] >= 10)
            peak_hold[mic_index] -= 10;  // más rápido (ANTES 5)
        else
            peak_hold[mic_index] = 0;

        hold_counter[mic_index] = 15;
    }
}

unsigned char get_hold_mode(unsigned char adc_value, unsigned char mic_index) {
    update_hold_peak(adc_value, mic_index);
    return get_hold_pattern(adc_value, mic_index);
}

void main(void) {
    // Declare your local variables here
    unsigned char mic1_value, mic2_value;
    unsigned char led_pattern1, led_pattern2;
    unsigned char modo;
    int temp1, temp2;
    // FILTRO DE OFFSET (ruido base del micrófono)
    //unsigned char base = 120; // AJUSTA este valor (100–140 aprox)

    // Input/Output Ports initialization
    // Port A initialization
    // Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
    DDRA=(0<<DDA7) | (0<<DDA6) | (0<<DDA5) | (0<<DDA4) | (0<<DDA3) | (0<<DDA2) | (0<<DDA1) | (0<<DDA0);
    // State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=T Bit0=T 
    PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);

    // Port B initialization
    // Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
    DDRB=(1<<DDB7) | (1<<DDB6) | (1<<DDB5) | (1<<DDB4) | (1<<DDB3) | (1<<DDB2) | (1<<DDB1) | (1<<DDB0);
    // State: Bit7=0 Bit6=0 Bit5=0 Bit4=0 Bit3=0 Bit2=0 Bit1=0 Bit0=0 
    PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

    // Port C initialization
    // Function: Bit7=In Bit6=In Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
    DDRC=(0<<DDC7) | (0<<DDC6) | (0<<DDC5) | (0<<DDC4) | (0<<DDC3) | (0<<DDC2) | (0<<DDC1) | (0<<DDC0);
    // State: Bit7=T Bit6=T Bit5=T Bit4=T Bit3=T Bit2=T Bit1=P Bit0=P 
    PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (1<<PORTC1) | (1<<PORTC0);

    // Port D initialization
    // Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
    DDRD=(1<<DDD7) | (1<<DDD6) | (1<<DDD5) | (1<<DDD4) | (1<<DDD3) | (1<<DDD2) | (1<<DDD1) | (1<<DDD0);
    // State: Bit7=0 Bit6=0 Bit5=0 Bit4=0 Bit3=0 Bit2=0 Bit1=0 Bit0=0 
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

    // ADC initialization
    // ADC Clock frequency: 500.000 kHz
    // ADC High Speed Mode: Off
    // ADC Auto Trigger Source: Software
    // Only the 8 most significant bits of
    // the AD conversion result are used
    ADMUX = ADC_VREF_TYPE;
    ADCSRA=(1<<ADEN) | (0<<ADSC) | (0<<ADATE) | (0<<ADIF) | (0<<ADIE) | (0<<ADPS2) | (0<<ADPS1) | (1<<ADPS0);
    SFIOR=(1<<ADHSM) | (0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);

    // SPI initialization
    // SPI disabled
    SPCR=(0<<SPIE) | (0<<SPE) | (0<<DORD) | (0<<MSTR) | (0<<CPOL) | (0<<CPHA) | (0<<SPR1) | (0<<SPR0);

    // TWI initialization
    // TWI disabled
    TWCR=(0<<TWEA) | (0<<TWSTA) | (0<<TWSTO) | (0<<TWEN) | (0<<TWIE);

    while (1) {
        // Place your code here
        mic1_value = read_adc(1);
        mic2_value = read_adc(2);
               
        // MIC 1
        temp1 = mic1_value - 128;
        if (temp1 < 0) temp1 = -temp1;
        mic1_value = temp1;

        // MIC 2
        temp2 = mic2_value - 128;
        if (temp2 < 0) temp2 = -temp2;
        mic2_value = temp2;         
        
        // Leer modo
        modo = PINC & 0x03;

        switch (modo) {
            case 0x00: // MODO BARRA
                led_pattern1 = get_bar_pattern(mic1_value);
                led_pattern2 = get_bar_pattern(mic2_value); 
                break;
            case 0x01: // MODO PUNTO
                led_pattern1 = get_point_pattern(mic1_value);
                led_pattern2 = get_point_pattern(mic2_value);
                break;
            case 0x02: // MODO HOLD
                led_pattern1 = get_hold_mode(mic1_value, 0);
                led_pattern2 = get_hold_mode(mic2_value, 1);
                break;
            default: // SEGURIDAD
                led_pattern1 = 0x00;
                led_pattern2 = 0x00;
                break;
        }
        delay_ms(30);
        
        PORTB = led_pattern1;
        PORTD = led_pattern2;
      }
}
