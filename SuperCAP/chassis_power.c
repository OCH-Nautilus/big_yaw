#include "chassis_power.h"
#include "config.h"
#include "send_current_task.h"
#include "CAN_receive.h"
#include "referee.h"
#include "math.h"
#include "chassis_task.h"
#include "pid.h"
#include "bsp_transmit.h"
#define CAP_C 8.3  //超级电容组的容值，能量/电压 e=1/2*c*v*v  1900j

PowerState_control_t PowerState_control=HalCAP;
extern SuperCAp_t SuperCAP;
extern pid_type_def speed[4];

//电机拟合参数定义
Power_fit_factor_t Power_fit_factor=
{
	.k1[FL]=3.5395e-07,
	.k2[FL]=2.4541e-07,
	.constant[FL]=0.74325,
	.k1[FR]=3.5395e-07,
	.k2[FR]=2.4541e-07,
	.constant[FR]=0.74325,
	.k1[RL]=3.5395e-07,
	.k2[RL]=2.4541e-07,
	.constant[RL]=0.74325,
	.k1[RR]=3.5395e-07,
	.k2[RR]=2.4541e-07,
	.constant[RR]=0.74325,
};

//功率控制参数定义
Power_Limit_type powerlimit={
	.set_superpower=20.0f,//充电功率
	.No_limited_Power=230,	//没有限制时跑的功率（起步，加速度）
	.Min_capVol = 8.0,		//V  被动电容保留能量用来飞坡
	.Add_HalfCAP_Power = 30,			//W 被动电容多跑的功率
	.Min_remainEnergy=25,	        //最低缓存能量
};

void PowerState_up(void)
{
	if(USART_Rx_data.mode.bits.controls_mode==CONTROL_KEYBOARD_CTRL&&(USART_Rx_data.key.bits.Key_Shift==1||USART_Rx_data.key.bits.Key_Flag_E==1))
		PowerState_control=FullCAP;
	else if(USART_Rx_data.mode.bits.controls_mode==CONTROL_KEYBOARD_CTRL&&(USART_Rx_data.key.bits.Key_Shift==0&&USART_Rx_data.key.bits.Key_Flag_E==0))
		PowerState_control=HalCAP;
	if(USART_Rx_data.mode.bits.controls_mode==CONTROL_RC_CTRL&&USART_Rx_data.rc_ctrl_s.bits.s_l==3)
		PowerState_control=FullCAP;
	else if(USART_Rx_data.mode.bits.controls_mode==CONTROL_RC_CTRL&&USART_Rx_data.rc_ctrl_s.bits.s_l!=3)
		PowerState_control=HalCAP;
}

void cal_powerSet(Power_Limit_type *power_limit,PowerState_control_t PowerState_control) 
{
	//电容能量-由电压转换为容组能量，初步设定0V到23V对应2000J
	power_limit->capEnergy=0.5*CAP_C*(SuperCAP.cap_v*SuperCAP.cap_v);
	//计算最小电容能量
	powerlimit.Min_capEnergy = 0.5*CAP_C*(powerlimit.Min_capVol*powerlimit.Min_capVol);	
	//裁判系统最大功率(直接由裁判系统传回)
	powerlimit.referee_max_power=70;//robot_status.chassis_power_limit;
	//更新裁判系统剩余缓存能量(直接由裁判系统传回)
	powerlimit.remainEnergy=30;//power_heat_data.buffer_energy;
	//电容能量利用系数给定
	powerlimit.P_capEngry = power_limit->referee_max_power/(0.5*23.0*23.0*CAP_C - powerlimit.Min_capEnergy) + 0.1;
	//缓冲能量保留系数
	powerlimit.P_remainEngry = power_limit->referee_max_power/(60 - power_limit->Min_remainEnergy);
	
	// 根据当前功率选择目标功率
	if(power_limit->remainEnergy>40&&powerlimit.referee_max_power>40&&chassis_motor[RL].speed_rpm<3000)//powerlimit.referee_max_power>40代表全局消耗功率还未达到20000J
		power_limit->HalfCAP_Power= power_limit->referee_max_power + power_limit->Add_HalfCAP_Power + 100;
	else if(powerlimit.referee_max_power<=40.0f&&USART_Rx_data.key.bits.Key_E==1)
		power_limit->HalfCAP_Power = 200;
	else
		power_limit->HalfCAP_Power = power_limit->referee_max_power + 20;
	
	switch(PowerState_control)//电容模式	
	{
		case FullCAP:
		#ifdef CAP
		if( power_limit->referee_max_power>= 40.0f)
		power_limit->set_power = power_limit->No_limited_Power;//set_power-动态功率上限， No_limited_Power-没有限制时跑的功率
		else if(power_limit->referee_max_power < 40.0f&&USART_Rx_data.key.bits.Key_Shift==1)
		power_limit->set_power = 60;
		else if(power_limit->referee_max_power < 40.0f&&USART_Rx_data.key.bits.Key_E==1)
		power_limit->set_power = 200;//set_power-动态功率上限， No_limited_Power-没有限制时跑的功率
		#endif  
		#ifdef BAT      
		if (power_limit->remainEnergy < power_limit->Min_remainEnergy*1.3f){	//最小阈值之前都是恒功率	//remainEnergy-裁判系统剩余能量;Min_remainEnergy-最小裁判系统剩余能量
		power_limit->set_power = power_limit->referee_max_power +			//referee_max_power-裁判系统最大功率;
								 power_limit->P_remainEngry*(power_limit->remainEnergy - power_limit->Min_remainEnergy); //P_remainEngry-缓冲能量利用系数
		}else{
			power_limit->set_power = power_limit->referee_max_power + 60;	
		}
		#endif
		break;
		case HalCAP:
		#ifdef CAP			
		if (power_limit->capEnergy < power_limit->Min_capEnergy*1.2f){	//最小阈值之前都是恒功率		
			power_limit->set_power = power_limit->referee_max_power + power_limit->P_capEngry*(power_limit->capEnergy - power_limit->Min_capEnergy); //capEnergy-电容能量;Min_capEnergy-最小电容能量
			}else{
			power_limit->set_power = power_limit->HalfCAP_Power;  //HalfCAP_Power 半电容功率
		}
		#endif
		#ifdef BAT
			if (power_limit->remainEnergy < power_limit->Min_remainEnergy*1.2f){	//最小阈值之前都是恒功率		
				power_limit->set_power = power_limit->referee_max_power +				//referee_max_power-裁判系统最大功率;
										 power_limit->P_remainEngry*(power_limit->remainEnergy - power_limit->Min_remainEnergy);	//P_remainEngry-缓冲能量利用系数,Min_remainEnergy-最小裁判系统剩余能量
			}else{
				power_limit->set_power = power_limit->HalfCAP_Power;	
			}			
		#endif
		break;
	}
	
	if(power_limit->capEnergy<300)
		power_limit->set_power=powerlimit.referee_max_power-power_limit->capEnergy/9.0f;
	else if(powerlimit.remainEnergy<25)
		power_limit->set_power=powerlimit.referee_max_power-(35.0f-powerlimit.remainEnergy)/2.0f;
	
}

