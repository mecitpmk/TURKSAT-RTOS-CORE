/*
 * msControl.h
 *
 *  Created on: Jan 9, 2022
 *      Author: Mecit
 */

#pragma once

//#include  "datapacket.h"

#include "inttypes.h"

#define UINT8 	uint8_t
#define UINT16	uint16_t
#define UINT32	uint32_t
#define INT8	int8_t

#define TRUE 1
#define FALSE 0

// MSCONTROL PARTS
enum
{
	CONSTANT = 0,
	INCREASE,
	DECREASE
};

enum
{
	SPEED_NOT_ADJUSTED = 0,
	SPEED_ADJUSTED     = !SPEED_NOT_ADJUSTED
};

enum
{
	SEPEARTING_MECHANISM_DEACTIVE	= 0,
	SEPEARTING_MECHANISM_ACTIVE		= !SEPEARTING_MECHANISM_DEACTIVE
};

struct MSControl
{

	float    lastAltitude      ;
	float    currentAltitude   ;
	float    DescentSpeed	   ;

	UINT32 LastReadedTime    ;
	UINT32 currentReadedTime ;



	union
	{
		struct
		{
			UINT8 SPEED_ADJUST   : 1 ;
			UINT8 SPEED_SHOULD   : 2 ; // CONSTANT, DECREASE , INCREASE
			UINT8 isReady		 : 1 ;
		};
		UINT8 RESET_FLAGS;
	}FLAGS;

};

extern struct MSControl descentControl;


void setRPM( void );
void releasePayload( void );
void testMotorWORK( void );

