/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_hal.h"
#include "dwt_stm32_delay.h"
#include "lcd.h"
#include "stdio.h"

TIM_HandleTypeDef htim1;



/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM1_Init(void);

uint32_t hcsr_time;
uint16_t distance;

void Buzzer_pwm_90(void)
{
htim1.Instance->CCR1 =40;
};

void Buzzer_pwm_50(void)
{
htim1.Instance->CCR1 =15;
};

void Buzzer_pwm_off(void)
{
htim1.Instance->CCR1 =0;
};


uint32_t hcsr04(void)
{
	uint32_t counter=0;
	// AT HERE WE MAKE TRIG PIN HIGH FOR 10 MICRO SECOND.
	
	HAL_GPIO_WritePin(GPIOA,TRIG_Pin,GPIO_PIN_SET);
	DWT_Delay_us(10);
	HAL_GPIO_WritePin(GPIOA,TRIG_Pin,GPIO_PIN_RESET);
	
	// NOW WE READ SIGNAL FROM ECHO PIN
	while(!(HAL_GPIO_ReadPin(GPIOA,ECHO_Pin))) //WE WAIT FOR ECHO PIN TO GO HIGH	
	{ // i write like this because i dont want to see warnings.
	};
	
		while(HAL_GPIO_ReadPin(GPIOA,ECHO_Pin))
		{
			counter++;
			DWT_Delay_us(1);
		}
		
		return 2*counter;
	
};

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
char data[16]="";
  
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
	DWT_Delay_Init();
	lcd_init(_LCD_4BIT, _LCD_FONT_5x8, _LCD_2LINE);
  
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	
	/* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    hcsr_time=hcsr04();
		distance=hcsr_time * .034/2;
		sprintf(data,"%d",distance);
		lcd_print(1,1,"Distance:");
		lcd_print(2,1,data);
		
		
		if(distance > 0 && distance <7 )
		{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12,GPIO_PIN_RESET);
		Buzzer_pwm_90();
		}
		else if(distance >= 7 && distance <= 12)
		{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11,GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12,GPIO_PIN_RESET);
		Buzzer_pwm_50();
		}
		else if (distance > 12 && distance < 41)
		{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12,GPIO_PIN_SET);
		Buzzer_pwm_off();
		}
		else
		{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12,GPIO_PIN_RESET);
		Buzzer_pwm_off();	
		};
		
		HAL_Delay(1000);
		lcd_clear();
  }
  
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
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
  /** Initializes the CPU, AHB and APB busses clocks 
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
}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */
/* TIM1 WORKS AT MAXIMUM 72MHZ. SO 72MHZ / (100*100) = 7200 
	WE CHOOSE PRESCALER VALUES AS 0 TO 7199 = 7200 SO I WROTE 7200-1
AND I WANT TO USE %100 PWM DUTY AND I CHOOSE PERIOD AS 100-1 = 0 TO 99.	
	
	*/
  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 7200-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 100-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, TRIG_Pin|LCD_EN_Pin|LCD_RS_Pin|LCD_D4_Pin 
                          |LCD_D5_Pin| GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_10, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LCD_D6_Pin|LCD_D7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : TRIG_Pin LCD_EN_Pin LCD_RS_Pin LCD_D4_Pin 
                           LCD_D5_Pin */
  GPIO_InitStruct.Pin = TRIG_Pin|LCD_EN_Pin|LCD_RS_Pin|LCD_D4_Pin 
                          |LCD_D5_Pin| GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : ECHO_Pin */
  GPIO_InitStruct.Pin = ECHO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ECHO_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LCD_D6_Pin LCD_D7_Pin */
  GPIO_InitStruct.Pin = LCD_D6_Pin|LCD_D7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
