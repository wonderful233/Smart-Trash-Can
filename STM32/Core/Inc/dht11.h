#ifndef __DHT11_H
#define __DHT11_H
#include "main.h"


uint8_t DHT11_Check(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void DHT11_Rst(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint8_t DHT11_Read_Data(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, uint8_t *temp, uint8_t *humi);
#endif
