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
// ToDo: Pár függvényt célszerû lenne külön fájlokba átrakni.

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "fatfs.h"

/* USER CODE BEGIN Includes */
#include <string.h>
#include "measure_config.h"
#include "cmd_def.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
osThreadId startMeasureTaskHandle;

#if defined(MEAS_TASK_SWITCHING_TIME) || defined(MEAS_PREEMPTION_TIME) || \
defined(MEAS_SEMAPHORE_SHUFFLING_TIME) || defined(MEAS_DEADLOCK_BREAKING_TIME) || defined(MEAS_DATAGRAM_THROUGHPUT_TIME)
osSemaphoreId measureSemaphoreA_xSemaphore = NULL;
osSemaphoreId measureSemaphoreB_xSemaphore = NULL;
osSemaphoreId measureSemaphoreC_xSemaphore = NULL;
#endif // defined(MEAS_TASK_SWITCHING_TIME) || defined(MEAS_PREEMPTION_TIME)
//|| defined(MEAS_SEMAPHORE_SHUFFLING_TIME) || defined(MEAS_DEADLOCK_BREAKING_TIME) || defined(MEAS_DATAGRAM_THROUGHPUT_TIME)

#if defined(MEAS_TASK_SWITCHING_TIME)
osThreadId switchingTimeTaskAHandle;
osThreadId switchingTimeTaskBHandle;
osThreadId switchingTimeTaskCHandle;
#endif // defined(MEAS_TASK_SWITCHING_TIME)

#if defined(MEAS_PREEMPTION_TIME)
osThreadId preemptionTimeTaskAHandle;
osThreadId preemptionTimeTaskBHandle;
osThreadId preemptionTimeTaskCHandle;
#endif

#if defined(MEAS_INTERRUPT_LATENCY_TIME)
osThreadId interruptLatencyTimeTaskHandle;
osSemaphoreId interruptLatency_xSemaphore = NULL;
#endif // defined(MEAS_INTERRUPT_LATENCY_TIME)

#if defined(MEAS_SEMAPHORE_SHUFFLING_TIME)
osThreadId semaphoreShufflingTimeTaskAHandle;
osThreadId semaphoreShufflingTimeTaskBHandle;
osSemaphoreId semaphoreShuffling_xSemaphore = NULL;
#endif // defined(MEAS_SEMAPHORE_SHUFFLING_TIME)

#if defined(MEAS_DEADLOCK_BREAKING_TIME)
osThreadId deadlockBreakingTimeTaskAHandle;
osThreadId deadlockBreakingTimeTaskBHandle;
osThreadId deadlockBreakingTimeTaskCHandle;
osMutexId deadlockBreaking_xMutex = NULL;
#endif // defined(MEAS_DEADLOCK_BREAKING_TIME)

#if defined(MEAS_DATAGRAM_THROUGHPUT_TIME)
osThreadId datagramThroughputTimeTaskAHandle;
osThreadId datagramThroughputTimeTaskBHandle;
osSemaphoreId datagramThroughput_xSemaphore = NULL;
osMessageQId datagramThrougput_xMessage = NULL;
#endif // defined(MEAS_DATAGRAM_THROUGHPUT_TIME)

#if defined(BLINKING_LED)
osThreadId forwardLEDTaskHandle;
osThreadId reverseLEDTaskHandle;
#endif // defined(BLINKING_LED)

#if defined(MEAS_W_LOAD)
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

SD_HandleTypeDef hsd;
HAL_SD_CardInfoTypedef SDCardInfo;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart6;

osThreadId switchChangedTaskHandle;
osMessageQId switchInterrupt_xMessage = NULL;

osThreadId tempMeasureTaskHandle;
osThreadId potmeterMeasureTaskHandle;

osThreadId bleTaskHandle;
osThreadId bleSendTaskHandle;
osThreadId bleReceiveTaskHandle;
osThreadId uart6TaskHandle;
osThreadId uart6SendTaskHandle;
osSemaphoreId bleEvent_xSemaphore = NULL;
osMessageQId bleSend_xMessage = NULL;
osMessageQId bleReceive_xMessage = NULL;
osMessageQId uart6Send_xMessage = NULL;
osMessageQId uart6Receive_xMessage = NULL;
uint8_t bleData[1];
uint8_t bleConnectionHndl;
uint8_t uart6Data[1];

osThreadId sdCardTaskHandle;
osMessageQId sdCardWrite_xMessage = NULL;
#endif // defined(MEAS_W_LOAD)
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
static void MX_USART1_UART_Init(void);
#endif // defined(MEAS_W_LOAD)

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void StartStartMeasureTask(void const * argument);

#if defined(MEAS_TASK_SWITCHING_TIME)
void StartSwitchingTimeTaskA(void const * argument);
void StartSwitchingTimeTaskB(void const * argument);
void StartSwitchingTimeTaskC(void const * argument);
#endif // defined(MEAS_TASK_SWITCHING_TIME)

#if defined(MEAS_PREEMPTION_TIME)
void StartPreemptionTimeTaskA(void const * argument);
void StartPreemptionTimeTaskB(void const * argument);
void StartPreemptionTimeTaskC(void const * argument);
#endif // defined(MEAS_PREEMPTION_TIME)

#if defined(MEAS_INTERRUPT_LATENCY_TIME)
void StartInterruptLatencyTimeTask(void const * argument);
#endif // defined(MEAS_INTERRUPT_LATENCY_TIME)

#if defined(MEAS_SEMAPHORE_SHUFFLING_TIME)
void StartSemaphoreShufflingTimeTaskA(void const * argument);
void StartSemaphoreShufflingTimeTaskB(void const * argument);
#endif // defined(MEAS_SEMAPHORE_SHUFFLING_TIME)

#if defined(MEAS_DEADLOCK_BREAKING_TIME)
void StartDeadlockBreakingTimeTaskA(void const * argument);
void StartDeadlockBreakingTimeTaskB(void const * argument);
void StartDeadlockBreakingTimeTaskC(void const * argument);
#endif // defined(MEAS_DEADLOCK_BREAKING_TIME)

#if defined(MEAS_DATAGRAM_THROUGHPUT_TIME)
void StartDatagramThroughputTimeTaskA(void const * argument);
void StartDatagramThroughputTimeTaskB(void const * argument);
#endif // defined(MEAS_DATAGRAM_THROUGHPUT_TIME)

