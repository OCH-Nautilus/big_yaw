#include "struct_typedef.h"
typedef struct
{
	
	float  Vx;//x轴速度
	float  Vy;//y轴速度
	float  Vz;//z轴速度
	float  V[4];//4个电机的目标转速
	float crd;//底盘目标角度与底盘的编码值差值
	float Vx2;//x轴的目标速度
	float Vy2;//y轴的目标速度
	float diff_angle;//底盘目标角度与底盘的弧度差值
	uint16_t last_mode;//底盘上一时刻模式
	uint16_t now_mode;//底盘当前模式
	int16_t Rotate_direction;//小陀螺旋转方向
	float limit_speed;
	float speed_top;
	float limit_speedadd;
	int16_t output[4];
	uint8_t front_set_num;// 底盘当前正方向设定索引
	float front_set[2];//底盘当前正方向设定

}CHASSIS_t;

float speed_limit_top(void);
float speed_limit_key(void);
float limit_add_speed(float cur_speed, float speed_ref) ;            
void chassis_init(void);
void chassis_assignment(CHASSIS_t *ch);
void chassis_ecdz(void);
void chassis_speed_calc(CHASSIS_t *ch,int16_t mode);
void chassis_current_calc(CHASSIS_t *ch);

extern CHASSIS_t CHASSIS;
