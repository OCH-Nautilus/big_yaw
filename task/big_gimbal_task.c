#include "big_gimbal_task.h"
#include "cmsis_os.h"
#include "pid.h"
#include "config.h"
#include "ins_task.h"
#include "lowpass.h"
#include "chassis_task.h"
#include "bsp_transmit.h"
#include "CAN_receive.h"
GIMBAL_t GIMBAL;
Modeling_Parameters_t Modeling_Parameters_yaw;


first_order_filter_type_t pitch_lowpass_key, yaw_lowpass_key;


// 无视觉
pid_type_def pid_big_yaw_angle;
pid_type_def pid_big_yaw_speed;

// 折叠

pid_type_def pid_big_yaw_angle_fold;
pid_type_def pid_big_yaw_speed_fold;
//掉头
pid_type_def pid_big_yaw_angle_dt;
pid_type_def pid_big_yaw_speed_dt;


// 自瞄装甲板
pid_type_def pid_big_yaw_vision_armor_angle;
pid_type_def pid_big_yaw_vision_armor_speed;

// 打符

pid_type_def pid_big_yaw_vision_buff_angle;
pid_type_def pid_big_yaw_vision_buff_speed;

float low_pass_yaw_key_num[1] = {LowPass_YAW_KEY_NUM};
float low_pass_pitch_key_num[1] = {LowPass_PITCH_KEY_NUM};

