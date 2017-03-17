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
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "fatfs.h"

/* USER CODE BEGIN Includes */
// ToDo: Szemaforral ind�tani az egyes m�r�seket, ezzel tehermentes�teni a processzort.
#include "measure_config.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
osThreadId startMeasureTaskHandle;

#if defined(MEAS_TASK_SWITCHING_TIME) || defined(MEAS_PREEMPTION_TIME) || defined(MEAS_SEMAPHORE_SHUFFLING_TIME) || defined(MEAS_DEADLOCK_BREAKING_TIME) || defined(MEAS_DATAGRAM_THROUGHPUT_TIME)
osSemaphoreId measureSemaphoreA_xSemaphore = NULL;
osSemaphoreId measureSemaphoreB_xSemaphore = NULL;
osSemaphoreId measureSemaphoreC_xSemaphore = NULL;
#endif

#if defined(MEAS_TASK_SWITCHING_TIME)
osThreadId switchingTimeTaskAHandle;
osThreadId switchingTimeTaskBHandle;
osThreadId switchingTimeTaskCHandle;
#endif

#if defined(MEAS_PREEMPTION_TIME)
osThreadId preemptionTimeTaskAHandle;
osThreadId preemptionTimeTaskBHandle;
osThreadId preemptionTimeTaskCHandle;
#endif

#if defined(MEAS_INTERRUPT_LATENCY_TIME)
osThreadId interruptLatencyTimeTaskHandle;
osSemaphoreId interruptLatency_xSemaphore = NULL;
#endif

#if defined(MEAS_SEMAPHORE_SHUFFLING_TIME)
osThreadId semaphoreShufflingTimeTaskAHandle;
osThreadId semaphoreShufflingTimeTaskBHandle;
osSemaphoreId semaphoreShuffling_xSemaphore = NULL;
#endif

#if defined(MEAS_DEADLOCK_BREAKING_TIME)
osThreadId deadlockBreakingTimeTaskAHandle;
osThreadId deadlockBreakingTimeTaskBHandle;
osThreadId deadlockBreakingTimeTaskCHandle;
osMutexId deadlockBreaking_xMutex = NULL;
#endif

#if defined(MEAS_DATAGRAM_THROUGHPUT_TIME)
osThreadId datagramThroughputTimeTaskAHandle;
osThreadId datagramThroughputTimeTaskBHandle;
osSemaphoreId datagramThroughput_xSemaphore = NULL;
osMessageQId datagramThrougput_xMessage = NULL;
#endif

#if defined(BLINKING_LED)
osThreadId forwardLEDTaskHandle;
osThreadId reverseLEDTaskHandle;
#endif

#if defined(MEAS_W_LOAD)
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

I2C_HandleTypeDef hi2c1;

SD_HandleTypeDef hsd;
HAL_SD_CardInfoTypedef SDCardInfo;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart6;

osThreadId uart6TaskHandle;
osSemaphoreId uart6_xSemaphore = NULL;
#endif
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);
static void MX_GPIO_Init(void);
#if defined(MEAS_W_LOAD)
static void MX_ADC1_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_ADC2_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
#endif

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void StartStartMeasureTask(void const * argument);

#if defined(MEAS_TASK_SWITCHING_TIME)
void StartSwitchingTimeTaskA(void const * argument);
void StartSwitchingTimeTaskB(void const * argument);
void StartSwitchingTimeTaskC(void const * argument);
#endif

#if defined(MEAS_PREEMPTION_TIME)
void StartPreemptionTimeTaskA(void const * argument);
void StartPreemptionTimeTaskB(void const * argument);
void StartPreemptionTimeTaskC(void const * argument);
#endif

#if defined(MEAS_INTERRUPT_LATENCY_TIME)
void StartInterruptLatencyTimeTask(void const * argument);
#endif

#if defined(MEAS_SEMAPHORE_SHUFFLING_TIME)
void StartSemaphoreShufflingTimeTaskA(void const * argument);
void StartSemaphoreShufflingTimeTaskB(void const * argument);
#endif

#if defined(MEAS_DEADLOCK_BREAKING_TIME)
void StartDeadlockBreakingTimeTaskA(void const * argument);
void StartDeadlockBreakingTimeTaskB(void const * argument);
void StartDeadlockBreakingTimeTaskC(void const * argument);
#endif

