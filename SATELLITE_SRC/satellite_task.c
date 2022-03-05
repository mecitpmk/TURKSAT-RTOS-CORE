/*
 * satellite_task.c
 *
 *  Created on: Jan 9, 2022
 *      Author: Mecit
 */


#include "satellite_tasks.h"
#include "msControl.h"
#include "string.h"
#include "sensors.h"
#include "sdSaver.h"

extern DMA_HandleTypeDef hdma_usart2_tx;
extern DMA_HandleTypeDef hdma_usart2_rx;

TaskHandle_t SensorTASK_HANDLE			= NULL;
TaskHandle_t CommunucationTASK_HANDLE	= NULL;
TaskHandle_t MotorControllerTASK_HANDLE	= NULL;

TimerHandle_t sensorTimer		= NULL;
TimerHandle_t healthDataTimer	= NULL;
TimerHandle_t fixedAltTimer		= NULL;


// Eventler için belirlediğimiz spesifik notify değerleri.
enum
{
	UART_RX_EVENT 	= 0x01,
	TELEMETRI_SEND 	= 0x08
};

static UINT32 UART_EVT_RX = (UINT32)UART_RX_EVENT;
static UINT32 TELEM_EVENT = (UINT32)TELEMETRI_SEND;




/* IDLE Rx event tamamlandığında bu fonksiyonumuz tetiklenmektedir.
 * CommuncationTask'ımızı notify ediyoruz.
 *
 * @retval void
 */
void HAL_UARTEx_RxEventCallback( UART_HandleTypeDef *huart, uint16_t Size )
{

	BaseType_t xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;

	uartBuffers.LastReceivedByte = Size;	// Gelen boyutu, LastReceived variable'ına aktar. (transfer total received bytes to LastReceivedByte)



	// Data geldiğine dair Notify işlemini sağla.
	xTaskNotifyFromISR( CommunucationTASK_HANDLE ,
						UART_EVT_RX ,
						eSetBits ,
						&xHigherPriorityTaskWoken );

	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}



/* Timer Callback Function
 * Bu fonksiyon 2 amaç için kullanılmaktadır.
 *
 * 	1. Amaç : 200m'de uydumuz sabitlenme görevine sahiptir. Sensörlerden alınan veriler doğrultusunda uydumuz altitude'unu 10 saniye boyunca korumalıdır.
 * 		10 saniye sonunda , model uydu oto motor kontrol algoritmasına geri dönmesine imkan tanınmaktadır.
 *
 * 	2. Amaç : Test gereksinimi gereği, yer istasyonundan gönderilen manual motor tahrik komutu, 10 saniye boyunca aktive kalmalıdır.
 * 		10 saniye sonunda, programımız callback sayesinde bu  fonksiyona girerek, PWM sinyalini kesmektedir.
 */
void fixAltTimerCLLBCK( TimerHandle_t pxTimer ) // This callback Timer function is also used in 10 sec Motor test.
{



	// end of 10 seconds Altitude shouldnt fixed anymore!.


	if ( !controlFlags.MotorTestActive ) // Eğer Manual Motor Tahrik komutu flag'i aktif değil ise : 1.Amaç.
	{
		vTaskSuspendAll();
		controlFlags.fixAltitude		= FALSE;
		controlFlags.autoMotorControl 	= TRUE;

		xTaskResumeAll();
	}

	else // 2.Amaç
	{
		// Manuel motor tahrik durdur.
		// PWM 0
	}

}



/* Timer Callback Function
 * Bu fonksiyon 1000ms'ın geçtiğini programımıza haberdar etmek için mevcuttur.
 *
 * @retval void
 */
void sensorTimerCLLBCK( TimerHandle_t pxTimer )
{

	// Telemetri için 1sec tamamlandı. Notify işlemini gerçekleştir.
    xTaskNotify( CommunucationTASK_HANDLE,
    			TELEM_EVENT,
				eSetBits);

}



/* Timer Callback Function
 * Bu fonksiyon 1000ms'ın geçtiğini programımıza haberdar etmek için mevcuttur.
 *
 * @retval void
 */
void healthTimerCLLBCK( TimerHandle_t pxTimer )
{
	// Send the strcuture with UART in Here.
	// Deactived for now.


//	sendTelemetryData( TELEM_FLIGHT_HEALTH );
	Telemetry_Health_Data.PACKET_NUMBER += 1;


	return;
}


/* Bu task , BMP , IMU , GPS okumalarını yapıp, telemetri gereksinimi kontrol eder.
 * Eğer telemetri gereksinimi mevcut ise telemetri structure'ını doldurur.
 *
 * @retval void
 */
