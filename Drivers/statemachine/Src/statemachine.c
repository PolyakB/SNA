/*
 * statemachine.c
 *
 *  Created on: 2021. okt. 1.
 *      Author: balaz
 */

#include "statemachine.h"
#include "stdint.h"
#include "si446x.h"
#include "control.h"

extern statemachine_t sm;
extern uint32_t startFreq;
extern uint32_t stopFreq;
extern uint32_t resolution;
extern si446x_t transmitter;
extern si446x_t receiver;

void StateMachine_Init(statemachine_t* sm, state_t state, UART_HandleTypeDef* huart)
{
	sm->state = state;
	sm->huart = huart;
}

void WaitingForCommand(statemachine_t* sm)
{

}

void GettingStartFrequency(statemachine_t* sm)
{

}

void GettingStopFrequency(statemachine_t* sm)
{

}

void GettingResolution(statemachine_t* sm)
{

}

void StartMeasurement(statemachine_t* sm)
{
	/* mondjuk led villanás */
	HAL_GPIO_WritePin(TestLED_GPIO_Port, TestLED_Pin, GPIO_PIN_SET);
	HAL_Delay(100);
	HAL_GPIO_WritePin(TestLED_GPIO_Port, TestLED_Pin, GPIO_PIN_RESET);
	sm->state = State_Measuring;
}

void Measuring(statemachine_t* sm)
{
	uint8_t switchPosition = 0;
	uint8_t RSSI=0;

	  for (uint32_t i = startFreq; i < stopFreq; i+=resolution)
	  {
		  HAL_GPIO_TogglePin(TestLED_GPIO_Port, TestLED_Pin);
		  Si446x_SetFrequency(&transmitter, i);
		  Si446x_SetFrequency(&receiver, i);
		  if(Si446x_GetBand(&transmitter) != NO_BAND)
		  {
			  HAL_UART_Transmit(sm->huart, &i, 4, 1000);				//Freq
			  for(int j = 0; j < 2; j++) //dpdt
			  {
				  for (int k = 0; k < 4; k++)	//sp4t
				  {
					  switchPosition = (k << 1) | j;
					  SP4T_SelectInput(k);
					  DPDT_SelectInput(j);
					  HAL_Delay(1);
					  Si446x_WriteTXFIFO(&transmitter);
					  HAL_Delay(1);
					  Si446x_StartTX(&transmitter);
					  Si446x_StartRX(&receiver);
					  RSSI = Si446x_GetRSSI(&receiver);
	 //				  HAL_UART_Transmit(sm->huart, &switchPosition, 1, 1000);	//switch
					  HAL_UART_Transmit(sm->huart, &RSSI, 1, 1000);				//RSSI
				  }
			  }
		  }
	  }
	  HAL_GPIO_WritePin(TestLED_GPIO_Port, TestLED_Pin, GPIO_PIN_RESET);
	  sm->state = State_WaitingForCommand;
}

void CalibrationOpen(statemachine_t* sm)
{

}

void CalibrationShort(statemachine_t* sm)
{

}

void CalibrationThrough(statemachine_t* sm)
{

}

void Calibration50Ohm(statemachine_t* sm)
{

}
