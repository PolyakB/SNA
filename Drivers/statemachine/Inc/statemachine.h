/*
 * statemachine.h
 *
 *  Created on: 2021. okt. 1.
 *      Author: balaz
 */

#ifndef INC_STATEMACHINE_H_
#define INC_STATEMACHINE_H_

#include "usart.h"

typedef enum{
	GET_START_FREQUENCY_CMD=1,
	GET_STOP_FREQUENCY_CMD,
	GET_RESOLUTION_CMD,
	START_MEASUREMENT_CMD,
	CALIBRATE_OPEN_CMD,
	CALIBRATE_SHORT_CMD,
	CALIBRATE_THROUGH_CMD,
	CALIBRATE_50_OHM_CMD,
}command_t;

typedef enum{
	State_WaitingForCommand = 0,
	State_GettingStartFrequency,
	State_GettingStopFrequency,
	State_GettingResolution,
	State_StartMeasurement,
	State_Measuring,
	State_CalibrationOpen,
	State_CalibrationShort,
	State_CalibrationThrough,
	State_Calibration50Ohm,
}state_t;

typedef struct statemachine{
	state_t state;
	UART_HandleTypeDef* huart;
}statemachine_t;

void StateMachine_Init(statemachine_t* sm, state_t state, UART_HandleTypeDef* huart);

void WaitingForCommand(statemachine_t* sm);
void GettingStartFrequency(statemachine_t* sm);
void GettingStopFrequency(statemachine_t* sm);
void StartMeasurement(statemachine_t* sm);
void Measuring(statemachine_t* sm);
void CalibrationOpen(statemachine_t* sm);
void CalibrationShort(statemachine_t* sm);
void CalibrationThrough(statemachine_t* sm);
void Calibration50Ohm(statemachine_t* sm);



#endif /* INC_STATEMACHINE_H_ */