void SensorTASK( void * pvParameters )
{
	/* Bu Task ,  BMP Sensörü, IMU Sensörü, ve GPS'ten alınan verileri eğer ki sensörlerden okunan tüm veriler hazırsa
	 * 	Telemetri Structure'ına Yer istasyonuna gönderilmek üzere kopyalar.
	 */


	sensorTimer	= xTimerCreate( "sTMR", pdMS_TO_TICKS( ( 1000/SENSOR_DATA_HZ ) ), pdTRUE, NULL, &sensorTimerCLLBCK ); // Telemetrileri 1Hz olacak şekilde göndermek üzere oluşturduğumuz Timer.
	xTimerStart( sensorTimer , portMAX_DELAY ); // Timer'ı başlatıyoruz.

	controlFlags.TelemetryNeeded = TRUE; // Telemetri gereksinimi öncelikli olarak TRUE olarak atanmıştır.
	for(;;)
	{
		// sensor readings...

		readBMP(); // Bmp sensörünü oku.
		readIMU(); // IMU sensörünü oku.

		// readGPS() => bu fonksiyon içerisinde bir flag olsun, 250 ms de bir (4HZ) yada 2HZ de bir okuyalım .Bunu da HW Timer ile sağlayalım. Flag değişsin, Anlayalım 250 ms geçtiğini ve okuyalım.


		// Eğer ki Telemetri gereksinimiz var ise
		if ( controlFlags.TelemetryNeeded && !controlFlags.TELEM_DATA_COPIED )
		{
			checkDatasReady(); // Tüm sensör dataları hazır olup olmadığını kontrol edelim.
			if ( D_READY == sensorDataRdyFlag ) // Tüm sensör dataları hazır ise.
			{
				controlFlags.TelemetryNeeded = FALSE; // Telemetri gereksinimi flag'ini değiştir.

				// copy the Telemetry to the buffer.
				memcpy ( &Telemetry_1HZ_Data_BUFF , &Telemetry_1HZ_Data , sizeof ( Telemetry_1HZ_Data ) ); // Telemetri Structure'rını kopyala.

				vTaskSuspendAll();
				controlFlags.TELEM_DATA_COPIED = TRUE; // Telemetrinin kopyalandığına dair flag'i değiştir.
				xTaskResumeAll();
			}

		}
		vTaskDelay( pdMS_TO_TICKS( 200 ) );
	}
	return;
}



/* Bu task'ta RTOS'un LightWeight olan TaskNotification Api'sini kullandık. Daha az RAM kullanımı ve %45 e varan hız verimini kullanmayı amaçladık.
 * Bu task , herhangi bir notification almadığı sürece CPU tüketmemektedir.
 *
 * Notification Process:
 * 	-1HZ Telemetri paketini göndermek üzere 	notify edilir.
 * 	-Uart'tan bir data paketi hazır olduğunda 	notify edilir.
 *
 * @retval void
 */
void CommunucationTASK( void * pvParameters )
{
	healthDataTimer	= xTimerCreate( "hTMR", pdMS_TO_TICKS( ( 1000/HEALTH_DATA_HZ ) ), pdTRUE, NULL, &healthTimerCLLBCK ); // Telemetri Sağlık durumu Timer'ı.
	xTimerStart( healthDataTimer , portMAX_DELAY ); // Telemetri sağlık durumu timer'ını başlat.


	HAL_UARTEx_ReceiveToIdle_DMA( PCGK_UART.uartInterface , uartBuffers.RX_Buffer, RX_BUFFER_SIZE ); // Bilinmeyen boyutta datalar gelebileceğinden dolayı idle DMA kullandık.
	__HAL_DMA_DISABLE_IT( &hdma_usart2_rx , DMA_IT_HT );

	UINT32 NOTIFIED_VALUE;
	for(;;)
	{

		xTaskNotifyWait( 0, 0xFFFFFFFF  , &NOTIFIED_VALUE, portMAX_DELAY ); // Notify gelene kadar bekle. Exit olduğunda gelen bitleri temizle (0xFFFFFFFF)

		if ( ( TELEMETRI_SEND & NOTIFIED_VALUE ) != 0 ) // Telemetri Eventi , 1Hz Timer tarafından bu notify oluşturulmuştur.
		{
			// Tüm sensörler okunduysa ve konteynır tarafından telemetri verileri alındı ise.
			if ( ( D_READY == sensorDataRdyFlag ) && ( CONTAINER_DATA_COLLECTED == CONTAINER_TELEM_STATUS ) ) // IF all sensors are readed and container datas have been collected.
			{
				transferContainerDatatoMainStruct(); // Combine Container Datas. (Konteynır'ın telemetrilerini gönderilecek structure'a kopyala.)
				sendTelemetryData( TELEM_1HZ ); // Telemetriyi gönder.
				logData( PAYLOAD_DATA ); // SD Karta kaydet.

				vTaskSuspendAll();
				controlFlags.TELEM_DATA_COPIED  = FALSE	; // Telemetriyi tekrar kopyalayabilmek için 	  flag'i değiştir.
				controlFlags.TelemetryNeeded 	= TRUE	; // Yeni Telemetri gerekli olduğunu anlamak için flag'i değiştir.
				sensorDataRdyFlag 				= FALSE	; // Sensör Dataları hazır değil.
				Telemetry_1HZ_Data.PACKET_NUMBER += 1	; // Bir sonraki gönderilecek telemetri paketi için paket numarasını arttır.

				//CONTAINER_TELEM_STATUS = CONTAINER_DATA_NOT_COLLECTED;  // GCS ile Haberleşme Testi icin Commentledim.
				xTaskResumeAll();
			}


		}
		if ( ( UART_RX_EVENT & NOTIFIED_VALUE ) != 0 ) // Eğerki DMA RX Idle tarafından notify edildi isek.
		{
			memset( uartBuffers.Main_Uart_Buffer, '\0' , MAIN_BUFFER_SIZE ); // Main Buffer'ın içerisindeki verileri temizle.
			memcpy( uartBuffers.Main_Uart_Buffer, uartBuffers.RX_Buffer , uartBuffers.LastReceivedByte ); // Alınan byte'ları main buffer'a kopyala. (Copy the received bytes to Main Buffers.)
			memset( uartBuffers.RX_Buffer , '\0' ,RX_BUFFER_SIZE ); // Kopyalama işlemi bittiği için (Main buffer'a memcpy'lediğimiz için)   RX buffer'ı temizle.

			HAL_UARTEx_ReceiveToIdle_DMA( PCGK_UART.uartInterface , uartBuffers.RX_Buffer, RX_BUFFER_SIZE ); // RX IDLE DMA'yı tekrar aktive hale getir.
			__HAL_DMA_DISABLE_IT( &hdma_usart2_rx , DMA_IT_HT );

			handleTelemPackage(); // Gelen paketi çözümle.

		}


	}
	return;
}



