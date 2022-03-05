/*
 * flagControl.h
 *
 *  Created on: Jan 9, 2022
 *      Author: Mecit
 */

#pragma once

#include "datapacket.h"

#define UINT8 	uint8_t
#define UINT16	uint16_t
#define UINT32	uint32_t
#define INT8	int8_t

enum DATA_FLAGS
{
	D_NOT_READY	= 0,
	D_READY		= !D_NOT_READY
};


union controlPanel
{
	struct
	{
		UINT32 IMU_READY	: 1;
		UINT32 GPS_READY	: 1;
		UINT32 TEMP_RDY		: 1;
		UINT32 PRES_RDY		: 1;
		UINT32 ALTI_RDY		: 1;


        UINT32 fixAltitude					: 1 ;
        UINT32 fixAltitudeBefore			: 1 ;
        UINT32 seperatedBefore				: 1 ;
        UINT32 autoMotorControl				: 1 ;
        UINT32 TelemetryNeeded				: 1 ;
        UINT32 manualReleaseActive			: 1 ;
        UINT32 MotorTestActive				: 1 ;
        UINT32 readyToSendSavedVideo		: 1 ;
        UINT32 isUartHANDLING				: 1	;
        UINT32 isSeperatingMechanismActive 	: 1 ;

        UINT32 TELEM_DATA_COPIED			: 1 ;
        UINT32 isvideoSendingProcessActive 	: 1	;

	};



	UINT32 RESET_FLAG;

};

extern enum DATA_FLAGS sensorDataRdyFlag;
extern union controlPanel controlFlags;


void checkDatasReady( void );
