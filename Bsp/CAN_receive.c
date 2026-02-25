#include "CAN_receive.h"
#include "struct_typedef.h"
#include "config.h"
#include "SuperCAP.h"
damiao_typedef big_yaw;

SuperCAp_t SuperCAP;

moto_measure_t chassis_motor[4];
// 发射弹丸电机

moto_measure_t trigger_motor; // 拨盘
float pm_voltage = 0, pm_current = 0, pm_power;
/**
 * @brief can1  CAN接收中断回调
 * @param
 */
int wwwqq = 0,
	qqxx = 0;
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

	CAN_RxHeaderTypeDef rx_header;
	uint8_t rx_data[8];
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data); // receive can data
	switch (rx_header.StdId)
	{

	case 0x201:
		chassis_motor[RL].msg_cnt++ <= 50 ? get_moto_offset(&chassis_motor[RL], rx_data) : encoder_data_handle(&chassis_motor[RL], rx_data);
		break;
	case 0x202:
		chassis_motor[FR].msg_cnt++ <= 50 ? get_moto_offset(&chassis_motor[FR], rx_data) : encoder_data_handle(&chassis_motor[FR], rx_data);
		break;
	case 0x203:
		chassis_motor[FL].msg_cnt++ <= 50 ? get_moto_offset(&chassis_motor[FL], rx_data) : encoder_data_handle(&chassis_motor[FL], rx_data);
		break;
	case 0x204:
		chassis_motor[RR].msg_cnt++ <= 50 ? get_moto_offset(&chassis_motor[RR], rx_data) : encoder_data_handle(&chassis_motor[RR], rx_data);
		break;
	case 0x212:
	{qqxx++;
		pm_voltage = (float)((int32_t)(rx_data[1] << 8) | (int32_t)(rx_data[0])) / 100.0f;
		pm_current = (float)((int32_t)(rx_data[3] << 8) | (int32_t)(rx_data[2])) / 100.0f;
		pm_power = pm_voltage * pm_current;
		break;
	}
	case 0x01:
		damiao_4310_data_handle(&big_yaw, rx_data);
		break;
	case 0x211:    // ????
					{
							SuperCAP.cap_v = (float)(rx_data[0]<<8|rx_data[1])/10.0f;
							SuperCAP.If_wireless_wrong = rx_data[2];
							SuperCAP.wireless_state = rx_data[3];
							SuperCAP.cap_wrong_code = rx_data[4];
  						SuperCAP.cap_state = rx_data[5];
							SuperCAP.real_power = (float)((int16_t)(rx_data[6]<<8|rx_data[7]))/10.0f;
							if(SuperCAP.cap_wrong_code!=0)
													break;
					}
	default:

		break;
	}
}
/**
 * @brief can2
 * @param
 */
int dx = 0;
int de = 0;
uint8_t rx_buff[8];
void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

	CAN_RxHeaderTypeDef rx_header;
	uint8_t rx_data[8];
	HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &rx_header, rx_data); // receive can data
	switch (rx_header.StdId)
	{
		case 0x205:
			trigger_motor.msg_cnt++ <= 50 ? get_moto_offset(&trigger_motor, rx_data) : encoder_data_handle(&trigger_motor, rx_data);
		break;
	
	
	default:

		break;
	}
}

static void get_moto_offset(moto_measure_t *ptr, uint8_t data[])
{
	ptr->ecd = (uint16_t)(data[0] << 8 | data[1]);
	ptr->offset_ecd = ptr->ecd;
}

void encoder_data_handle(moto_measure_t *ptr, uint8_t data[])
{
	ptr->last_ecd = ptr->ecd;
	ptr->ecd = (uint16_t)(data[0] << 8 | data[1]);
	ptr->speed_rpm = (int16_t)(data[2] << 8 | data[3]);
	ptr->given_current = (int16_t)(data[4] << 8 | data[5]);
	ptr->temperate = data[6];

	if (ptr->ecd - ptr->last_ecd > 4096)
	{
		ptr->round_cnt--;
	}
	else if (ptr->ecd - ptr->last_ecd < -4096)
	{
		ptr->round_cnt++;
	}
	ptr->total_ecd = ptr->round_cnt * 8192 + ptr->ecd - ptr->offset_ecd;
	ptr->total_angle = ptr->total_ecd * 360 / 8192;
}

void damiao_4310_data_handle(damiao_typedef *ptr, uint8_t data[])
{

	for (int i = 0; i < 8; i++)
	{
		rx_buff[i] = data[i];
	}
	ptr->ID = data[0] & 0x0F;
	ptr->ERR = (data[0] >> 4) & 0x0F;
	ptr->POS = (data[1] << 8) | data[2];
	ptr->VEL = (data[3] << 4) | (data[4] >> 4);
	ptr->T = ((data[4] & 0xF) << 8) | data[5];
	ptr->T_MOS = data[6];
	ptr->T_ROTOR = data[7];

	ptr->_pos = uint_to_float(ptr->POS, -3.14159, 3.14159, 16);
	ptr->_vel = uint_to_float(ptr->VEL, -45, 45, 12);
	ptr->_torq = uint_to_float(ptr->T, -18, 18, 12);
	ptr->Angle = ptr->_pos * 180 / 3.14159f;
}

float uint_to_float(int x_int, float x_min, float x_max, int bits)
{
	float span = x_max - x_min;
	float offset = x_min;
	return ((float)x_int) * span / ((float)((1 << bits) - 1)) + offset;
}

int float_to_uint(float x, float x_min, float x_max, int bits)
{
	//	mm++;
	/// Converts a float to an unsigned int, given range and number of bits ///
	float span = x_max - x_min;
	float offset = x_min;
	return (int)((x - offset) * ((float)((1 << bits) - 1)) / span);
}
