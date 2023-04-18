/*
 * si446x.h
 *
 *  Created on: 2021. okt. 6.
 *      Author: balaz
 */

#ifndef SI446X_SI446X_H_
#define SI446X_SI446X_H_

#include "stdint.h"
#include "spi.h"
#include "gpio.h"
#include "math.h"
#include "string.h"
#include "float.h"
/* Power Up */
#define POWER_UP_CMD 0x02

/* Power Up - Boot options */
#define BOOT_OPTIONS 0x01

/* Power Up - Xtal options */
#define XTAL_OPTIONS 0x0

/* No operation*/
#define NOP_CMD 0x00

/* Set property */
#define SET_PROPERTY_CMD 0x11

/* Get property */
#define GET_PROPERTY_CMD 0x12

/* Fast response registers read */
#define FRR_A_READ_CMD 0x50
#define FRR_B_READ_CMD 0x51
#define FRR_C_READ_CMD 0x53
#define FRR_D_READ_CMD 0x57

/* Start TX */
#define START_TX_CMD 0x31

/* Start TX - TX Complete state */
#define TXCOMPLETE_STATE 0x03

/* Start TX - Retransmit */
#define RETRANSMIT 0x0

/* Start TX - START */
#define START 0x0

/* Start TX - TX Len */
#define TXLEN 0x35

/* Write TX FIFO */
#define WRITE_TX_FIFO_CMD 0x66

/* Start RX */
#define START_RX_CMD 0x32

/* Start RX - CONDITION*/
#define START_RX_CONDITION 0x0

/* Start RX - RX Len*/
#define RXLEN 0x20

/* Start RX - RX_NEXT_STATE1*/
#define RX_NEXT_STATE1 0x08

/* Start RX - RX_NEXT_STATE2*/
#define RX_NEXT_STATE2 0x03

/* Start RX - RX_NEXT_STATE3*/
#define RX_NEXT_STATE3 0x03

/*RX hop*/
#define RX_HOP 0x36

/* Read CMD Buffer */
#define READ_CMD_BUFF 0x44

/* Get modem status for RSSI */
#define GET_MODEM_STATUS 0x22

/* Get modem status - MODEM_CLR_PEND*/
#define MODEM_CLR_PEND 0x0

/* Freq Control Integer Group */
#define FREQ_CONTROL_INTE_GROUP 0x40

/* Freq Control Integer Index */
#define FREQ_CONTROL_INTE_INDEX 0x00

/* Freq Control Fractional Group*/
#define FREQ_CONTROL_FRAC_GROUP 0x40

/* Freq Control Fractional Indexes*/
#define FREQ_CONTROL_FRAC_INDEX 0x01

/* Prescaler value */
#define NPRESC 2

/* Modem clock generation band Group */
#define MODEM_CLKGEN_BAND_GROUP 0x20

/* Modem clock generation band index */
#define MODEM_CLKGEN_BAND_GROUP_INDEX 0x51

/*Modem high performance mode*/
#define HIGH_PERFORMANCE_MODE 0x01

/* Modem modulation type Group */
#define MODEM_MOD_TYPE_GROUP 0x20

/* Modem modulation type index */
#define MODEM_MOD_TYPE_INDEX 0x0

/* SPI max delay */
#define RADIO_SPI_MAX_DELAY 1000

/* GET interrupt status */
#define GET_INT_STATUS 0x20

/* Power amplifier level group*/
#define PA_PWR_LVL_GROUP 0x22

/* Power amplifier level group*/
#define PA_PWR_LVL_INDEX 0x01

/*RSSI Compensation group*/
#define MODEM_RSSI_COMP_GROUP 0x20

/*RSSI Compensation index*/
#define MODEM_RSSI_COMP_INDEX 0x4e

/*RSSI Latch group*/
#define MODEM_RSSI_CONTROL_GROUP 0x20

/*RSSI Latch index*/
#define MODEM_RSSI_CONTROL_INDEX 0x4c

/*Check threshold at latch*/
#define CHECK_THRESH_AT_LATCH 0x0

/*RSSI Average*/
#define RSSI_AVERAGE 0x00

/*LATCH*/
#define LATCH 0x03

