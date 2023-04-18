/*
 * control.c
 *
 *  Created on: 2021. okt. 16.
 *      Author: balaz
 */

#include "control.h"

void Control_Init()
{
	SP4T_SelectInput(0);
	DPDT_SelectInput(0);
	SetVarAtt();
}

void SP4T_SelectInput(uint8_t input)
{
	if(input<4)
	{
		switch (input){
			case 0:		//P1 through
				HAL_GPIO_WritePin(RF_IN_CTRL_1_GPIO_Port, RF_IN_CTRL_1_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(RF_IN_CTRL_2_GPIO_Port, RF_IN_CTRL_2_Pin, GPIO_PIN_SET);
				break;
			case 1:		//P1 reflection
				HAL_GPIO_WritePin(RF_IN_CTRL_1_GPIO_Port, RF_IN_CTRL_1_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(RF_IN_CTRL_2_GPIO_Port, RF_IN_CTRL_2_Pin, GPIO_PIN_RESET);
				break;
			case 2:		//P2 through
				HAL_GPIO_WritePin(RF_IN_CTRL_1_GPIO_Port, RF_IN_CTRL_1_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(RF_IN_CTRL_2_GPIO_Port, RF_IN_CTRL_2_Pin, GPIO_PIN_SET);
				break;
			case 3: 	//P2 reflection
				HAL_GPIO_WritePin(RF_IN_CTRL_1_GPIO_Port, RF_IN_CTRL_1_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(RF_IN_CTRL_2_GPIO_Port, RF_IN_CTRL_2_Pin, GPIO_PIN_RESET);
				break;
		}
	}
}

void DPDT_SelectInput(uint8_t input)
{
	if(input<2)
	{
		switch (input) {
			case 0:
				HAL_GPIO_WritePin(RF_OUT_CTRL_GPIO_Port, RF_OUT_CTRL_Pin, GPIO_PIN_SET); // TX to PORT 1
				break;
			default:
				HAL_GPIO_WritePin(RF_OUT_CTRL_GPIO_Port, RF_OUT_CTRL_Pin, GPIO_PIN_RESET);
				break;
		}
	}
}

void SetVarAtt()
{
	HAL_GPIO_WritePin(RX_ATT_GPIO_Port, RX_ATT_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(TX_ATT_GPIO_Port, TX_ATT_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(RX_ATT_GPIO_Port, RX_ATT_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(TX_ATT_GPIO_Port, TX_ATT_Pin, GPIO_PIN_SET);
}