#if defined(MEAS_DATAGRAM_THROUGHPUT_TIME)
void StartDatagramThroughputTimeTaskA(void const * argument);
void StartDatagramThroughputTimeTaskB(void const * argument);
#endif

#if defined(BLINKING_LED)
void StartForwardLEDTask(void const * argument);
void StartReverseLEDTask(void const * argument);
#endif

#if defined(MEAS_W_LOAD)
void StartUART6Task(void const * argument);
#endif
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

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
#if defined(MEAS_W_LOAD)
  MX_ADC1_Init();
  MX_SDIO_SD_Init();
  MX_USART6_UART_Init();
  MX_ADC2_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
#endif

  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
#if defined(MEAS_DEADLOCK_BREAKING_TIME)
  osMutexDef(DEADLOCKMUT);
  deadlockBreaking_xMutex = osMutexCreate(osMutex(DEADLOCKMUT));
#endif
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
#if defined(MEAS_TASK_SWITCHING_TIME) || defined(MEAS_PREEMPTION_TIME) || defined(MEAS_SEMAPHORE_SHUFFLING_TIME) || defined(MEAS_DEADLOCK_BREAKING_TIME) || defined(MEAS_DATAGRAM_THROUGHPUT_TIME)
  osSemaphoreDef(MEASSEMA);
  osSemaphoreDef(MEASSEMB);
  osSemaphoreDef(MEASSEMC);
  measureSemaphoreA_xSemaphore = osSemaphoreCreate(osSemaphore(MEASSEMA), 1);
  measureSemaphoreB_xSemaphore = osSemaphoreCreate(osSemaphore(MEASSEMB), 1);
  measureSemaphoreC_xSemaphore = osSemaphoreCreate(osSemaphore(MEASSEMC), 1);
  osSemaphoreWait(measureSemaphoreA_xSemaphore, 0);
  osSemaphoreWait(measureSemaphoreB_xSemaphore, 0);
  osSemaphoreWait(measureSemaphoreC_xSemaphore, 0);
#endif
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  ThreadDef(STARTMEASURETASKID, StartStartMeasureTask, osPriorityNormal, 0, 128);
  startMeasureTaskHandle = osThreadCreate(Thread(STARTMEASURETASKID), NULL);

#if defined(MEAS_TASK_SWITCHING_TIME)
  ThreadDef(SWITCHINGTIMETASKAID, StartSwitchingTimeTaskA, osPriorityNormal, 0, 128);
  ThreadDef(SWITCHINGTIMETASKBID, StartSwitchingTimeTaskB, osPriorityNormal, 0, 128);
  ThreadDef(SWITCHINGTIMETASKCID, StartSwitchingTimeTaskC, osPriorityNormal, 0, 128);
  switchingTimeTaskAHandle = osThreadCreate(Thread(SWITCHINGTIMETASKAID), NULL);
  switchingTimeTaskBHandle = osThreadCreate(Thread(SWITCHINGTIMETASKBID), NULL);
  switchingTimeTaskCHandle = osThreadCreate(Thread(SWITCHINGTIMETASKCID), NULL);
#endif

#if defined(MEAS_PREEMPTION_TIME)
  ThreadDef(PREEMPTIONTIMETASKAID, StartPreemptionTimeTaskA, osPriorityNormal, 0, 128);
  ThreadDef(PREEMPTIONTIMETASKBID, StartPreemptionTimeTaskB, osPriorityAboveNormal, 0, 128);
  ThreadDef(PREEMPTIONTIMETASKCID, StartPreemptionTimeTaskC, osPriorityRealtime, 0, 128);
  preemptionTimeTaskAHandle = osThreadCreate(Thread(PREEMPTIONTIMETASKAID), NULL);
  preemptionTimeTaskBHandle = osThreadCreate(Thread(PREEMPTIONTIMETASKBID), NULL);
  preemptionTimeTaskCHandle = osThreadCreate(Thread(PREEMPTIONTIMETASKCID), NULL);
#endif

#if defined(MEAS_INTERRUPT_LATENCY_TIME)
  ThreadDef(INTERRUPTLATENCYTIMETASKID, StartInterruptLatencyTimeTask, osPriorityRealtime, 0, 128);
  interruptLatencyTimeTaskHandle = osThreadCreate(Thread(INTERRUPTLATENCYTIMETASKID), NULL);
#endif

