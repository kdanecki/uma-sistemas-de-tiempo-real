#include "utils_planif.h"


//****************************************
// Private functions for tracing and simulating cpu consumption. YOU CAN SEE BUT NEVER TOUCH HERE :)
//***************************************


#define time2save 20000 // data size to be stored in external memory before sending it via rs232
uint16_t  ev[4];

unsigned char trama[16];

//void my_trace(uint16_t tick, char tag,char userdata,char resource_id);
//void consumeCPU(unsigned char task_id, unsigned short ms);
//void InitHW();

void my_trace(uint16_t tick, char tag,char userdata,char resource_id)
{
    static unsigned mem_pointer=0;
    DigitalWrite(tag);
    ev[0]=tick;
    ev[1]=tag;
    ev[2]=userdata;
    ev[3]=resource_id;
    SRAMwriteblock(mem_pointer,(unsigned char*)ev,8);
    mem_pointer+=8;

    if (mem_pointer>time2save)
    {	
        DigitalWrite(127);
        mem_pointer=0;
        //Starting
        sprintf(trama,"INIT TRACE\r\n");
        RS232_Send(trama,'\r');
        //Write the number of tasks
        sprintf(trama,"%u 0 0 0\r\n",num_tasks);
        RS232_Send(trama,'\r');
        //Write the computation times: a row per task
        for (unsigned char q=0;q<num_tasks;q++)
        {
        sprintf(trama,"%u %u 0 0\r\n",q+1,computationTime[q]);
        RS232_Send(trama,'\r');
        }
        //Write the tasks' periods: a row per task
        for (unsigned char q=0;q<num_tasks;q++)
        {
        sprintf(trama,"%u %u 0 0\r\n",q+1,taskPeriod[q]);
        RS232_Send(trama,'\r');
        }
        //Write the first activation: a row per task
        for (unsigned char q=0;q<num_tasks;q++)
        {
            sprintf(trama,"%u %u 0 0\r\n",q+1,start[q]);
            RS232_Send(trama,'\r');
        }
        //read from memory and send through rs232
        while (mem_pointer<time2save)
        {
        SRAMreadblock(mem_pointer,(unsigned char*)ev,8);
        sprintf(trama,"%u %u %u %u\r\n",ev[0],ev[1],ev[2],ev[3]);
        RS232_Send(trama,'\r');
        mem_pointer+=8;
        }
        sprintf(trama,"END TRACE\r\n");
        RS232_Send(trama,'\r');
        mem_pointer=0;

     }
}

void consumeCPU(unsigned char task_id, unsigned short ms)
{
	TickType_t current_load=0;
	TickType_t last_tick=0;
	last_tick=xTaskGetTickCount();
	TickType_t tick;
	
	if (task_id!=0 && current_load==0)
	{	ev[0]=last_tick;
		ev[1]=10+task_id;
		my_trace(last_tick,task_id,10,0);
	}
	while (current_load<ms)
	{
		tick=xTaskGetTickCount();
		if (last_tick!=tick)
		{
			current_load++;
			last_tick=tick;
		}
	}
	current_load=0;
}

void InitHW()
{
// Initialize HW
cli();
SRAMinitiate();
DigitalInit();
RS232_Init(NULL,16,'\r');

sei();
}
