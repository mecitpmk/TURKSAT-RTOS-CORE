/*
 * msControl.c
 *
 *  Created on: Jan 9, 2022
 *      Author: Mecit
 */



#include  "msControl.h"
#include  "datapacket.h"
#include  "FreeRTOS.h"
#include  "task.h"


static void calculateDescentSpeed( void );
static void decisionForMotorSpeed( void );


typedef enum
{
	ALT_FIXED		= 0,
	AUTO_CONTROL	= !ALT_FIXED
}FLIGHT_CNTRL;

#define MS_TO_SEC_COEFF 1000

struct MSControl descentControl =  { .LastReadedTime = 0 ,  .FLAGS = {.RESET_FLAGS = 0} , .currentAltitude = 0 , .lastAltitude = 0 , .DescentSpeed = 0};



// Speed Should be bounded in 4-6 m/s

/* Bu fonksiyon, önceki okunan altitude değerinden , şimdiki altitude değerini çıkarıp,
 * 				 önceki okunan altitude zamanından , şimdiki okunan altitude zamanını çıkararak
 *
 * Y-axis'teki düşüş hızını tespit eder ve telemetri paketine kopyalar.
 *
 *
 * @retval void
 */
static void decisionForMotorSpeed( void )
{
	descentControl.DescentSpeed = ( ( descentControl.currentAltitude - descentControl.lastAltitude )/ ( (float)( descentControl.currentReadedTime - descentControl.LastReadedTime ) ) )*MS_TO_SEC_COEFF;
	if ( FALSE == controlFlags.TELEM_DATA_COPIED ) // IF the MAIN STRUCTURE NOT COPIED TO BUFFER YET DIRETLY PASTE IT
	{
		vTaskSuspendAll();
		Telemetry_1HZ_Data.DescentSpeed = descentControl.DescentSpeed;
		xTaskResumeAll();
	}
	else // IF THE MAIN STRUCTURE ALLREADY COPIED BUFFER, JUST CHANGE FROM THE BUFFER.
	{
		Telemetry_1HZ_Data_BUFF.DescentSpeed = descentControl.DescentSpeed;
	}
}



/*
 * Bu fonksiyon, Y-eksenindeki düşüş hızına göre motor pwm sinyalinin arttırılıp, azaltılıp ya da sabit kalacağına karar veren algoritmik yapıya sahiptir.
 *
 * Ayrıca eğer uydumuz otomatik kontrol modunda değilse irtifa sabit modunda ise , olduğu moda göre hızının yükseltilip alçalılmasına karar verecektir.
 *
 *
 * @retval void
 */
static void calculateDescentSpeed( void )
{

	decisionForMotorSpeed(); // Calculates DescentSpeed




	INT8 MIN_INTERVAL;
	INT8 MAX_INTERVAL;

	switch ( controlFlags.autoMotorControl )
	{
		case AUTO_CONTROL:
			MIN_INTERVAL = 4;
			MAX_INTERVAL = 6;
			break;
		case ALT_FIXED:
			MIN_INTERVAL = -1;
			MAX_INTERVAL = 1;
			break;
		default:
			break;
	}

	// Decision for speed INCREASE DECREASE OR What?
	if ( descentControl.DescentSpeed <= MIN_INTERVAL )
	{
		descentControl.FLAGS.SPEED_SHOULD = DECREASE;
	}
	else if ( descentControl.DescentSpeed >= MAX_INTERVAL )
	{
		descentControl.FLAGS.SPEED_SHOULD = INCREASE;
	}
	else
	{
		descentControl.FLAGS.SPEED_SHOULD = CONSTANT;
	}

	return ;
}


/* setRPM fonksiyonu gerekli hız karar fonksiyonundan aldığı geri beslemeye dayanarak,
 * motora verilecek olan pwm sinyalini arttıracak , azaltacak ya da hiç bir şey yapmayacak.
 *
 *
 * @retval void
 */
void setRPM( void )
{


	calculateDescentSpeed();

	switch ( descentControl.FLAGS.SPEED_SHOULD )
	{
		case DECREASE: // PWM AZALT.
			break;
		case INCREASE: // PWM YUKSELT.
			break;
		case CONSTANT: // Do Nothing.
			break;
		default :
			break;
	}


//	descentControl.FLAGS.SPEED_ADJUST = SPEED_ADJUSTED;

	return;
}


/* Ayrışma mekanizması fonksiyonu.
 *
 * Bu fonksiyon ile ayrışmayı tetikleyeceğiz.
 *
 * @retval void
 */
void releasePayload( void )
{
	// Relaese payload mechanism.
	// Necessary pwm things..
	return ;
}


/* Manuel Tahrik komutu  bu fonksiyonu call ederek,
 * Motora gerekli pwm sinyallerini iletecektir.
 *
 * @retval void
 */
void testMotorWORK( void )
{
	// This function give a constant speed to the motor.
	// Give constant pwm to the motor.
	return ;
}