/*Global XO tune group*/
#define GLOBAL_XO_TUNE_GROUP 0x00

/*Global XO tune index*/
#define GLOBAL_XO_TUNE_INDEX 0x00

/*Global XO tune value*/
#define TUNE_VALUE 0x7F

typedef enum band {
	RADIO_BAND_900,
	RADIO_BAND_600,
	RADIO_BAND_450,
	RADIO_BAND_300,
	RADIO_BAND_225,
	RADIO_BAND_150,
	NO_BAND,
}band;

typedef enum txcomplete_state{
	NOCHANGE,
	SLEEP,
	SPI_ACTIVE,
	READY,
	READY2,
	TX_TUNE,
	RX_TUNE,
	RESERVED,
	RX,
}txcomplete_state;

typedef enum modtype{
	CW,
	OOK,
	_2FSK,
	_2GFSK,
	_4FSK,
	_4GFSK,
}modtype;

typedef struct radio {
	GPIO_TypeDef* NSEL_Port;    // NSEL PORT
	uint16_t NSEL_Pin;			// NSEL PIN
	GPIO_TypeDef* SDN_Port;    	// NSEL PORT
	uint16_t SDN_Pin;			// NSEL PIN
	SPI_HandleTypeDef* hspi;	// SPI HANDLER
	uint32_t xtal_freq;			// XO frequency
	uint8_t channel;			//channel number (default = 0)
	modtype modulation;			//CW, OOK, 2FSK, 2GFSK, 4FSK, 4GFSK
	band band;					//radio band (150,225,300,450,600,900)
	uint8_t outdiv;				//output divider value
}si446x_t;


void Si446x_Init(si446x_t* radio, GPIO_TypeDef* NSEL_Port, uint16_t NSEL_Pin,GPIO_TypeDef* SDN_Port, uint16_t SDN_Pin, SPI_HandleTypeDef* hspi, uint32_t xtal_freq, modtype modulation);

void Si446x_SPI_Select(const si446x_t* radio);
void Si446x_SPI_Deselect(const si446x_t* radio);
void Si446x_SPI_TransmitData(const si446x_t* radio, uint8_t *pData, uint16_t Size, uint32_t Timeout);
void Si446x_SPI_ReceiveData(const si446x_t* radio, uint8_t *pData, uint16_t Size, uint32_t Timeout);
void Si446x_SPI_TransmitReceiveData(const si446x_t* radio, uint8_t *pTxData, uint8_t *pRxData, uint16_t Size, uint32_t Timeout);

uint8_t Si446x_PollCTS(const si446x_t* radio);
uint8_t Si446x_WaitForCTS(const si446x_t* radio);

void Si446x_GetPartInfo(const si446x_t* radio);
void Si446x_GetChipStatus(const si446x_t* radio);
void Si446x_GetInterruptStatus(const si446x_t* radio);
void Si446x_GetFifoInfo(const si446x_t* radio);

void Si446x_ClearAllIT(const si446x_t* radio);
void Si446x_ReadCommandBuffer(const si446x_t* radio, uint8_t* stream_buffer, uint8_t stream_len);
void Si446x_PowerUp(const si446x_t* radio);
void Si446x_SetModulationType(const si446x_t* radio);
modtype Si446x_GetModulationType(const si446x_t* radio);
band Si446x_SetBand(si446x_t* radio, uint32_t freq);
band Si446x_GetBand(const si446x_t* radio);
void Si446x_SetFrequency(si446x_t* radio, uint32_t freq);
uint32_t Si446x_GetFrequency(const si446x_t* radio);
void Si446x_SetPowerLevel(const si446x_t* radio, uint8_t pwr_lvl);
uint8_t Si446x_GetPowerLevel(const si446x_t* radio);
void Si446x_StartTX(const si446x_t* radio);
void Si446x_WriteTXFIFO(const si446x_t* radio);
void Si446x_StartRX(const si446x_t* radio);
uint8_t Si446x_GetRSSI(si446x_t* radio);
void Si446x_SetRSSIComp(const si446x_t* radio, uint8_t comp);
void Si446x_SetRSSILatch(const si446x_t* radio);
void Si446x_SetXOTune(const si446x_t* radio);

#endif /* SI446X_SI446X_H_ */