#if defined(MEAS_SEMAPHORE_SHUFFLING_TIME)
  ThreadDef(SEMAPHORESHUFFLINGTIMETASKAID, StartSemaphoreShufflingTimeTaskA, osPriorityNormal, 0, 128);
  ThreadDef(SEMAPHORESHUFFLINGTIMETASKBID, StartSemaphoreShufflingTimeTaskB, osPriorityAboveNormal, 0, 128);
  semaphoreShufflingTimeTaskAHandle = osThreadCreate(Thread(SEMAPHORESHUFFLINGTIMETASKAID), NULL);
  semaphoreShufflingTimeTaskBHandle = osThreadCreate(Thread(SEMAPHORESHUFFLINGTIMETASKBID), NULL);
#endif

#if defined(MEAS_DEADLOCK_BREAKING_TIME)
  ThreadDef(DEADLOCKBREAKINGTIMETASKAID, StartDeadlockBreakingTimeTaskA, osPriorityNormal, 0, 128);
  ThreadDef(DEADLOCKBREAKINGTIMETASKBID, StartDeadlockBreakingTimeTaskB, osPriorityAboveNormal, 0, 128);
  ThreadDef(DEADLOCKBREAKINGTIMETASKCID, StartDeadlockBreakingTimeTaskC, osPriorityRealtime, 0, 128);
  deadlockBreakingTimeTaskAHandle = osThreadCreate(Thread(DEADLOCKBREAKINGTIMETASKAID), NULL);
  deadlockBreakingTimeTaskBHandle = osThreadCreate(Thread(DEADLOCKBREAKINGTIMETASKBID), NULL);
  deadlockBreakingTimeTaskCHandle = osThreadCreate(Thread(DEADLOCKBREAKINGTIMETASKCID), NULL);
#endif

#if defined(MEAS_DATAGRAM_THROUGHPUT_TIME)
  ThreadDef(DATAGRAMTHROUGHPUTTIMETASKAID, StartDatagramThroughputTimeTaskA, osPriorityNormal, 0, 1280);
  ThreadDef(DATAGRAMTHROUGHPUTTIMETASKBID, StartDatagramThroughputTimeTaskB, osPriorityNormal, 0, 1280);
  datagramThroughputTimeTaskAHandle = osThreadCreate(Thread(DATAGRAMTHROUGHPUTTIMETASKAID), NULL);
  datagramThroughputTimeTaskBHandle = osThreadCreate(Thread(DATAGRAMTHROUGHPUTTIMETASKBID), NULL);
#endif

#if defined(BLINKING_LED)
  ThreadDef(FORWARDLEDTASKID, StartForwardLEDTask, osPriorityLow, 0, 128);
  ThreadDef(REVERSELEDTASKID, StartReverseLEDTask, osPriorityLow, 0, 128);
  forwardLEDTaskHandle = osThreadCreate(Thread(FORWARDLEDTASKID), NULL);
  reverseLEDTaskHandle = osThreadCreate(Thread(REVERSELEDTASKID), NULL);
#endif

#if defined(MEAS_W_LOAD)
  ThreadDef(UART6TASKID, StartUART6Task, osPriorityNormal, 0, 1024);
  uart6TaskHandle = osThreadCreate(Thread(UART6TASKID), NULL);
#endif

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
 

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

#if defined(MEAS_W_LOAD)
/* ADC1 init function */
static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

/* ADC2 init function */
static void MX_ADC2_Init(void)
{

  ADC_ChannelConfTypeDef sConfig;

    /**Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion) 
    */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.ScanConvMode = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DMAContinuousRequests = DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

    /**Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time. 
    */
  sConfig.Channel = ADC_CHANNEL_13;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

}

/* SDIO init function */
static void MX_SDIO_SD_Init(void)
{

  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 0;

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }

}

/* USART6 init function */
static void MX_USART6_UART_Init(void)
{

  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }

}
#endif

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
#if defined(MEAS_W_LOAD)
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
#endif

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

#if defined(MEAS_W_LOAD)
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_12|GPIO_PIN_13 
                          |GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PE2 PE4 PE5 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PD8 PD9 PD12 PD13 
                           PD14 PD15 PD3
                           PD4 PD5 PD6 PD7 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_12|GPIO_PIN_13 
                          |GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PD10 PD11 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
#else
#if defined(MEAS_LATENCY) || defined(MEAS_INTERRUPT_LATENCY_TIME)
  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif

  /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pins : PD3 PD4 PD5 PD6 PD7 */
    GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

