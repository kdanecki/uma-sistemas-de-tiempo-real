/*
 * File:   practica1.c
 * Author: kamil
 *
 * Created on 21 marca 2025, 10:59
 */

#include <xc.h>
#include <avr/io.h> 

void led_on(){
    PORTD|= 1 << 4;
}

void led_off(){
    PORTD&= ~(1 << 4);
}


void wait(){
    unsigned int j;
    unsigned int i;
    for (i= 0; i<=1000; i++){
         for (j=0; j<=1000; j++);
    }
}


void main(void) {

    DDRD |= 1 << 4; //Desplazando al 2º bit un 1 para configurar el puerto B (pin 1) como salida


        while(1) {

        led_on();
      asm("nop");
        wait();
        led_off();
        wait();
    }
}
