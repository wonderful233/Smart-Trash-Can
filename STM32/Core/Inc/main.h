/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
typedef enum {
    LID_CLOSED = 0, // �ر�
    LID_OPENED = 1  // ��
} Lid_State;

extern Lid_State lid1_state; // 1号垃圾桶开合状态
extern Lid_State lid2_state; // 2号垃圾桶开合状态

extern uint8_t key1_trigger;
extern uint8_t key2_trigger;


typedef struct {
    uint32_t rise_cnt;   // 上升沿计数
    uint32_t fall_cnt;   // 下降沿计数
    float  distance;   // 最终距离（cm）
    uint8_t  capture_ok; // 捕获完成标志
} Ultrasonic_Capture_t;


extern Ultrasonic_Capture_t ultra1; // 超声波1
extern Ultrasonic_Capture_t ultra2; // 超声波2


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
void Servo_Control(uint8_t bin_num, uint8_t action);
void USER_USART1_IRQHandler(void);
void delay_us(uint32_t us);

void Ultrasonic_Trigger(uint8_t num); // 触发超声波（1/2）
void Ultrasonic_Calc_Distance(void);  // 计算距离
void Ultrasonic_NonBlocking_Read(void);// 非阻塞触发+计算

void Key_Trigger(uint8_t key_num);

extern uint8_t uart1RxBuff[];
extern uint16_t uart1RxCounter;
extern uint8_t uart1RxState;

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define key1_Pin GPIO_PIN_0
#define key1_GPIO_Port GPIOA
#define key1_EXTI_IRQn EXTI0_IRQn
#define key2_Pin GPIO_PIN_1
#define key2_GPIO_Port GPIOA
#define key2_EXTI_IRQn EXTI1_IRQn
#define TRIG1_Pin GPIO_PIN_12
#define TRIG1_GPIO_Port GPIOB
#define TRIG2_Pin GPIO_PIN_14
#define TRIG2_GPIO_Port GPIOB
#define DHT11_1_Pin GPIO_PIN_5
#define DHT11_1_GPIO_Port GPIOB
#define DHT11_2_Pin GPIO_PIN_8
#define DHT11_2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
