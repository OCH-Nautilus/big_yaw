#ifndef __CAN_RECEIVE_H_
#define __CAN_RECEIVE_H_

#include "struct_typedef.h"
#include "can.h"
//#define abs(x) ((x) < 0 ? -(x) : (x))


typedef struct
{
  // 以下是电机电调直接回传的数据
  int16_t ecd;                    //电机的编码器数值
  int16_t last_ecd;               //上一次电机的编码器数值
  int16_t  speed_rpm;              //电机的转速值
	int16_t  given_current;				 	//电机实际电流
	uint8_t temperate;							 //电机温度

  // 以下是计算出来的电机相关数据 
  int32_t  round_cnt;              //电机旋转的总圈数
  int32_t  total_ecd;              //电机旋转的总编码器数值
  int32_t  total_angle;            //电机旋转的总角度
  int32_t  set_current;						 //电机设定的电流值
	uint32_t msg_cnt;								//每次接收到一次信息就加一
  // 以下电机计算相关数据时的中间变量，可以忽略 
  uint16_t offset_ecd;
} moto_measure_t;

typedef struct
{
	// 以下是电机电调直接回传的数据
	uint16_t ID;//ID 表示控制器的 ID，取 CAN_ID 的低 8 位
	uint16_t ERR;//ERR 表示故障，对应故障类型为：
	//0--下电
	//1--上电
//8——超压；
//9——欠压；
//A——过电流；
//B——MOS 过温；
//C——电机线圈过温；
//D——通讯丢失；
//E——过载；
	int POS;//POS 表示电机的位置信息
	int VEL;//VEL 表示电机的速度信息
	int T;//T 表示电机的扭矩信息
	uint8_t T_MOS;//T_MOS 表示驱动上 MOS 的平均温度，单位℃
	uint8_t T_ROTOR;//T_Rotor 表示电机内部线圈的平均温度，单位℃
	
	//以下是计算出来的电机相关数据 
	float _pos;//_pos位置
	float _vel;// _vel速度 
	float _torq;//_torq转距
	float Angle;
}damiao_typedef;

//typedef struct
//{
//	float C_Vol;	//电容电压
//	float C_Power;	//电容输出功率
//	float Mode_C;	//电容状态码(198为正常状态，17为c板发送错误）
//	float Real_Power;	//实际功率
//	float PowerData[4];
//}SuperCAp_t;

typedef enum 
{
  normol_cap=0,//????????
  just_out,//?????
  wireless_in,//??????
}cap_state_t;

typedef struct
{
  uint8_t cap_percent;//电容容量
  uint8_t If_wireless_wrong;//无线充电是否失败 ，失败需要上位机重置失败标志位
  uint8_t wireless_state;//当前无线充电状况
  uint8_t cap_wrong_code;//电容错误码
  cap_state_t cap_state;//当前电容情况
  float real_power;//当前真实功率（反馈到裁判端的）
  float cap_v;//超电电压
    float cap_c;//电容电流
    float dcdc_c;
}SuperCAp_t;

extern SuperCAp_t SuperCAP;

float uint_to_float(int x_int, float x_min, float x_max, int bits);
int float_to_uint(float x, float x_min, float x_max, int bits);
void get_moto_offset(moto_measure_t *ptr, uint8_t data[]);
void encoder_data_handle(moto_measure_t *ptr, uint8_t data[]);
void damiao_4310_data_handle(damiao_typedef *ptr,uint8_t data[]);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan);
extern moto_measure_t chassis_motor[4];
extern moto_measure_t trigger_motor; // 拨盘
extern damiao_typedef big_yaw;


#endif
