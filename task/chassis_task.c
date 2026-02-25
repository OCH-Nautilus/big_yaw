#include "chassis_task.h"
#include "CAN_receive.h"
#include "bsp_can.h"
#include "cmsis_os.h"
#include "config.h"
#include "pid.h"
#include "send_current_task.h"
#include "bsp_transmit.h"
#include "math.h"
#include "referee.h"
#include "chassis_power.h"
CHASSIS_t CHASSIS;
pid_type_def speed[4];
pid_type_def pid_yaw_follow;

/**
 * @brief 底盘总任务
 * @note
 * @param
 */
void chassis_task(void const *argument)
{
	chassis_init();
    vTaskDelay(10);
	for (;;)
	{
		chassis_ecdz();
		chassis_assignment(&CHASSIS);
		chassis_speed_calc(&CHASSIS, 1);
		chassis_current_calc(&CHASSIS);
#ifdef CHASSIS_SENT
		chassis_ctrl_current();
#else
		Error_Chassis();
#endif

		vTaskDelay(1);
	}
}

/**
 * @brief 底盘初始化
 * @note
 * @param
 */
void chassis_init()
{
    CHASSIS.Rotate_direction = 1;
	CHASSIS.front_set_num = 1;
	CHASSIS.Vx = 0;
	CHASSIS.Vx2 = 0;
	CHASSIS.Vy = 0;
	CHASSIS.Vy2 = 0;
	CHASSIS.Vz = 0;
	CHASSIS.V[FR] = 0;
	CHASSIS.V[RR] = 0;
	CHASSIS.V[RL] = 0;
	CHASSIS.V[FL] = 0;
	CHASSIS.last_mode = USART_Rx_data.mode.bits.chassis_mode;
    CHASSIS.front_set[0] = FRONT_SET_1;
    CHASSIS.front_set[1] = FRONT_SET_2;
    PID_init(&speed[FL], PID_MOTOR_MODE, PID_MOTOR_KP, PID_MOTOR_KI, PID_MOTOR_KD, PID_MOTOR_IOUT_MAX, PID_MOTOR_OUT_MAX);
	PID_init(&speed[RL], PID_MOTOR_MODE, PID_MOTOR_KP, PID_MOTOR_KI, PID_MOTOR_KD, PID_MOTOR_IOUT_MAX, PID_MOTOR_OUT_MAX);
	PID_init(&speed[FR], PID_MOTOR_MODE, PID_MOTOR_KP, PID_MOTOR_KI, PID_MOTOR_KD, PID_MOTOR_IOUT_MAX, PID_MOTOR_OUT_MAX);
	PID_init(&speed[RR], PID_MOTOR_MODE, PID_MOTOR_KP, PID_MOTOR_KI, PID_MOTOR_KD, PID_MOTOR_IOUT_MAX, PID_MOTOR_OUT_MAX);
	PID_init(&pid_yaw_follow, PID_YAW_FOLLOW_MODE, PID_YAW_FOLLOW_KP, PID_YAW_FOLLOW_KI, PID_YAW_FOLLOW_KD, PID_YAW_FOLLOW_IOUT_MAX, PID_YAW_FOLLOW_OUT_MAX);
}

/**
 * @brief 底盘参数赋值
 * @note
 * @param
 */
int aaa=0;
void chassis_assignment(CHASSIS_t *ch) 
{
	if (!ch->front_set_num)
	{aaa++;
		if (USART_Rx_data.mode.bits.controls_mode == CONTROL_RC_CTRL) // 遥控器控制
		{
			ch->Vx = -USART_Rx_data.rc_ctrl_r_vy * SENSITIVITY_CHASSIS_RC_X;
			ch->Vy = -USART_Rx_data.rc_ctrl_r_vx * SENSITIVITY_CHASSIS_RC_Y;
		}
		else 
		{
			ch->Vx = -(USART_Rx_data.key.bits.Key_S - USART_Rx_data.key.bits.Key_W) * speed_limit_key() * 2.0f;
			ch->Vy = -(USART_Rx_data.key.bits.Key_A - USART_Rx_data.key.bits.Key_D) * speed_limit_key() * 2.0f;
		}
	}
	else
	{
		if (USART_Rx_data.mode.bits.controls_mode == CONTROL_RC_CTRL) 
		{
			ch->Vx = USART_Rx_data.rc_ctrl_r_vy * SENSITIVITY_CHASSIS_RC_X;
			ch->Vy = USART_Rx_data.rc_ctrl_r_vx * SENSITIVITY_CHASSIS_RC_Y;
		}
		else 
		{
			ch->Vx = (USART_Rx_data.key.bits.Key_W - USART_Rx_data.key.bits.Key_S) * speed_limit_key() * 2.0f;
			ch->Vy = (USART_Rx_data.key.bits.Key_D - USART_Rx_data.key.bits.Key_A) * speed_limit_key() * 2.0f;
		}
	}
}

