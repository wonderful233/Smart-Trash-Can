/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "string.h"
#include "dht11.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define SERVO_CLOSE_PULSE 500
#define SERVO_OPEN_PULSE  1500
#define open1  "open1"	//垃圾桶1开
#define open2  "open2"	//垃圾桶2开
#define close1  "close1"	//垃圾桶1关
#define close2  "close2"	//垃圾桶2关

#define KEY_DEBOUNCE_MS    20              // 按键消抖时间（ms

uint8_t esp_send_flag = 0; // 标记是否需要发送数据到ESP8266
uint32_t timer_count = 0;

uint8_t uart1RxBuff[128] = {0};
uint16_t uart1RxCounter = 0;
uint8_t uart1RxState = 0;


uint8_t temp1, humi1;	// 第一个DHT11的温湿度
uint8_t temp2, humi2;	// 第二个DHT11的温湿度

Lid_State lid1_state = LID_CLOSED;	//1号垃圾桶开合状态
Lid_State lid2_state = LID_CLOSED;	//2号垃圾听开合状态

Ultrasonic_Capture_t ultra1 = {0, 0, 0.0, 0};
Ultrasonic_Capture_t ultra2 = {0, 0, 0.0, 0};

static uint8_t key1_trigger = 0;// key1中断触发标记
static uint8_t key2_trigger = 0;// key2中断触发标记
static uint32_t key1_tick = 0;  // key1消抖计时
static uint32_t key2_tick = 0;  // key2消抖计时
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern UART_HandleTypeDef huart2;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Servo_Control(uint8_t bin_num, uint8_t action);
void Key_Process(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	 __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
	 HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	 HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
	 HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_1); // 开启CH1捕获中断
	 HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_2); // 开启CH2捕获中断
	 HAL_TIM_Base_Start_IT(&htim2);
	 
	 __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, SERVO_CLOSE_PULSE);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
			
    /* USER CODE BEGIN 3 */
		
		Key_Process();	//按键检测
		
		
		//向esp32发送数据
		if (esp_send_flag) {
		char state_str[10];
		
		Ultrasonic_NonBlocking_Read();
			
//		uint8_t send_data[] = lid1_state;
//		HAL_UART_Transmit(&huart2,send_data2, sizeof(send_data2)-1,100);
		//1号垃圾桶发送数据
		if(lid1_state == LID_CLOSED)
		{
			strcpy(state_str, "close");
		}else
		{
			strcpy(state_str, "open");
		}
		if(DHT11_Read_Data(DHT11_1_GPIO_Port,DHT11_1_Pin,&temp1,&humi1)==1 )	
		{
			STM32_To_ESP8266(1, temp1, humi1, ultra1.distance,state_str); // 发送数据
		}
		
		//2号垃圾桶发送数据
		if(lid2_state == LID_CLOSED)
		{
			strcpy(state_str, "close");
		}else
		{
			strcpy(state_str, "open");
		}
		if(DHT11_Read_Data(DHT11_2_GPIO_Port,DHT11_2_Pin,&temp2,&humi2)==1)	
		{
			STM32_To_ESP8266(2, temp2, humi2, ultra2.distance,state_str); // 发送数据
		}

		esp_send_flag = 0; // 清除标志
	  }
		
		//---接收指令并解析执行-------
		if(uart1RxState==1)
		{
			if(strstr((const char *)uart1RxBuff,open1)!=NULL) 
			{
				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, SERVO_OPEN_PULSE);
				//Servo_Control(1,1);
				uint8_t send_data[] = "garbage1 open";
				HAL_UART_Transmit(&huart1,send_data, sizeof(send_data)-1,100);
				lid1_state = LID_OPENED;
				//HAL_Delay(1000);
			}
			else if(strstr((const char *)uart1RxBuff,close1)!=NULL)
			{
				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, SERVO_CLOSE_PULSE);
				//Servo_Control(1,0);
				uint8_t send_data[] = "garbage1 close";
				HAL_UART_Transmit(&huart1,send_data, sizeof(send_data)-1,100);
				lid1_state = LID_CLOSED;
				//HAL_Delay(1000);
			}
			else if(strstr((const char *)uart1RxBuff,open2)!=NULL)
			{
				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, SERVO_OPEN_PULSE);
				//Servo_Control(2,1);
				uint8_t send_data[] = "garbage2 open";
				HAL_UART_Transmit(&huart1,send_data, sizeof(send_data)-1,100);
				lid2_state = LID_OPENED;
				//HAL_Delay(1000);
			}
			else if(strstr((const char *)uart1RxBuff,close2)!=NULL)
			{
				__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, SERVO_CLOSE_PULSE);
				//Servo_Control(2,0);
				uint8_t send_data[] = "garbage2 close";
				HAL_UART_Transmit(&huart1,send_data, sizeof(send_data)-1,100);
				lid2_state = LID_CLOSED;
				//HAL_Delay(1000);
			}
			
			uart1RxState = 0;
			uart1RxCounter = 0;
			memset(uart1RxBuff, 0, sizeof(uart1RxBuff));
		}	
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//控制舵机函数，bin_num垃圾桶编号, action 1 打开， 0关闭
void Servo_Control(uint8_t bin_num, uint8_t action)
{
	
	uint16_t pwm_value = 0;
	if (action == 1) {
        pwm_value = SERVO_OPEN_PULSE;   // 打开：90°
    } else {
        pwm_value = SERVO_CLOSE_PULSE;  // 关闭：0°
    }
		
	switch (bin_num) {
        case 1:
            // 控制1号舵机（PA8/TIM1_CH1）
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, pwm_value);
            break;
            
        case 2:
            // 控制2号舵机（PA11/TIM1_CH4）
            __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, pwm_value);
            break;
            
        default:
            // 无效编号，直接返回
            return;
    }
	HAL_Delay(300);
}


