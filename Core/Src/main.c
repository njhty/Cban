/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint32_t sbus_last_ms = 0;
volatile uint8_t  sbus_online  = 0;
volatile uint8_t  sbus_seen    = 0; 
uint8_t buf[256];
int16_t ch[16];
uint8_t st_byte;
uint8_t fl_byte;
uint8_t stp_byte;
volatile uint8_t pro;
volatile uint8_t lose;
typedef struct
{
  int16_t right_stick_x; /* CH1   右摇杆 左右 */
  int16_t right_stick_y; /* CH2   右摇杆 上下 */
  int16_t left_stick_y;  /* CH3   左摇杆 上下（油门） */
  int16_t left_stick_x;  /* CH4   左摇杆 左右 */
  int16_t sw_a; /* CH5   三档开关 SWA */
  int16_t sw_b; /* CH6   三档开关 SWB */
  int16_t sw_c; /* CH7   二档开关 SWC */
  int16_t sw_d; /* CH8   二档开关 SWD */
  int16_t knob_a; /* CH9   旋钮 VRA */
  int16_t knob_b; /* CH10  旋钮 VRB */
} re_data;
re_data re;

/* USER CODE END PV */


    /* Private function prototypes -----------------------------------------------*/
    void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if (huart == &huart3 && Size==25 && buf[0]==0x0F)
  {
    st_byte = buf[0];
    fl_byte = buf[23];
    stp_byte = buf[24];
    ch[1] = (int16_t)(((uint32_t)buf[1] | ((uint32_t)buf[2] << 8)) & 0x07FFU                                   )-1024;
    ch[2] = (int16_t)((((uint32_t)buf[2] >> 3) | ((uint32_t)buf[3] << 5)) & 0x07FFU)-1024;
    ch[3] = (int16_t)((((uint32_t)buf[3] >> 6) | ((uint32_t)buf[4] << 2) | ((uint32_t)buf[5] << 10)) & 0x07FFU)-1024;
    ch[4] = (int16_t)((((uint32_t)buf[5] >> 1) | ((uint32_t)buf[6] << 7)) & 0x07FFU)-1024;
    ch[5] = (int16_t)((((uint32_t)buf[6] >> 4) | ((uint32_t)buf[7] << 4)) & 0x07FFU)-1024;
    ch[6] = (int16_t)((((uint32_t)buf[7] >> 7) | ((uint32_t)buf[8] << 1) | ((uint32_t)buf[9] << 9)) & 0x07FFU)-1024;
    ch[7] = (int16_t)((((uint32_t)buf[9] >> 2) | ((uint32_t)buf[10] << 6)) & 0x07FFU)-1024;
    ch[8] = (int16_t)((((uint32_t)buf[10] >> 5) | ((uint32_t)buf[11] << 3)) & 0x07FFU)-1024;
    ch[9] = (int16_t)(((uint32_t)buf[12] | ((uint32_t)buf[13] << 8)) & 0x07FFU)-1024;
    ch[10]= (int16_t)((((uint32_t)buf[13] >> 3) | ((uint32_t)buf[14] << 5)) & 0x07FFU)-1024;
    re.right_stick_x = ch[1];
    re.right_stick_y = ch[2];
    re.left_stick_y = ch[3];
    re.left_stick_x = ch[4];
    re.sw_a = ch[5];
    re.sw_b = ch[6];
    re.sw_c = ch[7];
    re.sw_d = ch[8];
    re.knob_a = ch[9];
    re.knob_b = ch[10];
    pro=(fl_byte>>3)&0x01;
    lose=(fl_byte>>2)&0x01;
    sbus_last_ms = HAL_GetTick();
    sbus_seen    = 1;

  }
    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, buf, 25);
    __HAL_DMA_DISABLE_IT(huart3.hdmarx, DMA_IT_HT);
  
}
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart){
  if (huart == &huart3){
    HAL_UARTEx_ReceiveToIdle_DMA(&huart3, buf, 25);
    __HAL_DMA_DISABLE_IT(huart3.hdmarx, DMA_IT_HT);
  }
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */

int main(void)
{

  /* USER CODE BEGIN 1 */
 
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
   HAL_UARTEx_ReceiveToIdle_DMA(&huart3, buf, 25);
  __HAL_DMA_DISABLE_IT(huart3.hdmarx, DMA_IT_HT);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
   if (sbus_seen == 0)
    {
        sbus_online = 0;
    }
    else if ((HAL_GetTick() - sbus_last_ms) > 100U)
    {
        sbus_online = 0;
    }
    else
    {
        sbus_online = 1;
    }

    HAL_Delay(10);

  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
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
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