/**
 * @brief 底盘参数计算
 * @note
 * @param
 */
float yaw_angle = 0;//此时大小yaw的叠加角度

void chassis_ecdz()//计算底盘与目标方向差值,底盘正方向需要重新标定
{

	yaw_angle =USART_Rx_data.small_yaw_pos * 180 / 3.14159f-big_yaw.Angle;
	if (yaw_angle > 180.0f)
		yaw_angle -= 360.0f;
	else if (yaw_angle < -180.0f)
		yaw_angle += 360.0f;

	CHASSIS.crd = yaw_angle - CHASSIS.front_set[CHASSIS.front_set_num];

	if (CHASSIS.crd > 180.0f)
		CHASSIS.crd -= 360.0f;
	else if (CHASSIS.crd < -180.0f)
		CHASSIS.crd += 360.0f;

	CHASSIS.diff_angle = CHASSIS.crd / 360 * 2 * 3.1415926f;
	if (USART_Rx_data.mode.bits.chassis_mode == CHASSIS_FOLLOW)
		CHASSIS.Vz = PID_calc(&pid_yaw_follow, CHASSIS.crd, 0);
	else if (USART_Rx_data.mode.bits.chassis_mode == CHASSIS_TOP)
		CHASSIS.Vz = speed_limit_top() * CHASSIS.Rotate_direction;
}

/**
 * @brief 底盘速度解算
 * @note
 * @param
 */float big_yaw_to_chassis_angle;
float small_yaw_to_big_yaw_angle;
void chassis_speed_calc(CHASSIS_t *ch, int16_t mode)
{
	small_yaw_to_big_yaw_angle=USART_Rx_data.small_yaw_pos-FOLD_SMALL_YAW_ANGLE;
	big_yaw_to_chassis_angle=big_yaw._pos-BIG_YAW_CHASSIS_ANGLE;
	if (mode == 1)
	{
		ch->Vx2 = (ch->Vx * cos(small_yaw_to_big_yaw_angle) + ch->Vy * sin(small_yaw_to_big_yaw_angle));
		ch->Vy2 = (ch->Vy * cos(small_yaw_to_big_yaw_angle) - ch->Vx * sin(small_yaw_to_big_yaw_angle));


		
		// 全向底盘
		double a = 0;
		double b = 0;
		double c = 0;
		a = (0.785385f - big_yaw_to_chassis_angle);
		b = (0.785385f + big_yaw_to_chassis_angle);
		c = a;
		
		
		ch->V[RR] = +(cos(a) * ch->Vx2) + (sin(a) * ch->Vy2) - ch->Vz;//相对于大yaw的速度
		ch->V[FR] = +(cos(b) * ch->Vx2) - (sin(b) * ch->Vy2) - ch->Vz;
		ch->V[RL] = -(cos(c) * ch->Vx2) - (sin(c) * ch->Vy2) - ch->Vz;
		ch->V[FL] = -(cos(b) * ch->Vx2) + (sin(b) * ch->Vy2) - ch->Vz;
	}
}

//void chassis_speed_calc(CHASSIS_t *ch, int16_t mode)
//{
//	ch->Vx2 = limit_add_speed(ch->Vx2, ch->Vx);
//	ch->Vy2 = limit_add_speed(ch->Vy2, ch->Vy);

//	if (mode == 0) // mode0麦轮 mode1全向轮
//	{
//		// 麦轮底盘 V1~4为电机目标转速
//		ch->Vx = (ch->Vx2 * cos(ch->diff_angle) + ch->Vy2 * sin(ch->diff_angle));
//		ch->Vy = (ch->Vy2 * cos(ch->diff_angle) - ch->Vx2 * sin(ch->diff_angle));