void gimbal_task(void const * argument)
{
  /* USER CODE BEGIN gimbal_task */
	 
  gimbal_init();
 vTaskDelay(10);
  /* Infinite loop */
  for(;;)
  {
	switch (USART_Rx_data.mode.bits.controls_mode)
	{
	case CONTROL_RC_CTRL:
		gimbal_mode_rc_ctrl();
		break;
	case CONTROL_KEYBOARD_CTRL:
	gimbal_mode_key_ctrl();
		break;
	default:
		break;
	}
	gimbal_pid_calc();
	GIMBAL.last_mode =USART_Rx_data.mode.bits.gimbal_mode;
    vTaskDelay(1);
  }
  /* USER CODE END gimbal_task */
}
void gimbal_init()
{
	USART_Rx_data.mode.bits.gimbal_mode=GIMBAL_IDLE;
	GIMBAL.PT_flag = 0;
	GIMBAL.last_mode =GIMBAL_IDLE;
	GIMBAL.IF_DT_OVER=1;
	GIMBAL.IF_FOLD_OVER=1;
	GIMBAL.angle_limit_flag=0;
	first_order_filter_init(&yaw_lowpass_key, LowPass_YAW_KEY_TIME, low_pass_yaw_key_num);
	first_order_filter_init(&pitch_lowpass_key, LowPass_PITCH_KEY_TIME, low_pass_pitch_key_num);


	// 无视觉(正常模式)
	PID_init(&pid_big_yaw_angle,PID_BIG_YAW_ANGLE_MODE,PID_BIG_YAW_ANGLE_KP,PID_BIG_YAW_ANGLE_KI,PID_BIG_YAW_ANGLE_KD,PID_BIG_YAW_ANGLE_IMAX_OUT,PID_BIG_YAW_ANGLE_MAX_OUT);
	PID_init(&pid_big_yaw_speed,PID_BIG_YAW_SPEED_MODE,PID_BIG_YAW_SPEED_KP,PID_BIG_YAW_SPEED_KI,PID_BIG_YAW_SPEED_KD,PID_BIG_YAW_SPEED_IMAX_OUT,PID_BIG_YAW_SPEED_MAX_OUT);
	//折叠模式
	PID_init(&pid_big_yaw_angle_fold,PID_BIG_YAW_FOLD_ANGLE_MODE,PID_BIG_YAW_FOLD_ANGLE_KP,PID_BIG_YAW_FOLD_ANGLE_KI,PID_BIG_YAW_FOLD_ANGLE_KD,PID_BIG_YAW_FOLD_ANGLE_IMAX_OUT,PID_BIG_YAW_FOLD_ANGLE_MAX_OUT);
	PID_init(&pid_big_yaw_speed_fold,PID_BIG_YAW_FOLD_SPEED_MODE,PID_BIG_YAW_FOLD_SPEED_KP,PID_BIG_YAW_FOLD_SPEED_KI,PID_BIG_YAW_FOLD_SPEED_KD,PID_BIG_YAW_FOLD_SPEED_IMAX_OUT,PID_BIG_YAW_FOLD_SPEED_MAX_OUT);
	// 装甲板
	PID_init(&pid_big_yaw_vision_armor_angle, PID_BIG_YAW_VISION_ARMOR_ANGLE_MODE, PID_BIG_YAW_VISION_ARMOR_ANGLE_KP, PID_BIG_YAW_VISION_ARMOR_ANGLE_KI, PID_BIG_YAW_VISION_ARMOR_ANGLE_KD, PID_BIG_YAW_VISION_ARMOR_ANGLE_IMAX_OUT, PID_BIG_YAW_VISION_ARMOR_ANGLE_MAX_OUT);
	PID_init(&pid_big_yaw_vision_armor_speed, PID_BIG_YAW_VISION_ARMOR_SPEED_MODE, PID_BIG_YAW_VISION_ARMOR_SPEED_KP, PID_BIG_YAW_VISION_ARMOR_SPEED_KI, PID_BIG_YAW_VISION_ARMOR_SPEED_KD, PID_BIG_YAW_VISION_ARMOR_SPEED_IMAX_OUT, PID_BIG_YAW_VISION_ARMOR_SPEED_MAX_OUT);
	// buff
	PID_init(&pid_big_yaw_vision_buff_angle,PID_BIG_YAW_VISION_BUFF_ANGLE_MODE,PID_BIG_YAW_VISION_BUFF_ANGLE_KP,PID_BIG_YAW_VISION_BUFF_ANGLE_KI,PID_BIG_YAW_VISION_BUFF_ANGLE_KD,PID_BIG_YAW_VISION_BUFF_ANGLE_IMAX_OUT,PID_BIG_YAW_VISION_BUFF_ANGLE_MAX_OUT);
	PID_init(&pid_big_yaw_vision_buff_speed,PID_BIG_YAW_VISION_BUFF_SPEED_MODE,PID_BIG_YAW_VISION_BUFF_SPEED_KP,PID_BIG_YAW_VISION_BUFF_SPEED_KI,PID_BIG_YAW_VISION_BUFF_SPEED_KD,PID_BIG_YAW_VISION_BUFF_SPEED_IMAX_OUT,PID_BIG_YAW_VISION_BUFF_SPEED_MAX_OUT);
}

/**
 * @brief 云台pid计算
 * @note
 * @param
 */
