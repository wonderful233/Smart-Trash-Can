#include "dht11.h"
#include "main.h"

//1号温湿度传感器口 DHT11_1_GPIO_Port   DHT11_1_Pin		PB5
//2号温湿度传感器口 DHT11_2_GPIO_Port   DHT11_2_Pin		PB8



void DHT11_IO_IN(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP; 
    HAL_GPIO_Init(GPIOx, &GPIO_InitStructure);
}

void DHT11_IO_OUT(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = GPIO_Pin;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOx, &GPIO_InitStructure);
}

void DHT11_Rst(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    DHT11_IO_OUT(GPIOx, GPIO_Pin);  // 配置为输出
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET); // 拉低总线
    HAL_Delay(20);                  // 拉低至少18ms
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);   // 释放总线（拉高）
    delay_us(35);                   // 等待35us（DHT11响应窗口）
}

//等待DHT11的回应
//返回1:存在
//返回0:未检测到DHT11的存在
uint8_t DHT11_Check(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{   
    uint8_t retry = 0;
    DHT11_IO_IN(GPIOx, GPIO_Pin); // 配置为输入模式
    
    // 等待DHT11拉低总线（响应起始）
    while (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET && retry < 100)
    {
        retry++;
        delay_us(1);
    };	 
    if (retry >= 100) return 0; // 无响应
    
    retry = 0;
    // 等待DHT11拉高总线（响应结束，准备发数据）
    while (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET && retry < 100)
    {
        retry++;
        delay_us(1);
    };
    if (retry >= 100) return 0;	// 响应超时
    
    return 1; // 检测到DHT11
}
//读dht11一个位
//返回值 1,0
uint8_t DHT11_Read_Bit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    uint8_t retry = 0;
    // 等待当前位的起始低电平（50us）
    while (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET && retry < 100)
    {
        retry++;
        delay_us(1);		
    }
    retry = 0;
    // 等待低电平结束，进入数据位高电平
    while (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_RESET && retry < 100)
    {
        retry++;
        delay_us(1);
    }		
    // 延时40us后判断电平：高=1，低=0（DHT11时序：0=26-28us，1=70us）
    delay_us(40);   
    if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin) == GPIO_PIN_SET)
        return 1;
    else
        return 0;         
}

//从DHT11读取一个字节
//返回值：读到的数据
uint8_t DHT11_Read_Byte(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{        
    uint8_t i, dat = 0;
    for (i = 0; i < 8; i++) 
    {
        dat = dat << 1; // 左移，接收下一位
        dat |= DHT11_Read_Bit(GPIOx, GPIO_Pin); // 或运算合并当前位
    }				    
    return dat;
}

//从DHT11读取一次数据
//temp:温度值(范围:0~50°)     humi:湿度值(范围:20%~90%)
//返回值：1,正常;0,读取失败
//从DHT11读取一次数据
//temp:温度值(范围:0~50°)     humi:湿度值(范围:20%~90%)
//返回值：1,正常;0,读取失败
uint8_t DHT11_Read_Data(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t *temp, uint8_t *humi)
{        
    uint8_t buf[5];
    uint8_t i;
    
    DHT11_Rst(GPIOx, GPIO_Pin);          // 复位指定DHT11
    if (DHT11_Check(GPIOx, GPIO_Pin) == 1) // 检测到响应
    {
        for (i = 0; i < 5; i++)          // 读取40位数据（5字节）
        {
            buf[i] = DHT11_Read_Byte(GPIOx, GPIO_Pin);
        }
        // 校验和：前4字节之和 = 第5字节
        if ((buf[0] + buf[1] + buf[2] + buf[3]) == buf[4])
        {
            *humi = buf[0]; // 湿度整数部分（DHT11无小数）
            *temp = buf[2]; // 温度整数部分
            return 1;       // 读取成功
        }
    }
    return 0; // 读取失败（无响应/校验和错误）
}


