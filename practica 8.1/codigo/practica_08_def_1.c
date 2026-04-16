/*******************************************************
This program was created by the CodeWizardAVR V4.06a 
Automatic Program Generator
� Copyright 1998-2025 Pavel Haiduc, HP InfoTech S.R.L.
http://www.hpinfotech.ro

Project : Práctica 8 'Cronómetro  de 100 minutos'
Version : 1.0
Date    : 31/03/2026
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

#define RESET_BUTTON (PIND.0 == 0)
#define STOP_BUTTON (PIND.1 == 0)
#define START_BUTTON (PIND.2 == 0)

// Declare your global variables here
const unsigned char tabla_7_segmentos[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F};

volatile unsigned char minutos_decenas = 0;
volatile unsigned char minutos_unidades = 0;
volatile unsigned char segundos_decenas = 0;
volatile unsigned char segundos_unidades = 0;
volatile unsigned char centesimas_decenas = 0;
volatile unsigned char centesimas_unidades = 0;

volatile unsigned char display_actual = 0;
volatile unsigned char running = 0;
unsigned char i = 0;

void actualizar_displays(void);
void incrementar_tiempo(void);
void reset_tiempo(void);

interrupt [TIM0_OVF] void timer0_ovf_isr(void) {
    TCNT0 = 131; // ~1 ms con 1MHz y prescaler 8
    actualizar_displays();
}

interrupt [TIM1_OVF] void timer1_ovf_isr(void) {
    TCNT1H = 0xD8; // preload para 10 ms
    TCNT1L = 0xF0;

    if (running)
        for (i = 0; i < 100; i++) {
            incrementar_tiempo();
        }
}

void main(void) {
    // ===== PUERTOS =====
    DDRA=0x00;
    PORTA=0x00;

    DDRB=0xFF;
    PORTB=0x00;

    DDRC=0x3F;
    PORTC=0x00;

    DDRD=0x00;
    PORTD=0x07;

    // ===== TIMER 0 =====
    TCCR0=(0<<WGM00) | (0<<WGM01) | (0<<COM01) | (0<<COM00)
         | (0<<CS02) | (1<<CS01) | (0<<CS00); // prescaler 8

    TCNT0=131;

    // ===== TIMER 1 =====
    TCCR1A=0x00;
    TCCR1B=(0<<WGM12) | (1<<CS11); // prescaler 8

    TCNT1H=0xD8;
    TCNT1L=0xF0;

    // ===== INTERRUPCIONES =====
    TIMSK=(1<<TOIE0) | (1<<TOIE1);

    #asm("sei")

    reset_tiempo();


    while (1) {
        // Place your code here
        // ===== VARIABLES ESTÁTICAS PARA FLANCO =====
        static unsigned char last_start = 1;
        static unsigned char last_stop  = 1;
        static unsigned char last_reset = 1;

        // ===== BOTÓN START =====
        if (!START_BUTTON && last_start) { // flanco de bajada
            delay_ms(20);
            if (!START_BUTTON) running = 1;
        }
        last_start = START_BUTTON;

        // ===== BOTÓN STOP =====
        if (!STOP_BUTTON && last_stop) {
            delay_ms(20);
            if (!STOP_BUTTON) running = 0;
        }
        last_stop = STOP_BUTTON;

        // ===== BOTÓN RESET =====
        if (!RESET_BUTTON && last_reset) {
            delay_ms(20);
            if (!RESET_BUTTON) {
                reset_tiempo();
                running = 0;
            }
        }
        last_reset = RESET_BUTTON;
   }
}

void actualizar_displays(void) {
   PORTC = 0x00;

   switch (display_actual) {
      case 0:
         PORTB = tabla_7_segmentos[minutos_decenas];
         PORTC = 0x01;
         break;
      case 1:
         PORTB = tabla_7_segmentos[minutos_unidades];
         PORTC = 0x02;
         break;
      case 2:
         PORTB = tabla_7_segmentos[segundos_decenas];
         PORTC = 0x04;
         break;
      case 3:
         PORTB = tabla_7_segmentos[segundos_unidades];
         PORTC = 0x08;
         break;
      case 4:
         PORTB = tabla_7_segmentos[centesimas_decenas];
         PORTC = 0x10;
         break;
      case 5:
         PORTB = tabla_7_segmentos[centesimas_unidades];
         PORTC = 0x20;
         break;
   }

   display_actual++;
   if (display_actual > 5) display_actual = 0;
}

void incrementar_tiempo(void) {
   centesimas_unidades++;
   if (centesimas_unidades > 9) {
      centesimas_unidades = 0;
      centesimas_decenas++;
      if (centesimas_decenas > 9) {
         centesimas_decenas = 0;
         segundos_unidades++;
         if (segundos_unidades > 9) {
            segundos_unidades = 0;
            segundos_decenas++;
            if (segundos_decenas > 5) {
               segundos_decenas = 0;
               minutos_unidades++;
               if (minutos_unidades > 9) {
                  minutos_unidades = 0;
                  minutos_decenas++;
                  if (minutos_decenas >= 10) {
                     reset_tiempo();
                  }
               }
            }
         }
      }
   }
}

void reset_tiempo(void) {
    minutos_decenas = 0;
    minutos_unidades = 0;
    segundos_decenas = 0;
    segundos_unidades = 0;
    centesimas_decenas = 0;
    centesimas_unidades = 0;

    display_actual = 0;
}