/*
 * parking.c
 *
 */ 

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>


#include "coppelia_interface_parking.h"
volatile unsigned char cars;
volatile unsigned char estado;

#define ABRIR 37999
#define CERRAR 36999

void init_leds() {
    DDRB|= 1 << 2;
    DDRB|= 1 << 1;
    DDRB|= 1;
    DDRD|= 1 << 7;
    DDRD|= 1 << 5;
    DDRD|= 1 << 4;
    DDRC|= 1 << 4;
    DDRC|= 1 << 3;
}

void init_interrupts() {
    DDRB&= ~(1 << 4);
    DDRB&= ~(1 << 3);
    
    PCICR = 1;
    PCMSK0= (1<<3) | (1<<4);
}

void init_pwm() {
    DDRB |= 1 << 1;
    TCCR1A = 2 | 192;
    TCCR1B = 24 | 2;
    ICR1 = 39999;
    
    OCR1A = 36999; // abrir
    //OCR1A = 36999; // cerrar
    
}

void led_on(unsigned char led){
    switch (led){
        case 0:
             PORTB|= 1 << 2; 
             break;
        case 1:
             PORTB|= 1 << 1; 
             break;
        case 2:
             PORTB|= 1; 
             break;
        case 3:
             PORTD|= 1 << 7; 
             break;
        case 4:
             PORTD|= 1 << 5; 
             break;        
        case 5:
             PORTD|= 1 << 4; 
             break;
        case 6:
             PORTC|= 1 << 4; 
             break;
        case 7:
             PORTC|= 1 << 3; 
             break;
             
    }
}

void init_all() {
    init_leds();
    init_interrupts();
    init_pwm();
    
    cars = 0;
    estado = 0;
    led_on(7);
    led_on(6);
}

void led_off(unsigned char led){
    
    
    switch (led){
        case 0:
             PORTB&= ~(1 << 2);
             break;
        case 1:
             PORTB&= ~(1 << 1); 
             break;
        case 2:
             PORTB&= ~1;
             break;
        case 3:
             PORTD&= ~(1 << 7);
             break;
        case 4:
             PORTD&= ~(1 << 5); 
             break;        
        case 5:
             PORTD&= ~(1 << 4);
             break;
        case 6:
             PORTC&= ~(1 << 4);
             break;
        case 7:
             PORTC&= ~(1 << 3);
             break;
             
    }
}

void digital_write(unsigned char value){
    char i;
    
    for (i = 0; i<3; i++){
        
        if (value & 1 << i ){
        
            led_on(i+2);
           
        } else {
        
            led_off(i+2);
        }
        
    }     
}

void wait(){
    unsigned int i;
    unsigned int j;
    for (i= 0; i<=1000; i++){
        for (j= 0; j<200; j++);
    }
}




ISR(PCINT0_vect){
    
    switch (estado){
        case 0:
            if ( PINB & (1<<PINB3)) {
               cars--;
               estado = 2;
               OCR1A = ABRIR;
               led_off(6);
            }else if (PINB & (1<<PINB4)){
              cars++;
              estado = 1;
              OCR1A = ABRIR;
              led_off(7);
            }
            

            break;
        case 1:
            if ( PINB & (1<<PINB3)) {
               estado = 0;
               OCR1A = CERRAR;
               led_on(7);
            }
            break;
                    
        case 2:
            if (PINB & (1<<PINB4)) {
               estado = 0;
               OCR1A = CERRAR;
               led_on(6);
            }
            break;
            
               
       
    }
    digital_write(cars);
}







int main(void)
{
    cli();
    //Your initializations here	
    
    init_all();
    Init_Coppelia_Interface();
    
	sei();
	
		while(1)
        {
            Receive_from_Coppelia();
		}		
		
}