void gimbal_pid_calc()
{ 
    float yaw_error;  // 存储最短路径误差
    // 提前计算最短路径误差（所有模式共用）
    yaw_error = shortestAngleDiff(INS.Yaw, GIMBAL.big_yaw_target);
     
    
    switch(USART_Rx_data.mode.bits.gimbal_mode)
    {
        case GIMBAL_NORMAL:
//            if(USART_Rx_data.mode.bits.controls_mode == CONTROL_RC_CTRL)
//            {
                PID_calc(&pid_big_yaw_angle, 0, yaw_error);
                GIMBAL.big_yaw_output = PID_calc(&pid_big_yaw_speed, -INS.Gyro[2], pid_big_yaw_angle.out);
//            }
//            else // KEY_ctrl
//            {
//                PID_calc(&pid_big_yaw_angle, 0, yaw_error);
//                GIMBAL.big_yaw_output = PID_calc(&pid_big_yaw_speed, -INS.Gyro[2], pid_big_yaw_angle.out);
//							
//            }		
        break;
            
        case GIMBAL_FOLD:
            PID_calc(&pid_big_yaw_angle_fold, 0, yaw_error);
            GIMBAL.big_yaw_output = -PID_calc(&pid_big_yaw_speed_fold, INS.Gyro[2], pid_big_yaw_angle_fold.out);
        break;
            
        case GIMBAL_VISION:
            switch(USART_Rx_data.mode.bits.vision_mode)
            {
                case VISION_ARMOR:
                    PID_calc(&pid_big_yaw_vision_armor_angle, 0, yaw_error);
                    GIMBAL.big_yaw_output = PID_calc(&pid_big_yaw_vision_armor_speed, -INS.Gyro[2], pid_big_yaw_vision_armor_angle.out);
                    break;
                    
                case VISION_SMALL_BUFF:
                    PID_calc(&pid_big_yaw_vision_buff_angle, 0, yaw_error);
                    GIMBAL.big_yaw_output = PID_calc(&pid_big_yaw_vision_buff_speed, -INS.Gyro[2], pid_big_yaw_vision_buff_angle.out);
                    break;
                    
                case VISION_BIG_BUFF:
                    PID_calc(&pid_big_yaw_vision_buff_angle, 0, yaw_error);
                    GIMBAL.big_yaw_output = PID_calc(&pid_big_yaw_vision_buff_speed, -INS.Gyro[2], pid_big_yaw_vision_buff_angle.out);
                    break;
                    
                case VISION_CLOSE:
                    PID_calc(&pid_big_yaw_angle, 0, yaw_error);
                    GIMBAL.big_yaw_output = PID_calc(&pid_big_yaw_speed, INS.Gyro[2], pid_big_yaw_angle.out);
                    break;
                    
                default:
                    PID_calc(&pid_big_yaw_angle, 0, yaw_error);
                    GIMBAL.big_yaw_output = PID_calc(&pid_big_yaw_speed, -INS.Gyro[2], pid_big_yaw_angle.out);
                    break;
            }
            break;
		case GIMBAL_IDLE:
			GIMBAL.big_yaw_output=0;
		break;
		default:

		break;
	}


}
/**********************************遥控器控制云台***********************************/
/**
 * @brief gimbal_mode_rc_ctrl
 * @note
 * @param
 */
void gimbal_mode_rc_ctrl()
{
	switch (USART_Rx_data.mode.bits.gimbal_mode)
	{
	case GIMBAL_IDLE:
		gimbal_mode_rc_idle();
		break;
	case GIMBAL_NORMAL:
		gimbal_mode_rc_normal();
		break;
	case GIMBAL_VISION:
		gimbal_mode_rc_vision();
		break;
	case GIMBAL_FOLD:
		gimbal_mode_rc_fold();
		break;
	default:
		
		break;
	}
}

void gimbal_mode_rc_idle()
{
	GIMBAL.big_yaw_target = INS.Yaw;
}

void gimbal_mode_rc_normal()
{	
	//大yaw控制逻辑
	GIMBAL.ratio_yaw = (zero_180((USART_Rx_data.small_yaw_pos/3.14f*180.0f)-(FOLD_SMALL_YAW_ANGLE/3.14f*180.0f)))/(YAW_LIMIT_ANGLE/3.14f*180.0f/2);
	if(big_yaw.ERR==1)
		GIMBAL.big_yaw_target=INS.Yaw-GIMBAL.ratio_yaw*GIMBAL.ratio_yaw*GIMBAL.ratio_yaw*0.5f;
	
	GIMBAL.big_yaw_target=zero_180(GIMBAL.big_yaw_target);

//	turn_round();
	
}

