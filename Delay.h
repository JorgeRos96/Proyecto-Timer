
/**
  ******************************************************************************
  * @file    Templates/Inc/main.h 
  * @author  MCD Application Team
  * @brief   Header for Delay.c module
  ******************************************************************************
  ******************************************************************************
*/

#include <stdint.h>

void Init_Delay (int fsist, int APB1);

void Delay_us (uint16_t us);

void Delay_ms (uint16_t ms);