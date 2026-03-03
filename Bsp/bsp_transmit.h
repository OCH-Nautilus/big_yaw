
#ifndef __BSP_TRANSMIT_H_
#define __BSP_TRANSMIT_H_
#include "struct_typedef.h"
#include "usart.h"
#include "stdio.h"

#define RECIVE_TASK_INIT_TIME 10
#define RECIVE_TASK_TIME_MS   1

#define DATA_COUNT_RX	100
#define DATA_COUNT_TX	57
#define DATA_COUNT	30//接收字节数

#define USART_RX_HEAD   0XA5
#define USART_RX_END    0XAA

#define USART_TX_HAED   0XA5
#define USART_TX_END    0XAA

#define USART_DATA_COUNT  44//发送字节数

// 接收端数据结构体
typedef struct
{
    uint8_t head;
    
    // 模式位域 - 总共2个字节
    union ModeUnion {
        uint16_t mode_pack;  // 用于整体操作的16位
        struct ModeBits {
            uint8_t controls_mode      : 2;  // 位0-1
            uint8_t gimbal_mode        : 2;  // 位2-3
            uint8_t vision_mode        : 2;  // 位4-5
            uint8_t shoot_mode         : 2;  // 位6-7
            uint8_t trigger_mode       : 2;  // 位8-9
            uint8_t chassis_mode       : 2;  // 位10-11
            uint8_t chassis_speed_mode : 2;  // 位12-13
           uint8_t IF_DT_FLAG     		 : 1;  // 位14
						uint8_t DT_OVER_FLAG     	 : 1;  // 位15
        } bits;
    } mode;
    
    int16_t rc_ctrl_r_vx;
    int16_t rc_ctrl_r_vy;
    int16_t rc_ctrl_l_vx;
    int16_t rc_ctrl_l_vy;
    float small_yaw_pos;
    float yaw;
    float mouse_vx;
    float mouse_vy;
    
    // 键盘按键位域 - 总共1个字节
    union KeyUnion {
        uint8_t key_pack;  // 用于整体操作的8位
        struct KeyBits {
            uint8_t Key_W : 1;  // 位0
            uint8_t Key_S : 1;  // 位1
            uint8_t Key_A : 1;  // 位2
            uint8_t Key_D : 1;  // 位3
            //uint8_t reserved_keys : 4;  // 位4-7，保留位
						uint8_t Key_Shift : 1;  //位4
						uint8_t Key_Flag_E : 1;//位5
						uint8_t Key_E : 1;//位6
        }bits;
    } key;
		
		union RC_CTRL_S_Union {
        uint8_t rc_s_pack;  // 用于整体操作的8位
        struct Rc_S_Bits {
            uint8_t s_l : 2;  // 位0-1
						uint8_t s_r : 2;	// 位2-3
					uint8_t reserved :4;// 位4-7，保留位
        }bits;
    } rc_ctrl_s;
		
    uint8_t tail;
} USART_Rx_data_t;

// 发送端数据结构体
typedef struct
{
	uint8_t head;
	float chassis_diff_angle;
	uint8_t trigger_back_over_flag;
	uint8_t trigger_weak_flag;
	float initial_speed;
	float ins_big_yaw;
	float big_yaw_target;
	uint16_t shooter_barrel_heat_limit;
	uint16_t shooter_barrel_cooling_value;
	uint16_t shooter_17mm_1_barrel_heat;
	uint16_t chassis_power_limit;
	float real_power;
	uint16_t buffer_energy;
	uint8_t tail;
}USART_TX_data_t;

typedef union
{
    int32_t data;
    uint8_t d[4];
} Algorithm_int32_u;

typedef union
{
    uint16_t data;
    uint8_t d[4];
} Algorithm_2_u;

typedef union
{
    int16_t data;
    uint8_t d[4];
} Algorithm_int16_u;

typedef union
{
    float data;
    uint8_t d[4];
} Algorithm_fp32_u;
extern USART_Rx_data_t USART_Rx_data;
extern uint8_t USART_Rx_data_handle[DATA_COUNT];
extern uint8_t USART_Tx_buff[USART_DATA_COUNT];

extern USART_TX_data_t  USART_TX_data;

void Transmit_Data_Task(void const *pvParameters);
void Head1_data_Handle (uint8_t *buff,USART_Rx_data_t *data);
void USART_Data_Send( USART_TX_data_t *data , uint8_t *buff);
void USART_Data_Handle(USART_TX_data_t *data);
void USART_Data_init(USART_TX_data_t *data);

#endif
