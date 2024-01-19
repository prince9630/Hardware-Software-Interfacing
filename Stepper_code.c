#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include "common.h"
#include "main.h"



void forward(uint8_t channel, double pwmValue);
void reverse(uint8_t channel, double pwmValue);
void stop(uint8_t channel, double pwmValue);

int32_t ispwmInit = 0;
int32_t channel = 0;
int32_t isChannel = 0;
int32_t pwmValue = 0;
int32_t ispwmValue = 0;
double pwmPercentage = 0;
 
TIM_HandleTypeDef htim1;				// Handle for timer

// Function:     pwminit
// Description:  Initialize pwm
// Prameter:     mode
// Return:       cmdReturnOK
ParserReturnVal_t dcinit(int mode)
{

        __HAL_RCC_GPIOA_CLK_ENABLE(); 
   	__HAL_RCC_GPIOC_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStructA = {0};
        GPIO_InitStructA.Pin = GPIO_PIN_1;
	GPIO_InitStructA.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructA.Pull = GPIO_NOPULL;
	GPIO_InitStructA.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructA.Alternate = 0;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStructA);

	GPIO_InitTypeDef GPIO_InitStructC = {0};
        GPIO_InitStructC.Pin = GPIO_PIN_6 | GPIO_PIN_8 |GPIO_PIN_9;
	GPIO_InitStructC.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructC.Pull = GPIO_NOPULL;
	GPIO_InitStructC.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructC.Alternate = 0;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStructC);
  __HAL_RCC_TIM1_CLK_ENABLE();			// Enable timer 1 clock

  TIM_OC_InitTypeDef sConfigOC = {0}; 
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  // Configuration of timer
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = HAL_RCC_GetPCLK2Freq() / 100000000 - 1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 100;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

  HAL_TIM_Base_Init(&htim1);
  HAL_TIM_PWM_Init(&htim1); 

  // Configuration of pwm
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

  __HAL_RCC_GPIOA_CLK_ENABLE();		// GPIO port A clock enable

  // Configuration of GPIO port A
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Configuration of channel 1, 2 and 3 of timer 1
  HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1);
  HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2);
  HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3);

  // Start channel 1, 2 and 3 of timer 1
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

  printf("PWM initilize successfully\n");

  ispwmInit = 1;

  return CmdReturnOk;
}


// Function:     pwm
// Description:  Glow led on specific channel
// Prameter:     mode
// Return:       cmdReturnOK
Copy code
ParserReturnVal_t dir(int mode)
{    
    int32_t isdir;
    int32_t dir;
    int32_t pwmValue;
    int32_t channel;
    double pwmPercentage;
  
    if(ispwmInit == 1)					
    {
        if(fetch_int32_arg(&channel) || fetch_int32_arg(&pwmValue) || fetch_int32_arg(&dir)) 
        {
            printf("Please enter valid parameters: channel, pwm value, and direction\n");
            return CmdReturnBadParameter1;
        }
    
        if(pwmValue < 0 || pwmValue > 100) 		
        {
            printf("Please enter the pwm value between 0 to 100 percentage\n");
            return CmdReturnBadParameter1;
        }  
    
        pwmPercentage = pwmValue;
      
        switch(dir)
        {
            case 1:
                forward(channel, pwmPercentage);
                break;
            case 2:
                reverse(channel, pwmPercentage);
                break;
            case 0:
                stop(channel, pwmPercentage);
                break;
            default:
                printf("Invalid direction. Please enter 0, 1, or 2 for stop, forward, or reverse respectively\n");
                return CmdReturnBadParameter1;
        }
    }
    else
    {
        printf("Please initialize pwm first\n");
    }
  
    return CmdReturnOk;
}   
// Function:     forward
// Description:  Rotate motor in forward direction
// Prameter:     channel: channel value
//               pwmPercentage: pwm value in percentage
// Return:       Nothing   
void forward(uint8_t channel, double pwmPercentage) 
{
  if(channel == 1)
  {
    htim1.Instance->CCR1 = pwmPercentage;	// Set pwm value on channel 1
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 1); // 1A
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, 0); // 2A
  }
  else if(channel == 2)
  {
    htim1.Instance->CCR2 = pwmPercentage;	// Set pwm value on channel 2
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 1); // 1A
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, 0); // 2A
  }
  else if(channel == 3)
  {
    htim1.Instance->CCR3 = pwmPercentage;	// Set pwm value on channel 3
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 1); // 1A
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, 0); // 2A
  }
}

void reverse(uint8_t channel, double pwmPercentage) 
{
  if(channel == 1)
  {
    htim1.Instance->CCR1 = pwmPercentage;	// Set pwm value on channel 1
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 0); // 1A
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, 1); // 2A
  }
  else if(channel == 2)
  {
    htim1.Instance->CCR2 = pwmPercentage;	// Set pwm value on channel 2
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 0); // 1A
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, 1); // 2A
  }
  else if(channel == 3)
  {
    htim1.Instance->CCR3 = pwmPercentage;	// Set pwm value on channel 3
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, 0); // 1A
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, 1); // 2A
  }
}

void stop(uint8_t channel, double pwmPercentage)
{
    switch (channel)
    {
        case 1:
            htim1.Instance->CCR1 = 0;  // Set pwm value on channel 1
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_6, GPIO_PIN_RESET); // 1A and 2A to LOW
            break;
        case 2:
            htim1.Instance->CCR2 = 0;  // Set pwm value on channel 2
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_6, GPIO_PIN_RESET); // 1A and 2A to LOW
            break;
        case 3:
            htim1.Instance->CCR3 = 0;  // Set pwm value on channel 3
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_6, GPIO_PIN_RESET); // 1A and 2A to LOW
            break;
        default:
            break;
    }
}
ADD_CMD("dcinit",dcinit,"                initialize Dc successfully")
ADD_CMD("direction",dir,"<channel> <pwm> rotate motor in specific direction")
