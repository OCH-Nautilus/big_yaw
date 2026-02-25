/* pid.h ----------------------------------------------------------
 *
 * 简易 PID 控制器
 * 支持“位置式”(mode = 0) 与“增量式”(mode = 1)
 * ---------------------------------------------------------------*/

#ifndef __PID_H
#define __PID_H

#include <stdint.h>

/*----------------- 宏 -----------------*/
/* 将 input 限制在 [-max, max] 区间 */
//#define LimitMax(input, max)            \
//    do {                                \
//        if ((input) > (max))            \
//            (input) = (max);            \
//        else if ((input) < -(max))      \
//            (input) = -(max);           \
//    } while (0)

/*----------------- 结构体定义 -----------------*/
typedef struct
{
    uint8_t mode;      /* 0: 位置式, 1: 增量式 */
    float   Kp;
    float   Ki;
    float   Kd;

    float   max_out;   /* 输出限幅 */
    float   max_iout;  /* 积分项限幅 */

    float   set;       /* 设定值 */
    float   ref;       /* 反馈值 */

    float   out;       /* 最终输出 */
    float   Pout;
    float   Iout;
    float   Dout;

    float   error[3];  /* error[0] 最新, error[1] 上一次, error[2] 上上次 */
} pid_type_def;

/*----------------- API 声明 -----------------*/
#ifdef __cplusplus
extern "C" {
#endif
/* 初始化 PID 结构体 */
extern void  PID_init(pid_type_def *pid,
                      uint8_t       mode,
                      float         Kp,
                      float         Ki,
                      float         Kd,
                      float         max_iout,
                      float         max_out);

/* 计算一次 PID 输出
 * ref : 当前测量值
 * set : 设定值
 * 返回  : 本次计算得到的输出值
 */
typedef struct 
{
    /* data */
    float disturb;       //????
    float last_disturb;  //????
    float pre_disturb;   //????
    float alpha;         //a
    float belta;         //b
    float output;
    float outmax;
}feedforward_control_t;
extern float PID_calc(pid_type_def *pid, float ref, float set);
void feedforward_control_init(feedforward_control_t *str, float alpha, float belta, float outmax);
float feedforward_control_calc(feedforward_control_t *str, float disturb);

#ifdef __cplusplus
}
#endif

#endif  /* __PID_H */
