#include "SuperCAP.h"
#include "cmsis_os.h"
#include "INS_task.h"
#include "referee.h"
#include "chassis_power.h"
pid_cap_t pid_buff =
{  
   .mode = 0,
   .Kp = 0.5,
   .Ki = 0.01,
   .Kd = 0,
   .max_out = 10,
   .max_iout = 5,
   .min_out = -10,
   .min_iout = -5,
};


uint16_t limit,limit_100 ;
uint16_t Power_Buffer,Power_Buffer_100 ;

/*向电容发送指令*/
/*   V 2.0.0   */
//void Send_SupPower(CAN_HandleTypeDef *hcan)     
//{
//	CAN_TxHeaderTypeDef tx_header;
//	uint8_t sendbuf[8];
//	
//	uint32_t	pTxMailbox;

//    limit = (robot_status.chassis_power_limit - 5);	//裁判系统功率上限
//	  //Power_Buffer = power_heat_data.buffer_energy*100;	//缓冲能量
//	  
////    pid_init(&pid_buff,0,0,0,0,0);
//    Power_Buffer = limit - PID_cap_calc(&pid_buff,power_heat_data.buffer_energy,30);

//    limit_100 = limit * 100; 
//    Power_Buffer_100 = Power_Buffer * 100; 
//    sendbuf[0] = limit_100 >> 8 ;//工作模式
//		sendbuf[1] = limit_100 ;
//    sendbuf[2] = Power_Buffer_100 >> 8 ;//裁判系统限制功率
//		sendbuf[3] = Power_Buffer_100 ;
//    // sendbuf[4] = Power_Buffer >> 8 ;//裁判系统反馈的当前功缓冲能量
//		// sendbuf[5] = Power_Buffer ;

//	  tx_header.StdId = 0x210;
//    tx_header.IDE   = CAN_ID_STD;
//    tx_header.RTR   = CAN_RTR_DATA;
//    tx_header.DLC   = 8;
//	
//	HAL_CAN_AddTxMessage(hcan, &tx_header, sendbuf,&pTxMailbox);
//}

void Send_SupPower(CAN_HandleTypeDef *hcan)  
{
   CAN_TxHeaderTypeDef tx_header;
   uint8_t sendbuf[8];
     uint32_t	pTxMailbox;
    tx_header.StdId = 0x210;
    tx_header.IDE   = CAN_ID_STD;
    tx_header.RTR   = CAN_RTR_DATA;
    tx_header.DLC   = 8;
    
      limit = (powerlimit.referee_max_power - 5);    //裁判系统功率上限
    Power_Buffer = limit - PID_cap_calc(&pid_buff,power_heat_data.buffer_energy,30);//计算发给电容的功率

    limit_100 = limit * 100; 
    Power_Buffer_100 = Power_Buffer * 100; 
    
    
    sendbuf[0] = 0;
      sendbuf[1] = 0;
      sendbuf[2] = Power_Buffer_100>>8;
      sendbuf[3] = Power_Buffer_100;
        

    HAL_CAN_AddTxMessage(hcan, &tx_header, sendbuf,&pTxMailbox);
}
/*******************************************************************
  * Function : 判断是否开启超级电容
*******************************************************************/
uint8_t IF_SupPower_ON_OR_OFF = 0;     // TODO
uint8_t SupPower_ON_OR_OFF = 1;

void SupPower_Mode_Change(void)
{
		Send_SupPower(&hcan1);  // 电容
}

#define LimitMax(input, max)   \
    {                          \
        if (input > max)       \
        {                      \
            input = max;       \
        }                      \
        else if (input < -max) \
        {                      \
            input = -max;      \
        }                      \
    }


float PID_cap_calc(pid_cap_t *pid, float ref, float set) 
{
    if (pid == 0) 
		{
        return 0.0f;
    }

    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];
    pid->set = set;
    pid->fdb = ref;
    pid->error[0] = set - ref;
    if (pid->mode == 0) {
        pid->Pout = pid->Kp * pid->error[0];
        pid->Iout += pid->Ki * pid->error[0];
        pid->Dbuf[2] = pid->Dbuf[1];
        pid->Dbuf[1] = pid->Dbuf[0];
        pid->Dbuf[0] = (pid->error[0] - pid->error[1]);
        pid->Dout = pid->Kd * pid->Dbuf[0];
        LimitMax(pid->Iout, pid->max_iout);
        pid->out = pid->Pout + pid->Iout + pid->Dout;
        LimitMax(pid->out, pid->max_out);
    } else if (pid->mode == 1) {
        pid->Pout = pid->Kp * (pid->error[0] - pid->error[1]);
        pid->Iout = pid->Ki * pid->error[0];
        pid->Dbuf[2] = pid->Dbuf[1];
        pid->Dbuf[1] = pid->Dbuf[0];
        pid->Dbuf[0] = (pid->error[0] - 2.0f * pid->error[1] + pid->error[2]);
        pid->Dout = pid->Kd * pid->Dbuf[0];
        pid->out += pid->Pout + pid->Iout + pid->Dout;
        LimitMax(pid->out, pid->max_out);
    }
    return pid->out;
}

