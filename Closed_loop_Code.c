#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "common.h"
#include "main.h"

HAL_StatusTypeDef rc;
TIM_HandleTypeDef tim1;
TIM_HandleTypeDef tim3;
TIM_HandleTypeDef tim11;

//Variable declerations
int intFlag = 0;
uint32_t rcSpeedSet, count = 0;
int16_t speedSet = 0;
float kp = 0.1, ki = 0.1, kd = 0.01, previousError = 0;
uint16_t currentRPM = 0, perviousPulse = 0;
float errorI = 0;
float errorD = 0;

       
     ParserReturnVal_t encoderInit (int mode)
{

  //Timer 1 initialization fpr PWM
  __HAL_RCC_TIM1_CLK_ENABLE ();
  tim1.Instance = TIM1;
  tim1.Init.Prescaler = HAL_RCC_GetPCLK2Freq () / 100000 - 1;
  tim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim1.Init.Period = HAL_RCC_GetPCLK2Freq () / 1000000 - 1;	//number here will be the number of ms for on/off set to 100-1.
  tim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim1.Init.RepetitionCounter = 0;
  tim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;	// this parameter will cause the timer to reset when timed out
  HAL_TIM_Base_Init (&tim1);

  TIM_OC_InitTypeDef sConfig;
  sConfig.OCMode = TIM_OCMODE_PWM1;
  sConfig.Pulse = 0;
  sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfig.OCNPolarity = TIM_OCNPOLARITY_LOW;
  sConfig.OCFastMode = TIM_OCFAST_DISABLE;
  sConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  HAL_TIM_PWM_ConfigChannel (&tim1, &sConfig, TIM_CHANNEL_1);
  //HAL_TIM_PWM_ConfigChannel (&tim1, &sConfig, TIM_CHANNEL_2);

  //Timer 11 initialization for Intrrupt
  __HAL_RCC_TIM11_CLK_ENABLE ();
  tim11.Instance = TIM11;
  tim11.Init.Prescaler = HAL_RCC_GetPCLK2Freq () / 100000 - 1;	//(set to 1000 - 1 for ms)
  tim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim11.Init.Period = HAL_RCC_GetPCLK2Freq () / 100000 - 1;	//number here will be the number of ms for on/off set to 100-1.
  tim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim11.Init.RepetitionCounter = 0;
  tim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;	// this parameter will cause the timer to reset when timed out
  HAL_TIM_Base_Init (&tim11);
  HAL_NVIC_SetPriority (TIM1_TRG_COM_TIM11_IRQn, 0, 0U);	// note the timer 11 IRQ on this board is shared with Timer 1
  HAL_NVIC_EnableIRQ (TIM1_TRG_COM_TIM11_IRQn);

  //Timer 3 initialization for encoder
  __HAL_RCC_TIM3_CLK_ENABLE ();

  tim3.Instance = TIM3;
  tim3.Init.Prescaler = 0;
  tim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim3.Init.Period = 0xffff;
  tim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim3.Init.RepetitionCounter = 0;
  rc = HAL_TIM_Base_Init (&tim3);
  if (rc != HAL_OK)
    {
      printf ("Failed to initialize Timer 3 Base,rc=%u\n", rc);
      return CmdReturnOk;
    }

  HAL_StatusTypeDef rc;
  TIM_Encoder_InitTypeDef encoderConfig;
  encoderConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  encoderConfig.IC1Polarity = 0;
  encoderConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  encoderConfig.IC1Prescaler = 0;
  encoderConfig.IC1Filter = 3;
  encoderConfig.IC2Polarity = 0;
  encoderConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  encoderConfig.IC2Prescaler = 0;
  encoderConfig.IC2Filter = 3;
  rc = HAL_TIM_Encoder_Init (&tim3, &encoderConfig);
  if (rc != HAL_OK)
    {
      printf ("Failed to initialize Timer 3 Encoder, rc=%u\n", rc);
      return CmdReturnOk;
    }


  rc = HAL_TIM_Encoder_Start (&tim3, TIM_CHANNEL_1);
  if (rc != HAL_OK)
    {
      printf ("Failed to start Timer 3 Encoder, rc=%u\n", rc);
      return CmdReturnOk;
    }
  rc = HAL_TIM_Encoder_Start (&tim3, TIM_CHANNEL_2);
  if (rc != HAL_OK)
    {
      printf ("Failed to start Timer 3 Encoder, rc=%u\n", rc);
      return CmdReturnOk;
    }


  //GPIO Initialization
  GPIO_InitTypeDef GPIO_InitStruct = { 0 };

   __HAL_RCC_GPIOB_CLK_ENABLE ();
  GPIO_InitStruct.Pin = (GPIO_PIN_0| GPIO_PIN_1);
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init (GPIOA, &GPIO_InitStruct);
  
  __HAL_RCC_GPIOA_CLK_ENABLE ();
  GPIO_InitStruct.Pin = (GPIO_PIN_6 | GPIO_PIN_7);
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 2;
  HAL_GPIO_Init (GPIOA, &GPIO_InitStruct);

  __HAL_RCC_GPIOA_CLK_ENABLE ();
  GPIO_InitStruct.Pin = (GPIO_PIN_8);
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 1;
  HAL_GPIO_Init (GPIOA, &GPIO_InitStruct); 
      HAL_TIM_PWM_Start (&tim1, TIM_CHANNEL_1);
      HAL_TIM_Base_Start_IT (&tim11);
  TIM3->CNT = 0;


  return CmdReturnOk;
}