//		ch->V[FL] = -ch->Vx2 - ch->Vy2 + ch->Vz;
//		ch->V[RL] = -ch->Vx2 + ch->Vy2 + ch->Vz;
//		ch->V[FR] = ch->Vx2 - ch->Vy2 + ch->Vz;
//		ch->V[RR] = ch->Vx2 + ch->Vy2 + ch->Vz;
//	}
//	else if (mode == 1)
//	{
//		// 全向底盘
//		double a = 0;
//		double b = 0;
//		double c = 0;
//		a = (0.785385f - ch->diff_angle);
//		b = (0.785385f + ch->diff_angle);
//		c = a;
////		ch->V[FR] = +cos(a) * ch->Vx2 + sin(a) * ch->Vy2 - ch->Vz;
////		ch->V[RR] = +cos(b) * ch->Vx2 - sin(b) * ch->Vy2 - ch->Vz;
////		ch->V[RL] = -cos(c) * ch->Vx2 - sin(c) * ch->Vy2 - ch->Vz;
////		ch->V[FL] = -cos(b) * ch->Vx2 + sin(b) * ch->Vy2 - ch->Vz;
//		ch->V[RR] = +cos(a) * ch->Vx2 + sin(a) * ch->Vy2 - ch->Vz;
//		ch->V[FR] = +cos(b) * ch->Vx2 - sin(b) * ch->Vy2 - ch->Vz;
//		ch->V[RL] = -cos(c) * ch->Vx2 - sin(c) * ch->Vy2 - ch->Vz;
//		ch->V[FL] = -cos(b) * ch->Vx2 + sin(b) * ch->Vy2 - ch->Vz;
//	}
//}
/**
 * @brief 底盘pid计算
 * @note
 * @param
 */
void chassis_current_calc(CHASSIS_t *ch)
{

	ch->output[FL] = PID_calc(&speed[FL], chassis_motor[FL].speed_rpm, ch->V[FL]);
	ch->output[RL] = PID_calc(&speed[RL], chassis_motor[RL].speed_rpm, ch->V[RL]);
	ch->output[FR] = PID_calc(&speed[FR], chassis_motor[FR].speed_rpm, ch->V[FR]);
	ch->output[RR] = PID_calc(&speed[RR], chassis_motor[RR].speed_rpm, ch->V[RR]);
}

// 加速度限制赋值
float speed_limit(void)
{
	if (USART_Rx_data.mode.bits.chassis_speed_mode == CHASSIS_SPEED_SHIFT)
		CHASSIS.limit_speedadd = CHASSIS_SPEED_LIMIT_SHIFT;
	else if (USART_Rx_data.mode.bits.chassis_speed_mode == CHASSIS_SPEED_FLY)
		CHASSIS.limit_speedadd = CHASSIS_SPEED_LIMIT_FLY;
	else
		CHASSIS.limit_speedadd = CHASSIS_SPEED_LIMIT_NORMAL;
	return CHASSIS.limit_speedadd;
}

// 加速度限制
float limit_add_speed(float cur_speed, float speed_ref)
{
	if (fabs(cur_speed - speed_ref) >= speed_limit())
	{
		if (cur_speed > speed_ref)
		{
			speed_ref = cur_speed - speed_limit();
		}
		if (cur_speed < speed_ref)
		{
			speed_ref = cur_speed + speed_limit();
		}
	}
	return speed_ref;
}

