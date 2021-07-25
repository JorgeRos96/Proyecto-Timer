/**
  ******************************************************************************
  * @file    Templates/Src/main.c 
  * @author  MCD Application Team
  * @brief   Proyecto para el manejo de los Timers del microcontrolador
	* 				 STM32F429ZI. 
	*					 Se van a probar varias fucionalidades:
	*					 -Apagar/encender un led con un intervalo ajustable
	*					 -Capturar señales a traves de un canal del Timer
	*					 -Generar señales a traves del Timer
	* 				
	*					 Para ello se va a realizar el encendido y apagado de un LED con 
	*					 la frecuencia generada por el Timer 2, a traves de una interrupción. 
	*					 Tambien se va a generar una señal PWM a traves de un canal 1 del 
	*					 Timer 3 con un ciclo de tabajo del 50%. Y por último se va a capturar
	*				   la señal PWM y se va a calcular su frecuencia a traves del canal 1 
	*					 del Timer 4.
	*					 
	*					 Pin de generacion de la señal PWM-> PC6
	*					 Pin de captura de la señal-> PD12
	*						
	*					 Se configura el reloj del sistema para que trabaje a una frecuencia 
	*					 de 180 MHz utilizando como fuente de reloj el PLL con el HSI. Con 
	*					 esta frecuencia del sistema se configuran las siguientes frecuencias:
	*
	*					 Frecuencia del Timer 2 = 2.4 Hz
	*					 Frecuencia de la señal PWM generada = 45 kHz
	*					 Frecuencia del Timer de captura = 1.37 kHz
	*						
	*					 La frecuencia minima que debe tener la señal introducida para que 
	*					 pueda ser capturada por el Timer 3 es la frecuencia del Timer 
	*					 (1.37 kHz) 
  *
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************
  * 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "LED.h"
#include "USART.h"
#include "Delay.h"

#ifdef _RTE_
#include "RTE_Components.h"             // Component selection
#endif




/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

uint8_t primera = 0;
uint32_t	valor1 = 0;
uint32_t	valor2 = 0;
uint32_t	diferencia = 0;
uint32_t	frecuencia = 0;
char buf[100];
int size = 0;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void Error_Handler(int fallo);

/* Private functions ---------------------------------------------------------*/
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_GPIO_Init(void);
static void MX_TIM4_Init(void);
/**

  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user 
             can eventually implement his proper time base source (a general purpose 
             timer for example or other time source), keeping in mind that Time base 
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
             handled in milliseconds basis.
       - Low Level Initialization
     */
	if (HAL_Init() != HAL_OK)
		Error_Handler(0);

  /* Inicialización y configuración del reloj a 168 MHz */
  SystemClock_Config();
  SystemCoreClockUpdate();
	
	/* Inicialización de la USART a traves de la función init_USART de la libreria USART
	*	 y habilitación de la transmisión
	*							- Baudrate = 9600 baud
	*							- Word length = 8 bits
	*							- Un bit de stop
	*							- Sin bit de paridad
	*							- Sin control de flujo
	*/
	if (init_USART() != 0)
		Error_Handler(2);

  /* Inicialización de los LEDs de la tarjeta*/
	LED_Init();
	/*Inicialización de los Timers*/
	MX_TIM2_Init();
	MX_TIM3_Init();
	MX_TIM4_Init();
	/*Inicializaciñon del Delay*/
	Init_Delay(180,4);
	/*Habilitación del Timer de captura en el Timer 4 canal 1*/
	HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1);
	/*Habilitación del Timer para la generación de la señal PWM en el Timer 4 canal 1*/
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	/*Habilitación del  Timer 2 con la generación de señal de interrupción*/
	HAL_TIM_Base_Start_IT(&htim2);
	
	Delay_ms(1);
	/* Texto que se desea enviar*/
	size = sprintf(buf,"\rLa frecuencia de la senal capturada es: %d Hz\n", frecuencia);
	/* Envío del array al terminal a traves de la función tx_USART de la librería USART*/
	if (tx_USART(buf, size) != 0)
		Error_Handler(3);
		
	
  /* Infinite loop */
  while (1)
  {	
		
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSI)
  *            SYSCLK(Hz)                     = 180000000
  *            HCLK(Hz)                       = 180000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSI Frequency(Hz)              = 16000000
  *            PLL_M                          = 8
  *            PLL_N                          = 180
  *            PLL_P                          = 2
  *            PLL_Q                          = 4
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	
  /** Se configura el HSI como fuente de reloj del PLL y se configuran
	* 	los parametros del PLL para ajusta la frecuencia a 180 MHz con una
	* 	frecuencia del HSI de 16 MHZ (por defecto).
	* 	SYSCLK =[(16MHz(frecuencia HSI)/8(PLLM))*180 (PLLN)]/2 (PLLP) = 180 MHz
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler(1);
  }
  /** Se activa el modo de Over Drive para poder alcanzar los 180 MHz
	* 	como frecuencia del sistema
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler(1);
  }
  /** Se selecciona el PLL como fuente de reloj del sistema y se configuran los parametros
	*		para configurar el HCLK, PCLK1 y PCLK2. La frecuencia máxima del HCLK es 180 MHZ, la 
	*		frecuencia máxima del PCLK1 es de 45 MHZ y la frecuencia máxima del PCLK2 es de 90 MHz
	*		HCLK = SYSCK/AHB = 180 MHz / 1 = 180 MHz
	*		PCLK1 = HCLK/APB1 = 180 MHz / 4 = 45 MHZ
	*		PCLK2 = HCLK/APB2 = 180 MHz / 2 = 90 MHZ
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler(1);
  }
}


/**
  * @brief Inicialización y configuración del Timer 2 con una frecuencia de 2.4 Hz
	* 			 Como la frecuencia del sistema es de 180 MHz y el Timer 3 tiene como
	*				 fuente de reloj el APB1 de 90 MHz, se establece un counter de 65536 y 
	*				 un prescaler de 572 para tener la frecuencia de 2.4 HZ.
	*				
	*					f = 90*10^6/(65536*572) = 2.4 Hz 
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void){
	
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

	/*Configuración del Timer 2*/
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0x23B;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0xffff;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler(4);
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler(4);
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler(4);
  }
}
/**
  * @brief Inicialización y configuración del Timer 3 con una frecuencia de 45 kHz
	* 			 Como la frecuencia del sistema es de 180 MHz y el Timer 3 tiene como
	*				 fuente de reloj el APB1 de 90 MHz, se establece un counter de 2000 para
	*				 tener la frecuencia de 45 kHz.
	*				
	*					f = 90*10^6/(2000) = 45 kHz 
	*					
	*				 También se inicializa el canal 1 del Timer 3 (pin PC6) para la generación
	* 			 de la señal PWM con un ciclo de trabajo del 50%. 
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
	TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
 
	/*Inicialización del Timer 3*/
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 1999;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

		if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler(5);
  }

	sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 2000/2;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler(5);
  }


}
/**
  * @brief Inicialización y configuración del Timer 4 con una frecuencia de 1.37 kHz
	* 			 Como la frecuencia del sistema es de 180 MHz y el Timer 3 tiene como
	*				 fuente de reloj el APB1 de 90 MHz, se establece un counter de 65536 para 
	*				 tener la frecuencia de 1.37 kHZ.
	*				
	*					f = 90*10^6/(65536) = 1.37 kHz 
	*					
	*				 Ademas se habilita el canal 1 como captura del flanco de subida en el 
	*				 pin PD12. 
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};
	
	/*Inicialización del Timer 4*/
	htim4.Instance = TIM4;
  htim4.Init.Period            = 0xffff;
  htim4.Init.Prescaler         = 0;
  htim4.Init.ClockDivision     = 0;
  htim4.Init.CounterMode       = TIM_COUNTERMODE_UP;
  htim4.Init.RepetitionCounter = 0;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if(HAL_TIM_IC_Init(&htim4) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler(6);
  }

  /* Configure the Input Capture of channel 2 */
  sConfigIC.ICPolarity  = TIM_ICPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter    = 0;   
  if(HAL_TIM_IC_ConfigChannel(&htim4, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    /* Configuration Error */
    Error_Handler(6);
  }
  
}
/**
  * @brief Función de callback del Timer para realizar el encendido del LED 2 (azul) en el pin PB7
	* @param htim: Timer que ha realizado la cuenta e invoca a la función de callback  
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{ 
	if(htim == &htim2)
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
}
/**
  * @brief Función de callback del Timer que captura la señal  a traves de la que se
	*				 calcula la frecuencia de la señal (cuadrada) capturada. Se captura un flanco
	*				 de subida y se anota el tiempo hasta el siguiente flanco de subida. Si el 
	*				 segundo tiempo que se captura es menor que el primero, el tiempo pasado es el
	*				 periodo total del contador menos el primer valor, más el segundo valor de tiempo.
	*	
	*				 La frecuencia se calcula como:
	*		
	*				 f = 2*PCLK1/diferencia
	*
	* @param htim: Timer que realiza la captura de la señal  
  * @retval None
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
 
	if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1){
  if(primera == 0) {
   valor1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
   primera = 1;
  }

  else if (primera == 1){
	 valor2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
	 primera = 0;
	 if(valor2 > valor1){
		diferencia = valor2 - valor1;
	 }
	 else if (valor2 < valor1){
		diferencia = ((0xffff - valor1)+valor2)+1;
	 }
	 frecuencia= 2*HAL_RCC_GetPCLK1Freq()/diferencia;
	}
}
}
/**
  * @brief  This function is executed in case of error occurrence.
	* @param  fallo: variable que indica donde se ha producido el error
  * @retval None
  */