#if defined(BLINKING_LED)
void StartForwardLEDTask(void const * argument);
void StartReverseLEDTask(void const * argument);
#endif // defined(BLINKING_LED)

#if defined(MEAS_W_LOAD)
void StartSwitchChangedTask(void const * argument);

void StartTempMeasureTask(void const * argument);
void StartPotmeterMeasureTask(void const * argument);

void StartBLETask(void const * argument);
void StartBLESendTask(void const * argument);
void StartBLEReceiveTask(void const * argument);

void StartUART6Task(void const * argument);
void StartUART6SendTask(void const * argument);

void StartSDCardTask(void const * argument);
#endif // defined(MEAS_W_LOAD)
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
  MX_USART1_UART_Init();
#endif // defined(MEAS_W_LOAD)

  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
#if defined(MEAS_DEADLOCK_BREAKING_TIME)
  osMutexDef(DEADLOCKMUT);
  deadlockBreaking_xMutex = osMutexCreate(osMutex(DEADLOCKMUT));
#endif // defined(MEAS_DEADLOCK_BREAKING_TIME)
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
#if defined(MEAS_TASK_SWITCHING_TIME) || defined(MEAS_PREEMPTION_TIME) || \
	defined(MEAS_SEMAPHORE_SHUFFLING_TIME) || defined(MEAS_DEADLOCK_BREAKING_TIME) || defined(MEAS_DATAGRAM_THROUGHPUT_TIME)
  osSemaphoreDef(MEASSEMA);
  osSemaphoreDef(MEASSEMB);
  osSemaphoreDef(MEASSEMC);
  measureSemaphoreA_xSemaphore = osSemaphoreCreate(osSemaphore(MEASSEMA), 1);
  measureSemaphoreB_xSemaphore = osSemaphoreCreate(osSemaphore(MEASSEMB), 1);
  measureSemaphoreC_xSemaphore = osSemaphoreCreate(osSemaphore(MEASSEMC), 1);
  osSemaphoreWait(measureSemaphoreA_xSemaphore, 0);
  osSemaphoreWait(measureSemaphoreB_xSemaphore, 0);
  osSemaphoreWait(measureSemaphoreC_xSemaphore, 0);
#endif // defined(MEAS_TASK_SWITCHING_TIME) || defined(MEAS_PREEMPTION_TIME)
//|| defined(MEAS_SEMAPHORE_SHUFFLING_TIME) || defined(MEAS_DEADLOCK_BREAKING_TIME) || defined(MEAS_DATAGRAM_THROUGHPUT_TIME)
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
#endif // defined(MEAS_TASK_SWITCHING_TIME)

#if defined(MEAS_PREEMPTION_TIME)
  ThreadDef(PREEMPTIONTIMETASKAID, StartPreemptionTimeTaskA, osPriorityNormal, 0, 128);
  ThreadDef(PREEMPTIONTIMETASKBID, StartPreemptionTimeTaskB, osPriorityAboveNormal, 0, 128);
  ThreadDef(PREEMPTIONTIMETASKCID, StartPreemptionTimeTaskC, osPriorityRealtime, 0, 128);
  preemptionTimeTaskAHandle = osThreadCreate(Thread(PREEMPTIONTIMETASKAID), NULL);
  preemptionTimeTaskBHandle = osThreadCreate(Thread(PREEMPTIONTIMETASKBID), NULL);
  preemptionTimeTaskCHandle = osThreadCreate(Thread(PREEMPTIONTIMETASKCID), NULL);
#endif // defined(MEAS_PREEMPTION_TIME)

#if defined(MEAS_INTERRUPT_LATENCY_TIME)
  ThreadDef(INTERRUPTLATENCYTIMETASKID, StartInterruptLatencyTimeTask, osPriorityRealtime, 0, 128);
  interruptLatencyTimeTaskHandle = osThreadCreate(Thread(INTERRUPTLATENCYTIMETASKID), NULL);
#endif // defined(MEAS_INTERRUPT_LATENCY_TIME)

#if defined(MEAS_SEMAPHORE_SHUFFLING_TIME)
  ThreadDef(SEMAPHORESHUFFLINGTIMETASKAID, StartSemaphoreShufflingTimeTaskA, osPriorityNormal, 0, 128);
  ThreadDef(SEMAPHORESHUFFLINGTIMETASKBID, StartSemaphoreShufflingTimeTaskB, osPriorityAboveNormal, 0, 128);
  semaphoreShufflingTimeTaskAHandle = osThreadCreate(Thread(SEMAPHORESHUFFLINGTIMETASKAID), NULL);
  semaphoreShufflingTimeTaskBHandle = osThreadCreate(Thread(SEMAPHORESHUFFLINGTIMETASKBID), NULL);
#endif // defined(MEAS_SEMAPHORE_SHUFFLING_TIME)

#if defined(MEAS_DEADLOCK_BREAKING_TIME)
  ThreadDef(DEADLOCKBREAKINGTIMETASKAID, StartDeadlockBreakingTimeTaskA, osPriorityNormal, 0, 128);
  ThreadDef(DEADLOCKBREAKINGTIMETASKBID, StartDeadlockBreakingTimeTaskB, osPriorityAboveNormal, 0, 128);
  ThreadDef(DEADLOCKBREAKINGTIMETASKCID, StartDeadlockBreakingTimeTaskC, osPriorityRealtime, 0, 128);
  deadlockBreakingTimeTaskAHandle = osThreadCreate(Thread(DEADLOCKBREAKINGTIMETASKAID), NULL);
  deadlockBreakingTimeTaskBHandle = osThreadCreate(Thread(DEADLOCKBREAKINGTIMETASKBID), NULL);
  deadlockBreakingTimeTaskCHandle = osThreadCreate(Thread(DEADLOCKBREAKINGTIMETASKCID), NULL);
#endif // defined(MEAS_DEADLOCK_BREAKING_TIME)