// 速度赋值——小陀螺
float speed_limit_top(void)
{
#ifdef BAT
	if (USART_Rx_data.mode.bits.chassis_speed_mode == CHASSIS_SPEED_NORMAL)
	{
		switch (robot_status.chassis_power_limit)
		{
		case 45:
			CHASSIS.speed_top = TOP_SPEED_45W_BAT - TOP_SPEED_ADD_BAT;
			break;

		case 50:
			CHASSIS.speed_top = TOP_SPEED_50W_BAT - TOP_SPEED_ADD_BAT;
			break;

		case 55:
			CHASSIS.speed_top = TOP_SPEED_55W_BAT - TOP_SPEED_ADD_BAT;
			break;

		case 60:
			CHASSIS.speed_top = TOP_SPEED_60W_BAT - TOP_SPEED_ADD_BAT;
			break;

		case 65:
			CHASSIS.speed_top = TOP_SPEED_65W_BAT - TOP_SPEED_ADD_BAT;
			break;

		case 70:
			CHASSIS.speed_top = TOP_SPEED_70W_BAT - TOP_SPEED_ADD_BAT;
			break;

		case 75:
			CHASSIS.speed_top = TOP_SPEED_75W_BAT - TOP_SPEED_ADD_BAT;
			break;

		case 80:
			CHASSIS.speed_top = TOP_SPEED_80W_BAT - TOP_SPEED_ADD_BAT;
			break;

		case 85:
			CHASSIS.speed_top = TOP_SPEED_85W_BAT - TOP_SPEED_ADD_BAT;
			break;

		case 90:
			CHASSIS.speed_top = TOP_SPEED_90W_BAT - TOP_SPEED_ADD_BAT;
			break;

		case 95:
			CHASSIS.speed_top = TOP_SPEED_95W_BAT - TOP_SPEED_ADD_BAT;
			break;

		case 100:
			CHASSIS.speed_top = TOP_SPEED_100W_BAT - TOP_SPEED_ADD_BAT;
			break;

		case 120:
			CHASSIS.speed_top = TOP_SPEED_120W_BAT - TOP_SPEED_ADD_BAT;
			break;

		default:
			CHASSIS.speed_top = TOP_SPEED_DEFAULT_BAT - TOP_SPEED_ADD_BAT;
			break;
		}
	}
	else if (USART_Rx_data.mode.bits.chassis_speed_mode == CHASSIS_SPEED_SHIFT)
	{
		CHASSIS.speed_top = TOP_SPEED_SHIFT_BAT;
	}
	else if (USART_Rx_data.mode.bits.chassis_speed_mode == CHASSIS_SPEED_FLY)
	{
		CHASSIS.speed_top = TOP_SPEED_FLY_BAT;
	}
#endif
#ifdef CAP
	if (USART_Rx_data.mode.bits.chassis_speed_mode == CHASSIS_SPEED_NORMAL)
	{
		switch (powerlimit.referee_max_power)
		{
		case 45:
			CHASSIS.speed_top = TOP_SPEED_45W_CAP - TOP_SPEED_ADD_CAP;
			break;

		case 50:
			CHASSIS.speed_top = TOP_SPEED_50W_CAP - TOP_SPEED_ADD_CAP;
			break;

		case 55:
			CHASSIS.speed_top = TOP_SPEED_55W_CAP - TOP_SPEED_ADD_CAP;
			break;

		case 60:
			CHASSIS.speed_top = TOP_SPEED_60W_CAP - TOP_SPEED_ADD_CAP;
			break;

		case 65:
			CHASSIS.speed_top = TOP_SPEED_65W_CAP - TOP_SPEED_ADD_CAP;
			break;

		case 70:
			CHASSIS.speed_top = TOP_SPEED_70W_CAP - TOP_SPEED_ADD_CAP;
			break;

		case 75:
			CHASSIS.speed_top = TOP_SPEED_75W_CAP - TOP_SPEED_ADD_CAP;
			break;

		case 80:
			CHASSIS.speed_top = TOP_SPEED_80W_CAP - TOP_SPEED_ADD_CAP;
			break;

		case 85:
			CHASSIS.speed_top = TOP_SPEED_85W_CAP - TOP_SPEED_ADD_CAP;
			break;

		case 90:
			CHASSIS.speed_top = TOP_SPEED_90W_CAP - TOP_SPEED_ADD_CAP;
			break;

		case 95:
			CHASSIS.speed_top = TOP_SPEED_95W_CAP - TOP_SPEED_ADD_CAP;
			break;

		case 100:
			CHASSIS.speed_top = TOP_SPEED_100W_CAP - TOP_SPEED_ADD_CAP;
			break;

		case 120:
			CHASSIS.speed_top = TOP_SPEED_120W_CAP - TOP_SPEED_ADD_CAP;
			break;

		default:
			CHASSIS.speed_top = TOP_SPEED_DEFAULT_CAP - TOP_SPEED_ADD_CAP;
			break;
		}
	}
	else if (USART_Rx_data.mode.bits.chassis_speed_mode == CHASSIS_SPEED_SHIFT)
	{
		CHASSIS.speed_top = TOP_SPEED_SHIFT_CAP;
	}
	else if (USART_Rx_data.mode.bits.chassis_speed_mode == CHASSIS_SPEED_FLY)
	{
		CHASSIS.speed_top = TOP_SPEED_FLY_CAP;
	}
#endif
	return CHASSIS.speed_top;
}

