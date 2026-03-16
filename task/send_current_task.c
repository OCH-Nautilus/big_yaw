#include "send_current_task.h"
#include "config.h"
#include "bsp_can.h"
#include "CAN_receive.h"
#include "cmsis_os.h"
#include "chassis_task.h"
#include "chassis_power.h"
#include "big_gimbal_task.h"
#include "SuperCAP.h"
#include "bsp_transmit.h"
int time = 0;

void send_current_task(void const * argument)
{
  /* USER CODE BEGIN current_task */
  vTaskDelay(30);
  /* Infinite loop */
  for(;;)
  {
		enable_disable_DM4310();
		
		if(communication_state==COMMUNICATION_NORMAL)
		{
			#ifdef GIMBAL_YAW_SENT
				yaw_ctrl_current();
			#else
					Error_Yaw();
			#endif
		}
    else
			Error_Yaw();
		vTaskDelay(1);
		if(communication_state==COMMUNICATION_NORMAL)
		{
			#ifdef CHASSIS_SENT
				chassis_ctrl_current();
			#else
					Error_Chassis();
			#endif
		}
		else
			Error_Chassis();
			
		
	vTaskDelay(1);

  }
  /* USER CODE END current_task */
}


/**
 * @brief 大yaw电流发送
 * @note
 * @param
 */
void yaw_ctrl_current()
{
	 if(USART_Rx_data.mode.bits.gimbal_mode!=GIMBAL_IDLE)
	 {
	 	ctrl_motor(&hcan1, 0x02, 0, 0, 0, 0, GIMBAL.big_yaw_output);
	 }
	 else
	 {
	 	ctrl_motor(&hcan1, 0x02, 0, 0, 0, 0, 0);
	 }
}

/**
 * @brief 大yaw错误电流发送
 * @note
 * @param
 */
void Error_Yaw()
{
	ctrl_motor(&hcan1, 0x02, 0, 0, 0, 0, 0);
}

/**
 * @brief 底盘电流发送
 * @note
 * @param
 */
void chassis_ctrl_current()
{
	chassis_power_control_xj();

	if(USART_Rx_data.mode.bits.chassis_mode==CHASSIS_IDLE)
		set_motor_current(&hcan1,0x200,0,0,0,0); 	
	else
	 set_motor_current(&hcan1,0x200,CHASSIS.output[RR], CHASSIS.output[RL], CHASSIS.output[FL], CHASSIS.output[FR]);
		
}






/**
 * @brief 底盘错误电流发送
 * @note
 * @param
 */
void Error_Chassis()
{
	set_motor_current(&hcan1, 0x200, 0, 0, 0, 0);
}

void enable_disable_DM4310(void)
{
	static int16_t dm_cnt=30;
 if (USART_Rx_data.mode.bits.gimbal_mode != GIMBAL_IDLE)
 {
     
     if (dm_cnt > 0)
     {
         damiao_init(&hcan1, 0x02);
         dm_cnt--;
				 vTaskDelay(1);
     }
     
 }
 else
 {
     damiao_exit(&hcan1, 0x02);
		dm_cnt=30;
		vTaskDelay(1);
 }
}