void gimbal_mode_rc_vision()
{

	//大yaw控制逻辑
	GIMBAL.ratio_yaw = (zero_180((USART_Rx_data.small_yaw_pos/3.14f*180.0f)-(FOLD_SMALL_YAW_ANGLE/3.14f*180.0f)))/(YAW_LIMIT_ANGLE/3.14f*180.0f/2.0f);
	//if(big_yaw.ERR==1&&GIMBAL.big_yaw_vision_control_flag==1)
		GIMBAL.big_yaw_target=INS.Yaw-GIMBAL.ratio_yaw*GIMBAL.ratio_yaw*GIMBAL.ratio_yaw*0.5f;

	GIMBAL.big_yaw_target=zero_180(GIMBAL.big_yaw_target);
}

void gimbal_mode_rc_fold()
{
	if(big_yaw.ERR==1)
		GIMBAL.big_yaw_target-= USART_Rx_data.rc_ctrl_l_vx * SENSITIVITY_YAW_RC;
                                     
	GIMBAL.big_yaw_target=zero_180(GIMBAL.big_yaw_target);
}

/**********************************键鼠控制云台***********************************/
/**
 * @brief gimbal_mode_key_ctrl
 * @note
 * @param
 */
void gimbal_mode_key_ctrl()
{
	switch (USART_Rx_data.mode.bits.gimbal_mode)
	{
	case GIMBAL_IDLE:
		gimbal_mode_key_idle();
		break;
	case GIMBAL_NORMAL:
		gimbal_mode_key_normal();
		break;
	case GIMBAL_VISION:
		gimbal_mode_key_vision();
		break;
		case GIMBAL_FOLD:
		gimbal_mode_key_fold();
		break;
	default:
		gimbal_mode_key_idle();
		break;
	}
}

void gimbal_mode_key_idle()
{
	GIMBAL.big_yaw_target = INS.Yaw;
}
int a111=0;

void gimbal_mode_key_normal()
{
	
	GIMBAL.ratio_yaw = (zero_180((USART_Rx_data.small_yaw_pos/3.14f*180.0f)-(FOLD_SMALL_YAW_ANGLE/3.14f*180.0f)))/(YAW_LIMIT_ANGLE/3.14f*180.0f/2.5f);
	GIMBAL.big_yaw_target=INS.Yaw-GIMBAL.ratio_yaw*GIMBAL.ratio_yaw*GIMBAL.ratio_yaw*1.0f;
	
		turn_round();

	GIMBAL.big_yaw_target=zero_180(GIMBAL.big_yaw_target);
}

void gimbal_mode_key_vision()
{
	GIMBAL.ratio_yaw = (zero_180((USART_Rx_data.small_yaw_pos/3.14f*180.0f)-(FOLD_SMALL_YAW_ANGLE/3.14f*180.0f)))/(YAW_LIMIT_ANGLE/3.14f*180.0f/2.0f);
	//if(big_yaw.ERR==1&&GIMBAL.big_yaw_vision_control_flag==1)
		GIMBAL.big_yaw_target=INS.Yaw-GIMBAL.ratio_yaw*GIMBAL.ratio_yaw*GIMBAL.ratio_yaw*0.5f;

	GIMBAL.big_yaw_target=zero_180(GIMBAL.big_yaw_target);
}

void gimbal_mode_key_fold()
{
	GIMBAL.big_yaw_target-= USART_Rx_data.mouse_vx * SENSITIVITY_YAW_MOUSE;
	
	turn_round();
	GIMBAL.big_yaw_target=zero_180(GIMBAL.big_yaw_target);
}

/*********************************其他功能*****************************************/

//  // 掉头完成判断
// void gimbal_task_key_dt_judge(void)
// {
// 	if (GIMBAL.IF_DT_OVER == 0)
// 	{
// 		GIMBAL.IF_DT = 0;
// 		if (fabs(GIMBAL.yaw_target - imu.yaw) <= 5)
// 		{
// 			GIMBAL.IF_DT_OVER = 1;			
// 		}
// 		else if (fabs(GIMBAL.yaw_target - imu.yaw) > 5)
// 		{
// 			GIMBAL.IF_DT_OVER = 0;
// 			mode.chassis_state=CHASSIS_IDLE;
// 		}
// 	}
	
