/**
  ******************************************************************************
  * @file    Templates/Src/Delay.c
  * @author  MCD Application Team
  * @brief   Fichero de configuración del Timer 6 para la realización de delays.
	*					 Esta librería dispone tanto de fichero de delay en us como en ms, y
	*					 se configura mediante la función Delay_init introduciendo por 
	*					 parametro la frecuencia del sistema (en MHz) y el divisor APB1.
	*					 
  *
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************
  * 
  ******************************************************************************
  */
#include "stm32f4xx_hal.h"

TIM_HandleTypeDef htim6;
	
	/**
  * @brief Inicialización y configuración del Timer 6 con una frecuencia de 1 MHz
	* 			 Se pasa por parametro la frecuencia del sistema y el divisor APB1 para
	*				 calcular el contador y configurar la frecuencia deseada.
	*				
	*					f = (2*fsist/APB1)/(Counter) = 1 MHz 
	*			
	*					Por lo que se configura el contador en funcion de la fsist y el APB1
	*					para obtener la frecuencia de 1MHz
  * @param fsist: valor de la frecuencia del sistema en MHz
	*	@param APB1: valor del divisor de la frecuencia del sistema para obtener PCLK1
  * @retval None
  */
	void Init_Delay(int fsist, int APB1){
		TIM_ClockConfigTypeDef sClockSourceConfig = {0};
		TIM_MasterConfigTypeDef sMasterConfig = {0};
		TIM_OC_InitTypeDef sConfigOC = {0};
		TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

		/*Configuración del Timer 6*/
		htim6.Instance = TIM6;
		htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
		htim6.Init.Prescaler = (2*fsist/(APB1))-1;
		//htim6.Init.AutoReloadPreload = 0xffff;
		htim6.Init.Period = 0xffff;
		htim6.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
		if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
		{
		}
		sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
		if (HAL_TIM_ConfigClockSource(&htim6, &sClockSourceConfig) != HAL_OK)
		{
		}
		sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
		sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
		if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
		{
		}
		HAL_TIM_Base_Start(&htim6);
	}

	/**
  * @brief Función de retardo del tiempo introducido por parametro en us
	*	@param us: retardo que se desea aplicar en us
  * @retval None
  */
	void Delay_us (uint16_t us){
		
		TIM6->CNT = 0;
		while (TIM6->CNT < us);
		
	}
	
	/**
  * @brief Función de retardo del tiempo introducido por parametro en ms
	*	@param ms: retardo que se desea aplicar en ms
  * @retval None
  */
	void Delay_ms (uint16_t ms){
		
		for (uint16_t i=0; i<ms; i++)
			Delay_us (1000); // delay of 1 ms
		
	}