#if defined(BLINKING_LED)
  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);
  /*Configure GPIO pins :PD12 PD13 PD14 PD15*/
    GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
#endif
#endif

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
#if defined(MEAS_LATENCY) || defined(MEAS_INTERRUPT_LATENCY_TIME)
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
#if defined(MEAS_LATENCY)
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, HAL_GPIO_ReadPin(GPIOC, GPIO_Pin));
#elif defined(MEAS_INTERRUPT_LATENCY_TIME)
	if(GPIO_Pin == GPIO_PIN_13)
		osSemaphoreRelease(interruptLatency_xSemaphore);
#endif

}
#endif

#if defined(MEAS_TASK_SWITCHING_TIME)
void StartSwitchingTimeTaskA(void const * argument)
{
	int i = 0;

	while(1)
	{
		if(osSemaphoreWait(measureSemaphoreA_xSemaphore, portMAX_DELAY) == osOK)
		{
			osSemaphoreRelease(measureSemaphoreB_xSemaphore);
			for(i=0;i<MEDIUM_PRIORITY_TASK_LOOP_DELAY/2;i++){}
		}
	}
}

void StartSwitchingTimeTaskB(void const * argument)
{
	int i = 0;

	while(1)
	{
		if(osSemaphoreWait(measureSemaphoreB_xSemaphore, portMAX_DELAY) == osOK)
		{
			osSemaphoreRelease(measureSemaphoreC_xSemaphore);
			for(i=0;i<MEDIUM_PRIORITY_TASK_LOOP_DELAY/2;i++){}
		}
	}
}

void StartSwitchingTimeTaskC(void const * argument)
{
	int i = 0;

	while(1)
	{
		if(osSemaphoreWait(measureSemaphoreC_xSemaphore, portMAX_DELAY) == osOK)
		{
			for(i=0;i<MEDIUM_PRIORITY_TASK_LOOP_DELAY/2;i++){}
		}
	}
}
#endif

#if defined(MEAS_PREEMPTION_TIME)
void StartPreemptionTimeTaskA(void const * argument)
{
	int i = 0;
	while(1)
	{
		if(osSemaphoreWait(measureSemaphoreA_xSemaphore, portMAX_DELAY) == osOK)
		{
			osSemaphoreRelease(measureSemaphoreB_xSemaphore);
			for(i=0;i<LOW_PRIORITY_TASK_LOOP_DELAY;i++){}
		}
	}
}

void StartPreemptionTimeTaskB(void const * argument)
{
	int i = 0;

	while(1)
	{
		if(osSemaphoreWait(measureSemaphoreB_xSemaphore, portMAX_DELAY) == osOK)
		{
			osSemaphoreRelease(measureSemaphoreC_xSemaphore);
			osDelay(11);
			for(i=0;i<MEDIUM_PRIORITY_TASK_LOOP_DELAY;i++){}
		}
	}
}

void StartPreemptionTimeTaskC(void const * argument)
{
	int i = 0;

	while(1)
	{
		if(osSemaphoreWait(measureSemaphoreC_xSemaphore, portMAX_DELAY) == osOK)
		{
			osDelay(23);
			for(i=0;i<HIGH_PRIORITY_TASK_LOOP_DELAY;i++){}
		}
	}
}
#endif

#if defined(MEAS_INTERRUPT_LATENCY_TIME)
void StartInterruptLatencyTimeTask(void const * argument)
{
	osSemaphoreDef(INTSEM);
	interruptLatency_xSemaphore = osSemaphoreCreate(osSemaphore(INTSEM), 1);

	while(1)
	{
		if(osSemaphoreWait(interruptLatency_xSemaphore, portMAX_DELAY) == osOK)
		{
			__asm volatile
			(
			"	push {r0, r1, r2}								\n"
			"   movw r0, #0x0810								\n"/* GPIOC c�m�nek bet�lt�se */
			"	movt r0, #0x4002								\n"
			"   ldr r1, [r0, #0]								\n"/* PORTC �rt�k�nek bet�lt�se */
			" 	lsr r1, r1, #10									\n"/* Bit helyre mozgat�sa */
			" 	and r1, r1, #0x00000008							\n"/* Maszkol�sok */
			"   movw r0, #0x0C14								\n"/* GPIOD c�m�nek bet�lt�se */
			"	movt r0, #0x4002								\n"
			" 	ldr r2, [r0, #0]								\n"
			" 	and r2, r2, #0xFFFFFFF7							\n"
			" 	orr r2, r2, r1									\n"
			" 	str r2, [r0, #0]								\n"/* �rt�k kihelyez�se a l�bra */
			"	pop {r0, r1, r2}								\n"
			);
		}
	}
}
#endif

