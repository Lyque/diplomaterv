/**
  ******************************************************************************
  * @file    Templates/Src/main.c 
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    13-March-2015
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
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
#include "cmsis_os.h"
#include "ff.h"

/** @addtogroup STM32F4xx_HAL_Examples
  * @{
  */

/** @addtogroup Templates
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

osThreadId LEDThread1Handle;
osThreadId LEDThread2Handle;
osThreadId SDCardInitializationThreadHandle;
osThreadId ADCReadThreadHandle;
osThreadId UARTThreadHandle;
osThreadId MotorThreadHandle;
xSemaphoreHandle xSemaphore;

int8_t newline[] = "\r\n";
/* Private function prototypes -----------------------------------------------*/
static void LED_Thread1(void const *argument);
static void LED_Thread2(void const *argument);
static void SDCardInitializationThread(void const *argument);
static void ADCReadThread(void const *argument);
static void UARTThread(void const *argument);
static void IndicatorLEDThread(void const *argument);
static void SystemClock_Config(void);
static void Error_Handler(void);

static void UARTSend(int16_t data, int32_t length);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{

  /* Configure the system clock to 168 MHz */
  SystemClock_Config();

  /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user 
             can eventually implement his proper time base source (a general purpose 
             timer for example or other time source), keeping in mind that Time base 
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
             handled in milliseconds basis.
       - Low Level Initialization
     */
  HAL_Init();

  /* Add your application code here
     */
  BSP_LED_On(LED5);
//  BSP_LED_On(LED6);

  /* Thread 1 definition */
  osThreadDef(LED1, LED_Thread1, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);

  /*  Thread 2 definition */
  osThreadDef(LED2, LED_Thread2, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);

  /*  Thread 3 definition */
  osThreadDef(SDCard, SDCardInitializationThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);

  /*  Thread 4 definition */
  osThreadDef(ADC1, ADCReadThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);

  /*  Thread 5 definition */
  osThreadDef(UART4, UARTThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);

  /*  Thread 6 definition */
  osThreadDef(IndicatorLED, IndicatorLEDThread, osPriorityNormal, 0, configMINIMAL_STACK_SIZE);

  /* Start thread 1 */
  LEDThread1Handle = osThreadCreate(osThread(LED1), NULL);

  /* Start thread 2 */
  LEDThread2Handle = osThreadCreate(osThread(LED2), NULL);

  /* Start thread 3 */
  SDCardInitializationThreadHandle = osThreadCreate(osThread(SDCard), NULL);

  /* Start thread 4 */
  ADCReadThreadHandle = osThreadCreate(osThread(ADC1), NULL);

  /* Start thread 5 */
  UARTThreadHandle = osThreadCreate(osThread(UART4), NULL);

  /* Start thread 6 */
  MotorThreadHandle = osThreadCreate(osThread(IndicatorLED), NULL);

  /* Start scheduler    */
  osKernelStart();

  Error_Handler();
}

static void LED_Thread1(void const *argument)
{
  uint32_t count = 0;
  volatile float32_t tangent;
  (void) argument;

  for (;;)
  {
	tangent = atan(0.5);
    count = osKernelSysTick() + 5000;

    /* Toggle LED3 every 200 ms for 5 s */
    while (count >= osKernelSysTick())
    {
      BSP_LED_Toggle(LED4);
      BSP_LED_Toggle(LED5);

      osDelay(200);
    }

    /* Turn off LED3 */
    BSP_LED_Off(LED4);
    BSP_LED_On(LED5);

    /* Suspend Thread 1 */
    //osThreadSuspend(NULL);

    count = osKernelSysTick() + 5000;

    /* Toggle LED3 every 400 ms for 5 s */
    while (count >= osKernelSysTick())
    {
      BSP_LED_Toggle(LED4);
      BSP_LED_Toggle(LED5);

      osDelay(100);
    }

    /* Resume Thread 2*/
    //osThreadResume(LEDThread2Handle);

  }
}

/**
  * @brief  Toggle LED4 thread
  * @param  argument not used
  * @retval None
  */
