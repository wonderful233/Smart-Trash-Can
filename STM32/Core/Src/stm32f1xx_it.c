/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <string.h>
#include <stdlib.h>
#include "usart.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

#define UART1_RX_BUF_LEN 50
#define CMD_BIN1_OPEN  ">>1:open"   // 1������Ͱ��
#define CMD_BIN1_CLOSE ">>1:close"  // 1������Ͱ�ر�
#define CMD_BIN2_OPEN  ">>2:open"   // 2������Ͱ��
#define CMD_BIN2_CLOSE ">>2:close"  // 2������Ͱ�ر�




/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart1;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line0 interrupt.
  */
void EXTI0_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_IRQn 0 */
// 标记key1触发，记录消抖起始时间
	uint8_t send_data[] = "key1scan";
	HAL_UART_Transmit(&huart2,send_data, sizeof(send_data)-1,100);
	Key_Trigger(1);
  /* USER CODE END EXTI0_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(key1_Pin);
  /* USER CODE BEGIN EXTI0_IRQn 1 */

  /* USER CODE END EXTI0_IRQn 1 */
}

/**
  * @brief This function handles EXTI line1 interrupt.
  */
void EXTI1_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI1_IRQn 0 */
	uint8_t send_data[] = "按键2中断触发";
	HAL_UART_Transmit(&huart2,send_data, sizeof(send_data)-1,100);
	Key_Trigger(2);
  /* USER CODE END EXTI1_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(key2_Pin);
  /* USER CODE BEGIN EXTI1_IRQn 1 */

  /* USER CODE END EXTI1_IRQn 1 */
}

/**
  * @brief This function handles TIM2 global interrupt.
  */
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */

  /* USER CODE END TIM2_IRQn 1 */
}

/**
  * @brief This function handles TIM4 global interrupt.
  */
void TIM4_IRQHandler(void)
{
  /* USER CODE BEGIN TIM4_IRQn 0 */
// ========== 处理TIM4 CH1（Echo1/PB6） ==========
  if(__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_CC1) != RESET)
  {
      // 读取当前捕获极性（直接操作CCER寄存器的CC1P位）
      // CC1P=0 → 上升沿捕获；CC1P=1 → 下降沿捕获
      if((TIM4->CCER & TIM_CCER_CC1P) == 0) 
      {
          // 1. 捕获到上升沿：记录起始计数，切换为下降沿捕获
          ultra1.rise_cnt = HAL_TIM_ReadCapturedValue(&htim4, TIM_CHANNEL_1);
          TIM4->CCER |= TIM_CCER_CC1P; // 置1 → 切换为下降沿捕获
      }
      else
      {
          // 2. 捕获到下降沿：记录结束计数，切回上升沿捕获，标记完成
          ultra1.fall_cnt = HAL_TIM_ReadCapturedValue(&htim4, TIM_CHANNEL_1);
          TIM4->CCER &= ~TIM_CCER_CC1P; // 清0 → 切回上升沿捕获
          ultra1.capture_ok = 1; // 超声波1捕获完成
      }
      __HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_CC1); // 清除CH1捕获标志
  }

  // ========== 处理TIM4 CH2（Echo2/PB7） ==========
  if(__HAL_TIM_GET_FLAG(&htim4, TIM_FLAG_CC2) != RESET)
  {
      if((TIM4->CCER & TIM_CCER_CC2P) == 0)
      {
          // 捕获上升沿：记录起始计数，切换为下降沿
          ultra2.rise_cnt = HAL_TIM_ReadCapturedValue(&htim4, TIM_CHANNEL_2);
          TIM4->CCER |= TIM_CCER_CC2P;
      }
      else
      {
          // 捕获下降沿：记录结束计数，切回上升沿
          ultra2.fall_cnt = HAL_TIM_ReadCapturedValue(&htim4, TIM_CHANNEL_2);
          TIM4->CCER &= ~TIM_CCER_CC2P;
          ultra2.capture_ok = 1; // 超声波2捕获完成
      }
      __HAL_TIM_CLEAR_FLAG(&htim4, TIM_FLAG_CC2); // 清除CH2捕获标志
  }
  /* USER CODE END TIM4_IRQn 0 */
  HAL_TIM_IRQHandler(&htim4);
  /* USER CODE BEGIN TIM4_IRQn 1 */

  /* USER CODE END TIM4_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt. 接收中断
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
// 1. 检查接收中断标志（RXNE：接收数据非空）
  if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET) {
    __HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);
		uart1RxBuff[uart1RxCounter]=(uint8_t)(huart1.Instance->DR&(uint8_t)0x00ff);
		uart1RxCounter++;
		__HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_RXNE);
  }
	
	if((__HAL_UART_GET_FLAG(&huart1,UART_FLAG_IDLE)!=RESET))
	{
		__HAL_UART_DISABLE_IT(&huart1,UART_IT_IDLE);
		uart1RxState=1;
	}
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/* USER CODE BEGIN 1 */



/* USER CODE END 1 */