// }

// /**
//  * @brief 掉头
//  * @note
//  * @param
//  */
// void gimbal_u_turn()
// {

// 	if (mode.gimbal_state == GIMBAL_NORMAL)
// 	{
// 		if (GIMBAL.IF_DT == 1 && GIMBAL.IF_DT_OVER == 0) // 云台掉头
// 		{
// 			GIMBAL.yaw_target = imu.yaw + 180;
// 			CHASSIS.front_set_num ^= 1;
// 			mode.chassis_state=CHASSIS_IDLE;
			
// 		}
// 	}

// 	gimbal_task_key_dt_judge(); // 云台掉头完成判断
// }
uint16_t last_dt_over_flag;
int yuuuuu=0;
void turn_round()
{
//	if(USART_Rx_data.mode.bits.IF_DT_FLAG==1&&USART_Rx_data.mode.bits.DT_OVER_FLAG==0)//触发掉头
//	{
//				CHASSIS.front_set_num ^= 1;		
//	}
	if(last_dt_over_flag==1&&USART_Rx_data.mode.bits.DT_OVER_FLAG==0&&USART_Rx_data.mode.bits.gimbal_mode==GIMBAL_NORMAL)//触发掉头
	{
				CHASSIS.front_set_num ^= 1;
			
	}
	if(last_dt_over_flag==1&&USART_Rx_data.mode.bits.DT_OVER_FLAG==0&&USART_Rx_data.mode.bits.gimbal_mode==GIMBAL_FOLD)//触发掉头
	{
		yuuuuu++;	
		CHASSIS.front_set_num ^= 1;
		GIMBAL.big_yaw_target+=180.0f;
	}
	last_dt_over_flag=USART_Rx_data.mode.bits.DT_OVER_FLAG;
}

	

 
// void u_turn_over()
// {	
// 	if(GIMBAL.IF_DT_OVER == 0)
// 	{
// 		GIMBAL.IF_DT = 0;
// 		mode.chassis_state=CHASSIS_IDLE;
// 		if(mode.gimbal_state==GIMBAL_NORMAL)
// 		{
// 			if(fabs(imu.yaw-INS.Yaw)<5.0f&&GIMBAL.u_turn_cnt[GIMBAL.u_turn_index]==1&&mode.gimbal_state==GIMBAL_NORMAL&&GIMBAL.u_turn_index<3)
// 			{//开始下一次掉头条件:1.索引值<3 2.分次掉头计数值为1 3.前一次大小yaw角度差小于5°
// 			//开始进行一次掉头：1.目标角+=45° 2.前一次掉头标志位清零 3.索引值递增
// 				GIMBAL.yaw_target+=45.0f;
// 				GIMBAL.u_turn_cnt[GIMBAL.u_turn_index]=0;
// 				GIMBAL.u_turn_index++;
// 			}
// 			//判断完整掉头完成 1.索引值=3 2.第四次掉头完成 3.大小yaw角度差小于5°	
// 			if (GIMBAL.u_turn_index==3&&GIMBAL.u_turn_cnt[GIMBAL.u_turn_index]==0&&fabs(imu.yaw-INS.Yaw)<5.0f)
// 			{
// 				GIMBAL.IF_DT_OVER = 1;
// 			}
// 		}
// 		else if(mode.gimbal_state==GIMBAL_FOLD)
// 		{
// 			if (fabs(GIMBAL.big_yaw_target - INS.Yaw) <= 5)
// 			{
// 				GIMBAL.IF_DT_OVER = 1;			
// 			}
// 			else if (fabs(GIMBAL.big_yaw_target - INS.Yaw) > 5)
// 			{
// 				GIMBAL.IF_DT_OVER = 0;
// 				mode.chassis_state=CHASSIS_IDLE;
// 			}
// 		}
		
