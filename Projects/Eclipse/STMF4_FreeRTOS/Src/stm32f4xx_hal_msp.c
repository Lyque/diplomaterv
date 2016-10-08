/**
  ******************************************************************************
  * @file    Templates/Src/stm32f4xx_hal_msp.c
  * @author  MCD Application Team
  * @version V1.2.1
  * @date    13-March-2015
  * @brief   HAL MSP module.
  *         
  @verbatim
 ===============================================================================
                     ##### How to use this driver #####
 ===============================================================================
    [..]
    This file is generated automatically by MicroXplorer and eventually modified 
    by the user

  @endverbatim
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
#include "stm32f4xx_hal.h"
#include "stm32f4_discovery.h"
#include "globalvar.h"
#include "config.h"

/** @addtogroup STM32F4xx_HAL_Driver
  * @{
  */

/** @defgroup HAL_MSP
  * @brief HAL MSP module.
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup HAL_MSP_Private_Functions
  * @{
  */

/**
  * @brief  Initializes the Global MSP.
  * @param  None
  * @retval None
  */
void HAL_MspInit(void)
{
  /* NOTE : This function is generated automatically by MicroXplorer and eventually  
            modified by the user
   */ 

	BSP_LED_Init(LED4);
	BSP_LED_Init(LED3);
	BSP_LED_Init(LED6);
	BSP_LED_Init(LED5);

	ADC_InitTypeDef adc_init;
	ADC_ChannelConfTypeDef adc_channel;
	UART_InitTypeDef uart_init;
	volatile HAL_StatusTypeDef status;

	hadc.Instance = ADCx;
	hadc.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV8;
	hadc.Init.Resolution = ADC_RESOLUTION12b;
	hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc.Init.ScanConvMode = DISABLE;
	hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc.Init.ContinuousConvMode = DISABLE;
	hadc.Init.NbrOfConversion = 1;
	hadc.Init.DiscontinuousConvMode = DISABLE;
	hadc.Init.NbrOfDiscConversion = 1;
	hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc.Init.DMAContinuousRequests = DISABLE;

	adc_channel.Channel = ADCx_CHANNEL;
	adc_channel.Rank = 1;
	adc_channel.SamplingTime = ADC_SAMPLETIME_84CYCLES;
	adc_channel.Offset = 0;

	status = HAL_ADC_Init(&hadc);
	status = HAL_ADC_ConfigChannel(&hadc, &adc_channel);

	huart.Instance = USART6;
	huart.Init.BaudRate = 9600;
	huart.Init.WordLength = UART_WORDLENGTH_8B;
	huart.Init.StopBits = UART_STOPBITS_1;
	huart.Init.Parity = UART_PARITY_NONE;
	huart.Init.Mode = UART_MODE_TX_RX;
	huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart.Init.OverSampling = UART_OVERSAMPLING_8;

	status = HAL_UART_Init(&huart);

	HAL_IndicatorLED_MspInit();
}

/**
  * @brief  DeInitializes the Global MSP.
  * @param  None  
  * @retval None
  */
void HAL_MspDeInit(void)
{
  /* NOTE : This function is generated automatically by MicroXplorer and eventually  
            modified by the user
   */
}

/**
  * @brief  Initializes the SD interface MSP.
  * @param  None
  * @retval None
  */
void HAL_SD_MspInit(SD_HandleTypeDef *hsd)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	SDIOx_CLK_ENABLE();
	SDIOx_PORT_FIRST_CLK_ENABLE();
	SDIOx_PORT_SECOND_CLK_ENABLE();

	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;

	GPIO_InitStruct.Pin = SDIOx_PIN_D0_FIRST_PORT | SDIOx_PIN_D1_FIRST_PORT | SDIOx_PIN_D2_FIRST_PORT | SDIOx_PIN_D3_FIRST_PORT | SDIOx_PIN_CK_FIRST_PORT;

	HAL_GPIO_Init(SDIOx_PORT_FIRST, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = SDIOx_PIN_CMD_SECOND_PORT;
	HAL_GPIO_Init(SDIOx_PORT_SECOND, &GPIO_InitStruct);
}

