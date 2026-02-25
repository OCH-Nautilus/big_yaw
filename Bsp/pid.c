#include <main.h>
#include "pid.h"
#include "string.h"
//#define LimitMax(input,max)  \
//{                            \
//	if(input>max)              \
//	{                          \
//		input=max;               \
//	}                          \
//	else if(input<-max)        \
//	{                          \
//		input=-max;              \
//	}                          \
//}                            \

//typedef struct 
//{
//	uint8_t mode;//0为位置式，1为增量式//PID三参数
//	float Kp;
//	float Ki;
//	float Kd; 
//	
//	float max_out;//最大输出
//	float max_iout;//最大积分输出
//	
//	float set; 
//	float ref; 
//	
//	float out; 
//	float Pout;
//	float Iout;
//	float Dout; 
//	float error[3];//误差项0最新1上一次2上上次pid_type_def
//}pid_type_def;


void PID_init(pid_type_def *pid, uint8_t mode, float Kp, float Ki, float Kd, float max_iout, float max_out) 
{
if(pid == NULL) 
return; 
pid->mode= mode; 
pid->Kp= Kp; 
pid->Ki= Ki; pid->Kd= Kd; pid->max_iout= max_iout; pid->max_out= max_out; 
pid->error[0] = pid->error[1]= pid->error[2]= pid->Pout= pid->Iout= pid->Dout= pid->out= 0.0f;
}
#define LimitMax(input , max)       \
    {                               \
		    if(input > max)             \
				 {                          \
				   input = max;             \
				 }                          \
				else if (input < -max)      \
				 {                          \
					input = -max;             \
				 }                          \
		}
		
float PID_calc(pid_type_def*pid, float ref, float set) 
{
	if (pid== NULL) return 0.0f; 
	
	pid->error[2] = pid->error[1];
	pid->error[1] = pid->error[0]; 
	pid->set= set; 
	pid->ref= ref; 
	pid->error[0]= set- ref; 
	if (pid->mode== 0) 
	{
		pid->Pout = pid->Kp* pid->error[0];
		pid->Iout+= pid->Ki* pid->error[0]; 
		pid->Dout= pid->Kd*(pid->error[0]- pid->error[1]); 
            
		LimitMax(pid->Iout,pid->max_iout); 
		pid->out= pid->Pout+ pid->Iout + pid->Dout; 
		LimitMax(pid->out, pid->max_out);
	} 
	else if(pid->mode== 1) 
	{
		pid->Pout= pid->Kp* (pid->error[0]- pid->error[1]); 
		pid->Iout= pid->Ki* pid->error[0]; pid->Dout= pid->Kd*(pid->error[0]- 2.0f* pid->error[1]+ pid->error[2]); 
		pid->out+= pid->Pout+ pid->Iout + pid->Dout;
		LimitMax(pid->out, pid->max_out); 
	}
		return pid->out;
}

void feedforward_control_init(feedforward_control_t *str, float alpha, float belta, float outmax)
{
    memset(str, 0, sizeof(feedforward_control_t));
    str->alpha = alpha;
    str->belta = belta;
		str->outmax = outmax;
}



float feedforward_control_calc(feedforward_control_t *str, float disturb)
{
    str->disturb = disturb;
    str->output = str->alpha * (str->disturb - str->last_disturb) +
                  str->belta * (str->disturb - 2 * str->last_disturb + str->pre_disturb);
    LimitMax(str->output, str->outmax);
    str->pre_disturb = str->last_disturb;
    str->last_disturb = str->disturb;
    return str->output; 
}

