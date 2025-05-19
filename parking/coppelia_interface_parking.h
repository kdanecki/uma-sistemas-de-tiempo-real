/* 
 * File:   coppelia_interface.h
 * DO NOT TOUCH THIS FILE!
 */

#ifndef COPPELIA_INTERFACE_H
#define	COPPELIA_INTERFACE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "rs232atmega.h"
extern volatile unsigned char cars;
char buffer[32]="";
char trama[32]="";//trama wuth NO timestamp
char trama2[32]=""; //trama to send including the timestamp
char old_trama[32]="";


ISR(TIMER2_COMPA_vect)
{
    static unsigned timestamp=0;
	sprintf(trama,"%u %u %u %u\r\n",cars,(PORTC&(1<<PORTC3))>>PORTC3,(PORTC&(1<<PORTC4))>>PORTC4,OCR1A);
	if (strcmp(trama,old_trama)!=0)	
    {   
        sprintf(trama2,"%u %s\r\n",timestamp,trama);
        RS232_Send(trama2,'\r');
        strcpy(old_trama,trama);
    }
    timestamp+=1;
}

void Init_Coppelia_Interface()
{
    RS232_Init(buffer,32,'\n');
    //Init interruptions pins. In this example PINS B2,B3,B4 will produce interruptions
    //In order to simulate the interruption the pin is configured as OUTPUT!! 
    //when writing in PORT, the value is also passed to PIN which generates a PC interrupt.
    
    DDRB|=(1<<PINB3) + (1<<PINB4);
    DDRD|=(1<<PIND2);
    
    //Init timer 2
    TCCR2A=(1<<WGM21);  //CTC mode
    TCCR2B=(1<<CS22)+(1<<CS21)+(1<<CS20);  //1024 prescaler
    OCR2A=156;  // 10 ms, should be 156.25. It is used for sending data via serial port to coppelia
    TIMSK2= (1<<OCIE2A);  //interrupt in output compare with OCR2A
    
}
void Receive_from_Coppelia()
{
				unsigned dato;
		
				RS232InputReport rp;
				RS232_Read(&rp);
				if (rp.bufov==0 && rp.lasterr==0 && rp.lastinputchar=='\n')
				{
					dato=(unsigned)atoi(buffer);
                    cli();
                    
                    //1) Set pin to 0
                    //2) Set the correponding pin to 1
                    
                    PORTB&=~(1<<PORTB4);
                    PORTB&=~(1<<PORTB3);
                    PORTD&=~(1<<PORTD2);
                    
                    switch (dato)
                    {
                            case 6:  PORTB|=(1<<PORTB4);break;
                            case 8:  PORTB|=(1<<PORTB3);break;
                            case 4:  PORTD|=(1<<PORTD2);break;
               
    				}
                        sei();
                }
			

}
#ifdef	__cplusplus
}
#endif

#endif	/* COPPELIA_INTERFACE_H */

