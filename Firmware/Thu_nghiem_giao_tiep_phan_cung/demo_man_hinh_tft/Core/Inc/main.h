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
#include "stm32f4xx_hal.h"

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
#define SPI1_CS_Pin GPIO_PIN_0
#define SPI1_CS_GPIO_Port GPIOB
#define TCH_IRQ_Pin GPIO_PIN_1
#define TCH_IRQ_GPIO_Port GPIOB
#define TCH_IRQ_EXTI_IRQn EXTI1_IRQn
#define DB15_Pin GPIO_PIN_15
#define DB15_GPIO_Port GPIOB
#define DB14_Pin GPIO_PIN_8
#define DB14_GPIO_Port GPIOA
#define BL_Pin GPIO_PIN_9
#define BL_GPIO_Port GPIOA
#define RST_Pin GPIO_PIN_10
#define RST_GPIO_Port GPIOA
#define DB13_Pin GPIO_PIN_11
#define DB13_GPIO_Port GPIOA
#define DB12_Pin GPIO_PIN_12
#define DB12_GPIO_Port GPIOA
#define DB11_Pin GPIO_PIN_15
#define DB11_GPIO_Port GPIOA
#define DB10_Pin GPIO_PIN_3
#define DB10_GPIO_Port GPIOB
#define DB9_Pin GPIO_PIN_4
#define DB9_GPIO_Port GPIOB
#define DB8_Pin GPIO_PIN_5
#define DB8_GPIO_Port GPIOB
#define RS_Pin GPIO_PIN_6
#define RS_GPIO_Port GPIOB
#define RD_Pin GPIO_PIN_7
#define RD_GPIO_Port GPIOB
#define WR_Pin GPIO_PIN_8
#define WR_GPIO_Port GPIOB
#define CS_Pin GPIO_PIN_9
#define CS_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
