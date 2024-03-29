/* simple_monitor.c:
 *
 *   Template code for implementing a simple task, using the ADD_TASK()
 *   macro.  Also contains template code for a simple monitor command.
 *
 */

#include <stdio.h>
#include <stdint.h>

#include "common.h"

void ExampleInit(void *data)
{

  /* Place Initialization things here.  This function gets called once
   * at startup.
   */

}

void ExampleTask(void *data)
{

  /* Place your task functionality in this function.  This function
   * will be called repeatedly, as if you placed it inside the main
   * while(1){} loop.
   */

}

ADD_TASK(ExampleTask,  /* This is the name of the function for the task */
	 ExampleInit,  /* This is the initialization function */
	 NULL,         /* This pointer is passed as 'data' to the functions */
	 0,            /* This is the number of milliseconds between calls */
	 "This is the help text for the task")
  

ParserReturnVal_t CmdExample(int mode)
{
  if(mode != CMD_INTERACTIVE) return CmdReturnOk;

  /* Put your command implementation here */
  for(int i=0; i<2; i++){
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,1);
	  HAL_Delay(2000);                           //led will be on for 2 seconds
	  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,0);
	  HAL_Delay(2000);                           //led will be off for 2 seconds
  }
  printf("Successfully blinked\n");
  return CmdReturnOk;
}

ADD_CMD("on",CmdExample,"                blinking led")
