#ifndef _CHASSIS_POWER_CONTEROL_H
#define _CHASSIS_POWER_CONTEROL_H



typedef enum
{
	HalCAP= 0,//被动电容模式-保留飞坡能量
	FullCAP=1,//电容模式-利用率较高
}PowerState_control_t;//电容控制模式

typedef struct{

	
	float predict_power;//目标电流和转速拟合的功率
	float predict_power_old;//当前电流和转速拟合的功率Z
	
	
	float set_power;//动态功率上限
	float set_superpower;//充电功率
	short referee_max_power;//裁判系统最大功率
	short remainEnergy;//裁判系统剩余能量
	short Current_set[4];//求解的目标电流
	short Speed_Now[4];//当前电机转速rpm
	short Current_Last[4];//上一时刻电机电流
	

	double k_dynamic;//电流计算导出值（无实际用处）
	float capEnergy;//电容能量-由电压转换为容组能量，初步设定0V到23V对应2000J
	float Min_capEnergy;//最小电容能量
	float HalfCAP_Power;
	
	//静态配置参数
	float No_limited_Power;//没有限制时跑的功率
	float Add_HalfCAP_Power;//被动电容时多跑的功率
	float P_capEngry;//电容能量利用系数
	float Min_capVol;//最小电容电压
	float P_remainEngry;//缓冲能量利用系数
	float Min_remainEnergy;//最小剩余能量
}Power_Limit_type;


typedef struct{

	float k1[4];
	float k2[4];
	float constant[4];

}Power_fit_factor_t;






extern Power_Limit_type powerlimit;
extern Power_fit_factor_t Power_fit_factor;
extern PowerState_control_t PowerState_control;
extern float total_power;

void chassis_power_control_xj(void);




#endif