#if defined(MEAS_SEMAPHORE_SHUFFLING_TIME)
void StartSemaphoreShufflingTimeTaskA(void const * argument)
{
	int i = 0;

	osSemaphoreDef(SEMSHUFSEM);
	semaphoreShuffling_xSemaphore = osSemaphoreCreate(osSemaphore(SEMSHUFSEM), 1);

	while(1)
	{
		if(osSemaphoreWait(measureSemaphoreA_xSemaphore, portMAX_DELAY) == osOK)
		{
			osSemaphoreRelease(measureSemaphoreB_xSemaphore);
			if(osSemaphoreWait(semaphoreShuffling_xSemaphore, portMAX_DELAY) == osOK)
			{
				for(i=0;i<MEDIUM_PRIORITY_TASK_LOOP_DELAY;i++){}
				osSemaphoreRelease(semaphoreShuffling_xSemaphore);
			}
		}
	}
}

void StartSemaphoreShufflingTimeTaskB(void const * argument)
{
	while(1)
	{
		if(osSemaphoreWait(measureSemaphoreB_xSemaphore, portMAX_DELAY) == osOK)
		{
			osDelay(10);
			if(osSemaphoreWait(semaphoreShuffling_xSemaphore, portMAX_DELAY) == osOK)
			{
				osSemaphoreRelease(semaphoreShuffling_xSemaphore);
			}
		}
	}
}
#endif

#if defined(MEAS_DEADLOCK_BREAKING_TIME)
void StartDeadlockBreakingTimeTaskA(void const * argument)
{
	int i = 0;

	while(1)
	{
		if(osSemaphoreWait(measureSemaphoreA_xSemaphore, portMAX_DELAY) == osOK)
		{
			osSemaphoreRelease(measureSemaphoreB_xSemaphore);
			if(osMutexWait(deadlockBreaking_xMutex, portMAX_DELAY) == osOK)
			{
				for(i=0;i<LOW_PRIORITY_TASK_LOOP_DELAY;i++){}
				osMutexRelease(deadlockBreaking_xMutex);
			}
		}
	}
}

void StartDeadlockBreakingTimeTaskB(void const * argument)
{
	int i = 0;

	while(1)
	{
		if(osSemaphoreWait(measureSemaphoreB_xSemaphore, portMAX_DELAY) == osOK)
		{
			osSemaphoreRelease(measureSemaphoreC_xSemaphore);
			osDelay(11);
			for(i=0;i<MEDIUM_PRIORITY_TASK_LOOP_DELAY;i++){}
		}
	}
}

void StartDeadlockBreakingTimeTaskC(void const * argument)
{
	while(1)
	{
		if(osSemaphoreWait(measureSemaphoreC_xSemaphore, portMAX_DELAY) == osOK)
		{
			osDelay(23);
			if(osMutexWait(deadlockBreaking_xMutex, portMAX_DELAY) == osOK)
			{
				osMutexRelease(deadlockBreaking_xMutex);
			}
		}
	}
}
#endif

#if defined(MEAS_DATAGRAM_THROUGHPUT_TIME)
void StartDatagramThroughputTimeTaskA(void const * argument)
{
	osSemaphoreDef(DATAGRAMSEM);
	datagramThroughput_xSemaphore = osSemaphoreCreate(osSemaphore(DATAGRAMSEM), 1);
	osMessageQDef(DATAGRAMMES, 128, uint32_t);
	datagramThrougput_xMessage = osMessageCreate(osMessageQ(DATAGRAMMES), NULL);

	uint32_t i = 0;
	uint32_t temp;
	osEvent event;

	while(1)
	{
		if(osSemaphoreWait(datagramThroughput_xSemaphore, portMAX_DELAY) == osOK)
		{
			if(osSemaphoreWait(measureSemaphoreA_xSemaphore, portMAX_DELAY) == osOK)
			{
				osSemaphoreRelease(measureSemaphoreB_xSemaphore);
				for(i=0;i<128;i++)
				{
					event = osMessageGet(datagramThrougput_xMessage, portMAX_DELAY);
					if(event.status == osEventMessage)
					{
						temp = (uint32_t)event.value.p;
						if(temp != i)
							return;
						// L�b �ll�t�sa
					}
				}
				osSemaphoreRelease(datagramThroughput_xSemaphore);
				portYIELD();
			}
		}
	}
}

