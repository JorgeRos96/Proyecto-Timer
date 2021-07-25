/**
  ******************************************************************************
  * @file    Templates/Src/LED.c
  * @author  MCD Application Team
  * @brief   Fichero de inicializacion de los LEDs de la tarjeta					 
	*					 del microcontrolador.
	*					 Pines de los LEDs:
	*					  LED rojo->  PB14	(User LD3)
	*					  LED azul->  PB7	  (User LD2)
	*					  LED verde-> PB0		(User LD1)
	*					
  *
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************
  * 
  ******************************************************************************
  */
	#include "LED.h"
	
/**
  * @brief LED Initialization Function
  * @param None
  * @retval None
  */
void LED_Init(void){
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_14|GPIO_PIN_7, GPIO_PIN_RESET);/*Configure GPIO pins : PB0 PB14 PB7 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_14|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}
