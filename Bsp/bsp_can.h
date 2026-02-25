#ifndef __BSP_CAN_H_
#define __BSP_CAN_H_

#include "can.h"
#define P_MIN -12.5f
#define P_MAX 12.5f
#define V_MIN -30.0f
#define V_MAX 30.0f
#define KP_MIN 0.0f
#define KP_MAX 500.0f
#define KD_MIN 0.0f
#define KD_MAX 5.0f
#define T_MIN -10.0f
#define T_MAX 10.0f
extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

void can_filter_init(void);
void can1_user_init(void);
void can2_user_init(void);
void damiao_init(CAN_HandleTypeDef* hcan,uint16_t id);
void damiao_exit(CAN_HandleTypeDef* hcan,uint16_t id);
void damiao_record(CAN_HandleTypeDef* hcan,uint16_t id);

void set_motor_current( CAN_HandleTypeDef *hcan,uint32_t id_range, int16_t v1, int16_t v2, int16_t v3, int16_t v4 );
void ctrl_motor(CAN_HandleTypeDef* hcan,uint16_t id, float _pos, float _vel,float _KP, float _KD, float _torq);//_pos位置 _vel速度 _torq转距

#endif
