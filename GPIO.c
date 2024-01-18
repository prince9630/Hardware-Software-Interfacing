/* my_main.c: main file for monitor */
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>

#include "common.h"

/* This include will give us the CubeMX generated defines */
#include "main.h"

void init_GPIOA(void) {
  GPIO_InitTypeDef GPIO_InitStruct;
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_5;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

ParserReturnVal_t CmdGPIO(int mode)
{
  uint32_t pin, val;

  if(mode != CMD_INTERACTIVE) return CmdReturnOk;

  if(fetch_uint32_arg(&pin) || (pin != 0 && pin != 1 && pin != 5)) {
    printf("Please give input of a valid pin number (0, 1, or 5) of GPIOA\n");   //give the input which pin you need to active
    return CmdReturnBadParameter1;
  }

  if(fetch_uint32_arg(&val) || (val != 0 && val != 1)) {
    printf("Please give input (0 or 1) to control the LED on that pin\n");  //give input to turn on or off the led
    return CmdReturnBadParameter2;
  }

  init_GPIOA();

  if(pin == 5) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, val);
  } else if(pin == 1) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, val);
  } else if(pin == 0) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, val);
  }

  return CmdReturnOk;
}

ADD_CMD("GPIO",CmdGPIO,"Pase a pin number (0, 1, or 5) of GPIOA and a value (0 or 1) to control the LED on that pin")   //command to control the led 