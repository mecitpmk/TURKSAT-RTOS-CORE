/*
 * sensors.h
 *
 *  Created on: Jan 9, 2022
 *      Author: Mecit
 */

#pragma once



#include "inttypes.h"

typedef enum
{
	OUTSIDE_BOUNDRY = 0,
	INSIDE_BOUNDRY  = !OUTSIDE_BOUNDRY
}BOUNDRY_RESULT;

struct IMU
{
	float PITCH;
	float ROLL;
	float YAW;

};

struct BMP_Sensor
{
	float PRESSURE;
	float TEMPERATURE;
	float ALTITUDE;
};

struct Sensors_Boundry
{
	const float MIN_TEMP;
	const float MAX_TEMP;

	const float MIN_PRESS;
	const float MAX_PRESS;

	const float MIN_ALT;
	const float MAX_ALT;

	const float MIN_PITCH;
	const float MAX_PITCH;

	const float MIN_YAW;
	const float MAX_YAW;

	const float MIN_ROLL;
	const float MAX_ROLL;
};



extern const struct Sensors_Boundry	Sensor_Boundries;
extern struct IMU IMU_Data;
extern struct BMP_Sensor BMP_Data;

BOUNDRY_RESULT checkIMUBoundries( void );
BOUNDRY_RESULT checkBMPBoundries( void );


void findFlightStatus( void );
void readIMU( void );
void readBMP( void );

