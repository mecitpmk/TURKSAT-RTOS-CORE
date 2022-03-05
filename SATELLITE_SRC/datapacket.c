/*
 * datapacket.c
 *
 *  Created on: Jan 8, 2022
 *      Author: Mecit
 */


#include "datapacket.h"
#include "string.h"
#include "stm32g4xx_hal.h"



#include "FreeRTOS.h"
#include "timers.h"
#include "msControl.h"
#include "task.h"
#include "sdSaver.h"



#define SOF_HEADER 0x5A5A5A5A
#define EOF_HEADER 0x5A5A5A5A
#define PACKAGE_TYPE_IDX 4


//Static Function Prototypes
static void solvePackageType( void );
static void handleManualCommands( void );
static void handleGCSMission( void );
static void GCS_Command_Inconsistent ( void );
static void bufferParser ( void );



//Externs
//extern UART_HandleTypeDef huart2; // TO SEND Telemetry datas.. // Comment it if PCGK_UART.uartInterface used otherwise uncomment and use &huart2 or &whateveryouwant
extern TimerHandle_t fixedAltTimer	;
extern union controlPanel controlFlags;
extern DMA_HandleTypeDef hdma_usart2_rx;


//Structures
struct Telemetry_1HZ 	 Telemetry_1HZ_Data		= { .START_OF_FRAME = SOF_HEADER , .END_OF_FRAME = EOF_HEADER , .PACKAGE_TYPE = TELEM_1HZ , .PACKET_NUMBER = 1 , .PACKAGE_SIZE = sizeof( struct Telemetry_1HZ ) ,.FLIGHT_STATUS = STAT_WAITING,
													.VIDEO_TRANSMISSION_STATUS = VIDEO_NOT_SAVED, .TEAM_ID = 26222};

struct Telemetry_Health  Telemetry_Health_Data	= { .START_OF_FRAME = SOF_HEADER , .END_OF_FRAME = EOF_HEADER , .PACKAGE_TYPE = TELEM_FLIGHT_HEALTH , .PACKET_NUMBER = 1 , .PACKAGE_SIZE= sizeof(struct Telemetry_Health),
													.Fault_in_GCS_TELEM = 0};

struct GCS_Tel_Response  GCS_Telemetry_Response = { .START_OF_FRAME = SOF_HEADER  , .END_OF_FRAME = EOF_HEADER , .PACKAGE_TYPE = GCS_TELEM_RESPONSE };

struct Container_Telem   Telemetry_Container	= { .START_OF_FRAME = SOF_HEADER , .END_OF_FRAME = EOF_HEADER , .PACKAGE_TYPE = CONTAINER_TELEMETRY };

struct Telemetry_1HZ 	 Telemetry_1HZ_Data_BUFF = { .START_OF_FRAME = SOF_HEADER , .END_OF_FRAME = EOF_HEADER , .PACKAGE_TYPE = TELEM_1HZ , .PACKET_NUMBER = 1 , .PACKAGE_SIZE = sizeof( struct Telemetry_1HZ ),
													 .TEAM_ID = 26222};

//CONTAINER_TELEM_STATUS_t CONTAINER_TELEM_STATUS = CONTAINER_DATA_NOT_COLLECTED;
CONTAINER_TELEM_STATUS_t CONTAINER_TELEM_STATUS = CONTAINER_DATA_COLLECTED; // Test icin direkt olarak collected atandı.

GCS_Command_t			GCS_Command				= {.START_OF_FRAME = SOF_HEADER , .END_OF_FRAME = EOF_HEADER , .PACKAGE_TYPE = GCS_TELEM_COMMAND };

GCS_Telemetry_t 		GCS_Telemetry			= {.START_OF_FRAME = SOF_HEADER , .END_OF_FRAME = EOF_HEADER , .PACKAGE_TYPE = GCS_TELEM_MISSION };

VideoHandler_t			VideoHandler 			= {.START_OF_FRAME = SOF_HEADER , .END_OF_FRAME = EOF_HEADER , .PACKAGE_TYPE = VIDEO_PACKAGE_REQUEST , .videoID = 1};



