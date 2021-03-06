#ifndef __PID_H
#define __PID_H
#include "stm32f1xx_hal.h"
typedef struct
{
    float SetSpeed;            //定义设定值
    float ActualSpeed;        //定义实际值
    float err;                //定义偏差值
    float err_next;            //定义上一个偏差值
    float err_last;            //定义最上前的偏差值
    float Kp, Ki, Kd;          //定义比例、积分、微分系数
    float integral;            //定义积分值
    float output;
} PidTypeDef;

void PID_init(PidTypeDef *pid, float Kp, float Ki, float Kd);
float PID_PositionRealize(PidTypeDef *pid, float speed, int16_t ActualSpeed);
float PID_IncermentRealize(PidTypeDef *pid, float speed, int16_t ActualSpeed);
#endif /* __PID_H */
