#ifndef __SUPERCAP_H
#define __SUPERCAP_H

#include "bsp_can.h"

typedef struct
{
    unsigned char mode;
    float Kp;
    float Ki;
    float Kd;
    float T;

    float max_out;
    float min_out;
    float max_iout;
    float min_iout;
    float set;
    float fdb;

    float out;
    float Pout;
    float Iout;
    float Dout;
    float Dbuf[3];
    float error[3];

} pid_cap_t;

#define ERR_V_HIGH                    (1u << 0)
#define ERR_V_LOW                     (1u << 1)
#define ERR_C_ERROR                   (1u << 2)
#define ERR_C_HIGH                    (1u << 3)
#define ERR_BUCKBOOST                 (1u << 4)
#define ERR_CAN                       (1u << 5)
#define ERR_MASK                      (0x0000FFFF)
#define CAP_LOW                       (1u << 16)
#define CAP_INING                     (1u << 17)
#define CAP_OUTING                    (1u << 18)
#define CAP_FULL                      (1u << 19)
#define CAP_MASK                      (0xFFFF0000)


float PID_cap_calc(pid_cap_t *pid, float ref, float set);
extern void SupPower_Mode_Change(void);


#endif
