#include "bsp_transmit.h"
#include <stdlib.h>
#include "cmsis_os.h"
#include "CAN_receive.h"
#include "ins_task.h"
#include "chassis_task.h"
#include "referee.h"
#include "big_gimbal_task.h"
USART_Rx_data_t USART_Rx_data;
USART_TX_data_t  USART_TX_data;

uint8_t USART_Rx_data_handle[DATA_COUNT];
uint8_t USART_Tx_buff[USART_DATA_COUNT] = {0};

/**
  * @Name    Transmit_Data_Task
  * @brief   串口数据发送任务
  * @param   None
  * @Data    2024-03-14
*/
int j=0;
void Transmit_Data_Task(void const *pvParameters)
{
	vTaskDelay(RECIVE_TASK_INIT_TIME);
	
	USART_Data_init(&USART_TX_data);
	for(;;)
	{ 
		j++;
		USART_Data_Send(&USART_TX_data,USART_Tx_buff);
		
		vTaskDelay(RECIVE_TASK_TIME_MS);
	}
	
}
/**
  * @Name    USART_Data_init
  * @brief   串口数据初始化
  * @param   data_init: [输入/出] 
  * @Data    2024-03-14
*/

void USART_Data_init(USART_TX_data_t *data_init)
{
		    if (data_init == NULL)
    {
        return;
    }
		
    data_init->head = 0;
    data_init->chassis_diff_angle=0;
    data_init->trigger_back_over_flag=0;
    data_init->trigger_weak_flag=0;
    data_init->initial_speed=0;
		data_init->ins_big_yaw=0;
		data_init->big_yaw_target=0;
		data_init->shooter_17mm_1_barrel_heat=0;
		data_init->shooter_barrel_cooling_value=0;
		data_init->shooter_barrel_heat_limit=0;
		data_init->chassis_power_limit=0;
		data_init->real_power=0;
		data_init->buffer_energy=0;
		data_init->tail = 0;
}

/**
  * @Name    USART_Data_Handle
  * @brief   串口发送数据处理
  * @param   data: [输入/出] 
  * @Data    2024-03-14
*/
int aaaa;
void USART_Data_Handle(USART_TX_data_t *data)
{
		if (data == NULL)
    {
        return;
    }
	
		data->head = USART_TX_HAED;

		data->chassis_diff_angle=CHASSIS.diff_angle;
    data->initial_speed=shoot_data.initial_speed;
		data->ins_big_yaw=INS.Yaw;
		data->big_yaw_target=GIMBAL.big_yaw_target;
		data->shooter_17mm_1_barrel_heat=power_heat_data.shooter_17mm_barrel_heat;
		data->shooter_barrel_cooling_value=robot_status.shooter_barrel_cooling_value;
		data->shooter_barrel_heat_limit=robot_status.shooter_barrel_heat_limit; // 热量上限
		data->chassis_power_limit=robot_status.chassis_power_limit;
		data->real_power=SuperCAP.real_power;
		data->buffer_energy=power_heat_data.buffer_energy;
    data->tail = USART_TX_END;
}

/**
  * @Name    USART_Data_Send
  * @brief   串口数据发送
  * @param   data: [输入/出] 
**			 buff: [输入/出] 
  * @Data    2024-03-14
*/

void USART_Data_Send( USART_TX_data_t *data , uint8_t *buff)
{
	  if (data == NULL)
    {
        return;
    }
		
		USART_Data_Handle(data);
		
	  memcpy( buff,  &data->head,1 );
	  memcpy( buff+1, &data->chassis_diff_angle , 4 );
    memcpy( buff+5, &data->trigger_back_over_flag , 1 );
    memcpy( buff+6, &data->trigger_weak_flag , 1 );
    memcpy( buff+7, &data->initial_speed , 4 );
		memcpy( buff+11, &data->ins_big_yaw , 4 );
		memcpy( buff+15, &data->big_yaw_target , 4 );
	  
		memcpy( buff+19, &data->shooter_barrel_heat_limit , 4 );
		memcpy( buff+23, &data->shooter_barrel_cooling_value , 4 );
		memcpy( buff+27, &data->shooter_17mm_1_barrel_heat , 4 );
		memcpy( buff+31, &data->chassis_power_limit , 4 );
		memcpy( buff+35, &data->real_power , 4 );
		memcpy( buff+39, &data->buffer_energy , 4 );
		memcpy( buff+43, &data->tail , 1 );
		
		HAL_UART_Transmit_DMA(&huart1,buff,USART_DATA_COUNT);
}

/**
  * @Name    Head1_data_Handle
  * @brief   
  * @param   buff: [输入/出] 
  * @Data    2024-01-31
*/
void Head1_data_Handle(uint8_t *buff, USART_Rx_data_t *data)
{
    
    if(buff[0] == USART_RX_HEAD && buff[DATA_COUNT-1] == USART_RX_END)
    {
         
        data->head = buff[0];
        uint16_t mode_pack = (buff[2] << 8) | buff[1];  
        data->mode.mode_pack = mode_pack;
        data->rc_ctrl_r_vx = (int16_t)((buff[4] << 8) | buff[3]);
        data->rc_ctrl_r_vy = (int16_t)((buff[6] << 8) | buff[5]);
        data->rc_ctrl_l_vx = (int16_t)((buff[8] << 8) | buff[7]);
        data->rc_ctrl_l_vy = (int16_t)((buff[10] << 8) | buff[9]);
        Algorithm_fp32_u small_yaw_pos_u, yaw_u;
        for(int i = 0; i < 4; i++) 
			  {
            small_yaw_pos_u.d[i] = buff[11 + i];
            yaw_u.d[i] = buff[15 + i];
        }
        data->small_yaw_pos = small_yaw_pos_u.data;
        data->yaw = yaw_u.data;
        Algorithm_fp32_u mouse_vx_u, mouse_vy_u;
        for(int i = 0; i < 4; i++) 
				{
            mouse_vx_u.d[i] = buff[19 + i];
            mouse_vy_u.d[i] = buff[23 + i];
        }
        data->mouse_vx = mouse_vx_u.data;
        data->mouse_vy = mouse_vy_u.data;
        data->key.key_pack = buff[27];
				data->rc_ctrl_s.rc_s_pack=buff[28];
        data->tail = buff[29];
    }
}




