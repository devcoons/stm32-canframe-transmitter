/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclalast_transmissionr.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclalast_transmissionr in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAlast_transmissionD. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
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
#include "stm32f7xx_hal.h"
#include "can.h"
#include "dma2d.h"
#include "ltdc.h"
#include "quadspi.h"
#include "gpio.h"
#include "fmc.h"

/* USER CODE BEGIN Includes */

#include "stm32746g_discovery.h"
#include "stm32746g_discovery_ts.h"
#include "stm32746g_discovery_lcd.h"
#include <string.h>
#include <stdio.h>

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/


static uint32_t cycle_time = 100;
static CanTxMsgTypeDef can_frame = { .StdId = 0x123, .DLC = 8 , .Data = { 0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08}};
static char text_buffer[100];
static char messages[9][100];
uint8_t transition = 0x00;
uint8_t state = 0x00;
uint32_t last_transmission = 0;



/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

void add_message(char * message)
{
	memmove(messages[0],messages[1],100);
	memmove(messages[1],messages[2],100);
	memmove(messages[2],messages[3],100);
	memmove(messages[3],messages[4],100);
	memmove(messages[4],messages[5],100);
	memmove(messages[5],messages[6],100);
	memmove(messages[6],messages[7],100);
	memmove(messages[7],messages[8],100);
	strcpy(messages[8],message);
}

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */
	 SCB->VTOR = (__IO uint32_t)0x08000000;

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

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
  MX_DMA2D_Init();
  MX_FMC_Init();
  MX_LTDC_Init();
  MX_CAN1_Init();
  MX_QUADSPI_Init();

  /* USER CODE BEGIN 2 */

  /* Set TX message */
  hcan1.pTxMsg = &can_frame;

  /* LCD Initialization */
  BSP_LCD_Init();
  BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);

  /* Enable the LCD */
  BSP_LCD_DisplayOn();

  /* Select the LCD Background Layer  */
  BSP_LCD_SelectLayer(0);

  /* Clear the Background Layer */
  BSP_LCD_Clear(LCD_COLOR_WHITE);

  /* Configure the transparency for background */
  BSP_LCD_SetTransparency(0, 0xFF);

  /* Initialize Touchscreen */
  BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
  BSP_LCD_SetFont(&Font20);
  BSP_LCD_DisplayStringAt(0, 10,(uint8_t*)"CAN Frame Transmitter",CENTER_MODE);

  sprintf(text_buffer,"[*] - CANBus interface is initialized                             ");add_message(text_buffer);
  sprintf(text_buffer,"[*] - Configuration:                                              ");add_message(text_buffer);
  sprintf(text_buffer,"[ ] -        FrameID: 0x%03X                                      ",can_frame.StdId); add_message(text_buffer);
  sprintf(text_buffer,"[ ] -        DLC    : %d                                          ",can_frame.DLC);add_message(text_buffer);
  sprintf(text_buffer,"[ ] -        DATA   : 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X    ",
		  can_frame.Data[0],can_frame.Data[1],can_frame.Data[2],can_frame.Data[3],can_frame.Data[4],can_frame.Data[5],can_frame.Data[6],can_frame.Data[7]);
  add_message(text_buffer);
  sprintf(text_buffer,"[ ] -        C.Tlast_transmission : %dms                                       ",cycle_time);add_message(text_buffer);
  sprintf(text_buffer,"[?] - Press the button below to start                            ");add_message(text_buffer);

  TS_StateTypeDef touch;
  uint8_t symbol_c = 0;
  BSP_TS_GetState(&touch);

  while (1)
  {
		if(transition == 0x00)
		{
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_FillRect(BSP_LCD_GetXSize()/2-150,BSP_LCD_GetYSize()-60,300,50);
			BSP_LCD_DrawRect(BSP_LCD_GetXSize()/2-150,BSP_LCD_GetYSize()-60,300,50);
			BSP_LCD_SetFont(&Font20);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()-40,(uint8_t*)"Activate",CENTER_MODE);
			transition = 0xFF;
		}
		else if(transition == 0x01)
		{
			BSP_LCD_SetBackColor(LCD_COLOR_DARKGRAY);
			BSP_LCD_SetTextColor(LCD_COLOR_DARKGRAY);
			BSP_LCD_FillRect(BSP_LCD_GetXSize()/2-150,BSP_LCD_GetYSize()-60,300,50);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_DrawRect(BSP_LCD_GetXSize()/2-150,BSP_LCD_GetYSize()-60,300,50);
			BSP_LCD_SetFont(&Font20);
			BSP_LCD_SetBackColor(LCD_COLOR_DARKGRAY);
			BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
			BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()-40,(uint8_t*)"Activate",CENTER_MODE);
			transition = 0xFF;
		}
		else if(transition == 0x02)
		{
			BSP_LCD_SetBackColor(LCD_COLOR_GREEN);
			BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
			BSP_LCD_FillRect(BSP_LCD_GetXSize()/2-150,BSP_LCD_GetYSize()-60,300,50);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_DrawRect(BSP_LCD_GetXSize()/2-150,BSP_LCD_GetYSize()-60,300,50);
			BSP_LCD_SetFont(&Font20);
			BSP_LCD_SetBackColor(LCD_COLOR_GREEN);
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()-40,(uint8_t*)"Deactivate",CENTER_MODE);
			transition = 0xFF;
		}
		else if(transition == 0x03)
		{
			BSP_LCD_SetBackColor(0xFF00CC00);
			BSP_LCD_SetTextColor(0xFF00CC00);
			BSP_LCD_FillRect(BSP_LCD_GetXSize()/2-150,BSP_LCD_GetYSize()-60,300,50);
			BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_DrawRect(BSP_LCD_GetXSize()/2-150,BSP_LCD_GetYSize()-60,300,50);
			BSP_LCD_SetFont(&Font20);
			BSP_LCD_SetBackColor(0xFF00CC00);
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			BSP_LCD_DisplayStringAt(0, BSP_LCD_GetYSize()-40,(uint8_t*)"Deactivate",CENTER_MODE);
			transition = 0xFF;
		}

		BSP_TS_GetState(&touch);

		if(touch.touchDetected == 1)
		{
			if(touch.touchX[0] > (BSP_LCD_GetXSize()/2-150) && touch.touchX[0] < (BSP_LCD_GetXSize()/2+150)
			&& touch.touchY[0] > (BSP_LCD_GetYSize()-60) && touch.touchY[0] < (BSP_LCD_GetYSize()-10)
			)
			{
				if(transition == 0xff && state == 0x00)
				{
					if(state != 0x01)
					{
								state = 0x01;
								transition = 0x01;
								add_message("[*] - Activating CAN frame transmission                          ");
							}
						}
						if(transition == 0xff && state== 0x02)
						{
							if(state != 0x03)
							{
								state = 0x03;
								transition = 0x03;

								add_message("[*] - Deactivating CAN frame transmission                        ");
							}
						}
					}
				}
				else
				{
					if(transition == 0xff && state == 0x01)
					{
						transition = 0x02;
						state=0x02;
						add_message("[*] - Completed                                                  ");
					}

					if(transition == 0xff && state == 0x03)
					{
						transition = 0x00;
						state=0x00;
						add_message("[*] - Completed                                                  ");
					}
				}


				if(transition == 0xff )
				{
					BSP_LCD_SetFont(&Font12);
					BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
					BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
					for(int i=0;i<9;i++)
					{
						if(strstr(messages[i],"Error") != NULL)
						{
							BSP_LCD_SetFont(&Font12);
							BSP_LCD_SetBackColor(LCD_COLOR_RED);
							BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
						}
						else
						{
							BSP_LCD_SetFont(&Font12);
							BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
							BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
						}
						BSP_LCD_DisplayStringAt(0,25+19+(18*i) ,messages[i],LEFT_MODE);
					}
				}



				if(transition == 0xff && state == 0x02 && last_transmission < HAL_GetTick())
				{
					last_transmission = HAL_GetTick() + (cycle_time - 4);
					if(HAL_CAN_Transmit(&hcan1,100) ==HAL_OK)
					{
						switch(symbol_c)
						{
						case 0:
							add_message("[-] - Transmission OK                                             ");
							break;
						case 1:
							add_message("[\\] - Transmission OK                                             ");
							break;
						case 2:
							add_message("[-] - Transmission OK                                             ");
							break;
						case 3:
							add_message("[/] - Transmission OK                                             ");
							break;

						}
						symbol_c = symbol_c == 3 ? 0: symbol_c+1;
					}
					else
					{
						switch(symbol_c)
						{
						case 0:
							add_message("[-] - Transmission Error                                         ");
							break;
						case 1:
							add_message("[/] - Transmission Error                                         ");
							break;
						case 2:
							add_message("[-] - Transmission Error                                         ");
							break;
						case 3:
							add_message("[\\] - Transmission Error                                         ");
							break;

						}
						symbol_c = symbol_c == 3 ? 0: symbol_c+1;
					}
				}


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
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Activate the Over-Drive mode 
    */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 50;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2;
  PeriphClkInitStruct.PLLSAIDivQ = 1;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt tlast_transmission
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
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
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