//void USER_USART1_IRQHandler(void) {
//  
//  // 1. 检查接收中断标志（RXNE：接收数据非空）
//  if (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET) {
//    __HAL_UART_ENABLE_IT(&huart1,UART_IT_IDLE);
//		uart1RxBuff[uart1RxCounter]=(uint8_t)(huart1.Instance->DR&(uint8_t)0x00ff);
//		uart1RxCounter++;
//		__HAL_UART_CLEAR_FLAG(&huart1,UART_FLAG_RXNE);
//  }
//	
//	if((__HAL_UART_GET_FLAG(&huart1,UART_FLAG_IDLE)!=RESET))
//	{
//		__HAL_UART_DISABLE_IT(&huart1,UART_IT_IDLE);
//		uart1RxState=1;
//	}
//}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim == &htim2) {
		
    timer_count++;
    // 每10000ms置位发送标志（TIM2定时周期为100ms）
    if (timer_count >= 100) {
      esp_send_flag = 1;
      timer_count = 0; // 重置计数
    }
  }
}

void delay_us(uint32_t us)
{
    uint32_t ticks = 0;
    uint32_t start = 0;
    uint32_t now = 0;
    uint32_t reload = SysTick->LOAD; // SysTick重载值（72MHz下=71999，对应1ms）

    // 计算需要的SysTick滴答数（72MHz → 1us=72个滴答）
    ticks = us * (SystemCoreClock / 1000000); 
    start = SysTick->VAL; // 记录初始SysTick值（递减计数）

    while(1)
    {
        now = SysTick->VAL;
        // 处理SysTick溢出（VAL从0→reload）
        if(now > start)
        {
            if((reload - now + start) >= ticks) break;
        }
        else
        {
            // 核心修正：SysTick递减，差值=start - now（原逻辑写反导致延时不准）
            if((start - now) >= ticks) break;
        }
        __NOP(); // 空操作，允许CPU响应串口中断
    }
}


void Ultrasonic_Trigger(uint8_t num)
{
    if(num == 1)
    {
        // 触发超声波1：10μs高电平
        HAL_GPIO_WritePin(TRIG1_GPIO_Port, TRIG1_Pin, GPIO_PIN_RESET);
        delay_us(2);
        HAL_GPIO_WritePin(TRIG1_GPIO_Port, TRIG1_Pin, GPIO_PIN_SET);
        delay_us(10);
        HAL_GPIO_WritePin(TRIG1_GPIO_Port, TRIG1_Pin, GPIO_PIN_RESET);
    }
    else if(num == 2)
    {
        // 触发超声波2：10μs高电平
        HAL_GPIO_WritePin(TRIG2_GPIO_Port, TRIG2_Pin, GPIO_PIN_RESET);
        delay_us(2);
        HAL_GPIO_WritePin(TRIG2_GPIO_Port, TRIG2_Pin, GPIO_PIN_SET);
        delay_us(10);
        HAL_GPIO_WritePin(TRIG2_GPIO_Port, TRIG2_Pin, GPIO_PIN_RESET);
    }
}

/**
 * @brief  计算超声波距离（基于捕获的计数差）
 */
