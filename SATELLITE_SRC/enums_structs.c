/*
 * enums_structs.c
 *
 *  Created on: Dec 30, 2021
 *      Author: Mecit
 */

#include "enums_structs.h"



// extern variables
extern UART_HandleTypeDef huart2;



///////////// INIT VARIABLES ///////////////////



struct sUartBuffers uartBuffers 		  =  { .LastReceivedByte = 0 , .Main_Uart_Buffer = {0} , .RX_Buffer = {0} };
struct Packet_UART_Interface PCGK_UART    =  { .uartInterface = &huart2 };



///////////// INIT VARIABLES ///////////////////


