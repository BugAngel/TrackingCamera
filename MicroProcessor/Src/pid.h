#ifndef __PID_H
#define __PID_H
#include "stm32f1xx_hal.h"
typedef struct
{
    float SetSpeed;            //�����趨ֵ
    float ActualSpeed;        //����ʵ��ֵ
    float err;                //����ƫ��ֵ
    float err_next;            //������һ��ƫ��ֵ
    float err_last;            //��������ǰ��ƫ��ֵ
    float Kp, Ki, Kd;          //������������֡�΢��ϵ��
    float integral;            //�������ֵ
    float output;
} PidTypeDef;

void PID_init(PidTypeDef *pid, float Kp, float Ki, float Kd);
float PID_PositionRealize(PidTypeDef *pid, float speed, int16_t ActualSpeed);
float PID_IncermentRealize(PidTypeDef *pid, float speed, int16_t ActualSpeed);
#endif /* __PID_H */