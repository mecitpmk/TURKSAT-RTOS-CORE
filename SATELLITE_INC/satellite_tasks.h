/*
 * satellite_tasks.h
 *
 *  Created on: Jan 9, 2022
 *      Author: Mecit
 */

#pragma once

#include "string.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "main.h"
#include "stdio.h"
#include "enums_structs.h"


#define SENSOR_DATA_HZ  (1)   //   1  Hz  1 times in 1 second.
#define HEALTH_DATA_HZ  (0.5) // 0.5Hz  1 times in 2 second.

extern TaskHandle_t SensorTASK_HANDLE;
extern TaskHandle_t CommunucationTASK_HANDLE;
extern TaskHandle_t MotorControllerTASK_HANDLE;


extern TimerHandle_t sensorTimer;
extern TimerHandle_t healthDataTimer;
extern TimerHandle_t fixedAltTimer	;





void sensorTimerCLLBCK( TimerHandle_t pxTimer );
void healthTimerCLLBCK( TimerHandle_t pxTimer );
void fixAltTimerCLLBCK( TimerHandle_t pxTimer );

void SensorTASK( void * pvParameters );
void CommunucationTASK( void * pvParameters );
void MotorControlTASK( void * pvParameters );
