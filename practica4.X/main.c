/*
 * File:   main.c
 * Author: Usuario
 *
 * Created on April 4, 2025, 11:04 AM
 */

#include <avr/io.h>
#include<avr/interrupt.h>

volatile unsigned char cont;

void init_all (){
    
    DDRB|= 1 << 2; 
    DDRB|= 1 << 1; 
    DDRB|= 1; 
    DDRD|= 1 << 7;
    DDRD|= 1 << 5; 
    DDRD|= 1 << 4; 
    DDRC|= 1 << 4; 
    DDRC|= 1 << 3; 


    
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
    
    for (i = 0; i<8; i++){
        
        if (value & 1 << i){
        
            led_on(i);
           
        } else {
        
            led_off(i);
        }
        
    }  
   
}



void configCTC(){
    TCCR1A = 0;
    TCCR1B = (1 << WGM12);
    TCNT1L = 3036;
    OCR1A = 0xFFFF;
    TCCR1B |= (1 << CS10) (1 << CS11);
    TIMSK1 |= (1 << TOIE1);
   
    
    
}


ISR(TIMER1_OVF_vect){
    cont++;  
    TCNT1L = 3036;
}

void main(void) {
    cli();
    cont = 1;
    init_all();
    configCTC();
    
    sei();
    while (1){
        digital_write(cont);
    }
    return;
}