// 速度赋值——键鼠
float speed_limit_key(void)
{
#ifdef BAT
	if (USART_Rx_data.mode.bits.chassis_speed_mode == CHASSIS_SPEED_NORMAL)
	{
		switch (robot_status.chassis_power_limit)
		{
		case 45:
			CHASSIS.limit_speed = SPEED_45W_BAT - SPEED_ADD_BAT;
			break;

		case 50:
			CHASSIS.limit_speed = SPEED_50W_BAT - SPEED_ADD_BAT;
			break;

		case 55:
			CHASSIS.limit_speed = SPEED_55W_BAT - SPEED_ADD_BAT;
			break;

		case 60:
			CHASSIS.limit_speed = SPEED_60W_BAT - SPEED_ADD_BAT;
			break;

		case 65:
			CHASSIS.limit_speed = SPEED_65W_BAT - SPEED_ADD_BAT;
			break;

		case 70:
			CHASSIS.limit_speed = SPEED_70W_BAT - SPEED_ADD_BAT;
			break;

		case 75:
			CHASSIS.limit_speed = SPEED_75W_BAT - SPEED_ADD_BAT;
			break;

		case 80:
			CHASSIS.limit_speed = SPEED_80W_BAT - SPEED_ADD_BAT;
			break;

		case 85:
			CHASSIS.limit_speed = SPEED_85W_BAT - SPEED_ADD_BAT;
			break;

		case 90:
			CHASSIS.limit_speed = SPEED_90W_BAT - SPEED_ADD_BAT;
			break;

		case 95:
			CHASSIS.limit_speed = SPEED_95W_BAT - SPEED_ADD_BAT;
			break;

		case 100:
			CHASSIS.limit_speed = SPEED_100W_BAT - SPEED_ADD_BAT;
			break;

		case 120:
			CHASSIS.limit_speed = SPEED_120W_BAT - SPEED_ADD_BAT;
			break;

		default:
			CHASSIS.limit_speed = SPEED_DEFAULT_BAT - SPEED_ADD_BAT;
			break;
		}
	}
	else if (USART_Rx_data.mode.bits.chassis_speed_mode == CHASSIS_SPEED_SHIFT)
	{
		CHASSIS.limit_speed = SPEED_SHIFT_BAT;
	}
	else if (USART_Rx_data.mode.bits.chassis_speed_mode == CHASSIS_SPEED_FLY)
	{
		CHASSIS.limit_speed = SPEED_FLY_BAT;
	}
#endif
#ifdef CAP
	if (USART_Rx_data.mode.bits.chassis_speed_mode == CHASSIS_SPEED_NORMAL)
	{
		switch (powerlimit.referee_max_power)
		{
		case 45:
			CHASSIS.limit_speed = SPEED_45W_CAP - SPEED_ADD_CAP;
			break;

		case 50:
			CHASSIS.limit_speed = SPEED_50W_CAP - SPEED_ADD_CAP;
			break;

		case 55:
			CHASSIS.limit_speed = SPEED_55W_CAP - SPEED_ADD_CAP;
			break;

		case 60:
			CHASSIS.limit_speed = SPEED_60W_CAP - SPEED_ADD_CAP;
			break;

		case 65:
			CHASSIS.limit_speed = SPEED_65W_CAP - SPEED_ADD_CAP;
			break;

		case 70:
			CHASSIS.limit_speed = SPEED_70W_CAP - SPEED_ADD_CAP;
			break;

		case 75:
			CHASSIS.limit_speed = SPEED_75W_CAP - SPEED_ADD_CAP;
			break;

		case 80:
			CHASSIS.limit_speed = SPEED_80W_CAP - SPEED_ADD_CAP;
			break;

		case 85:
			CHASSIS.limit_speed = SPEED_85W_CAP - SPEED_ADD_CAP;
			break;

		case 90:
			CHASSIS.limit_speed = SPEED_90W_CAP - SPEED_ADD_CAP;
			break;

		case 95:
			CHASSIS.limit_speed = SPEED_95W_CAP - SPEED_ADD_CAP;
			break;

		case 100:
			CHASSIS.limit_speed = SPEED_100W_CAP - SPEED_ADD_CAP;
			break;

		case 120:
			CHASSIS.limit_speed = SPEED_120W_CAP - SPEED_ADD_CAP;
			break;

		default:
			CHASSIS.limit_speed = SPEED_DEFAULT_CAP - SPEED_ADD_CAP;
			break;
		}
	}
	else if (USART_Rx_data.mode.bits.chassis_speed_mode == CHASSIS_SPEED_SHIFT)
	{
		CHASSIS.limit_speed = SPEED_SHIFT_CAP;
	}
	else if (USART_Rx_data.mode.bits.chassis_speed_mode == CHASSIS_SPEED_FLY)
	{
		CHASSIS.limit_speed = SPEED_FLY_CAP;
	}
#endif
	return CHASSIS.limit_speed;
}

