/*
 * si446x.c
 *
 *  Created on: 2021. okt. 6.
 *      Author: balaz
 */
#include "si446x.h"

void Si446x_Init(si446x_t* radio, GPIO_TypeDef* NSEL_Port, uint16_t NSEL_Pin, GPIO_TypeDef* SDN_Port, uint16_t SDN_Pin, SPI_HandleTypeDef* hspi, uint32_t xtal_freq, modtype modulation)
{
	radio->NSEL_Pin = NSEL_Pin;
	radio->NSEL_Port = NSEL_Port;
	radio->SDN_Pin = SDN_Pin;
	radio->SDN_Port = SDN_Port;
	radio->hspi = hspi;
	radio->xtal_freq = xtal_freq;
	radio->channel = 0;
	radio->modulation = modulation;
	radio->band = RADIO_BAND_900;
	radio->outdiv = 4;

	HAL_GPIO_WritePin(radio->SDN_Port, radio->SDN_Pin, GPIO_PIN_SET); 		//SDN UP
	HAL_Delay(1);															// wait >300us
	HAL_GPIO_WritePin(radio->SDN_Port, radio->SDN_Pin, GPIO_PIN_RESET); 	//SDN UP

	HAL_Delay(6); //POR

	Si446x_PowerUp(radio);

	Si446x_ClearAllIT(radio);

	Si446x_WaitForCTS(radio);

	Si446x_SetModulationType(radio);
	Si446x_SetPowerLevel(radio, 50);
//	Si446x_SetRSSIComp(radio, 35);
	Si446x_SetRSSILatch(radio);
	Si446x_SetXOTune(radio);
}

void Si446x_SPI_Select(const si446x_t* radio)
{
	HAL_GPIO_WritePin(radio->NSEL_Port, radio->NSEL_Pin, GPIO_PIN_RESET);
}

void Si446x_SPI_Deselect(const si446x_t* radio)
{
	HAL_GPIO_WritePin(radio->NSEL_Port, radio->NSEL_Pin, GPIO_PIN_SET);
}

void Si446x_SPI_TransmitData(const si446x_t* radio, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	HAL_SPI_Transmit(radio->hspi, pData, Size, Timeout);
}

void Si446x_SPI_ReceiveData(const si446x_t* radio, uint8_t *pData, uint16_t Size, uint32_t Timeout)
{
	HAL_SPI_Receive(radio->hspi, pData, Size, Timeout);
}

void Si446x_SPI_TransmitReceiveData(const si446x_t* radio, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout)
{
	HAL_SPI_TransmitReceive(radio->hspi, pTxData, pRxData, Size, Timeout);
}

uint8_t Si446x_PollCTS(const si446x_t* radio)
{
	uint8_t command[] = {READ_CMD_BUFF};
	uint8_t buffer = 0;
	Si446x_SPI_Select(radio);
//	Si446x_SPI_TransmitReceiveData(radio, command, &buffer, 2, RADIO_SPI_MAX_DELAY);
	Si446x_SPI_TransmitData(radio, command, 1, RADIO_SPI_MAX_DELAY);
	Si446x_SPI_ReceiveData(radio, &buffer, 1, RADIO_SPI_MAX_DELAY);
	Si446x_SPI_Deselect(radio);
	if(buffer == 0xFF)
	{
		return 1; //azaz CTS
	}
	else
	{

		return 0;
	}

}

uint8_t Si446x_WaitForCTS(const si446x_t* radio)
{
	while(!Si446x_PollCTS(radio))
	{
		for (int i = 0; i < 7200; i++)
		{

		}
	}
	return 1;
}

void Si446x_ReadCommandBuffer(const si446x_t* radio, uint8_t* stream_buffer, uint8_t stream_len)
{
	uint8_t command = READ_CMD_BUFF;
	uint8_t buffer = 0;

	for(int i=0; i<stream_len; i++)
	{
		stream_buffer[i] = 0;
	}

	while(buffer != 0xFF)
	{
		Si446x_SPI_Deselect(radio);
		Si446x_SPI_Select(radio);
		Si446x_SPI_TransmitData(radio, &command, 1, RADIO_SPI_MAX_DELAY);
		Si446x_SPI_ReceiveData(radio, &buffer, 1, RADIO_SPI_MAX_DELAY);
	}

	Si446x_SPI_ReceiveData(radio, stream_buffer, stream_len, RADIO_SPI_MAX_DELAY);
	Si446x_SPI_Deselect(radio);
}