#if defined(MEAS_DATAGRAM_THROUGHPUT_TIME)
  ThreadDef(DATAGRAMTHROUGHPUTTIMETASKAID, StartDatagramThroughputTimeTaskA, osPriorityNormal, 0, 128);
  ThreadDef(DATAGRAMTHROUGHPUTTIMETASKBID, StartDatagramThroughputTimeTaskB, osPriorityNormal, 0, 128);
  datagramThroughputTimeTaskAHandle = osThreadCreate(Thread(DATAGRAMTHROUGHPUTTIMETASKAID), NULL);
  datagramThroughputTimeTaskBHandle = osThreadCreate(Thread(DATAGRAMTHROUGHPUTTIMETASKBID), NULL);
#endif // defined(MEAS_DATAGRAM_THROUGHPUT_TIME)

#if defined(BLINKING_LED)
  ThreadDef(FORWARDLEDTASKID, StartForwardLEDTask, osPriorityLow, 0, 128);
  ThreadDef(REVERSELEDTASKID, StartReverseLEDTask, osPriorityLow, 0, 128);
  forwardLEDTaskHandle = osThreadCreate(Thread(FORWARDLEDTASKID), NULL);
  reverseLEDTaskHandle = osThreadCreate(Thread(REVERSELEDTASKID), NULL);
#endif // defined(BLINKING_LED)

#if defined(MEAS_W_LOAD)
  ThreadDef(SWITCHCHANGEDTASKID, StartSwitchChangedTask, osPriorityNormal, 0, 128);
  ThreadDef(TEMPMEASTASKID, StartTempMeasureTask, osPriorityNormal, 0, 128);
  ThreadDef(POTMETERMEASTASKID, StartPotmeterMeasureTask, osPriorityNormal, 0, 128);
  ThreadDef(BLETASKID, StartBLETask, osPriorityNormal, 0, 1024);
  ThreadDef(UART6TASKID, StartUART6Task, osPriorityNormal, 0, 128);
  ThreadDef(SDCARDTASKID, StartSDCardTask, osPriorityNormal, 0, 1280);
  switchChangedTaskHandle = osThreadCreate(Thread(SWITCHCHANGEDTASKID), NULL);
  tempMeasureTaskHandle = osThreadCreate(Thread(TEMPMEASTASKID), NULL);
  potmeterMeasureTaskHandle = osThreadCreate(Thread(POTMETERMEASTASKID), NULL);
  bleTaskHandle = osThreadCreate(Thread(BLETASKID), NULL);
  uart6TaskHandle = osThreadCreate(Thread(UART6TASKID), NULL);
  sdCardTaskHandle = osThreadCreate(Thread(SDCARDTASKID), NULL);
#endif // defined(MEAS_W_LOAD)
  // ToDo: A taszkoknak szükséges stackméretet végigbogarászni.

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
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
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
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
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
  hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
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
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
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
  hsd.Init.ClockDiv = 1;

}

/* USART1 init function */
static void MX_USART1_UART_Init(void)
{

  huart1.Instance = USART1;
  huart1.Init.BaudRate = BLEBAUDRATE;
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
  huart6.Init.BaudRate = UART6BAUDRATE;
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
#endif // defined(MEAS_W_LOAD)

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
#endif // defined(MEAS_W_LOAD)

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

#if defined(MEAS_LATENCY) || defined(MEAS_INTERRUPT_LATENCY_TIME)
  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif // defined(MEAS_LATENCY) || defined(MEAS_INTERRUPT_LATENCY_TIME)

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
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
#else // defined(MEAS_W_LOAD)

#if defined(MEAS_LATENCY) || defined(MEAS_INTERRUPT_LATENCY_TIME)
  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
#endif // defined(MEAS_LATENCY) || defined(MEAS_INTERRUPT_LATENCY_TIME)

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
#endif // defined(BLINKING_LED)
#endif // defined(MEAS_W_LOAD)

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
#if defined(MEAS_LATENCY) || defined(MEAS_INTERRUPT_LATENCY_TIME) || defined(MEAS_W_LOAD)
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
#if defined(MEAS_W_LOAD)
#if defined(MEAS_LATENCY) || defined(MEAS_INTERRUPT_LATENCY_TIME)
	if(GPIO_Pin == GPIO_PIN_13)
	{
#if defined(MEAS_LATENCY)
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, HAL_GPIO_ReadPin(GPIOC, GPIO_Pin));
#elif defined(MEAS_INTERRUPT_LATENCY_TIME)
		osSemaphoreRelease(interruptLatency_xSemaphore);
#endif // defined(MEAS_LATENCY)
	}
	else
	{
		osMessagePut(switchInterrupt_xMessage, (uint32_t)GPIO_Pin, 0);
	}
#else
	osMessagePut(switchInterrupt_xMessage, (uint32_t)GPIO_Pin, 0);
#endif // defined(MEAS_LATENCY) || defined(MEAS_INTERRUPT_LATENCY_TIME)
#elif defined(MEAS_LATENCY)
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, HAL_GPIO_ReadPin(GPIOC, GPIO_Pin));
#elif defined(MEAS_INTERRUPT_LATENCY_TIME)
	osSemaphoreRelease(interruptLatency_xSemaphore);
