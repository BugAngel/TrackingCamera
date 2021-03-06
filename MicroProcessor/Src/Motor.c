/**
******************************************************************************
    * @file     Motor.c
    * @author   Johnny Sun
    * @version  1.0
    * @date     2014/10/22
    * @brief    Motor Driver
      WangRong modify it to stm32 in 2016/3/11
			下面那个接B6，上面那个接B7
******************************************************************************
*/

#include "Motor.h"

const int32_t ABOVE_STEER_Middle = 1349 ;
const int32_t ABOVE_STEER_MIN = -600;
const int32_t ABOVE_STEER_MAX = 600;

const int32_t BELOW_STEER_Middle = 1299 ;
const int32_t BELOW_STEER_MIN = -400;//下
const int32_t BELOW_STEER_MAX = 400; //上

/**
  * @brief  下面的舵机控制.
  * @param  position_value:位置改变量
  * @retval NONE
  */
void BelowSteerChange(int32_t position_value)
{

    if (position_value > BELOW_STEER_MAX)
    {
        position_value = BELOW_STEER_MAX;
    }
    else if (position_value < BELOW_STEER_MIN)
    {
        position_value = BELOW_STEER_MIN;
    }

    TIM4->CCR1 = position_value + BELOW_STEER_Middle;
}


/**
  * @brief  上面的舵机控制.
  * @param  position_value:位置改变量
  * @retval NONE
  */
void AboveSteerChange(int32_t position_value)
{

    if (position_value > ABOVE_STEER_MAX)
    {
        position_value = ABOVE_STEER_MAX;
    }
    else if (position_value < ABOVE_STEER_MIN)
    {
        position_value = ABOVE_STEER_MIN;
    }

    TIM4->CCR2 = position_value + ABOVE_STEER_Middle;
}

void Stop()
{
    AboveSteerChange(0);
    BelowSteerChange(0);
}

#if 0
/**
  * @brief  右电机控制
  * @param  speed_value:预期速度
  * @retval NONE
  */
void MotorSpeedR(int32_t speed_value)
{

    if (speed_value > 0)     //正转
    {
        if (speed_value > MOTOR_SPEED_MAX)
        {
            speed_value = MOTOR_SPEED_MAX;
        }
        if (speed_value < MOTOR_SPEED_MIN)
        {
            speed_value = MOTOR_SPEED_MIN;
        }
        TIM4->CCR1 = speed_value;
        TIM4->CCR2 = 0;
    }
    else                       //反转
    {
        if (speed_value < -MOTOR_SPEED_MAX)
        {
            speed_value = -MOTOR_SPEED_MAX;
        }
        if (speed_value > -MOTOR_SPEED_MIN)
        {
            speed_value = -MOTOR_SPEED_MIN;
        }
        TIM4->CCR1 = 0;
        TIM4->CCR2 = -speed_value;
    }
}
/**
  * @brief  左电机控制
  * @param  speed_value:预期速度
  * @retval NONE
  */
void MotorSpeedL(int32_t speed_value)
{
    if (speed_value > 0)  //正转
    {
        if (speed_value > MOTOR_SPEED_MAX)
        {
            speed_value = MOTOR_SPEED_MAX;
        }
        else if (speed_value < MOTOR_SPEED_MIN)
        {
            speed_value = MOTOR_SPEED_MIN;
        }
        TIM4->CCR3 = speed_value;
        TIM4->CCR4 = 0;
    }
    else  // 倒转
    {
        if (speed_value < -MOTOR_SPEED_MAX)
        {
            speed_value = -MOTOR_SPEED_MAX;
        }
        else if (speed_value > -MOTOR_SPEED_MIN)
        {
            speed_value = -MOTOR_SPEED_MIN;
        }
        TIM4->CCR3 = 0;
        TIM4->CCR4 = -speed_value;
    }
}
#endif



