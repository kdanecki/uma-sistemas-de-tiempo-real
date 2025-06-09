/*
 * File:   main.c
 * Author: Usuario
 *
 * Created on March 24, 2025, 1:03 PM
 */



#include <xc.h>
#include <avr/io.h> //Librería para programar periféricos del microcontrolador 


void init (){
    
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
    int i;
    int bit;
    for (i = 0; i<8; i++){
        
        if (value & 1 << i){
        
            led_on(i);
           
        } else {
        
            led_off(i);
        }
        
    }
}

void wait(){
    unsigned int j;
    unsigned int i;
    for (i= 0; i<=4000; i++){
        for (j=0; j<=100; j++);
    }
}


int main(void) {
    unsigned char led = 0;
    int i = 0;
    init();
    
    while(1) {
        digital_write(i++);
        wait();
    } 
    
    return 0;
  
}