#endif // defined(MEAS_W_LOAD)
}
#endif // defined(MEAS_LATENCY) || defined(MEAS_INTERRUPT_LATENCY_TIME) || defined(MEAS_W_LOAD)

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
#endif // defined(MEAS_TASK_SWITCHING_TIME)

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
#endif // defined(MEAS_PREEMPTION_TIME)

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
			"   movw r0, #0x0810								\n"/* GPIOC címének betöltése */
			"	movt r0, #0x4002								\n"
			"   ldr r1, [r0, #0]								\n"/* PORTC értékének betöltése */
			" 	lsr r1, r1, #10									\n"/* Bit helyre mozgatása */
			" 	and r1, r1, #0x00000008							\n"/* Maszkolások */
			"   movw r0, #0x0C14								\n"/* GPIOD címének betöltése */
			"	movt r0, #0x4002								\n"
			" 	ldr r2, [r0, #0]								\n"
			" 	and r2, r2, #0xFFFFFFF7							\n"
			" 	orr r2, r2, r1									\n"
			" 	str r2, [r0, #0]								\n"/* Érték kihelyezése a lábra */
			"	pop {r0, r1, r2}								\n"
			);
		}
	}
}
#endif // defined(MEAS_INTERRUPT_LATENCY_TIME)

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
#endif // defined(MEAS_SEMAPHORE_SHUFFLING_TIME)

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
#endif // defined(MEAS_DEADLOCK_BREAKING_TIME)

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
			// OUT láb magasba állítása
			__asm volatile
			(
			"	push {r0, r1, r2}								\n"
			"   movw r0, #0x0C14								\n" /* GPIOD címének betöltése */
			"	movt r0, #0x4002								\n"
			" 	ldr r2,[r0, #0]									\n"
			" 	orr r2, r2, #0x00000008							\n"
			" 	str r2, [r0, #0]								\n" /* Magas érték kiírása az OUT lábra */
			"	pop {r0, r1, r2}								\n"
			);
			for(i=0;i<128;i++)
			{
				osMessagePut(datagramThrougput_xMessage, i, portMAX_DELAY);
			}
			if(osSemaphoreWait(datagramThroughput_xSemaphore, portMAX_DELAY) == osOK)
				osSemaphoreRelease(datagramThroughput_xSemaphore);
			// OUT láb alacsonyba állítása
			__asm volatile
			(
			"	push {r0, r1, r2}								\n"
			"   movw r0, #0x0C14								\n" /* GPIOD címének betöltése */
			"	movt r0, #0x4002								\n"
			" 	ldr r2,[r0, #0]									\n"
			" 	and r2, r2, #0xFFFFFFF7							\n"
			" 	str r2, [r0, #0]								\n" /* Alacsony érték kiírása az OUT lábra */
			"	pop {r0, r1, r2}								\n"
			);
		}
	}
}
#endif // defined(MEAS_DATAGRAM_THROUGHPUT_TIME)

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
#endif // defined(BLINKING_LED)

#if defined(MEAS_W_LOAD)
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if(UartHandle->Instance == USART6)
	{
		// Az adat karakterenként érkezik, amit a MessageQ-ba töltünk.
		osMessagePut(uart6Receive_xMessage, uart6Data[0], 0);
		// Majd újra elindítjuk az adat vételét.
		HAL_UART_Receive_IT(UartHandle, uart6Data, 1);
	}
	else if(UartHandle->Instance == USART1)
	{
		osMessagePut(bleReceive_xMessage, bleData[0], 0);
		HAL_UART_Receive_IT(UartHandle, bleData, 1);
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	// Amennyiben overrun error lép fel, úgy töröljük a bitet, és eldobjuk az adatregiszter tartalmát (?)
	if(__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE))
	{
		__HAL_UART_CLEAR_OREFLAG(huart);
		__HAL_UART_FLUSH_DRREGISTER(huart);
	}

	if(huart->Instance == USART6)
	{
		uint8_t i;
		uint8_t message[MESSAGE_LENGTH] = "error   :error\n";

		// Ha a GUI-tól jött vezérlõparancs, akkor jelezzük, hogy hibás volt az átvitel.
		if(uart6Send_xMessage!=NULL)
			for(i=0;i<MESSAGE_LENGTH;i++)
			{
				osMessagePut(uart6Send_xMessage, message[i], 10);
			}
		// Majd újra elindítjuk az adat vételét.
		HAL_UART_Receive_IT(huart, uart6Data, 1);
	}
	else if(huart->Instance == USART1)
	{
		// Újra elindítjuk az adat vételét.
		HAL_UART_Receive_IT(huart, bleData, 1);
	}
}

void RemoteControllerConnected()
{
	GPIO_PinState state;
	uint8_t message[MESSAGE_LENGTH];
	uint8_t value[VALUE_LENGTH+1];
	uint8_t i;

	// led0 értékének kiolvasása és elküldése
	memcpy(message, "led0    :", 9);

	state = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8);
	if(state == GPIO_PIN_SET)
	{
		value[0] = 0xAA;
		value[1] = 0xAA;
		value[2] = 0xAA;
		value[3] = 0xAA;
		value[4] = '\n';
	}
	else
	{
		value[0] = 0x55;
		value[1] = 0x55;
		value[2] = 0x55;
		value[3] = 0x55;
		value[4] = '\n';
	}

	memcpy(&message[9], value, 5);

	if(uart6Send_xMessage!=NULL)
		for(i=0;i<MESSAGE_LENGTH;i++)
			osMessagePut(uart6Send_xMessage, message[i], 10);

	if(sdCardWrite_xMessage!=NULL)
		for(i=0;i<MESSAGE_LENGTH;i++)
		{
			osMessagePut(sdCardWrite_xMessage, message[i], 10);
		}

	// led1 értékének kiolvasása és elküldése
	memcpy(message, "led1    :", 9);

	state = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_9);
	if(state == GPIO_PIN_SET)
	{
		value[0] = 0xAA;
		value[1] = 0xAA;
		value[2] = 0xAA;
		value[3] = 0xAA;
		value[4] = '\n';
	}
	else
	{
		value[0] = 0x55;
		value[1] = 0x55;
		value[2] = 0x55;
		value[3] = 0x55;
		value[4] = '\n';
	}

	memcpy(&message[9], value, 5);

	if(uart6Send_xMessage!=NULL)
		for(i=0;i<MESSAGE_LENGTH;i++)
			osMessagePut(uart6Send_xMessage, message[i], 10);

	if(sdCardWrite_xMessage!=NULL)
		for(i=0;i<MESSAGE_LENGTH;i++)
		{
			osMessagePut(sdCardWrite_xMessage, message[i], 10);
		}

	// switch0 értékének kiolvasása és elküldése
	memcpy(message, "switch0 :", 9);

	state = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10);
	if(state == GPIO_PIN_SET)
	{
		value[0] = 0xAA;
		value[1] = 0xAA;
		value[2] = 0xAA;
		value[3] = 0xAA;
		value[4] = '\n';
	}
	else
	{
		value[0] = 0x55;
		value[1] = 0x55;
		value[2] = 0x55;
		value[3] = 0x55;
		value[4] = '\n';
	}

	memcpy(&message[9], value, 5);

	if(uart6Send_xMessage!=NULL)
		for(i=0;i<MESSAGE_LENGTH;i++)
			osMessagePut(uart6Send_xMessage, message[i], 10);

	if(sdCardWrite_xMessage!=NULL)
		for(i=0;i<MESSAGE_LENGTH;i++)
		{
			osMessagePut(sdCardWrite_xMessage, message[i], 10);
		}

	// switch1 értékének kiolvasása és elküldése
	memcpy(message, "switch1 :", 9);

	state = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_11);
	if(state == GPIO_PIN_SET)
	{
		value[0] = 0xAA;
		value[1] = 0xAA;
		value[2] = 0xAA;
		value[3] = 0xAA;
		value[4] = '\n';
	}
	else
	{
		value[0] = 0x55;
		value[1] = 0x55;
		value[2] = 0x55;
		value[3] = 0x55;
		value[4] = '\n';
	}

	memcpy(&message[9], value, 5);

	if(uart6Send_xMessage!=NULL)
		for(i=0;i<MESSAGE_LENGTH;i++)
			osMessagePut(uart6Send_xMessage, message[i], 10);

	if(sdCardWrite_xMessage!=NULL)
		for(i=0;i<MESSAGE_LENGTH;i++)
		{
			osMessagePut(sdCardWrite_xMessage, message[i], 10);
		}
}