// Gelen paketin header'larını çözümlemek için kullanacağımız structure yapısı.
typedef struct
{
	UINT16 startPosition;
	UINT16 endPosition;
	UINT8  isVALID;
}Eof_Seof_t;

static Eof_Seof_t parsedDataInfo = {.startPosition = 0,.endPosition = 0,.isVALID = 0};




// Şu anlık devre dışı
//static struct Telemetry_Health  Telemetry_Health_BUFF = { .START_OF_FRAME = SOF_HEADER , .END_OF_FRAME = EOF_HEADER , .PACKAGE_TYPE = TELEM_FLIGHT_HEALTH , .PACKET_NUMBER = 1 , .PACKAGE_SIZE = TELEM_HEALTH_BUFF_SIZE,
//														.Fault_in_GCS_TELEM = 0};
//




/* @param TELEM_PACKAGE_TYPES => @group of TELEMETRY_PACKAGE_TYPES
 *
 * Bu fonksiyon, aldığı paket tipine göre, point edilmiş uartInterface'i üzerinden datayı gönderir.
 *
 * @retval void
 */
void sendTelemetryData( const TELEMETRY_PACKAGE_TYPES TELEM_PACKAGE_TYPES )
{

	switch (TELEM_PACKAGE_TYPES)
	{
		case TELEM_1HZ:
			while ( HAL_UART_Transmit_DMA( PCGK_UART.uartInterface,  ( UINT8* )&Telemetry_1HZ_Data_BUFF, sizeof( Telemetry_1HZ_Data_BUFF ) ) != HAL_OK ); // Print waiting for data only just one time.
			break;
		case TELEM_FLIGHT_HEALTH:
//			while ( HAL_UART_Transmit_DMA( PCGK_UART.uartInterface,  ( UINT8* )&Telemetry_Health_Data, sizeof ( Telemetry_Health_Data ) ) != HAL_OK ); // Print waiting for data only just one time.
			break;
		case VIDEO_PACKAGE_REQUEST:
			while ( HAL_UART_Transmit_DMA( PCGK_UART.uartInterface,  ( UINT8* )&VideoHandler, sizeof ( VideoHandler ) ) != HAL_OK ); // Print waiting for data only just one time.
			break;
		case GCS_TELEM_RESPONSE:
			while ( HAL_UART_Transmit_DMA( PCGK_UART.uartInterface,  ( UINT8* )&GCS_Telemetry_Response, sizeof ( GCS_Telemetry_Response ) ) != HAL_OK );
			break;
		default :
			break;

	}

}


/* Bu fonksiyon gelen yer istasyon paketi doğrulandıktan sonra,
 * 	5.byte'ı kontrol edilir. 5.Byte bizlere bu paketin hangi tür paket olduğunu söylemektedir.
 * 	Bu paket şunlar olabilir:
 * 		-Container Telemeri Paketi
 * 		-Yer istasyonu video paketi
 * 		-Yer istasyonu komut paketi
 *
 * 	Gelen paketin tipi tespit edildikten sonra direkt olarak daha önce oluşturulmuş olan structure'a copy edilir ve gerekli handle fonksiyonları çağırılır.
 *
 *
 * @retval void
 */
static void solvePackageType( void )
{
	/* Do Something according to incoming Telemetry type.
	 * For example if its about GCS Mission (commmand) Copy it and Handle it.
	 *  or if its about Container telemetry copy it immediately.
	 */

	switch ( uartBuffers.Main_Uart_Buffer[ parsedDataInfo.startPosition +4 ] )
	{
		case CONTAINER_TELEMETRY:
			if ( ( ( parsedDataInfo.endPosition+3 - parsedDataInfo.startPosition )+1 ) == sizeof( GCS_Telemetry ) )
			{
				memcpy( &Telemetry_Container , uartBuffers.Main_Uart_Buffer , sizeof( Telemetry_Container ) );
				vTaskSuspendAll();
				CONTAINER_TELEM_STATUS = CONTAINER_DATA_COLLECTED;
				xTaskResumeAll();
			}
			break;
		case GCS_TELEM_MISSION:
			if ( ( ( parsedDataInfo.endPosition+3 - parsedDataInfo.startPosition )+1 ) == sizeof( GCS_Telemetry ) )
			{
				controlFlags.isvideoSendingProcessActive = TRUE;
				memcpy( &GCS_Telemetry , uartBuffers.Main_Uart_Buffer, sizeof( GCS_Telemetry ) );
				handleGCSMission();
			}
			else
			{
				GCS_Command_Inconsistent();
			}

			break;
		case GCS_TELEM_COMMAND:
			if ( ( ( parsedDataInfo.endPosition+3 - parsedDataInfo.startPosition )+1 ) == sizeof( GCS_Command ) )
			{
				memcpy( &GCS_Command , uartBuffers.Main_Uart_Buffer , sizeof( GCS_Command ) );
				handleManualCommands();
			}
			else
			{
				GCS_Command_Inconsistent();
			}

			break;
		default :
			break;
	}

}




