/*
 * control.h
 *
 *  Created on: 2021. okt. 16.
 *      Author: balaz
 */

#ifndef CONTROL_CONTROL_H_
#define CONTROL_CONTROL_H_

#include "gpio.h"

void Control_Init();
void SP4T_SelectInput(uint8_t input);
void DPDT_SelectInput(uint8_t input);
void SetVarAtt();

#endif /* CONTROL_CONTROL_H_ */
