/**
  ******************************************************************************
  * @file    Templates/Src/USART.h 
  * @author  MCD Application Team
  * @brief   Librer�a USART para la incializaci�n de la USART3 y el env�o de datos
	*					 a un terminal del PC.
  *
  * @note    modified by ARM
  *          The modifications allow to use this file as User Code Template
  *          within the Device Family Pack.
  ******************************************************************************

  ******************************************************************************
  */

#include <stdio.h>
#include <string.h>
#include "Driver_USART.h"
 

int init_USART (void);
int tx_USART (char ch[], int size );