void ChangeLed0State(GPIO_PinState state)
{
	GPIO_PinState newState;
	uint8_t message[MESSAGE_LENGTH];
	uint8_t value[VALUE_LENGTH+1];
	uint8_t i;

	memcpy(message, "led0    :", 9);

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, state);
	osDelay(1);
	newState = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_8);
	if(newState == GPIO_PIN_SET)
	{
		value[0] = 0xAA;
		value[1] = 0xAA;
		value[2] = 0xAA;
		value[3] = 0xAA;
		value[4] = '\n';
	}
	else
	{
		value[0] = 0x55;
		value[1] = 0x55;
		value[2] = 0x55;
		value[3] = 0x55;
		value[4] = '\n';
	}

	memcpy(&message[9], value, 5);

	if(uart6Send_xMessage!=NULL)
		for(i=0;i<MESSAGE_LENGTH;i++)
			osMessagePut(uart6Send_xMessage, message[i], 10);

	if(sdCardWrite_xMessage!=NULL)
		for(i=0;i<MESSAGE_LENGTH;i++)
		{
			osMessagePut(sdCardWrite_xMessage, message[i], 10);
		}
}

void ChangeLed1State(GPIO_PinState state)
{
	GPIO_PinState newState;
	uint8_t message[MESSAGE_LENGTH];
	uint8_t value[VALUE_LENGTH+1];
	uint8_t i;

	memcpy(message, "led1    :", 9);

	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, state);
	osDelay(1);
	newState = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_9);
	if(newState == GPIO_PIN_SET)
	{
		value[0] = 0xAA;
		value[1] = 0xAA;
		value[2] = 0xAA;
		value[3] = 0xAA;
		value[4] = '\n';
	}
	else
	{
		value[0] = 0x55;
		value[1] = 0x55;
		value[2] = 0x55;
		value[3] = 0x55;
		value[4] = '\n';
	}

	memcpy(&message[9], value, 5);

	if(uart6Send_xMessage!=NULL)
		for(i=0;i<MESSAGE_LENGTH;i++)
			osMessagePut(uart6Send_xMessage, message[i], 10);

	if(sdCardWrite_xMessage!=NULL)
		for(i=0;i<MESSAGE_LENGTH;i++)
		{
			osMessagePut(sdCardWrite_xMessage, message[i], 10);
		}
}

void StartSwitchChangedTask(void const * argument)
{
	GPIO_PinState sw0_state = GPIO_PIN_RESET;
	GPIO_PinState sw1_state = GPIO_PIN_RESET;
	uint8_t message[MESSAGE_LENGTH];
	uint8_t value[VALUE_LENGTH+1];
	uint16_t gpioPin = 0;
	osEvent event;
	uint8_t i;

	osMessageQDef(SWITCHMES, 1, uint16_t);
	switchInterrupt_xMessage = osMessageCreate(osMessageQ(SWITCHMES), NULL);

	sw0_state = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10);
	sw1_state = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_11);

	while(1)
	{
		event = osMessageGet(switchInterrupt_xMessage, portMAX_DELAY);

		if(event.status == osEventMessage)
		{
			gpioPin = (uint16_t)event.value.p;

			memcpy(message, "switch", 6);

			if(gpioPin == GPIO_PIN_10)
			{
				sw0_state = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_10);

				memcpy(&message[6], "0 :",3);

				if(sw0_state == GPIO_PIN_SET)
				{
					value[0] = 0xAA;
					value[1] = 0xAA;
					value[2] = 0xAA;
					value[3] = 0xAA;
					value[4] = '\n';
				}
				else
				{
					value[0] = 0x55;
					value[1] = 0x55;
					value[2] = 0x55;
					value[3] = 0x55;
					value[4] = '\n';
				}

				memcpy(&message[9], value, 5);
			}
			else if(gpioPin == GPIO_PIN_11)
			{
				sw1_state = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_11);

				memcpy(&message[6], "1 :",3);

				if(sw1_state == GPIO_PIN_SET)
				{
					value[0] = 0xAA;
					value[1] = 0xAA;
					value[2] = 0xAA;
					value[3] = 0xAA;
					value[4] = '\n';
				}
				else
				{
					value[0] = 0x55;
					value[1] = 0x55;
					value[2] = 0x55;
					value[3] = 0x55;
					value[4] = '\n';
				}

				memcpy(&message[9], value, 5);
			}

			if(uart6Send_xMessage!=NULL)
				for(i=0;i<MESSAGE_LENGTH;i++)
					osMessagePut(uart6Send_xMessage, message[i], 10);
			if(sdCardWrite_xMessage!=NULL)
				for(i=0;i<MESSAGE_LENGTH;i++)
				{
					osMessagePut(sdCardWrite_xMessage, message[i], 10);
				}
		}
	}
}