void Si446x_GetPartInfo(const si446x_t* radio)
{
	uint8_t command = {0x01};
	uint8_t reply[16] = {0xA5};

	if(Si446x_WaitForCTS(radio))
	{
		Si446x_SPI_Select(radio);
		Si446x_SPI_TransmitData(radio, &command, 1, RADIO_SPI_MAX_DELAY);
		Si446x_SPI_Deselect(radio);

		Si446x_ReadCommandBuffer(radio, reply, 16);

		Si446x_SPI_Deselect(radio);
	}
	Si446x_SPI_Deselect(radio);
}

void Si446x_GetChipStatus(const si446x_t* radio)
{
	uint8_t command[] = {0x23, 0x00};
	uint8_t reply[16] = {0xA5};

	if(Si446x_WaitForCTS(radio))
	{
		Si446x_SPI_Select(radio);
		Si446x_SPI_TransmitData(radio, command, 2, RADIO_SPI_MAX_DELAY);
		Si446x_SPI_Deselect(radio);

		Si446x_ReadCommandBuffer(radio, reply, 16);

		Si446x_SPI_Deselect(radio);
	}
}

void Si446x_ClearAllIT(const si446x_t* radio)
{
	uint8_t command[] = {	GET_INT_STATUS,
							0x00,
							0x00,
							0x00
	};

	if(Si446x_WaitForCTS(radio))
	{
		Si446x_SPI_Select(radio);
		Si446x_SPI_TransmitData(radio, command, 4, RADIO_SPI_MAX_DELAY);
		Si446x_SPI_Deselect(radio);
	}
}

void Si446x_GetInterruptStatus(const si446x_t* radio)
{
	uint8_t stream[16];
	uint8_t command[] = {	GET_INT_STATUS,
							0x00,
							0x00,
							0x00
	};

	if(Si446x_WaitForCTS(radio))
	{
		Si446x_SPI_Select(radio);
		Si446x_SPI_TransmitData(radio, command, 4, RADIO_SPI_MAX_DELAY);
		Si446x_SPI_Deselect(radio);
	}

	Si446x_ReadCommandBuffer(radio, stream, 8);
}

void Si446x_GetFifoInfo(const si446x_t* radio)
{
	uint8_t stream[16];
	uint8_t command[] = {	0x15,
							0x00
	};

	if(Si446x_WaitForCTS(radio))
	{
		Si446x_SPI_Select(radio);
		Si446x_SPI_TransmitData(radio, command, 2, RADIO_SPI_MAX_DELAY);
		Si446x_SPI_Deselect(radio);
	}

	Si446x_ReadCommandBuffer(radio, stream, 2);
}

void Si446x_PowerUp(const si446x_t* radio)
{

	uint8_t command[] = {POWER_UP_CMD, BOOT_OPTIONS, XTAL_OPTIONS,
			(radio->xtal_freq & 0xff000000)>>24,
			(radio->xtal_freq & 0x00ff0000)>>16,
			(radio->xtal_freq & 0x0000ff00)>>8,
			(radio->xtal_freq & 0x000000ff)};

	if(Si446x_WaitForCTS(radio))
	{
		Si446x_SPI_Select(radio);
		Si446x_SPI_TransmitData(radio, command, 7, RADIO_SPI_MAX_DELAY);
		Si446x_SPI_Deselect(radio);
	}
}

void Si446x_SetModulationType(const si446x_t* radio)
{
	uint8_t command[] = {	SET_PROPERTY_CMD,
							MODEM_MOD_TYPE_GROUP,
							1,
							MODEM_MOD_TYPE_INDEX,
							radio->modulation,
					  	 };

	if(Si446x_WaitForCTS(radio))
	{
		Si446x_SPI_Select(radio);
		Si446x_SPI_TransmitData(radio, command, 5, RADIO_SPI_MAX_DELAY);
		Si446x_SPI_Deselect(radio);
	}
}

