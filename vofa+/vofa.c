#include "vofa.h"
#include "freertos.h" 
#include "bsp_dwt.h"
#include "task.h"
#include "usart.h"
#include "referee.h"
#include "big_gimbal_task.h"
#include "ins_task.h"
#include "chassis_task.h"
#include "bsp_transmit.h"
#include "CAN_receive.h"
#include "config.h"
#include "chassis_power.h"
Vofa_data_m_2 Vofa_data_2={.tail={0x00,0x00,0x80,0x7f}};
Vofa_data_m_4 Vofa_data_4={.tail={0x00,0x00,0x80,0x7f}};
Vofa_data_m_8 Vofa_data_8={.tail={0x00,0x00,0x80,0x7f}};


extern pid_type_def speed[4];
extern PowerState_control_t PowerState_control;

extern pid_type_def pid_big_yaw_angle_fold;
extern pid_type_def pid_big_yaw_speed_fold;

extern int a111;
extern int yuuuuu;
extern float  pm_power;

void StartVOFATask(void const * argument)
{
  for(;;)
  {
		//Vofa_Send_Data8(USART_Rx_data.mode.bits.gimbal_mode,CHASSIS.V[0],USART_Rx_data.key.bits.Key_W,CHASSIS.Vx,GIMBAL.big_yaw_target,yuuuuu,GIMBAL.big_yaw_target,CHASSIS.front_set_num);
		//Vofa_Send_Data8(pid_big_yaw_angle_fold.set,pid_big_yaw_angle_fold.ref,pid_big_yaw_angle_fold.out,pid_big_yaw_speed_fold.set,pid_big_yaw_speed_fold.ref,pid_big_yaw_speed_fold.out,GIMBAL.big_yaw_target,INS.Yaw);
		//Vofa_Send_Data8(SuperCAP.real_power,pm_power,PowerState_control,chassis_motor[RL].speed_rpm,powerlimit.referee_max_power,USART_Rx_data.mode.bits.chassis_speed_mode,0,0);
		
		vTaskDelay(10);
  }
}

void Vofa_Send_Data2(float data1, float data2)
{
    Vofa_data_2.ch_data[0] = data1;
    Vofa_data_2.ch_data[1] = data2;
    HAL_UART_Transmit_DMA(&huart1, (uint8_t *)&Vofa_data_2, sizeof(Vofa_data_2));   
//    CDC_Transmit_FS((uint8_t *)&Vofa_data_2,sizeof(Vofa_data_2)); 
}

void Vofa_Send_Data4(float data1, float data2,float data3, float data4)
{
    Vofa_data_4.ch_data[0] = data1;
    Vofa_data_4.ch_data[1] = data2;
    Vofa_data_4.ch_data[2] = data3;
    Vofa_data_4.ch_data[3] = data4;
    HAL_UART_Transmit_DMA(&huart1, (uint8_t *)&Vofa_data_4, sizeof(Vofa_data_4));   
}

void Vofa_Send_Data8(float data1, float data2,float data3, float data4,float data5, float data6,float data7, float data8)
{ 
    Vofa_data_8.ch_data[0] = data1;
    Vofa_data_8.ch_data[1] = data2;
    Vofa_data_8.ch_data[2] = data3;
    Vofa_data_8.ch_data[3] = data4;
    Vofa_data_8.ch_data[4] = data5;
    Vofa_data_8.ch_data[5] = data6;
    Vofa_data_8.ch_data[6] = data7;
    Vofa_data_8.ch_data[7] = data8;
    HAL_UART_Transmit_DMA(&huart6, (uint8_t *)&Vofa_data_8, sizeof(Vofa_data_8));   
}
