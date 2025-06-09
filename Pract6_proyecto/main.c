#include "FreeRTOS.h"
#include "task.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "semphr.h"

// MAIN PROGRAM
int main(void) {
    vTaskStartScheduler();
    return 0;
}


// IDLE TASK
void vApplicationIdleHook(void) {
    // THIS RUNS WHILE NO OTHER TASK RUNS
}