/*
 * @Author: 孙羽
 * @Date: 2023-06-05 23:52:39
 * @LastEditors: 孙羽
 * @LastEditTime: 2023-07-22 01:08:47
 * @Telephone: 15235320302
 * @QQ: 984464809
 * @Description: 基于 RoboMaster 裁判系统串口协议附录 V1.5（20230707）
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef REFEREE_USART_TASK_H
#define REFEREE_USART_TASK_H
#include "main.h"

#define USART_RX_BUF_LENGHT     512
#define REFEREE_FIFO_BUF_LENGTH 1024
#define PC_DATA_COUNT 24
typedef struct
{
	float accel[3];
	float angular[3];
	float roll;
	float pitch;
	float yaw;
	float T;
	
}Gyro_t;

extern Gyro_t Gyros;
extern uint8_t Gyro_receive_buff[48];    // 接收缓冲
extern uint8_t Gyro_receive[11];
/**
  * @brief          referee task
  * @param[in]      pvParameters: NULL
  * @retval         none
  */
/**
  * @brief          裁判系统任务
  * @param[in]      pvParameters: NULL
  * @retval         none
  */
extern void REFEREE_Task(void const *pvParameters);
void gyroscope_Handle(Gyro_t *gyro,uint8_t *buff);


#endif
