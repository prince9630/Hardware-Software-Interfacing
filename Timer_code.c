#include <stdio.h>
#include <stdint.h>
#include "common.h"
#include "main.h"

TIM_HandleTypeDef tim11;
int a = 0;

ParserReturnVal_t Timer(int mode)
{
  uint32_t val, rc;
  if (mode != CMD_INTERACTIVE)
    return CmdReturnOk;

  printf("Timer For LD2\n");

  rc = fetch_uint32_arg(&val);
  if (rc)
  {
    printf("Please supply the time value in seconds to flash the built-in LED\n");
    return CmdReturnBadParameter1;
  }

  __HAL_RCC_TIM11_CLK_ENABLE();
  tim11.Instance = TIM11;
  tim11.Init.Prescaler = HAL_RCC_GetPCLK2Freq() / 1000000000 - 1; // set the prescaler
  tim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim11.Init.Period = (val * 1000); // set the period in milliseconds
  tim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim11.Init.RepetitionCounter = 0;
  tim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  HAL_TIM_Base_Init(&tim11);

  HAL_NVIC_SetPriority(TIM1_TRG_COM_TIM11_IRQn, 10, 0U);
  HAL_NVIC_EnableIRQ(TIM1_TRG_COM_TIM11_IRQn);

  HAL_TIM_Base_Start_IT(&tim11);
  return CmdReturnOk;
}
ADD_CMD("timer", Timer, "timer <Value of required time in s>")

// FUNCTION      : TIM1_TRG_COM_TIM11_IRQHandler
//
// DESCRIPTION   :
//   This is the interrupt handler for timer 11 and 1. It is usually created by CubeMX if we were using it
//
// PARAMETERS    :
//   None
//
// RETURNS       :
//   Nothing
void TIM1_TRG_COM_TIM11_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&tim11);
}

// FUNCTION      : HAL_TIM_PeriodElapsedCallback
//
// DESCRIPTION   :
//   This is the interrupt handler for timer 11 and 1. It is usually created by CubeMX if we were using it
//
// PARAMETERS    :
//   htim - the handle to the timer configuration information
//
// RETURNS       :
//   Nothing
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim == &tim11)
  {
    printf("Blinking %d\n", a++);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5); // toggle the on-board LED
  }
}