void StartTempMeasureTask(void const * argument)
{
	uint32_t adc_val = 0;

	uint8_t message[MESSAGE_LENGTH];
	uint8_t i;

	memcpy(message, "loctemp :", 9);

	while(1)
	{
		HAL_ADC_Start(&hadc1);
		if(HAL_ADC_PollForConversion(&hadc1, portMAX_DELAY) == HAL_OK)
		{
			adc_val = HAL_ADC_GetValue(&hadc1);
		}
		else
		{
			adc_val = 0xFFFFFFFF;
		}

		memcpy(&message[9], &adc_val, 4);
		message[13] = '\n';

		if(uart6Send_xMessage!=NULL)
			for(i=0;i<MESSAGE_LENGTH;i++)
				osMessagePut(uart6Send_xMessage, message[i], 10);

		if(sdCardWrite_xMessage!=NULL)
			for(i=0;i<MESSAGE_LENGTH;i++)
			{
				osMessagePut(sdCardWrite_xMessage, message[i], 10);
			}

		osDelay(500);
	}
}

void StartPotmeterMeasureTask(void const * argument)
{
	uint32_t adc_val = 0;

	uint8_t message[MESSAGE_LENGTH];
	uint8_t i;

	memcpy(message, "potmeter:", 9);

	while(1)
	{
		HAL_ADC_Start(&hadc2);
		if(HAL_ADC_PollForConversion(&hadc2, portMAX_DELAY) == HAL_OK)
		{
			adc_val = HAL_ADC_GetValue(&hadc2);
		}
		else
		{
			adc_val = 0xFFFFFFFF;
		}

		memcpy(&message[9], &adc_val, 4);
		message[13] = '\n';

		if(uart6Send_xMessage!=NULL)
			for(i=0;i<MESSAGE_LENGTH;i++)
			{
				osMessagePut(uart6Send_xMessage, message[i], 10);
			}

		if(sdCardWrite_xMessage!=NULL)
			for(i=0;i<MESSAGE_LENGTH;i++)
			{
				osMessagePut(sdCardWrite_xMessage, message[i], 10);
			}

		osDelay(500);
	}
}

void BLE_Send(uint8 len1,uint8* data1,uint16 len2,uint8* data2) {
	uint8_t message[64];
	uint8_t length;
	uint8_t i;

	length = len1+len2;
	memcpy(message, &length, 1);
	memcpy(&message[1],data1,len1);
	memcpy(&message[len1+1],data2,len2);
	length = len1+len2;
	//Add to UART FIFO
	if(bleSend_xMessage != NULL)
		for(i=0;i<length+1;i++)
		{
			osMessagePut(bleSend_xMessage, message[i], 10);
		}
}

void StartBLETask(void const * argument)
{
	osMessageQDef(BLEMES, 64, uint8_t);
	bleSend_xMessage = osMessageCreate(osMessageQ(BLEMES), NULL);
	osMessageQDef(BLERECEIVEMES, 64, uint8_t);
	bleReceive_xMessage = osMessageCreate(osMessageQ(BLERECEIVEMES), NULL);
	osSemaphoreDef(BLEVENTSEM);
	bleEvent_xSemaphore = osSemaphoreCreate(osSemaphore(BLEVENTSEM), 1);
	osStatus rsp;

	// Malloc elkerülése végett
	uint8_t period_val_memory[2];
	uint8_t config_val_memory[2];

	uint8array *period_val;
	uint8array *config_val;

	period_val = period_val_memory;
	config_val = config_val_memory;

	period_val->len = 1;
	period_val->data[0] = 100;

	config_val->len = 1;
	config_val->data[0] = 1;

	// BGLib config
	bglib_output = &BLE_Send;
	bd_addr sensortag_address;

	sensortag_address.addr[0] = SENSORTAG_ADDRESS_0;
	sensortag_address.addr[1] = SENSORTAG_ADDRESS_1;
	sensortag_address.addr[2] = SENSORTAG_ADDRESS_2;
	sensortag_address.addr[3] = SENSORTAG_ADDRESS_3;
	sensortag_address.addr[4] = SENSORTAG_ADDRESS_4;
	sensortag_address.addr[5] = SENSORTAG_ADDRESS_5;

	ThreadDef(BLESENDTASKID, StartBLESendTask, osPriorityNormal, 0, 128);
	ThreadDef(BLERECEIVETASKID, StartBLEReceiveTask, osPriorityRealtime, 0, 128);
	bleSendTaskHandle = osThreadCreate(Thread(BLESENDTASKID), NULL);
	bleReceiveTaskHandle = osThreadCreate(Thread(BLERECEIVETASKID), NULL);

	osSemaphoreWait(bleEvent_xSemaphore, 0);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_4, GPIO_PIN_SET);
	rsp = osSemaphoreWait(bleEvent_xSemaphore, 2000);
	while(1)
	{
		rsp = osErrorOS;
		osDelay(1000);
		do
		{
			ble_cmd_system_reset(RESETTOMAINPROGRAM);
			rsp = osSemaphoreWait(bleEvent_xSemaphore, 2000);
		}
		while(rsp != osOK);

		// Hello BLE
		if(rsp == osOK)
		{
			osDelay(10);
			ble_cmd_system_hello();
			rsp = osSemaphoreWait(bleEvent_xSemaphore, 2000);
		}

		// Connect to SensorTag
		if(rsp == osOK)
		{
			osDelay(10);
			ble_cmd_gap_connect_direct(sensortag_address.addr, gap_address_type_public, BLECONNINTERVALMIN, BLECONNINTERVALMAX, BLECONNTIMEOUT, BLECONNSLAVELATENCY);
			rsp = osSemaphoreWait(bleEvent_xSemaphore, 2000);
		}

		// Config temperature sensor period
		if(rsp == osOK)
		{
			osDelay(10);
			ble_cmd_attclient_attribute_write(bleConnectionHndl, BLETEMPPERIODHNDL, period_val->len, period_val->data);
			rsp = osSemaphoreWait(bleEvent_xSemaphore, 2000);
		}
		// Config light sensor period
		if(rsp == osOK)
		{
			osDelay(10);
			ble_cmd_attclient_attribute_write(bleConnectionHndl, BLELIGHTPERIODHNDL, period_val->len, period_val->data);
			rsp = osSemaphoreWait(bleEvent_xSemaphore, 2000);
		}

		// Start temperature measurement
		if(rsp == osOK)
		{
			osDelay(10);
			ble_cmd_attclient_attribute_write(bleConnectionHndl, BLETEMPCONFIGHNDL, config_val->len, config_val->data);
			rsp = osSemaphoreWait(bleEvent_xSemaphore, 2000);
		}
		// Start light measurement
		if(rsp == osOK)
		{
			osDelay(10);
			ble_cmd_attclient_attribute_write(bleConnectionHndl, BLELIGHTCONFIGHNDL, config_val->len, config_val->data);
			rsp = osSemaphoreWait(bleEvent_xSemaphore, 2000);
		}

		while(rsp == osOK)
		{
			osDelay(1000);
			// Read temperature data
			if(rsp == osOK)
			{
				osDelay(10);
				ble_cmd_attclient_read_by_handle(bleConnectionHndl, BLETEMPDATAHNDL);
				rsp = osSemaphoreWait(bleEvent_xSemaphore, 2000);
			}
			if(rsp == osOK)
			{
				osDelay(10);
				ble_cmd_attclient_read_by_handle(bleConnectionHndl, BLELIGHTDATAHNDL);
				rsp = osSemaphoreWait(bleEvent_xSemaphore, 2000);
			}
		}
		ble_cmd_connection_disconnect(bleConnectionHndl);
	}
}

