#ifndef UTILS_PLANIF_H
#define UTILS_PLANIF_H

#include "FreeRTOS.h"
#include "task.h"
#include "sram.h"
#include "digital.h"

/******************************************************************************
 * Problem data.
 ******************************************************************************/

extern unsigned char num_tasks;
extern TickType_t taskPeriod[];
extern unsigned int computationTime[];
extern unsigned char taskPrio[];
extern unsigned int start[];

/****************************************************/

void my_trace(uint16_t tick, char tag,char userdata,char resource_id);
void consumeCPU(unsigned char task_id, unsigned short ms);
void InitHW();

#endif