/**
  * @brief  DeInitializes the SD interface MSP.
  * @param  None
  * @retval None
  */
void HAL_SD_MspDeInit(SD_HandleTypeDef *hsd)
{

}

/**
  * @brief ADC MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param hadc: ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	static DMA_HandleTypeDef  hdma_adc;

	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO clock */
	ADCx_PORT_CLK_ENABLE();

	/* ADC1 Periph clock enable */
	ADCx_CLK_ENABLE();

	/*##-2- Configure peripheral GPIO ##########################################*/
	/* ADC1 Channel3 GPIO pin configuration */
	GPIO_InitStruct.Pin = GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(ADCx_PORT, &GPIO_InitStruct);
}

/**
  * @brief ADC MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO to their default state
  * @param hadc: ADC handle pointer
  * @retval None
  */
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc)
{
  static DMA_HandleTypeDef  hdma_adc;

  /*##-1- Reset peripherals ##################################################*/
  __HAL_RCC_ADC_FORCE_RESET();
  __HAL_RCC_ADC_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks ################################*/
  /* De-initialize the ADC3 Channel8 GPIO pin */
  __HAL_RCC_ADC1_CLK_DISABLE();

//  /*##-3- Disable the DMA Streams ############################################*/
//  /* De-Initialize the DMA Stream associate to transmission process */
//  HAL_DMA_DeInit(&hdma_adc);
//
//  /*##-4- Disable the NVIC for DMA ###########################################*/
//  HAL_NVIC_DisableIRQ(ADCx_DMA_IRQn);
}

/**
  * @brief UART MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO clock */
	__GPIOC_CLK_ENABLE();

	/* UART4 Periph clock enable */
	__USART6_CLK_ENABLE();

	/*##-2- Configure peripheral GPIO ##########################################*/
	/* ADC1 Channel3 GPIO pin configuration */
	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	GPIO_InitStruct.Alternate = GPIO_AF8_USART6;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

/**
  * @brief UART MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO configuration to their default state
  * @param huart: UART handle pointer
  * @retval None
  */
void HAL_UART_MspDeInit(UART_HandleTypeDef *huart)
{
  /*##-1- Reset peripherals ##################################################*/
  __USART6_FORCE_RESET();
  __USART6_RELEASE_RESET();

  /*##-2- Disable peripherals and GPIO Clocks #################################*/
  /* Configure UART Tx as alternate function  */
  HAL_GPIO_DeInit(GPIOC, GPIO_PIN_0);
  /* Configure UART Rx as alternate function  */
  HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1);
}

/**
  * @brief IndicatorLED pin MSP Initialization
  *        This function configures the hardware resources used in this example:
  *           - Peripheral's clock enable
  *           - Peripheral's GPIO Configuration
  * @retval None
  */
void HAL_IndicatorLED_MspInit()
{
	GPIO_InitTypeDef  GPIO_InitStruct;

	/*##-1- Enable peripherals and GPIO Clocks #################################*/
	/* Enable GPIO clock */
	__GPIOE_CLK_ENABLE();

	/*##-2- Configure peripheral GPIO ##########################################*/
	/* IndicatorLED GPIO pin configuration */
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FAST;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
}

/**
  * @brief IndicatorLED pin MSP De-Initialization
  *        This function frees the hardware resources used in this example:
  *          - Disable the Peripheral's clock
  *          - Revert GPIO configuration to their default state
  * @param huart: GPIO handle pointer
  * @retval None
  */
void HAL_IndicatorLED_MspDeInit()
{
  /*##-1- Disable peripherals and GPIO Clocks #################################*/
  /* Configure Motor pin*/
  HAL_GPIO_DeInit(GPIOE, GPIO_PIN_0);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