void Ultrasonic_Calc_Distance(void)
{
    uint32_t time_us = 0;
    float temp_dist = 0.0;

    // ========== 计算超声波1距离 ==========
    if(ultra1.capture_ok)
    {
        // 计算脉冲宽度（μs），处理计数器溢出
        if(ultra1.fall_cnt > ultra1.rise_cnt)
        {
            time_us = ultra1.fall_cnt - ultra1.rise_cnt;
        }
        else
        {
            time_us = (65535 - ultra1.rise_cnt) + ultra1.fall_cnt;
        }

        // 精准公式：距离(cm) = 时间(μs) × 0.017
        temp_dist = time_us * 0.017; 
        // 保留一位小数（四舍五入）
        ultra1.distance = (float)((int)(temp_dist * 10 + 0.5)) / 10; 

        // 过滤无效值（2.0~400.0cm）
        if(ultra1.distance < 2.0 || ultra1.distance > 400.0) 
        {
            ultra1.distance = 0.0;
        }
        ultra1.capture_ok = 0; // 重置标志
    }

    // ========== 计算超声波2距离 ==========
    if(ultra2.capture_ok)
    {
        if(ultra2.fall_cnt > ultra2.rise_cnt)
        {
            time_us = ultra2.fall_cnt - ultra2.rise_cnt;
        }
        else
        {
            time_us = (65535 - ultra2.rise_cnt) + ultra2.fall_cnt;
        }

        temp_dist = time_us * 0.017;
        ultra2.distance = (float)((int)(temp_dist * 10 + 0.5)) / 10; 

        if(ultra2.distance < 2.0 || ultra2.distance > 400.0) 
        {
            ultra2.distance = 0.0;
        }
        ultra2.capture_ok = 0;
    }
}


/**
 * @brief  非阻塞读取两个超声波（每200ms触发一次）
 */
void Ultrasonic_NonBlocking_Read(void)
{
    static uint32_t ultra_tick = 0;
    if(HAL_GetTick() - ultra_tick >= 200)
    {
        ultra_tick = HAL_GetTick();
        Ultrasonic_Trigger(1); // 触发超声波1
        HAL_Delay(10);         // 间隔避免声波干扰
        Ultrasonic_Trigger(2); // 触发超声波2
        Ultrasonic_Calc_Distance(); // 计算距离
    }
}


// ==================== 函数3：按键中断触发标记（中断中调用） ====================
/**
 * @brief  按键触发标记函数（EXTI中断服务函数中调用）
 * @param  key_num: 按键编号（1=key1/PA0，2=key2/PA1）
 * @retval 无
 */
void Key_Trigger(uint8_t key_num)
{
    if (key_num == 1)
    {
        key1_trigger = 1;
        key1_tick = HAL_GetTick(); // 记录触发时间（用于消抖）
    }
    else if (key_num == 2)
    {
        key2_trigger = 1;
        key2_tick = HAL_GetTick();
    }
}


// ==================== 函数4：按键消抖+舵机控制（主循环调用） ====================
/**
 * @brief  按键消抖和舵机控制处理函数
 * @note   需在main函数的while(1)中循环调用
 * @param  无
 * @retval 无
 */
void Key_Process(void)
{
    // 处理key1（PA0）-控制垃圾桶1
    if (key1_trigger == 1)
    {
        // 非阻塞消抖：间隔超过消抖时间
        if (HAL_GetTick() - key1_tick >= KEY_DEBOUNCE_MS)
        {
            // 二次校验：按键确实按下（引脚为低电平）
            if (HAL_GPIO_ReadPin(key1_GPIO_Port, key1_Pin) == GPIO_PIN_RESET)
            {
                if(lid1_state == LID_CLOSED)
								{
									__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, SERVO_OPEN_PULSE);
									lid1_state = LID_OPENED;
								}else
								{
									__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, SERVO_CLOSE_PULSE);
									lid1_state = LID_CLOSED;
								}
                key1_trigger = 0; // 清除触发标记
                // 调试打印（可选，需开启串口）
                
            }
        }
    }

    // 处理key2（PA1）-控制垃圾桶2
    if (key2_trigger == 1)
    {
        if (HAL_GetTick() - key2_tick >= KEY_DEBOUNCE_MS)
        {
            if (HAL_GPIO_ReadPin(key2_GPIO_Port, key2_Pin) == GPIO_PIN_RESET)
            {
								if(lid2_state == LID_CLOSED)
								{
									__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, SERVO_OPEN_PULSE);
									lid2_state = LID_OPENED;
								}else
								{
									__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4, SERVO_CLOSE_PULSE);
									lid2_state = LID_CLOSED;
								}
                key2_trigger = 0;
                
            }
        }
    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