float total_power;
float power_sacle_t;
void powerlimit_control_xj(void)
{
	fp32 chassis_max_power = 0;

	float initial_give_power[4];   // 计算的初始功率
	float initial_total_power = 0; //计算的总功率
	fp32 scaled_give_power[4];     //缩放初始功率


	fp32 toque_coefficient = 1.99688994e-6f; // (20/16384)*(0.3)*(187/3591)/9.55
//	fp32 a = 1.23e-07;						 // k1
//	fp32 k2 = 1.453e-07;					 // k2
//	fp32 constant = 4.081f;



	chassis_max_power = powerlimit.set_power ;
	float predict_power=0;
	float predict_power_old=0;
	
	for(int i=0;i<4;i++)
	{
		powerlimit.Current_set[i]=speed[i].out;
		powerlimit.Speed_Now[i]=chassis_motor[i].speed_rpm;
		powerlimit.Current_Last[i]=chassis_motor[i].given_current;

		initial_give_power[i]=(1.99688994e-6f*powerlimit.Current_set[i]*powerlimit.Speed_Now[i]+Power_fit_factor.k1[i]*powerlimit.Speed_Now[i]*powerlimit.Speed_Now[i]+Power_fit_factor.k2[i]*powerlimit.Current_set[i]*powerlimit.Current_set[i]+Power_fit_factor.constant[i]);
		
		if (initial_give_power[i] < 0) // 不包括负功率（暂时性）negative power not included (transitory)
		continue;
		predict_power_old+=(1.99688994e-6f*powerlimit.Current_Last[i]*powerlimit.Speed_Now[i]+Power_fit_factor.k1[i]*powerlimit.Speed_Now[i]*powerlimit.Speed_Now[i]+Power_fit_factor.k2[i]*powerlimit.Current_Last[i]*powerlimit.Current_Last[i]+Power_fit_factor.constant[i]);
		predict_power+=initial_give_power[i];//预测的总功率
		initial_total_power+=initial_give_power[i];//四个轮子的总功率
	}
	powerlimit.predict_power_old=predict_power_old;
	powerlimit.predict_power=predict_power;

	if (initial_total_power > chassis_max_power) // 判断是否大于最大功率determine if larger than max power
	{
		fp32 power_scale = chassis_max_power / initial_total_power;
		power_sacle_t=power_scale;
		for (uint8_t i = 0; i < 4; i++)
		{
			scaled_give_power[i]= initial_give_power[i] * power_scale; // 获得可扩展的功率get scaled power
			if (scaled_give_power[i] < 0)
			{
				continue;
			}
			fp32 a = Power_fit_factor.k1[i];		
			fp32 b = toque_coefficient * chassis_motor[i].speed_rpm;
			fp32 c = Power_fit_factor.k2[i] * chassis_motor[i].speed_rpm * chassis_motor[i].speed_rpm - scaled_give_power[i] + Power_fit_factor.constant[i];
		
			if (speed[i].out > 0) // 根据原始力矩的方向选择计算公式
			{
				fp32 temp = (-b + sqrt(b * b - 4 * a * c)) / (2 * a);
				powerlimit.k_dynamic=temp;
				if (temp > 16000)
				{
				
					CHASSIS.output[i] = 16000;
				}
				else
					CHASSIS.output[i] = temp;
			}
			else
			{
				fp32 temp = (-b - sqrt(b * b - 4 * a * c)) / (2 * a);
				if (temp < -16000)
				{
					CHASSIS.output[i] = -16000;
				}
				else
					CHASSIS.output[i] = temp;
			}			
		}
	}
	total_power = initial_total_power;
}




void chassis_power_control_xj(void)
{
	PowerState_up();
	cal_powerSet(&powerlimit,PowerState_control);
	powerlimit_control_xj();
	

}