modtype Si446x_GetModulationType(const si446x_t* radio)
{

	uint8_t stream[16] = {0xA5};

	memset(stream,0xa5,16);

	uint8_t command[] = {	GET_PROPERTY_CMD,
							MODEM_MOD_TYPE_GROUP,
							1,
							MODEM_MOD_TYPE_INDEX
					  	};


	if(Si446x_WaitForCTS(radio))
	{
		Si446x_SPI_Select(radio);
		Si446x_SPI_TransmitData(radio, command, 4, RADIO_SPI_MAX_DELAY);
		Si446x_SPI_Deselect(radio);
 	}

	Si446x_ReadCommandBuffer(radio, stream, 16);

	Si446x_SPI_Deselect(radio);

	switch (stream[0]) {
		case 0:
			return CW;
		case 1:
			return OOK;
		case 2:
			return _2FSK;
		case 3:
			return _2GFSK;
		case 4:
			return _4FSK;
		case 5:
			return _4GFSK;
		default:
			return _2FSK;
	}
}

void Si446x_SetFrequency(si446x_t* radio, uint32_t freq)
{
	//prescaler = 2
	//3600/div
	// value:	 0		1 		2		3	 	4	 	5	 	6	 	7
	//	div		 4		6 		8 		12 		16 		24		24		24
	// freq		 900	600		450		300		225		150		150		150

	uint8_t INTE = 0;
	uint32_t FRAC = 0;

	band current_band = Si446x_SetBand(radio, freq);
	current_band = Si446x_GetBand(radio);
	uint32_t right_element = NPRESC*radio->xtal_freq/radio->outdiv;



	//INTE = 56; //((freq/(radio->xtal_freq/1000000))*(radio->outdiv/NPRESC)-1) & 0x7F;
	INTE = (freq/right_element-1) & 0x7F;
	//FRAC = 978671; //((freq/(radio->xtal_freq/1000000)*(radio->outdiv/NPRESC)-INTE)*pow(2,19)) & 0x0FFFFF;
	FRAC = (((double)freq/(double)right_element)-(double)INTE)*pow(2,19)+1;

	uint8_t command_inte[] = {	SET_PROPERTY_CMD,
								FREQ_CONTROL_INTE_GROUP,
								1,
								FREQ_CONTROL_INTE_INDEX,
								INTE
	};

	uint8_t command_frac[] = {	SET_PROPERTY_CMD,
								FREQ_CONTROL_FRAC_GROUP,
								3,
								FREQ_CONTROL_FRAC_INDEX,
								(FRAC & 0x0F0000) >> 16,
								(FRAC & 0x00FF00) >> 8,
								(FRAC & 0x0000FF),
								};

	if(current_band != NO_BAND)
	{
		Si446x_SPI_Select(radio);

		if(Si446x_WaitForCTS(radio))
		{
			Si446x_SPI_Select(radio);
			Si446x_SPI_TransmitData(radio, command_inte, sizeof(command_inte), RADIO_SPI_MAX_DELAY);
			Si446x_SPI_Deselect(radio);
		}

		if(Si446x_WaitForCTS(radio))
		{
			Si446x_SPI_Select(radio);
			Si446x_SPI_TransmitData(radio, command_frac, sizeof(command_frac), RADIO_SPI_MAX_DELAY);
			Si446x_SPI_Deselect(radio);
		}

		Si446x_SPI_Deselect(radio);
	}

}

