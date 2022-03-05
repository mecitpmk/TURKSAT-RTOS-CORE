/*
 * enums_structs.h
 *
 *  Created on: Dec 30, 2021
 *      Author: Mecit
 */

#pragma once

#include "stm32g4xx_hal.h"
#include "datapacket.h"


#include "flagControl.h"

#define UINT8 	uint8_t
#define UINT16	uint16_t
#define UINT32	uint32_t
#define INT8	int8_t

//////////////////////////////// ENUM PARTS ////////////////////////////////
enum
{
	RX_BUFFER_SIZE   = 272,
	MAIN_BUFFER_SIZE = RX_BUFFER_SIZE +64
};
//////////////////////////////// END OF ENUM PARTS ////////////////////////////////



//////////////////////////////// STRUCT PARTS ////////////////////////////////

struct sUartBuffers // 's' meaning structure
{
	UINT8 RX_Buffer[RX_BUFFER_SIZE];
	UINT8 Main_Uart_Buffer[MAIN_BUFFER_SIZE];
	UINT16 LastReceivedByte;
};


struct Packet_UART_Interface
{
	UART_HandleTypeDef * const uartInterface ;
};


// externed Variables
extern struct sUartBuffers uartBuffers;
extern struct Packet_UART_Interface PCGK_UART;

//////////////////////////////// END OF STRUCT PARTS ////////////////////////////////



