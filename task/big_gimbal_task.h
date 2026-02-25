#include "struct_typedef.h"
#include "pid.h"

typedef struct
{

uint16_t vision_block;
uint16_t found_flag;//视觉发现目标
uint16_t last_mode;//云台上一时刻模式
uint16_t PT_flag;//平头触发标志位
uint32_t DT_Time;//掉头时间
uint16_t IF_DT_OVER;//掉头完成标志位
uint16_t IF_DT;//掉头触发标志位
uint16_t IF_FOLLOW_ALLOW;//运行底盘跟随标志位
uint16_t IF_FOLD;//触发折叠标志位
uint16_t IF_FOLD_OVER;//折叠完成标志位
uint16_t u_turn_cnt[4];//掉头分步计次
uint16_t u_turn_index;//掉头分步计次索引值
uint16_t rise_over;//大pitch升起完成标志位
uint16_t down_over;//大pitch折叠完成标志位
uint16_t angle_limit_flag;//限位标志位
uint16_t big_yaw_vision_control_flag;//视觉模式下控制大yaw允许标志位
float big_yaw_target;//大yaw目标角
float big_yaw_output;//大yaw目标输出电流
float kalman_pitch_target;
float fold_yaw_pos;//折叠目标角
float fold_big_pitch_pos;//折叠目标角
float big_yaw_diff_angle;//大yaw与小yaw的偏差角度
float ratio_yaw;//大yaw转动系数
}GIMBAL_t;

typedef struct
{
  float Inertia;//云台转动惯量
  float Viscosity_coefficient; //粘滞系数
  float Gravity;//重力矩
  float F_Coulomb; //库伦力
  
  float target_acc;//经过pid算出来的目标加速度
  float target_vel;//目标角速度
  float target_tq;//通过建模算出来的最佳力矩
}Modeling_Parameters_t;
//力矩=转动惯量乘以角加速度+粘滞系数乘以角速度+重力矩乘以sin(角度)+库伦摩擦力乘以角速度的方向
//  力矩T = Inertia*target_yaw_acc + Viscosity_coefficient * target_yaw_vel +Gravity*sin(target_angle) + F_Coulomb*sign(target_yaw_vel)

void gimbal_init(void);
void gimbal_pid_calc(void);
void gimbal_mode_rc_ctrl(void);
void gimbal_mode_rc_idle(void);
void gimbal_mode_rc_normal(void);
void gimbal_mode_rc_vision(void);
void gimbal_mode_rc_fold(void);

float pitch_protect(float data);
void gimbal_u_turn(void);
int Get_Sign(float a);
float tq_limit(float tq);
float Modeling_Parameters_cacl(Modeling_Parameters_t Modeling_Parameters);
void gimbal_mode_key_ctrl(void);
void gimbal_mode_key_idle(void);
void gimbal_mode_key_normal(void);
void gimbal_mode_key_fold(void);

void gimbal_mode_key_vision(void);
float record_small_yaw_pos(void);
float record_big_pitch_pos(void);
void fold_state_judge(void);
void u_turn(void);
void turn_round(void);	


 void yaw_limit(void);
float zero_PI(float angle);
float shortestAngleDiff(float current, float target) ;
float zero_180(float angle);


extern GIMBAL_t GIMBAL;
extern pid_type_def pid_yaw_angle;
extern pid_type_def pid_yaw_speed;
float smoothAngleTransition(float current, float target);


//写一个根据掉头时的标志位决定底盘状态

