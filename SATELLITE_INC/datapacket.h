/*
 * datapacket.h
 *
 *  Created on: Jan 8, 2022
 *      Author: Mecit
 */

#pragma once
#include "stm32g4xx_hal.h"

#include "enums_structs.h"

#define UINT8 	uint8_t
#define UINT16	uint16_t
#define UINT32	uint32_t
#define INT8	int8_t


typedef enum
{
	TELEM_1HZ 				= 0,
	TELEM_FLIGHT_HEALTH		= 1,
	GCS_TELEM_REQUEST		= 2,
	GCS_TELEM_MISSION		= 3,
	GCS_TELEM_COMMAND		= 4,
	GCS_TELEM_RESPONSE		= 5,
	CONTAINER_TELEMETRY		= 6,
	VIDEO_PACKAGE_REQUEST	= 7,
	UNKNOWN

}TELEMETRY_PACKAGE_TYPES;

typedef enum
{
    STAT_WAITING		= 0,
    STAT_RISING			= 1,
	STAT_FLIGHTFALL		= 2,
	STAT_SEPERATING		= 3,
    STAT_PAYFALL		= 4,
    STAT_FIXEDALT		= 5,
    STAT_RESCUE			= 6
}FLIGHT_STATUS_t;


typedef enum
{
	VIDEO_NOT_SAVED = 0,
	VIDEO_SAVED		= !VIDEO_NOT_SAVED
}VIDEO_TRANSMISSION_STATUS_t;

typedef enum
{
	TELEM_ERROR 	= 0,
	TELEM_VALIDATED = !TELEM_ERROR
}TELEM_VALID_STATUS;


typedef enum
{
	CONTAINER_DATA_NOT_COLLECTED = 0,
	CONTAINER_DATA_COLLECTED 	 = !CONTAINER_DATA_NOT_COLLECTED
}CONTAINER_TELEM_STATUS_t;



typedef enum
{
	MANUAL_RELEASE	= 0,
	TEST_MOTOR		= !MANUAL_RELEASE
}GCS_COMMAND;

struct Telemetry_1HZ
{
	/* Main Telemetry Strcuture , Takes Sensore Data and
	 * Collect Container Telemetry Data.
	 */


	const UINT32  START_OF_FRAME  	; // 0x5A5A5A5A
	const UINT8  PACKAGE_TYPE	  	; // this is 1Hz telemetry Structure @param => enum PACKAGE_TYPES
	const UINT8  PACKAGE_SIZE    	;
	const UINT16 TEAM_ID			;
	UINT16 PACKET_NUMBER ;  // will be increased every Hz



	float Temperature  ;
	float Pressure     ;
	float Altitude     ;  // relative to Y-Axis
	float DescentSpeed ;  // relative to Y-Axis
	float GPS_Latitude 		;
	float GPS_Longtitude 	;
	float GPS_Altitude 		;
	float Container_Altitude 		;
	float Container_Pressure		;
	float Container_GPS_Latitude	;
	float Container_GPS_Longtitude	;
	float Container_GPS_Altitude	;
	float AltitudeDifference		;
	float pitch	;
	float roll	;
	float yaw	;

	FLIGHT_STATUS_t FLIGHT_STATUS ;			    // for GCS Define => enum FLIGHT_STATUS_t : byte
	VIDEO_TRANSMISSION_STATUS_t VIDEO_TRANSMISSION_STATUS;

	const UINT32 END_OF_FRAME ; // 0x5A5A5A5A


} __attribute__((packed));



struct Telemetry_Health
{
	const UINT32 START_OF_FRAME  ; // 0x5A5A5A5A

	const UINT8 PACKAGE_TYPE	  ; // this is 1Hz telemetry Structure @param => enum PACKAGE_TYPES
	const UINT8 PACKAGE_SIZE    ;
	UINT32 PACKET_NUMBER 		  ; // will be increased every Hz

	UINT16 Fault_in_Temperature ; // According to our boundries (if outside in our boundries increase counter.)
	UINT16 Fault_in_Pressure    ; // According to our boundries (if outside in our boundries increase counter.)
	UINT16 Fault_in_Altitude	  ; // According to our boundries (if outside in our boundries increase counter.)

	UINT32 Fault_in_GCS_TELEM   ;

	const UINT32 END_OF_FRAME    ; // 0x5A5A5A5A

} __attribute__((packed));

typedef struct
{
	const UINT32 START_OF_FRAME  	; // 0x5A5A5A5A

	UINT8 PACKAGE_TYPE 		  	;

	UINT16 videoID				;

	UINT8 videoByte[255]			;

	UINT8 isEnd					;
	const UINT32 END_OF_FRAME    	; // 0x5A5A5A5A
}__attribute__((packed))GCS_Telemetry_t;

typedef struct
{
	const UINT32 START_OF_FRAME  	; // 0x5A5A5A5A

	UINT8 PACKAGE_TYPE 		  	;

	UINT8 COMMAND					;

	const UINT32 END_OF_FRAME    	; // 0x5A5A5A5A
}__attribute__((packed))GCS_Command_t;

struct GCS_Tel_Response
{

	const UINT32 START_OF_FRAME  ; // 0x5A5A5A5A

	UINT8 PACKAGE_TYPE 		  ;

	UINT8 RESPONSE_STATUS		  ;

	const UINT32 END_OF_FRAME    ; // 0x5A5A5A5A
}__attribute__((packed));

struct Container_Telem
{
	// Container Telemetry Directly Copied to this Structure
	const UINT32 START_OF_FRAME  	; // 0x5A5A5A5A

	UINT8 PACKAGE_TYPE				;

	float Container_Altitude 		;
	float Container_Pressure		;
	float Container_GPS_Latitude	;
	float Container_GPS_Longtitude	;
	float Container_GPS_Altitude	;

	const UINT32 END_OF_FRAME		; // 0x5A5A5A5A
}__attribute__((packed));

typedef struct
{
	const UINT32 START_OF_FRAME  	; // 0x5A5A5A5A

	UINT8 PACKAGE_TYPE			;

	UINT16 videoID				;

	const UINT32 END_OF_FRAME		; // 0x5A5A5A5A
}__attribute__((packed))VideoHandler_t;

extern struct Telemetry_1HZ 	Telemetry_1HZ_Data;
extern struct Telemetry_1HZ 	Telemetry_1HZ_Data_BUFF;
extern struct Telemetry_Health 	Telemetry_Health_Data;
extern struct GCS_Tel_Response  GCS_Telemetry_Response;
extern struct Container_Telem   Telemetry_Container;



extern CONTAINER_TELEM_STATUS_t CONTAINER_TELEM_STATUS;
extern GCS_Command_t 			GCS_Command;
extern GCS_Telemetry_t 			GCS_Telemetry;
extern VideoHandler_t			VideoHandler;
void sendTelemetryData( const TELEMETRY_PACKAGE_TYPES TELEM_PACKAGE_TYPES );
void handleTelemPackage ( void );
void transferContainerDatatoMainStruct( void );
