/*
 * sdSaver.c
 *
 *  Created on: Feb 8, 2022
 *      Author: Mecit
 */

#include "sdSaver.h"
#include "datapacket.h"


/* Kayıt tipine göre SD KART'a erişim sağlayıp, log etmemize yarayan sarmalayıcı fonksiyondur.
 * 	Kayıt tipleri:
 * 		-VIDEO_DATA
 * 		-PAYLOAD_DATA
 *
 *
 * @return void
 */
void logData ( const uint8_t SavingType )
{
	switch ( SavingType )
	{
		case VIDEO_DATA:
			//Access to SD CARD.
			break;
		case PAYLOAD_DATA:
		{
			// Acess to SD CARD.
			break;
		}

		default:
			break;
	}
}