/* Bu fonksiyondaki amaç konteynır tarafından gelen paketindeki dataları, yer istasyonuna telemetri paketini göndermeden önce
 * telemetri structure'ına kopyalamaktır.
 *
 *  İçerdiği datalar:
 *  	-Basınç
 *  	-Yükseklik
 *  	-GPS Altitude
 *  	-GPS Longtitude
 *  	-Gps Latitude
 *
 * @retval void
 */
void transferContainerDatatoMainStruct( void )
{
	/* Purpose : Maybe when the container sends their telemetry, sensor data (Telemetry_1HZ_Data) hasnot been prepared yet.
	 * So that if we directly copy them to the (Telemetry_1HZ_Data_BUFF) after memcpy our container datas will be diseppear.
	 * To prevent this, first we check the conditions and after that we copy them.
	 */
	Telemetry_1HZ_Data_BUFF.Container_Altitude 		 = Telemetry_Container.Container_Altitude;
	Telemetry_1HZ_Data_BUFF.Container_Pressure 		 = Telemetry_Container.Container_Pressure;
	Telemetry_1HZ_Data_BUFF.Container_GPS_Altitude 	 = Telemetry_Container.Container_GPS_Altitude;
	Telemetry_1HZ_Data_BUFF.Container_GPS_Latitude 	 = Telemetry_Container.Container_GPS_Latitude;
	Telemetry_1HZ_Data_BUFF.Container_GPS_Longtitude = Telemetry_Container.Container_GPS_Longtitude;
	Telemetry_1HZ_Data_BUFF.AltitudeDifference 		 = Telemetry_1HZ_Data_BUFF.Altitude - Telemetry_Container.Container_Altitude;

}



/* Gelen yer istasyonu paketi, komut paketi içeriyor ise içeren komut tipine göre programımızı yönlendiren fonksiyon.
 * 	Komut tipleri :
 * 		- Manuel Ayrışma komutu
 * 		- Motor Tahrik (10 saniye boyunca) komutu
 *
 * @retval void
 */
static void handleManualCommands( void )
{
	switch ( GCS_Command.COMMAND )
	{
		case MANUAL_RELEASE:
			// Activate Manual Release Mechanism in Here.
			controlFlags.manualReleaseActive = TRUE;
			releasePayload();
			break;
		case TEST_MOTOR:
			xTimerStart( fixedAltTimer, portMAX_DELAY ); // start timer for 10 sec.
			testMotorWORK(); // This function gives a motor constant speed .
			break;
		default:
			break;
	}


}




/* Gelen yer istasyonu paketi, video datası içeriyorsa otomatikmen bu fonksiyon call edilmektedir.
 *  Bu fonksiyon, video kayıt edilmediği sürece, yer istasyonundan gelen paketin ID'si, arzu edilen video ID'si ile eş değer ise
 *  paket geçerli kabul edilip, yer istasyonundan bir sonraki video paketini talep eder.
 *
 *  Eğer paket geçersiz ise, arzu edilen paket ID'sinden başka bir paket yollandı ise, kontrolcümüz bunun farkında olup,
 *  	arzu edilen paketin ID'sini tekrardan yer istasyonuna iletir.
 *
 * @retval void
 */
