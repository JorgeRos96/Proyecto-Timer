/**
  ******************************************************************************
  * @file    Templates/Src/USART.c 
  * @author  MCD Application Team
  * @brief   Fichero de inicialización y configuración de la USART3 a traves de
	*					 las funciones del CMSIS Driver para la USART.
	*					 Se configuran los pines en el RTE_Device.h de la siguiente manera:
	*						 	-Pin de tx: PD8
	*							-Pin de rx: PD9
	*
	*					 La USART se ha configurado de la siguiente manera:
	*							- Baudrate = 9600 baud
	*							- Word length = 8 bits
	*							- Un bit de stop
	*							- Sin bit de paridad
	*							- Sin control de flujo
  *
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************

  ******************************************************************************
  */


#include "USART.h"
#include "stm32f4xx.h" 


extern ARM_DRIVER_USART Driver_USART3;
static ARM_DRIVER_USART * USARTdrv = &Driver_USART3;

/**
  * @brief Función de inicialización de la USART3 y habilitación de la transmisión
	*							- Baudrate = 9600 baud
	*							- Word length = 8 bits
	*							- Un bit de stop
	*							- Sin bit de paridad
	*							- Sin control de flujo
  * @param None
  * @retval None
  */
int init_USART (void){
		
		int status = 0;
	
		/*Inicialización de la USART a traves de la función Initialize del CMSIS Driver de la USART*/
		status =  USARTdrv->Initialize(NULL);
		if (status != 0) return status;
	  /*Encendido del USART a traves de la función PowerControl del CMSIS Driver de la USART */
		status =  USARTdrv->PowerControl(ARM_POWER_FULL);
		if (status != 0) return status;
    /*Configuración de la USART a 9600 baudios con la funcion Control del CMSIS Driver de la USART */
		status =   USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS |
                      ARM_USART_DATA_BITS_8 |
                      ARM_USART_PARITY_NONE |
                      ARM_USART_STOP_BITS_1 |
                      ARM_USART_FLOW_CONTROL_NONE, 9600);
		if (status != 0) return status;

     
		/* Habilitación de la línea de transmisión a traves de la unción Control del CMSIS Driver de la USART */
		status =   USARTdrv->Control (ARM_USART_CONTROL_TX, 1);
		if (status != 0) return status;
	
		return status;
}

/**
  * @brief Función que realiza el envío, de los datos recibidos por parametro, a traves de la USART3
	*				 que devuelve el estado en el que ha finalizado la transimisión
	* @param ch: Array con los datos que se deasean transmitir a traves de la USART
	* @param size: Tamaño del array de datos a transmitir
	* @retval status: variable que recoge si se ha realizado correctamente la transmisión de datos 
  */
int tx_USART (char ch[], int size ){
	int status = 0;
	ARM_USART_STATUS st;
 	for (int i = 0; i<size; i++){
		/*Envío de datos por la USART*/
		status = USARTdrv->Send((uint8_t *)&ch[i], 1);
		if(status != 0) 
			return status;
		
		st = USARTdrv->GetStatus();
		/* Espera necesaria a que se termine el envío de datos, que se visualiza en el bit TC del registro SR de la USART*/
		while (st.tx_busy)		st = USARTdrv->GetStatus();
	}
	
	return status;
}