uint32_t Si446x_GetFrequency(const si446x_t* radio)
{
	uint8_t buffer[4] = {0};

	uint8_t INTE = 0;
	uint32_t FRAC = 0;

	uint8_t command_inte[] = {	GET_PROPERTY_CMD,
								FREQ_CONTROL_INTE_GROUP,
								1,
								FREQ_CONTROL_INTE_INDEX
							 };

	uint8_t command_frac[] = {	GET_PROPERTY_CMD,
								FREQ_CONTROL_FRAC_GROUP,
								3,
								FREQ_CONTROL_FRAC_INDEX
								};



	if(Si446x_WaitForCTS(radio))
	{
		Si446x_SPI_Select(radio);
		Si446x_SPI_TransmitData(radio, command_inte, sizeof(command_inte), RADIO_SPI_MAX_DELAY);
		Si446x_SPI_Deselect(radio);
	}

	Si446x_ReadCommandBuffer(radio, buffer, 1);

	INTE = buffer[0];


	if(Si446x_WaitForCTS(radio))
	{
		Si446x_SPI_Select(radio);
		Si446x_SPI_TransmitData(radio, command_frac, sizeof(command_frac), RADIO_SPI_MAX_DELAY);
		Si446x_SPI_Deselect(radio);
	}

	Si446x_ReadCommandBuffer(radio, buffer, 4);

	FRAC = buffer[2] + (buffer[1] << 8) + (buffer[0] << 16);

	return (INTE+((double)FRAC/pow(2,19)))*(NPRESC*radio->xtal_freq/radio->outdiv);

}

band Si446x_SetBand(si446x_t* radio, uint32_t freq)
{
	if(freq >= 119000000 && freq <= 159000000)
	{
		uint8_t command[] = {	SET_PROPERTY_CMD,
								MODEM_CLKGEN_BAND_GROUP,
								1,
								MODEM_CLKGEN_BAND_GROUP_INDEX,
								((HIGH_PERFORMANCE_MODE << 3) | RADIO_BAND_150)};

		radio->band = RADIO_BAND_150;
		radio->outdiv = 24;

		if(Si446x_WaitForCTS(radio))
		{
			Si446x_SPI_Select(radio);
			Si446x_SPI_TransmitData(radio, command, 5, RADIO_SPI_MAX_DELAY);
			Si446x_SPI_Deselect(radio);
		}

		return RADIO_BAND_150;
	}
	if(freq >= 177000000 && freq <= 239000000)
	{
		uint8_t command[] = {	SET_PROPERTY_CMD,
								MODEM_CLKGEN_BAND_GROUP,
								1,
								MODEM_CLKGEN_BAND_GROUP_INDEX,
								((HIGH_PERFORMANCE_MODE << 3) | RADIO_BAND_225)};

		radio->band = RADIO_BAND_225;
		radio->outdiv = 16;

		if(Si446x_WaitForCTS(radio))
		{
			Si446x_SPI_Select(radio);
			Si446x_SPI_TransmitData(radio, command, 5, RADIO_SPI_MAX_DELAY);
			Si446x_SPI_Deselect(radio);
		}

		return RADIO_BAND_225;
	}
	if(freq > 239000000 && freq <= 319000000)
	{
		uint8_t command[] = {	SET_PROPERTY_CMD,
								MODEM_CLKGEN_BAND_GROUP,
								1,
								MODEM_CLKGEN_BAND_GROUP_INDEX,
								((HIGH_PERFORMANCE_MODE << 3) | RADIO_BAND_300)};

		radio->band = RADIO_BAND_300;
		radio->outdiv = 12;

		if(Si446x_WaitForCTS(radio))
		{
			Si446x_SPI_Select(radio);
			Si446x_SPI_TransmitData(radio, command, 5, RADIO_SPI_MAX_DELAY);
			Si446x_SPI_Deselect(radio);
		}

		return RADIO_BAND_300;
	}
	if(freq >= 353000000 && freq <= 479000000)
	{
		uint8_t command[] = {	SET_PROPERTY_CMD,
								MODEM_CLKGEN_BAND_GROUP,
								1,
								MODEM_CLKGEN_BAND_GROUP_INDEX,
								((HIGH_PERFORMANCE_MODE << 3) | RADIO_BAND_450)};

		radio->band = RADIO_BAND_450;
		radio->outdiv = 8;

		if(Si446x_WaitForCTS(radio))
		{
			Si446x_SPI_Select(radio);
			Si446x_SPI_TransmitData(radio, command, 5, RADIO_SPI_MAX_DELAY);
			Si446x_SPI_Deselect(radio);
		}

		return RADIO_BAND_450;
	}
	if(freq > 479000000 && freq < 705000000)
	{

		uint8_t command[] = {	SET_PROPERTY_CMD,
								MODEM_CLKGEN_BAND_GROUP,
								1,
								MODEM_CLKGEN_BAND_GROUP_INDEX,
								((HIGH_PERFORMANCE_MODE << 3) | RADIO_BAND_600)};

		/* Not working, ERRATA */

		radio->band = NO_BAND;
		radio->outdiv = 6;


		if(Si446x_WaitForCTS(radio))
		{
			Si446x_SPI_Select(radio);
			Si446x_SPI_TransmitData(radio, command, 5, RADIO_SPI_MAX_DELAY);
			Si446x_SPI_Deselect(radio);
		}


		return NO_BAND;
	}
	if(freq >= 705000000 && freq <= 960000000)
	{
		uint8_t command[] = {	SET_PROPERTY_CMD,
								MODEM_CLKGEN_BAND_GROUP,
								1,
								MODEM_CLKGEN_BAND_GROUP_INDEX,
								((HIGH_PERFORMANCE_MODE << 3) | RADIO_BAND_900)};

		radio->band = RADIO_BAND_900;
		radio->outdiv = 4;

		if(Si446x_WaitForCTS(radio))
		{
			Si446x_SPI_Select(radio);
			Si446x_SPI_TransmitData(radio, command, 5, RADIO_SPI_MAX_DELAY);
			Si446x_SPI_Deselect(radio);
		}

		return RADIO_BAND_900;
	}
	return NO_BAND;
}