void StartDatagramThroughputTimeTaskB(void const * argument)
{
	uint32_t i = 0;
	while(1)
	{
		if(osSemaphoreWait(measureSemaphoreB_xSemaphore, portMAX_DELAY) == osOK)
		{
			// OUT l�b magasba �ll�t�sa
			__asm volatile
			(
			"	push {r0, r1, r2}								\n"
			"   movw r0, #0x0C14								\n" /* GPIOD c�m�nek bet�lt�se */
			"	movt r0, #0x4002								\n"
			" 	ldr r2,[r0, #0]									\n"
			" 	orr r2, r2, #0x00000008							\n"
			" 	str r2, [r0, #0]								\n" /* Null �rt�kek ki�r�sa a l�bakra */
			"	pop {r0, r1, r2}								\n"
			);
			for(i=0;i<128;i++)
			{
				osMessagePut(datagramThrougput_xMessage, i, portMAX_DELAY);
			}
			if(osSemaphoreWait(datagramThroughput_xSemaphore, portMAX_DELAY) == osOK)
				osSemaphoreRelease(datagramThroughput_xSemaphore);
			// OUT l�b alacsonyba �ll�t�sa
			__asm volatile
			(
			"	push {r0, r1, r2}								\n"
			"   movw r0, #0x0C14								\n" /* GPIOD c�m�nek bet�lt�se */
			"	movt r0, #0x4002								\n"
			" 	ldr r2,[r0, #0]									\n"
			" 	and r2, r2, #0xFFFFFFF7							\n"
			" 	str r2, [r0, #0]								\n" /* Null �rt�kek ki�r�sa a l�bakra */
			"	pop {r0, r1, r2}								\n"
			);
		}
	}
}

#endif

#if defined(BLINKING_LED)
void StartForwardLEDTask(void const * argument)
{
	while(1)
	  {
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
		osDelay(400);
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
		osDelay(500);
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
		osDelay(600);
	  }
}

void StartReverseLEDTask(void const * argument)
{
	while(1)
	  {
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
		osDelay(100);
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
		osDelay(200);
		HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
		osDelay(300);
	  }
}
#endif

#if defined(MEAS_W_LOAD)
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if (UartHandle->Instance == USART6)
		osSemaphoreRelease(uart6_xSemaphore);
}

void StartUART6Task(void const * argument)
{
	uint8_t data[1];
	uint16_t size = sizeof(data)/sizeof(uint8_t);

	osSemaphoreDef(SEM);
	uart6_xSemaphore = osSemaphoreCreate(osSemaphore(SEM), 1);

	osSemaphoreWait(uart6_xSemaphore, 0);
	while(1)
	{
		HAL_UART_Receive_IT(&huart6, data, size);
		if(osSemaphoreWait(uart6_xSemaphore, portMAX_DELAY) == osOK)
		{
			HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
			HAL_UART_Transmit(&huart6, data, size, 100);
		}
		else
		{
			HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
		}
	}
}
#endif
/* USER CODE END 4 */

/* StartStartMeasureTask function */
void StartStartMeasureTask(void const * argument)
{
  /* init code for FATFS */
#if defined(MEAS_W_LOAD)
  MX_FATFS_Init();
#endif

  /* USER CODE BEGIN 5 */

  /* Infinite loop */
  while(1)
  {
#if defined(MEAS_TASK_SWITCHING_TIME) || defined(MEAS_PREEMPTION_TIME) || defined(MEAS_SEMAPHORE_SHUFFLING_TIME) || defined(MEAS_DEADLOCK_BREAKING_TIME) || defined(MEAS_DATAGRAM_THROUGHPUT_TIME)
	  osSemaphoreRelease(measureSemaphoreA_xSemaphore);
#endif

    osDelay(100);
  }
  /* USER CODE END 5 */ 
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
/* USER CODE BEGIN Callback 0 */

/* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
/* USER CODE BEGIN Callback 1 */

/* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
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
