/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

#define MODBUS_TX_ENABLE_Pin  GPIO_PIN_1
#define MODBUS_TX_ENABLE_Port GPIOA

#define UserLED_Pin GPIO_PIN_13
#define UserLED_GPIO_Port GPIOC


#define UserLED_toggle() HAL_GPIO_TogglePin (UserLED_GPIO_Port, UserLED_Pin)
//#define UserLED_on() HAL_GPIO_WritePin  (UserLED_GPIO_Port, UserLED_Pin, GPIO_PIN_SET)
//#define UserLED_off() HAL_GPIO_WritePin (UserLED_GPIO_Port, UserLED_Pin, GPIO_PIN_RESET)
#define UserLED_on() HAL_GPIO_WritePin  (UserLED_GPIO_Port, UserLED_Pin, GPIO_PIN_RESET)
#define UserLED_off() HAL_GPIO_WritePin (UserLED_GPIO_Port, UserLED_Pin, GPIO_PIN_SET)


#define LED1_Pin GPIO_PIN_5
#define LED1_GPIO_Port GPIOA

#define LED1_toggle()   HAL_GPIO_TogglePin (LED3_GPIO_Port, LED1_Pin)
#define LED1_on()       HAL_GPIO_WritePin  (LED3_GPIO_Port, LED1_Pin, GPIO_PIN_SET)
#define LED1_off()      HAL_GPIO_WritePin  (LED3_GPIO_Port, LED1_Pin, GPIO_PIN_RESET)

#define LED2_Pin GPIO_PIN_6
#define LED2_GPIO_Port GPIOA

#define LED2_toggle()   HAL_GPIO_TogglePin (LED3_GPIO_Port, LED2_Pin)
#define LED2_on()       HAL_GPIO_WritePin  (LED3_GPIO_Port, LED2_Pin, GPIO_PIN_SET)
#define LED2_off()      HAL_GPIO_WritePin  (LED3_GPIO_Port, LED2_Pin, GPIO_PIN_RESET)

#define LED3_Pin GPIO_PIN_7
#define LED3_GPIO_Port GPIOA

#define LED3_toggle()   HAL_GPIO_TogglePin (LED3_GPIO_Port, LED3_Pin)
#define LED3_on()       HAL_GPIO_WritePin  (LED3_GPIO_Port, LED3_Pin, GPIO_PIN_SET)
#define LED3_off()      HAL_GPIO_WritePin  (LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET)



/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
