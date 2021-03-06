/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2017 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "Motor.h"
#include "wifi.h"
#include "control.h"
#include "pid.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
#include "stdio.h"
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
    HAL_UART_Transmit(&huart1 , (uint8_t *)&ch, 1 , 0xffff);
    return ch;
}

extern uint8_t usart1_temp;
volatile int8_t X_axis = 60; //收到的横坐标
volatile int8_t Y_axis = 80; //收到的纵坐标
int flag = 0;
int32_t ABOVE_PWMSet;
int32_t BELOW_PWMSet;
int32_t ABOVE_PWMSetTemp;
int32_t BELOW_PWMSetTemp;
PidTypeDef AboveSteerPID;
PidTypeDef BelowSteerPID;

void PID_Init(void)
{
    PID_init(&AboveSteerPID, 1.5, 0, 2);
    PID_init(&BelowSteerPID, 1.5, 0, 2);
}

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();

  /* USER CODE BEGIN 2 */
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1); //下面舵机启动
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2); //上面舵机启动
    PID_Init();

    while (HAL_UART_Receive_IT(&huart1, &usart1_temp,
                               sizeof(usart1_temp)) != HAL_OK);//开启串口中断
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    //X_axis=20;
    while (1)
    {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
//			AboveSteerChange(300);
        int i;
        for (i = 0; i < 10; i++)
        {
					ABOVE_PWMSetTemp= (int32_t)PID_PositionRealize(&AboveSteerPID, 0, X_axis - 60);
            if (ABOVE_PWMSetTemp > 200)
            {
                ABOVE_PWMSetTemp = 200;
            }
            else if (ABOVE_PWMSetTemp < -200)
            {
                ABOVE_PWMSetTemp = -200;
            }
						ABOVE_PWMSet+=ABOVE_PWMSetTemp;
            AboveSteerChange(-ABOVE_PWMSet);                           //舵机转向控制
						HAL_Delay(10);
        }

        for (i = 0; i < 10; i++)
        {
					  BELOW_PWMSet += (int32_t)PID_PositionRealize(&BelowSteerPID, 0, Y_axis - 80); //
            if (BELOW_PWMSet > 400)
            {
                BELOW_PWMSet = 400;
            }
            else if (BELOW_PWMSet < -400)
            {
                BELOW_PWMSet = -400;
            }
            BelowSteerChange(BELOW_PWMSet);                           //舵机转向控制
						HAL_Delay(10);
//            BELOW_PWMSetTemp = (int32_t)PID_PositionRealize(&BelowSteerPID, 0, Y_axis - 80); //
//            if (BELOW_PWMSetTemp > 400)
//            {
//                BELOW_PWMSetTemp = 400;
//            }
//            else if (BELOW_PWMSetTemp < -400)
//            {
//                BELOW_PWMSetTemp = -400;
//            }
//						BELOW_PWMSet+=BELOW_PWMSetTemp;
//            BelowSteerChange(BELOW_PWMSet);                           //舵机转向控制
//						HAL_Delay(10);
        }

//          if (flag==1)
//          {
//              AboveSteerChange(0);
//              flag=0;
//          }
//          else
//          {
//              AboveSteerChange(ABOVE_STEER_MIN);
//                              flag=1;
//
//          }
//          HAL_Delay(1000);
//          int i=1;
//          while(i<10)
//          {
//              Pid_Control(X_axis-40,Y_axis-60);
//              HAL_Delay(10);
//              i++;
//          }
    }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
    /* User can add his own implementation to report the HAL error return state */
    while (1)
    {
    }
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
      ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
