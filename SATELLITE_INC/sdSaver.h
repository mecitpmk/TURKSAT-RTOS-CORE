/*
 * sdSaver.h
 *
 *  Created on: Feb 8, 2022
 *      Author: Mecit
 */

#pragma once


#include "inttypes.h"
#include "stdio.h"

typedef enum
{
	VIDEO_DATA 		= 0,
	PAYLOAD_DATA 	= 1
}SAVING_TYPE;

void logData ( const uint8_t SavingType );