static void Error_Handler(int fallo)
{
  if(fallo == 0)
		/* Mensaje si se ha producido un error en la inicializacón de la librería HAL*/
		printf(buf,"\r Se ha producido un error al inicializar la librería HAL\n");
	else if (fallo == 1)
		/* Mensaje si se ha producido un error en la inicializacón del reloj del sistema*/
		printf(buf,"\r Se ha producido un error al inicializar el reloj del sistema\n");
	else if(fallo == 2)
		/* Mensaje si se ha producido un error en la inicializacón de la USART*/
		printf(buf,"\r Se ha producido un error al inicializar la USART\n");
	else if (fallo == 3)
		/* Mensaje si se ha producido un error en el envío de datos de la USART*/
		printf(buf,"\r Se ha producido un error al enviar datos por la USART\n");
	else if (fallo == 4)
		/* Mensaje si se ha producido un error en la inicialización del Timer 2*/
		printf(buf,"\r Se ha producido un error al inicializar el Timer 2\n");
	else if (fallo == 5)
		/* Mensaje si se ha producido un error en la inicialización del Timer 3*/
		printf(buf,"\r Se ha producido un error al inicializar el Timer 3\n");
	else if (fallo == 6)
		/* Mensaje si se ha producido un error en la inicialización del Timer 3*/
		printf(buf,"\r Se ha producido un error al inicializar el Timer 4\n");

  while(1)
  {
  }
}




/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
