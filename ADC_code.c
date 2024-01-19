#include <stdio.h>
#include <stdint.h>
#include "common.h"
#include "main.h"

	void labInit1(void *data)
	{
		/* Place Initialization things here.  This function gets called once
		* at startup.
		*/

	}

	void labTask1(void *data)
	{
		/* Place your task functionality in this function.  This function
		* will be called repeatedly, as if you placed it inside the main
		* while(1){} loop.
		*/
	}

	ADD_TASK(labTask1,	/* This is the name of the function for the task */
		 labInit1,	/* This is the initialization function */
		 NULL,		/* This pointer is passed as 'data' to the functions */
		 0,		/* This is the number of milliseconds between calls */
		 "This is the help text for the task\n")
		 
			 
		 
        ParserReturnVal_t adc(int mode)   //  initialize cmdGPIO
		
	{

		ADC_HandleTypeDef hadc1;
		
		HAL_StatusTypeDef rc;
		
		GPIO_InitTypeDef GPIO_InitStruct;


	  if(mode != CMD_INTERACTIVE) return CmdReturnOk;
	  
	  uint32_t val1,rc1;
	  
	  if(mode != CMD_INTERACTIVE) return CmdReturnOk;

	  rc1 = fetch_uint32_arg(&val1);
	  
	  if(rc1) 
	  {
	  
	    printf("Please supply 1 | 0 to turn on or off the LED\n");
	    
	    return CmdReturnBadParameter1;
	    
	  }
	/* ADC pins configuration Enable the clock for the ADC GPIOs */
	
	__HAL_RCC_GPIOC_CLK_ENABLE();

	/* Configure these ADC pins in analog mode using HAL_GPIO_Init() */
	
	GPIO_InitStruct.Pin = GPIO_PIN_0| GPIO_PIN_1|GPIO_PIN_2;
	
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	__HAL_RCC_ADC1_CLK_ENABLE();
	
	hadc1.Instance = ADC1;
	
	hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
	
	hadc1.Init.Resolution = ADC_RESOLUTION_12B;
	
	hadc1.Init.ScanConvMode = DISABLE;
	
	hadc1.Init.ContinuousConvMode = DISABLE;
	
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;


	hadc1.Init.NbrOfConversion = 1;
	
	hadc1.Init.DMAContinuousRequests = DISABLE;
	
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	
	rc = HAL_ADC_Init(&hadc1);
	
	if(rc != HAL_OK) 
	
	{
	
	printf("ADC1 initialization failed with rc=%u\n",rc);
	
	}
	
	ADC_ChannelConfTypeDef sConfig;

	/* Configure the selected channel */
	
	switch(val1)
{
	case 0:
		sConfig.Channel = ADC_CHANNEL_10; // initialize channel 10
		break;
	case 1:
		sConfig.Channel = ADC_CHANNEL_11; // initialize channel 11
		break;
	case 2:
		sConfig.Channel = ADC_CHANNEL_12; // initialize channel 12
		break;
	default:
		printf("Invalid Pin\n");
		return CmdReturnOk;
}t

	sConfig.Rank = 1; /* Rank needs to be 1, otherwiseno conversion
	is done */
	
	sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
	
	rc = HAL_ADC_ConfigChannel(&hadc1, &sConfig);
	
	if ( rc != HAL_OK)
	
	{
	
	printf("ADC channel configure failed with rc=%u\n",(unsigned)rc);
	
	return 0;
	
	}


	uint32_t val;              // undifined intiger for value
	
	float voltage;          // undifined intiger for voltage

	

	/* Read the ADC converted values */
	
	while (1)
{
  /* Start the ADC peripheral */
  rc = HAL_ADC_Start(&hadc1);

  if (rc != HAL_OK)
  {
    printf("ADC start failed with rc=%u\n", (unsigned)rc);
    return 0;
  }

  /* Wait for end of conversion */
  rc = HAL_ADC_PollForConversion(&hadc1, 100);

  if (rc != HAL_OK)
  {
    printf("ADC poll for conversion failed with rc=%u\n", (unsigned)rc);
    return 0;
  }

  val = HAL_ADC_GetValue(&hadc1);
  voltage = (3.3 * val) / 4095; // voltage formula for 12 bit adc

  printf("ADC value: %d\n", val);       // print adc value
  printf("VOLTAGE value: %.2f V\n", voltage); // print voltage

  HAL_Delay(1000);

  rc = HAL_ADC_Stop(&hadc1);

  if (rc != HAL_OK)
  {
    printf("ADC stop failed with rc=%u\n", (unsigned)rc);
    return 0;
  }
}

return CmdReturnOk; // return CmdReturnOK