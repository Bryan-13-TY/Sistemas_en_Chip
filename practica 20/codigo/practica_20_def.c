/*******************************************************
This program was created by the CodeWizardAVR V4.06a 
Automatic Program Generator
� Copyright 1998-2025 Pavel Haiduc, HP InfoTech S.R.L.
http://www.hpinfotech.ro

Project : Práctica 20 'Juego de Ping-Ping'
Version : 1.0
Date    : 20/04/2026
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
#include <delay.h>
#include <stdlib.h>

#define inP1_right PIND.5
#define inP1_left PIND.4
#define inP2_right PIND.3
#define inP2_left PIND.2
#define in_reset PIND.1
#define in_play PIND.0
#define TOP_LIMIT 0x18
#define BOTTOM_LIMIT 0x03


// Declare your global variables here
unsigned char num[7] = {0x06, 0x00, 0x00, 0x04, 0x00, 0x00, 0x0c};
bit botonp;
bit botona, game = 1;
bit dir; // dirección izquerda: 0, derecha: 1
int opc, index_ball;
int dir_ball, timer; // supizq: 0, sup: 1, supder: 2, infder: 3, inf: 4, indizq: 5
unsigned char scoreP1, scoreP2;
const char tabla7segmentos[10] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};

void verVector(unsigned char myVector[7]) {
    unsigned char i, v = 0x01;

    for (i = 0; i < 7; i++) {
        PORTA = ~v << 1; // inicia desde el bit A1, por eso el corrimiento hacia la izquierda
        PORTB = myVector[i];
        delay_ms(3);
        v = v << 1;
    }
}

void jugador1_move(int my_opc) {
    unsigned char temp;

    if (my_opc == 1) {
        temp = num[0] << 1;
        if (temp > TOP_LIMIT) {
            num[0] = TOP_LIMIT;
        } else {
            num[0] = temp;
        }
    } else {
        temp = num[0] >> 1;
        if (temp < BOTTOM_LIMIT) {
            num[0] = BOTTOM_LIMIT;
        } else {
            num[0] = temp;
        }
    }
}

void jugador2_move(int my_opc) {
    unsigned char temp;

    if (my_opc == 3) {
        temp = num[6] << 1;
        if (temp > TOP_LIMIT) {
            num[6] = TOP_LIMIT;
        } else {
            num[6] = temp;
        }
    } else {
        temp = num[6] >> 1;
        if (temp < BOTTOM_LIMIT) {
            num[6] = BOTTOM_LIMIT;
        } else {
            num[6] = temp;
        }
    }
}

void checa_boton(void) {
    if (inP1_right == 0) {
        botona = 0;
        opc = 1;
    } else if (inP1_left == 0) {
        botona = 0;
        opc = 2;
    } else if (inP2_right == 0) {
        botona = 0;
        opc = 3;
    } else if (inP2_left == 0) {
        botona = 0;
        opc = 4;
    } else if (in_reset == 0) {
        botona = 0;
        opc = 5;
    } else if (in_play == 0) {
        botona = 0;
        opc = 6;
    } else {
        botona = 1;
    }

    if (botonp && !botona) { // hubo cambio de flanco de 1 a 0
        if (opc == 1 || opc == 2) {
            jugador1_move(opc);
        } else if (opc == 3 || opc == 4) {
            jugador2_move(opc);
        } else if (opc == 5) {
            scoreP1 = 0;
            scoreP2 = 0;
            game = 1;
        } else {
            game = !game;
        }

        delay_ms(8); // para eliminar rebotes
    }

    if (!botonp && botona) { // hubo cambio de flanco de 0 a 1
        delay_ms(8); // para eliminar rebotes
    }

    botonp = botona;
    opc = -1;
}

void recorreVector_der() {
    unsigned int r_num = rand() % 6;
    
    if (index_ball + 1 == 6) {
        dir = 0;
        if ((num[index_ball + 1] & num[index_ball]) == 0) {
            scoreP2++;
            num[index_ball] = 0x00;
            index_ball = 3;
            num[index_ball] = 0x04;
        } else {
            if (r_num < 3) {
                dir_ball = r_num;
            } else {
                dir_ball = 1;
            }
        }
    } else {
        num[index_ball + 1] = num[index_ball];
        num[index_ball] = 0x00;
        index_ball++;
    }
}

void recorreVector_izq() {
    unsigned int r_num = rand() % 6;
    
    if (index_ball - 1 == 0) {
        dir = 1;
        if ((num[index_ball - 1] ^ num[index_ball]) > num[index_ball - 1]) {
            scoreP1++;
            num[index_ball] = 0x00;
            index_ball = 3;
            num[index_ball] = 0x04;
        } else {
            if (r_num >= 3) {
                dir_ball = r_num;
            } else {
                dir_ball = 4;
            }
        }
    } else {
        num[index_ball - 1] = num[index_ball];
        num[index_ball] = 0x00;
        index_ball--;
    }
}

void pelota_move() {
    unsigned char temp;

    if (dir) { // abajo
        switch (dir_ball) {
            case 3:
                temp = num[index_ball] >> 1;
                if (temp != 0x00) {
                    num[index_ball] = temp;
                    recorreVector_der();
                } else {
                    dir_ball = 5;
                    recorreVector_der();
                }
                break;
            case 4:
                recorreVector_der();
                break;
            case 5:
                temp = num[index_ball] << 1;
                if (temp != 0x10) {
                    num[index_ball] = temp;
                    recorreVector_der();
                } else {
                    dir_ball = 3;
                    recorreVector_der();
                }
                break;
            default:
                dir_ball = 4;
                break;
        }
    } else {
        switch (dir_ball) {
            case 2:
                temp = num[index_ball] >> 1;
                if (temp != 0x00) {
                    num[index_ball] = temp;
                    recorreVector_izq();
                } else {
                    dir_ball = 0;
                    recorreVector_izq();
                }
                break;
            case 1:
                recorreVector_izq();
                break;
            case 0:
                temp = num[index_ball] << 1;
                if (temp != 0x10) {
                    num[index_ball] = temp;
                    recorreVector_izq();
                } else {
                    dir_ball = 2;
                    recorreVector_izq();
                }
                break;
            default:
                dir_ball = 1;
                break;
        }
    }
}

void verMarcador() {
    PORTD.7 = 0;
    PORTC = 0x00;
    PORTD.6 = 1;
    PORTC = tabla7segmentos[scoreP1];
    delay_ms(2);

    PORTD.6 = 0;
    PORTC = 0x00;
    PORTD.7 = 1;
    PORTC = tabla7segmentos[scoreP2];
    delay_ms(2);
}

void main(void) {
    // Declare your local variables here
    opc = -1;
    index_ball = 3;
    timer = 0;
    dir = 1;
    scoreP1 = 0;
    scoreP2 = 0;
    dir_ball = 1;
    srand(1);

    // Input/Output Ports initialization
    // Port A initialization
    // Function: Bit7=Out Bit6=Out Bit5=Out Bit4=Out Bit3=Out Bit2=Out Bit1=Out Bit0=In 
    DDRA=(1<<DDA7) | (1<<DDA6) | (1<<DDA5) | (1<<DDA4) | (1<<DDA3) | (1<<DDA2) | (1<<DDA1) | (0<<DDA0);
    // State: Bit7=0 Bit6=0 Bit5=0 Bit4=0 Bit3=0 Bit2=0 Bit1=0 Bit0=T 
    PORTA=(0<<PORTA7) | (0<<PORTA6) | (0<<PORTA5) | (0<<PORTA4) | (0<<PORTA3) | (0<<PORTA2) | (0<<PORTA1) | (0<<PORTA0);

    // Port B initialization
    // Function: Bit7=In Bit6=In Bit5=In Bit4=Out Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
    DDRB=(0<<DDB7) | (0<<DDB6) | (0<<DDB5) | (1<<DDB4) | (1<<DDB3) | (1<<DDB2) | (1<<DDB1) | (1<<DDB0);
    // State: Bit7=T Bit6=T Bit5=T Bit4=0 Bit3=0 Bit2=0 Bit1=0 Bit0=0 
    PORTB=(0<<PORTB7) | (0<<PORTB6) | (0<<PORTB5) | (0<<PORTB4) | (0<<PORTB3) | (0<<PORTB2) | (0<<PORTB1) | (0<<PORTB0);

    // Port C initialization
    // Function: Bit7=In Bit6=Out Bit5=Out Bit4=Out Bit3=Out Bit2=Out Bit1=Out Bit0=Out 
    DDRC=(0<<DDC7) | (1<<DDC6) | (1<<DDC5) | (1<<DDC4) | (1<<DDC3) | (1<<DDC2) | (1<<DDC1) | (1<<DDC0);
    // State: Bit7=T Bit6=0 Bit5=0 Bit4=0 Bit3=0 Bit2=0 Bit1=0 Bit0=0 
    PORTC=(0<<PORTC7) | (0<<PORTC6) | (0<<PORTC5) | (0<<PORTC4) | (0<<PORTC3) | (0<<PORTC2) | (0<<PORTC1) | (0<<PORTC0);

    // Port D initialization
    // Function: Bit7=Out Bit6=Out Bit5=In Bit4=In Bit3=In Bit2=In Bit1=In Bit0=In 
    DDRD=(1<<DDD7) | (1<<DDD6) | (0<<DDD5) | (0<<DDD4) | (0<<DDD3) | (0<<DDD2) | (0<<DDD1) | (0<<DDD0);
    // State: Bit7=0 Bit6=0 Bit5=P Bit4=P Bit3=P Bit2=P Bit1=P Bit0=P 
    PORTD=(0<<PORTD7) | (0<<PORTD6) | (1<<PORTD5) | (1<<PORTD4) | (1<<PORTD3) | (1<<PORTD2) | (1<<PORTD1) | (1<<PORTD0);

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
        if (scoreP1 >= 10 || scoreP2 >= 10) {
            game = 1;
        }

        checa_boton();
        
        if (!game) {
            if (timer > 5) {
                timer = 0;
                pelota_move();
            } else {
                timer++;
            }
        }

        verVector(num);
        verMarcador();
    }
}