band Si446x_GetBand(const si446x_t* radio)
{
	uint8_t buffer[2];

	uint8_t command[] = {	GET_PROPERTY_CMD,
							MODEM_CLKGEN_BAND_GROUP,
							1,
							MODEM_CLKGEN_BAND_GROUP_INDEX,
						};

	if(Si446x_WaitForCTS(radio))
	{
		Si446x_SPI_Select(radio);
		Si446x_SPI_TransmitData(radio, command, 4, RADIO_SPI_MAX_DELAY);
		Si446x_SPI_Deselect(radio);
	}

	Si446x_ReadCommandBuffer(radio, buffer, 2);

	switch (buffer[0] & 0x07) {
		case 0:
			return RADIO_BAND_900;
		case 1:
			return NO_BAND;
		case 2:
			return RADIO_BAND_450;
		case 3:
			return RADIO_BAND_300;
		case 4:
			return RADIO_BAND_225;
		case 5:
			return RADIO_BAND_150;
		default:
			return NO_BAND;
	}
}

void Si446x_SetPowerLevel(const si446x_t* radio, uint8_t pwr_lvl)
{
	uint8_t command[] = {	SET_PROPERTY_CMD,
							PA_PWR_LVL_GROUP,
							1,
							PA_PWR_LVL_INDEX,
							pwr_lvl
					  	 };

	if(Si446x_WaitForCTS(radio))
	{
		Si446x_SPI_Select(radio);
		Si446x_SPI_TransmitData(radio, command, 5, RADIO_SPI_MAX_DELAY);
		Si446x_SPI_Deselect(radio);
	}
}

uint8_t Si446x_GetPowerLevel(const si446x_t* radio)
{
	uint8_t stream = 0xA5;

	uint8_t command[] = {	GET_PROPERTY_CMD,
							PA_PWR_LVL_GROUP,
							1,
							PA_PWR_LVL_INDEX
					  	};


	if(Si446x_WaitForCTS(radio))
	{
		Si446x_SPI_Select(radio);
		Si446x_SPI_TransmitData(radio, command, 4, RADIO_SPI_MAX_DELAY);
		Si446x_SPI_Deselect(radio);
 	}

	Si446x_ReadCommandBuffer(radio, &stream, 1);

	Si446x_SPI_Deselect(radio);

	return stream;
}

void Si446x_WriteTXFIFO(const si446x_t* radio)
{
//	uint8_t command[7] = {	WRITE_TX_FIFO_CMD,
//							0x2d,
//							0xd4,
//							0x2d,
//							0xd4,
//							0xFF,
//							0xFF
//	};

	uint8_t command[TXLEN+1];
	command[0] = WRITE_TX_FIFO_CMD;
	memset(&command[1],0xFF,TXLEN);

	Si446x_SPI_Select(radio);
	Si446x_SPI_TransmitData(radio, command, TXLEN+1, RADIO_SPI_MAX_DELAY);
	Si446x_SPI_Deselect(radio);
}