/* Bu task , BMP sensöründen alınan verilere dayanarak,
 * Otomatik motor hız kontrol ve irtifa sabitleme görevlerini bu fonksiyon altında gerçekleştirmektedir.
 * @retval void
 */
void MotorControlTASK( void * pvParameters )
{
	fixedAltTimer = xTimerCreate( "fATMR", pdMS_TO_TICKS( ( 10000 ) ), pdFALSE, NULL, &fixAltTimerCLLBCK ); // manual motor tahrik ve irtifa sabitleme timer'ını oluşturduk fakat başlatmadık.
	for(;;)
	{
		if ( SEPEARTING_MECHANISM_ACTIVE == controlFlags.isSeperatingMechanismActive ) // Ayrışma mekanizması flag'i aktif ise.
		{

			releasePayload(); // Ayrıştırmayı gerçekleştir.

			vTaskSuspendAll();
			controlFlags.autoMotorControl 				= TRUE;  // oto motor kontrol devrede.
			controlFlags.isSeperatingMechanismActive 	= FALSE; // ayrışma mekanizması devre dışı.
			xTaskResumeAll();
		}

		else if ( ( STAT_FIXEDALT == Telemetry_1HZ_Data.FLIGHT_STATUS ) && ( TRUE == controlFlags.fixAltitude ) ) // Uçuş durumu  İrtifa sabitleme durumundaysa.
		{
			// Altitude fix

			static UINT8 flg = 0; //
			if (0 == flg) // Timer daha önce init edilmedi ise . (if the timer is not initalized before.)
			{
				flg = 1; // tekrar if condition'dan timer'ı aktive etmemek için flag'ı değiştir.
				xTimerStart( fixedAltTimer, portMAX_DELAY ); // timer'ı başlat. (start timer.)
			}

			if ( TRUE == descentControl.FLAGS.isReady ) // BMP Sensörü gerekli okumaları yaptıysa.
			{
				// speed kontrol for Fixing Altitude.

				setRPM(); // Hız sabitlemek için setRPM fonksiyonunu call ediyoruz. Bu fonksiyon gerekli PWM sinyalini motorumuza iletecektir.
				descentControl.FLAGS.isReady = FALSE; // Diğer bir ölçüm için flag'i değiştir.

			}

		}
		else if ( TRUE == controlFlags.autoMotorControl  ) // Eğer oto motor kontrol devrede ise
		{

			if ( TRUE == descentControl.FLAGS.isReady ) // BMP ölçümü hazır ise. (if measurement is not ready pass.)
			{

				setRPM(); // Motor hız kontrolünü sağla.
				descentControl.FLAGS.isReady = FALSE; //  Diğer bir ölçüm için flag'i değiştir. (wait for another Measurement.)
			}

		}
	}
	return;
}