// 	}
		
	
	
// }

// void u_turn()
// {
	
// 	if(mode.gimbal_state==GIMBAL_NORMAL)
// 	{
// 		if(GIMBAL.IF_DT == 1 && GIMBAL.IF_DT_OVER == 0)//触发掉头
// 		{
// 			GIMBAL.u_turn_cnt[0]=GIMBAL.u_turn_cnt[1]=GIMBAL.u_turn_cnt[2]=GIMBAL.u_turn_cnt[3]=1;			
// 			CHASSIS.front_set_num ^= 1;
// 			mode.chassis_state=CHASSIS_IDLE;
// 			GIMBAL.u_turn_index=0;			
// 		}
// 	}
// 	else if(mode.gimbal_state==GIMBAL_FOLD)
// 	{
// 		if(GIMBAL.IF_DT == 1 && GIMBAL.IF_DT_OVER == 0)//触发掉头
// 		{
// 			GIMBAL.big_yaw_target+=180.0f;
// 			CHASSIS.front_set_num ^= 1;
// 			mode.chassis_state=CHASSIS_IDLE;
// 		}
// 	}

// 	u_turn_over();
// }

int Get_Sign(float a)
{
	int x=(a<0)?(-1):1;
	return x;
}


float tq_limit(float tq)
{
	if(tq>7.5f)
		tq=7.5f;
	if(tq<-7.5f)
		tq=-7.5f;

	return tq;
}
float Modeling_Parameters_cacl(Modeling_Parameters_t Modeling_Parameters)
{
	Modeling_Parameters.target_tq = Modeling_Parameters.Inertia*Modeling_Parameters.target_acc
                                      +Modeling_Parameters.Viscosity_coefficient*Modeling_Parameters.target_vel
                                      //+Modeling_Parameters.Gravity*sin(yaw_vision_set)
                                      +Modeling_Parameters.F_Coulomb*Get_Sign(Modeling_Parameters.target_vel);
	Modeling_Parameters.target_tq = tq_limit(Modeling_Parameters.target_tq);
	
	return Modeling_Parameters.target_tq;
}

/**
 * @brief 过零处理
 * @note  
 * @param
 */
float zero_180(float angle)
{
	if(angle>180.0f)
		angle-=360.0f;
	else if(angle<-180.0f)
			angle+=360.0f;
	
	return angle;
}

float zero_PI(float angle)
{
	if(angle>3.14159f)
		angle-=6.28318f;
	else if(angle<-3.14159f)
			angle+=6.28318f;
	
	return angle;
}

/**
 * @brief 计算最短路径角度差值（针对±180范围）
 * @param current 当前角度 (-180~180)
 * @param target 目标角度 (-180~180)
 * @return 最短路径差值 (-180~180)
 */
float shortestAngleDiff(float current, float target) 
{
    float diff = target - current;
    
    if (diff > 180.0f) 
		{
        diff -= 360.0f;
    } else if (diff < -180.0f) 
		{
        diff += 360.0f;
    }
    
    return diff;
}

/**
 * @brief 大yaw在视觉模式下控制判断
 * @note  大yaw
 * @param
 */
 void big_yaw_vision_control_judge()
 {
	if(USART_Rx_data.mode.bits.gimbal_mode==GIMBAL_VISION)
	{
		if(fabs(USART_Rx_data.small_yaw_pos-FOLD_SMALL_YAW_ANGLE)>BIG_YAW_VISION_CONTROL_ANGLE)//如果小yaw相对于大yaw的偏差角度在30度以外，则允许大yaw进行视觉控制	
		{
			GIMBAL.big_yaw_vision_control_flag=1;
		}
		else//不允许大yaw进行视觉控制	
		{
			GIMBAL.big_yaw_vision_control_flag=0;
		}
	}


 }