void StartBLESendTask(void const * argument)
{
	osEvent event;
	uint8_t character;
	uint16_t size = sizeof(character)/sizeof(uint8_t);

	while(1)
	{
		event = osMessageGet(bleSend_xMessage, portMAX_DELAY);
		if(event.status == osEventMessage)
		{
			character = (uint32_t)event.value.p;

			// Az üzenetek küldése közben ne legyen megszakítás!
			portENTER_CRITICAL();
			HAL_UART_Transmit(&huart1, &character, size, 100);
			portEXIT_CRITICAL();
		}
	}
}

void StartBLEReceiveTask(void const * argument)
{
	volatile uint8_t data[64];
	uint16_t size = sizeof(data)/sizeof(uint8_t);
	uint8_t i;
	osEvent event;

	// BLE variables. Source: https://eewiki.net/display/Wireless/Getting+Started+with+the+BlueGiga+BLE112+Bluetooth+Low+Energy+module
	const struct ble_msg *BTMessage;
	struct ble_header BTHeader;

	HAL_UART_Receive_IT(&huart1, bleData, 1);
	while(1)
	{
		i=0;
		memset(data, 0, size);
		while(i<size)
		{
			if(i==0)
				event = osMessageGet(bleReceive_xMessage, portMAX_DELAY);
			else
				event = osMessageGet(bleReceive_xMessage, 2);

			if(event.status == osEventMessage)
			{
				data[i] = event.value.p;
				i++;
			}
			else
				break;
		}

		BTHeader.type_hilen = data[0];
		BTHeader.lolen = data[1];
		BTHeader.cls = data[2];
		BTHeader.command = data[3];

		BTMessage = ble_get_msg_hdr(BTHeader);

		if(!BTMessage)
			osDelay(10);
		else
			BTMessage->handler(&data[4]);
	}
}

void StartUART6Task(void const * argument)
{
	// Beérkezõ parancsok lehetséges (érvényes) értékei
	const char connectString[] = "connect ";
	const char led0String[] = "led0    ";
	const char led1String[] = "led1    ";

	uint8_t data[MESSAGE_LENGTH];
	uint8_t entity[ENTITY_LENGTH+1];
	uint8_t value[VALUE_LENGTH];
	uint8_t i;
	osEvent event;

	osMessageQDef(UART6MES, 140, uint8_t);
	uart6Send_xMessage = osMessageCreate(osMessageQ(UART6MES), NULL);
	osMessageQDef(UART6RECEIVEMES, 140, uint8_t);
	uart6Receive_xMessage = osMessageCreate(osMessageQ(UART6RECEIVEMES), NULL);

	ThreadDef(UART6SENDTASKID, StartUART6SendTask, osPriorityNormal, 0, 128);
	uart6SendTaskHandle = osThreadCreate(Thread(UART6SENDTASKID), NULL);

	// Egyszerre csak egy karaktert várunk.
	HAL_UART_Receive_IT(&huart6, uart6Data, 1);
	while(1)
	{
		i = 0;
		while(i<MESSAGE_LENGTH)
		{
			// Az elsõ karakterre folyamatosan várunk, viszont ha nem érkezik rövid idõn belül új karakter, akkor feltételezzük, hogy hiba történt \
			és eldobjuk az eddigi csomagot, illetve a következõ sorvége karakterig eldobjuk a karaktereket (vagy kiürítjük a MessageQ-t)
			if(i==0)
				event = osMessageGet(uart6Receive_xMessage, portMAX_DELAY);
			else
				event = osMessageGet(uart6Receive_xMessage, 10);

			if(event.status == osEventMessage)
			{
				data[i] = event.value.p;

				// Ha a sorvége karakter az üzenet közepén érkezett, akkor hiba történt, ezért eldobjuk a csomagot.
				if(data[i] == '\n' && i!=(MESSAGE_LENGTH-1))
					i=0;
				else
					i++;
			}
			else
				i=0;
		}
		// Beérkezett adat feldarabolása
		memcpy(entity, data, ENTITY_LENGTH);
		entity[ENTITY_LENGTH] = '\0';
		memcpy(value, &data[ENTITY_LENGTH+1], VALUE_LENGTH);

		// A GUI csatlakozott
		if(!strcmp(entity, connectString))
		{
			RemoteControllerConnected();
		}
		// LED0 állítása
		else if(!strcmp(entity, led0String))
		{
			if(value[0] == 0xAA && value[1] == 0xAA && value[2] == 0xAA && value[3] == 0xAA)
				ChangeLed0State(GPIO_PIN_SET);
			else
				ChangeLed0State(GPIO_PIN_RESET);
		}
		// LED1 állítása
		else if(!strcmp(entity, led1String))
		{
			if(value[0] == 0xAA && value[1] == 0xAA && value[2] == 0xAA && value[3] == 0xAA)
				ChangeLed1State(GPIO_PIN_SET);
			else
				ChangeLed1State(GPIO_PIN_RESET);
		}
		else
		{
			// Valószínûleg bithiba történt, esetleg nem érkezett meg valamelyik byte, ekkor a további hibák\
			akkumulálódását elkerülvén megkeressük a következõ sorvége karaktert, vagy kiürítjük a MessageQ-t
			do
			{
				event = osMessageGet(uart6Receive_xMessage,0);
			} while(event.status == osEventMessage && event.value.p !='\n');
		}
	}
}

