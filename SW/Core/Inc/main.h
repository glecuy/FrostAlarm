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


#define SIM800L_Reset_Pin  GPIO_PIN_15
#define SIM800L_Reset_Port GPIOA

#define LED_RED_toggle()   HAL_GPIO_TogglePin (LED_SIM_OK_GPIO_Port, LED_RED_Pin)
#define LED_RED_on()       HAL_GPIO_WritePin  (LED_SIM_OK_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET)
#define LED_RED_off()      HAL_GPIO_WritePin  (LED_SIM_OK_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET)

#define LED_RED_Pin GPIO_PIN_5
#define LED_RED_GPIO_Port GPIOA

#define LED_RED_toggle()   HAL_GPIO_TogglePin (LED_SIM_OK_GPIO_Port, LED_RED_Pin)
#define LED_RED_on()       HAL_GPIO_WritePin  (LED_SIM_OK_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET)
#define LED_RED_off()      HAL_GPIO_WritePin  (LED_SIM_OK_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET)

#define LED_ORANGE_Pin GPIO_PIN_6
#define LED_ORANGE_GPIO_Port GPIOA

#define LED_ORANGE_toggle()   HAL_GPIO_TogglePin (LED_SIM_OK_GPIO_Port, LED_ORANGE_Pin)
#define LED_ORANGE_on()       HAL_GPIO_WritePin  (LED_SIM_OK_GPIO_Port, LED_ORANGE_Pin, GPIO_PIN_SET)
#define LED_ORANGE_off()      HAL_GPIO_WritePin  (LED_SIM_OK_GPIO_Port, LED_ORANGE_Pin, GPIO_PIN_RESET)

#define LED_SIM_OK_Pin GPIO_PIN_7
#define LED_SIM_OK_GPIO_Port GPIOA

#define LED_SIM_OK_toggle()   HAL_GPIO_TogglePin (LED_SIM_OK_GPIO_Port, LED_SIM_OK_Pin)
#define LED_SIM_OK_on()       HAL_GPIO_WritePin  (LED_SIM_OK_GPIO_Port, LED_SIM_OK_Pin, GPIO_PIN_SET)
#define LED_SIM_OK_off()      HAL_GPIO_WritePin  (LED_SIM_OK_GPIO_Port, LED_SIM_OK_Pin, GPIO_PIN_RESET)



/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