void Si446x_StartTX(const si446x_t* radio)
{
	uint8_t command[] = {	START_TX_CMD,
							radio->channel,
							(TXCOMPLETE_STATE << 4 ) | (RETRANSMIT << 2) | (START),
							(TXLEN & 0xF00) >> 8,
							(TXLEN & 0x0FF)
	};

	if(Si446x_WaitForCTS(radio))
	{
		Si446x_SPI_Select(radio);
		Si446x_SPI_TransmitData(radio, command, sizeof(command), RADIO_SPI_MAX_DELAY);
		Si446x_SPI_Deselect(radio);
	}

}

void Si446x_StartRX(const si446x_t* radio)
{
	uint8_t command[] = {	START_RX_CMD,
							radio->channel,
							START_RX_CONDITION,
							(RXLEN & 0xF00) >> 8,
							(RXLEN & 0x0FF),
							RX_NEXT_STATE1,
							RX_NEXT_STATE2,
							RX_NEXT_STATE3
	};

	if(Si446x_WaitForCTS(radio))
	{
		Si446x_SPI_Select(radio);
		Si446x_SPI_TransmitData(radio, command, 8, RADIO_SPI_MAX_DELAY);
		Si446x_SPI_Deselect(radio);
	}

}

uint8_t Si446x_GetRSSI(si446x_t* radio)
{
	uint8_t command[] = {	GET_MODEM_STATUS,
							MODEM_CLR_PEND
	};

	uint8_t buffer[8];

	if(Si446x_WaitForCTS(radio))
	{
		Si446x_SPI_Select(radio);
		Si446x_SPI_TransmitData(radio, command, 2, RADIO_SPI_MAX_DELAY);
		Si446x_SPI_Deselect(radio);
	}

	Si446x_ReadCommandBuffer(radio, buffer, 8);

	//read MODEM_PEND
	//read MODEM_STATUS
	//read CURR_RSSI
	//read LATCH_RSSI
	//read ANT1_RSSI
	//read ANT2_RSSI
	//read AFC_FREQ_OFFSET [15:0]


	return buffer[2];

}

void Si446x_SetRSSIComp(const si446x_t* radio, uint8_t comp)
{
	uint8_t command[] = {	SET_PROPERTY_CMD,
							MODEM_RSSI_COMP_GROUP,
							1,
							MODEM_RSSI_COMP_INDEX,
							comp
	};

	if(Si446x_WaitForCTS(radio))
	{
		Si446x_SPI_Select(radio);
		Si446x_SPI_TransmitData(radio, command, 5, RADIO_SPI_MAX_DELAY);
		Si446x_SPI_Deselect(radio);
	}

}

void Si446x_SetRSSILatch(const si446x_t* radio)
{
	uint8_t command[] = {	SET_PROPERTY_CMD,
							MODEM_RSSI_CONTROL_GROUP,
							1,
							MODEM_RSSI_CONTROL_INDEX,
							(CHECK_THRESH_AT_LATCH << 5) | (RSSI_AVERAGE << 3) | (LATCH)
	};

	if(Si446x_WaitForCTS(radio))
	{
		Si446x_SPI_Select(radio);
		Si446x_SPI_TransmitData(radio, command, 5, RADIO_SPI_MAX_DELAY);
		Si446x_SPI_Deselect(radio);
	}
}

void Si446x_SetXOTune(const si446x_t* radio)
{
	uint8_t command[] = {	SET_PROPERTY_CMD,
							GLOBAL_XO_TUNE_GROUP,
							1,
							GLOBAL_XO_TUNE_INDEX,
							TUNE_VALUE
	};

	if(Si446x_WaitForCTS(radio))
	{
		Si446x_SPI_Select(radio);
		Si446x_SPI_TransmitData(radio, command, 5, RADIO_SPI_MAX_DELAY);
		Si446x_SPI_Deselect(radio);
	}
}