void StartUART6SendTask(void const * argument)
{
	osEvent event;
	uint8_t character;
	uint16_t size = sizeof(character)/sizeof(uint8_t);

	while(1)
	{
		event = osMessageGet(uart6Send_xMessage, portMAX_DELAY);
		if(event.status == osEventMessage)
		{
			character = (uint32_t)event.value.p;

			// Az üzenetek küldése közben ne legyen megszakítás!
			portENTER_CRITICAL();
			HAL_UART_Transmit(&huart6, &character, size, 100);
			portEXIT_CRITICAL();
		}
	}
}

// FatFS open for append. Source: http://elm-chan.org/fsw/ff/res/app1.c
FRESULT open_append (
    FIL* fp,            /* [OUT] File object to create */
    const uint8_t* path    /* [IN]  File name to be opened */
)
{
    FRESULT fr;

    /* Opens an existing file. If not exist, creates a new file. */
    fr = f_open(fp, (char*)path, FA_WRITE | FA_OPEN_ALWAYS);

    if (fr == FR_OK) {
        /* Seek to end of the file to append data */
    	fr = f_lseek(fp, f_size(fp));
        if (fr != FR_OK)
            f_close(fp);
    }
    return fr;
}

void StartSDCardTask(void const * argument)
{
	// SDCard deinicializálásához szükséges külsõ változó
	extern Disk_drvTypeDef  disk;

	osMessageQDef(SDCARDMES, 255, uint8_t);
	sdCardWrite_xMessage = osMessageCreate(osMessageQ(SDCARDMES), NULL);

	FATFS fileSystem;
	FIL logFile;
	UINT testBytes;
	FRESULT res;

	osEvent event;
	uint8_t cardMounted = 0;

	int i;
	uint8_t message[255];

	while(1)
	{
		i = 0;
		memset(message,0,255);

		// CD láb ellenõrzése
		event = osMessageGet(sdCardWrite_xMessage, portMAX_DELAY);
		if(event.status == osEventMessage)
		{
			message[i] = (uint32_t)event.value.p;

			do
			{
				i++;

				event = osMessageGet(sdCardWrite_xMessage, 0);
				if(event.status == osEventMessage)
					message[i] = (uint32_t)event.value.p;
			} while(event.status == osEventMessage);

			if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_RESET)
			{
				if(!cardMounted)
				{
					if(f_mount(&fileSystem, SD_Path, 1) == FR_OK)
					{
						uint8_t path[12] = "logfile.txt";
						path[11] = '\0';

						cardMounted = 1;

						// Ez nem szép dolog, de enélkül a szektorméret elérése után megáll a tudomány
						portENTER_CRITICAL();
						res = open_append(&logFile, path);
						portEXIT_CRITICAL();

						if(res == FR_OK)
						{
							portENTER_CRITICAL();
							res = f_write(&logFile, message, i, &testBytes);
							portEXIT_CRITICAL();
						}
						if(res == FR_OK)
						{
							portENTER_CRITICAL();
							res = f_sync(&logFile);
							portEXIT_CRITICAL();
						}
						if(res == FR_OK)
						{
							//portENTER_CRITICAL();
							res = f_close(&logFile);
							//portEXIT_CRITICAL();
						}

						// Hiba esetén leválasztjuk, majd újra felcsatoljuk a kártyát
						if(res != FR_OK)
						{
							f_mount(NULL, SD_Path, 1);
							// Ez nem szép dolog, de enélkül nem inicializálja fel többször az SDCardot
							disk.is_initialized[0] = 0;
							cardMounted = 0;
						}
						else
							HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
					}
				}
				else
				{
					uint8_t path[12] = "logfile.txt";
					path[11] = '\0';

					// Ez nem szép dolog, de enélkül a szektorméret elérése után megáll a tudomány
					portENTER_CRITICAL();
					res = open_append(&logFile, path);
					portEXIT_CRITICAL();

					if(res == FR_OK)
					{
						portENTER_CRITICAL();
						res = f_write(&logFile, message, i, &testBytes);
						portEXIT_CRITICAL();
					}
					if(res == FR_OK)
					{
						portENTER_CRITICAL();
						res = f_sync(&logFile);
						portEXIT_CRITICAL();
					}
					if(res == FR_OK)
					{
						//portENTER_CRITICAL();
						res = f_close(&logFile);
						//portEXIT_CRITICAL();
					}

					// Hiba esetén leválasztjuk, majd újra felcsatoljuk a kártyát
					if(res != FR_OK)
					{
						f_mount(NULL, SD_Path, 1);
						// Ez nem szép dolog, de enélkül nem inicializálja fel többször az SDCardot
						disk.is_initialized[0] = 0;
						cardMounted = 0;
					}
					else
						HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
				}
			}
			else
			{
				if(cardMounted)
				{
					f_mount(NULL, SD_Path, 1);
					// Ez nem szép dolog, de enélkül nem inicializálja fel többször az SDCardot
					disk.is_initialized[0] = 0;
				}

				cardMounted = 0;
			}
		}
	}
}
#endif // defined(MEAS_W_LOAD)
/* USER CODE END 4 */

/* StartStartMeasureTask function */
void StartStartMeasureTask(void const * argument)
{
  /* init code for FATFS */
#if defined(MEAS_W_LOAD)
  MX_FATFS_Init();
#endif // defined(MEAS_W_LOAD)

  /* USER CODE BEGIN 5 */

  /* Infinite loop */
  while(1)
  {
#if defined(MEAS_TASK_SWITCHING_TIME) || defined(MEAS_PREEMPTION_TIME) \
	|| defined(MEAS_SEMAPHORE_SHUFFLING_TIME) || defined(MEAS_DEADLOCK_BREAKING_TIME) || defined(MEAS_DATAGRAM_THROUGHPUT_TIME)
	  osSemaphoreRelease(measureSemaphoreA_xSemaphore);
#endif // defined(MEAS_TASK_SWITCHING_TIME) || defined(MEAS_PREEMPTION_TIME)
//|| defined(MEAS_SEMAPHORE_SHUFFLING_TIME) || defined(MEAS_DEADLOCK_BREAKING_TIME) || defined(MEAS_DATAGRAM_THROUGHPUT_TIME)

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