static void handleGCSMission( void )
{

	if (VIDEO_NOT_SAVED == Telemetry_1HZ_Data.VIDEO_TRANSMISSION_STATUS)
	{
		if ( GCS_Telemetry.videoID == VideoHandler.videoID )
		{

			// Gelen video ID verisi doğru. Al SD Karda Kaydet.
			// Incoming Video ID is correct. Take it and Store it to SD Card.
			// storeVideoToSD()
			if ( GCS_Telemetry.isEnd )
			{
				vTaskSuspendAll();
				Telemetry_1HZ_Data.VIDEO_TRANSMISSION_STATUS = VIDEO_SAVED 	; // Change flags.
				controlFlags.readyToSendSavedVideo			 = VIDEO_SAVED 	; // That flags says us to you can send SAVED video's to another frequency bands.
				controlFlags.isvideoSendingProcessActive 	 = FALSE		;
				xTaskResumeAll();
			}
			else
			{
				VideoHandler.videoID += 1;
				sendTelemetryData( VIDEO_PACKAGE_REQUEST );
			}
		}
		else // CGS'den Gelen videoID'si istenilen VideoID'si ile eşleşmiyor ise, gereken videoID'yi GCS'den Request et!
		{
			sendTelemetryData( VIDEO_PACKAGE_REQUEST );
		}
	}
}








/* Gelen datanın ilk 4 byte'ının 0x5A5A5A5A ise ve son 4 byte'ı 0x5A5A5A5A ise
 * startPosition ve endPosition'a index değerlerini aktarıyoruz ve
 * parsedDataInfo structure'ına gelen datanın geçerli olup olmadığını aktarıyoruz.
 *
 *
 * @retval void
 */
static void bufferParser ( void )
{

	memset( &parsedDataInfo , 0,  sizeof( parsedDataInfo ) );
	UINT8 findedBefore = FALSE ;
	for (UINT16 i = 0 ; i < MAIN_BUFFER_SIZE-4; i++) // Change MAIN_BUFFER_SIZE to uartBuffers.LastReceivedByte
	{
		if (  uartBuffers.Main_Uart_Buffer[i] == 0x5A  &&  uartBuffers.Main_Uart_Buffer[i+1] == 0x5A  && uartBuffers.Main_Uart_Buffer[i+2] == 0x5A  && uartBuffers.Main_Uart_Buffer[i+3] == 0x5A   )
		{
			if ( !findedBefore )
			{
				parsedDataInfo.startPosition = i;
				findedBefore = TRUE;
			}
			else
			{
				parsedDataInfo.endPosition 	= i;
				parsedDataInfo.isVALID		= TRUE;
				break;
			}
		}
	}
}


/* Gelen paketin geçersiz olduğunu yer istasyonuna iletir.
 * Telemetri Sağlık durumundaki Telemetri Hata Sayacını 1 arttırır.
 *
 * @retval void.
 */
static void GCS_Command_Inconsistent ( void )
{
	// Increase fault counter , Send Response to GCS ( says hey your SOF_HEADER and EOF_HEADER IS NOT OKAY, SEND me them again.)
	// or SOF_HEADER and EOF_HEADER is OK But Size IS NOT OK!
	vTaskSuspendAll();
	Telemetry_Health_Data.Fault_in_GCS_TELEM += 1;
	xTaskResumeAll();
	GCS_Telemetry_Response.RESPONSE_STATUS = TELEM_ERROR;
	sendTelemetryData( GCS_TELEM_RESPONSE );
}

/* Bu fonksiyon, CommunucationTask tarafından, IDLE event tamamlandıtkan sonra call edilmektedir.
 * Gelen paketin geçerli olup olmadığını kontrol eder . Paket geçerli bir paket ise paketi çözümler değil ise
 * Yer istasyonuna gönderilen paketin geçersiz olduğunu iletir.
 *
 * @retval void
 */
void handleTelemPackage ( void )
{


	bufferParser();
	if ( TELEM_VALIDATED == parsedDataInfo.isVALID)
	{
		// telem is validated checks whats the telemetry type.
		solvePackageType();
	}
	else
	{
		GCS_Command_Inconsistent();
	}




}