static void LED_Thread2(void const *argument)
{
  uint32_t count;
  (void) argument;

  for (;;)
  {
    count = osKernelSysTick() + 10000;

    /* Toggle LED4 every 500 ms for 10 s */
    while (count >= osKernelSysTick())
    {
      BSP_LED_Toggle(LED3);
//      BSP_LED_Toggle(LED6);

      osDelay(500);
    }

    count = osKernelSysTick() + 10000;

    while (count >= osKernelSysTick())
        {
          BSP_LED_Toggle(LED3);
//          BSP_LED_Toggle(LED6);

          osDelay(250);
        }

    /* Turn off LED4 */
    BSP_LED_Off(LED3);
//    BSP_LED_On(LED6);

    /* Resume Thread 1 */
//    osThreadResume(LEDThread1Handle);

    /* Suspend Thread 2 */
//    osThreadSuspend(NULL);
  }
}

static void SDCardInitializationThread(void const *argument)
{
	volatile HAL_SD_ErrorTypedef sdError;
	uint32_t cardStatus;
	uint32_t readBuffer[512];

	FIL fil;
	volatile FRESULT f_err_code;
	static FATFS FATFS_Obj;

	hsd.Instance = SDIOx;

	while(1)
	{
		cardStatus = 0;
		sdError = HAL_SD_Init(&hsd, &sdCardinfo);

		if(sdError == SD_OK)
			sdError = HAL_SD_SendSDStatus(&hsd,&cardStatus);

		if(sdError == SD_OK)
			sdError = HAL_SD_ReadBlocks(&hsd,readBuffer,(uint64_t)0x10,(uint32_t)512,(uint32_t)1);

		SDIOx_CLK_DISABLE();
		osDelay(1);

//		f_err_code = f_mount(&FATFS_Obj,"SD:",1);
//		/* Try to mount card */
//			if (f_err_code == FR_OK) {
//				/* Try to open file */
//				if ((f_err_code = f_open(&fil, "SD:first_file.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE)) == FR_OK) {
//					/* Read SDCARD size */
//
//					/* Format string */
//					sprintf(buffer, "Total card size: kaki kBytes\n");
//
//					/* Write total card size to file */
//					f_puts(buffer, &fil);
//
//
//					/* Write free card size to file */
//					f_puts(buffer, &fil);
//
//					/* Close file */
//					f_close(&fil);
//
//					/* Turn led ON */
//					BSP_LED_On(LED6);
//				}
//
//				/* Unmount SDCARD */
//				f_mount(NULL, "SD:", 1);
//			}
		osDelay(2000);
	}
}

static void ADCReadThread(void const *argument)
{
	volatile int16_t sample;
	while(1)
	{
		HAL_ADC_Start(&hadc);
		HAL_ADC_PollForConversion(&hadc, 1000);
		sample = HAL_ADC_GetValue(&hadc);
		HAL_ADC_Stop(&hadc);
		if(sample > 2000)
			BSP_LED_On(LED6);
		else
			BSP_LED_Off(LED6);
		UARTSend(sample, 4);
		osDelay(1000);
	}
}

static void UARTThread(void const *argument)
{
	volatile int8_t temp[256];
	int8_t i = 0;

	while(1)
	{
		for(i=0;i<10;i++)
			temp[i] = 0;
		i = 0;
//		BSP_LED_On(LED6);
//		while(temp[i-1] != '\r')
//		{
//			HAL_UART_Receive(&huart, &temp[i], 1, 10000);
//			if(temp[i] != 0)
//				i++;
//		}
//		BSP_LED_Off(LED6);
//		i = 0;
//		while(temp[i] != '\r')
//		{
//			HAL_UART_Transmit(&huart, &temp[i], 1, 10000);
//			i++;
//		}
//
//		HAL_UART_Transmit(&huart, newline, 2, 10000);
//
//		BSP_LED_On(LED6);
		osDelay(100);
	}
}

static void IndicatorLEDThread(void const *argument)
{
	uint8_t i,j;
	while(1)
	{
		for(i=0;i<10;i++)
		{
			for(j=0;j<10;j++)
			{
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_SET);
				osDelay(i);
				HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_RESET);
				osDelay(10-i);
			}
		}
	}
}

/**
 * @brief Convert raw numeric data to ascii, then send the data on UART
 */
static void UARTSend(int16_t data, int32_t length)
{
	int8_t converted_data[length];

	itoa(data, converted_data, 10);

	HAL_UART_Transmit(&huart, converted_data, length, 1000);
	HAL_UART_Transmit(&huart, newline, 2, 10000);
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;
  
  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();
  
  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 288;//336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 6;//7;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  
  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported  */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}
/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while(1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