ADD_CMD ("initialize", encoderInit, "		Initialize the pins")
      
ParserReturnVal_t stop(int mode)
{
  if (mode != CMD_INTERACTIVE)
    return CmdReturnOk;

  intFlag = 0;
  HAL_TIM_PWM_Stop(&tim1, TIM_CHANNEL_1);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, RESET);

  currentRPM = 0;
  errorD = 0;
  errorI = 0;
  previousError = 0;
  
  printf("Motor stopped.\r\n");

  return CmdReturnOk;
}

ADD_CMD ("stop", stop, "		Stops the motor")
       
ParserReturnVal_t rpm(int mode)
{
  if (mode != CMD_INTERACTIVE)
    return CmdReturnOk;

  printf("%d RPM OF THE MOTOR\n", currentRPM);

  return CmdReturnOk;
}

ADD_CMD ("output_speed", rpm, "		Gives the RPM of the motor")
      
ParserReturnVal_t rotateMotor(int mode)
{
  if (mode != CMD_INTERACTIVE)
    return CmdReturnOk;

  rcSpeedSet = fetch_int16_arg(&speedSet);
  if (rcSpeedSet)
  {
    printf("Please supply MOTOR 1\n");
    return CmdReturnBadParameter1;
  }
  else
  {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

    int8_t setSpeedPercentage = ((speedSet * 100) / 210);
    TIM1->CCR1 = setSpeedPercentage;
    printf("%d\n", setSpeedPercentage);
    intFlag = 1;
  }

  return CmdReturnOk;
}

ADD_CMD ("input_speed", rotateMotor, "<target RPM>	supply the target speed")
      
     void TIM1_TRG_COM_TIM11_IRQHandler (void)
{
  HAL_TIM_IRQHandler (&tim11);
}

float error = 0;
uint16_t previousRPM = 0;


void HAL_TIM_PeriodElapsedCallback (TIM_HandleTypeDef * htim)
{
   if (htim == &tim11)		
    {
      if (intFlag == 1)
	{
	  if (TIM3->CNT < 65535)
	    {
	      if (perviousPulse > TIM3->CNT)
		{
		  currentRPM = previousRPM;
		}
	      else
		{
		  currentRPM = (((TIM3->CNT) - perviousPulse) * 600) / (120);
		}
	      perviousPulse = TIM3->CNT;
	      previousRPM = currentRPM;
	      error = speedSet - currentRPM;
	      previousError = error;
	      errorI += (error * 0.1);
	      float iTerm = errorI * ki;
	      errorD = (error - previousError);
	      float dTerm = kd * ((errorD - error) / 0.1);
	      errorD = error;
	      error = (error * kp) + iTerm + dTerm;
	      float correctedPWM = (error *100 / 210)  ;
	      TIM1->CCR1 = correctedPWM;
	      
	      
	      
	    }
	  else
	    {
	      TIM3->CNT = 0;
	      perviousPulse = -1;
	      error = 0;
	     }
	}
    }
}