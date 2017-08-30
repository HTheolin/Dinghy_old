/*
 * Led.cpp
 *
 *  Created on: 5 sep. 2016
 *      Author: Bjorn
 */
#include "Headers/Led.h"

void Led::Startup()
{
	  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	  GPIO_InitTypeDef GPIO_InitDef;

	  GPIO_InitDef.GPIO_Pin = GPIO_Pin_5;
	  GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitDef.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitDef.GPIO_PuPd = GPIO_PuPd_NOPULL;

	  GPIO_Init(GPIOA, &GPIO_InitDef);
}

void Led::Shutdown()
{

}

void Led::Sleep()
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_5);
}

void Led::WakeUp()
{
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
}

int Led::Poll()
{
	return 0;
}

