#include "lowpass.h"
#include "math.h"

// 初始化滤波器

	
/**
  * @brief          一阶低通滤波初始化
  * @author         RM
  * @param[in]      一阶低通滤波结构体
  * @param[in]      间隔的时间，单位 s
  * @param[in]      滤波参数
  * @retval         返回空
  */
void first_order_filter_init(first_order_filter_type_t *first_order_filter_type, float frame_period, const float num[1])
{
    first_order_filter_type->frame_period = frame_period;
    first_order_filter_type->num[0] = num[0];
    first_order_filter_type->input = 0.0f;
    first_order_filter_type->out = 0.0f;
}



/**
  * @brief          一阶低通滤波计算
  * @author         RM
  * @param[in]      一阶低通滤波结构体
  * @param[in]      间隔的时间，单位 s
  * @retval         返回空
  */
float first_order_filter_cali(first_order_filter_type_t *first_order_filter_type, float input)
{
    first_order_filter_type->input = input;
    first_order_filter_type->out =
        first_order_filter_type->num[0] / (first_order_filter_type->num[0] + first_order_filter_type->frame_period) * first_order_filter_type->out + first_order_filter_type->frame_period / (first_order_filter_type->num[0] + first_order_filter_type->frame_period) * first_order_filter_type->input;
  return first_order_filter_type->out;
}


//float lowpassFilter_two( float input ){
//   	static float angle_fliter_1 = 0.0f;
//    static float angle_fliter_2 = 0.0f;
//    static float angle_fliter_3 = 0.0f;
//	
//    static const float fliter_num[3] = {1.725709860247969f, -0.75594777109163436f, 0.030237910843665373f};
//    
//    angle_fliter_1 = angle_fliter_2;
//    angle_fliter_2 = angle_fliter_3;
//    angle_fliter_3 = angle_fliter_2 * fliter_num[0] + angle_fliter_1 * fliter_num[1] + input * fliter_num[2];
////    output = angle_fliter_3;
//	  return angle_fliter_3;
//}

float lowpassFilter_two( float input ){
   	static float angle_fliter_1 = 0.0f;
    static float angle_fliter_2 = 0.0f;
    static float angle_fliter_3 = 0.0f;
	
    static const float fliter_num[3] = {1.725709860247969f, -0.75594777109163436f, 0.030237910843665373f};
    
    angle_fliter_1 = angle_fliter_2;
    angle_fliter_2 = angle_fliter_3;
    angle_fliter_3 = angle_fliter_2 * fliter_num[0] + angle_fliter_1 * fliter_num[1] + input * fliter_num[2];
//    output = angle_fliter_3;
	  return angle_fliter_3;
}





