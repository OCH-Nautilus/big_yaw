#ifndef __LOW_PASS_H
#define __LOW_PASS_H
typedef __packed struct
{
    float input;        //输入数据
    float out;          //滤波输出的数据
    float num[1];       //滤波参数
    float frame_period; //滤波的时间间隔 单位 s
} first_order_filter_type_t;
//一阶滤波初始化
extern void first_order_filter_init(first_order_filter_type_t *first_order_filter_type, float frame_period, const float num[1]);
//一阶滤波计算
extern float first_order_filter_cali(first_order_filter_type_t *first_order_filter_type, float input);
float lowpassFilter_two( float input );
#endif

