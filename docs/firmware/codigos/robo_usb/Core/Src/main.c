/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbd_cdc_if.h"
#include "MY_NRF24.h"
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
SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

/* USER CODE BEGIN PV */
uint8_t buff_radio = 16;
uint8_t entrada[10]; //buffer para guardar os dados recebidos
uint8_t saida[8]; //buffer para guardar os dados a seren enviados
uint8_t temdados = 0; // tipo um Kbhit aqui (quantos bytes pra ler)
uint8_t comando1 = 0; //pwm do motor 1 de 0 a 250 (28800 KHz)
uint8_t comando2 = 0; //pwm do motor 2 de 0 a 250 (28800 KHz)
uint8_t logica = 0;
short  enc1 = 32000;
short  enc2 = 32000;
short  encvel1 = 0;
short  encvel2 = 0;
short  sp = 0;
uint64_t RxpipeAddrs = 0x11223344AA; // endereço 1 do radio (enviar)
uint64_t TxpipeAddrs = 0x11223344BB; // endereço 2 do radio (receber)
uint8_t myRxData[16]; //colocar o valor de buff_radio
uint8_t myTxData[16]; //colocar o valor de buff_radio
uint8_t cont = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM4_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
int abs(int);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  //HAL_Delay(500); //500 ms antes de começar
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  MX_TIM4_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
  TIM2->CNT = 32000; //meio de escala da velocidade (velocidade inicial igual a zero)
  TIM1->CNT = 32000; //meio de escala da velocidade (velocidade inicial igual a zero)
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); // inicializa o PWM 1  M1
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2); // inicializa o PWM 2  M1
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3); // inicializa o PWM 3  M2
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4); // inicializa o PWM 4  M2
  htim3.Instance->CCR1 = comando1; //pwm do motor 1 de 0 a 250 (28800 KHz)
  htim3.Instance->CCR2 = comando1; //pwm do motor 2 de 0 a 250 (28800 KHz)
  htim3.Instance->CCR3 = comando2; //pwm do motor 1 de 0 a 250 (28800 KHz)
  htim3.Instance->CCR4 = comando2; //pwm do motor 2 de 0 a 250 (28800 KHz)

  HAL_TIM_Base_Start_IT(&htim4);
//  HAL_TIM_Base_Start_IT(&htim2);
 /////////////////////**** RECEPTOR ****///////////////////////////
	NRF24_begin(CSNpin_GPIO_Port,CEpin_GPIO_Port,CSNpin_Pin,CEpin_Pin,hspi1);
	printRadioSettings();
	NRF24_setAutoAck(false);      //Ativar para o caso de reconhecimento
	NRF24_setChannel(52);         //Deve ser o mesmo do receptor
	NRF24_setPayloadSize(buff_radio);
	NRF24_openWritingPipe(TxpipeAddrs);
	NRF24_openReadingPipe(1, RxpipeAddrs);

//    HAL_Delay(500); //500 ms antes de começar
//    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); //pisca led



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	NRF24_startListening();
	while(cont<buff_radio)
	  {
		myTxData[cont]=0;
		myRxData[cont]=0;
		cont++;
	  }
	  cont = 0;
  while (1)
  {
	 if(NRF24_available()) //verifica se há dados disponíveis
	{
		NRF24_read(myRxData, buff_radio); // Lê dados recebido (32 bytes)
		if (myRxData[0] == 254) //se o pacote chegou na ordem certa
			{
			// aqui falta fazer interpretação da lógica de direção (tá no byte 7 - oitavo)
			logica = myRxData[7];
			comando1 = myRxData[8]; //M1
			comando2 = myRxData[9]; //M2
	 	 	if (logica == 10) //velocidade + +
				{
				  htim3.Instance->CCR1 = comando1; //pwm do motor 1 de 0 a 250 (28800 KHz) M1+
				  htim3.Instance->CCR2 = 0;        //pwm do motor 2 de 0 a 250 (28800 KHz) M1-
				  htim3.Instance->CCR3 = comando2; //pwm do motor 1 de 0 a 250 (28800 KHz) M2+
				  htim3.Instance->CCR4 = 0;        //pwm do motor 2 de 0 a 250 (28800 KHz) M2-
				}
			if (logica == 18) //volocidade + -
				{
				  htim3.Instance->CCR1 = comando1; //pwm do motor 1 de 0 a 250 (28800 KHz) M1+
				  htim3.Instance->CCR2 = 0;        //pwm do motor 2 de 0 a 250 (28800 KHz) M1-
				  htim3.Instance->CCR3 = 0;        //pwm do motor 1 de 0 a 250 (28800 KHz) M2+
				  htim3.Instance->CCR4 = comando2; //pwm do motor 2 de 0 a 250 (28800 KHz) M2-
				}
			if (logica == 12) //velocidade - +
				{
				  htim3.Instance->CCR1 = 0;        //pwm do motor 1 de 0 a 250 (28800 KHz) M1+
				  htim3.Instance->CCR2 = comando1; //pwm do motor 2 de 0 a 250 (28800 KHz) M1-
				  htim3.Instance->CCR3 = comando2; //pwm do motor 1 de 0 a 250 (28800 KHz) M2+
				  htim3.Instance->CCR4 = 0;        //pwm do motor 2 de 0 a 250 (28800 KHz) M2-
				}
			if (logica == 20) //velocidade  - -
				{
				  htim3.Instance->CCR1 = 0;        //pwm do motor 1 de 0 a 250 (28800 KHz) M1+
				  htim3.Instance->CCR2 = comando1; //pwm do motor 2 de 0 a 250 (28800 KHz) M1-
				  htim3.Instance->CCR3 = 0;        //pwm do motor 1 de 0 a 250 (28800 KHz) M2+
				  htim3.Instance->CCR4 = comando2; //pwm do motor 2 de 0 a 250 (28800 KHz) M2-
				}
			}
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); //pisca led
	}else
	{
		if (entrada[0] == 254) //se o pacote chegou na ordem certa
			{
			// aqui falta fazer interpretação da lógica de direção (tá no byte 7 - oitavo)
			logica = entrada[7];
			comando1 = entrada[8]; //M1
			comando2 = entrada[9]; //M2
	 	 	if (logica == 10) //velocidade + +
				{
				  htim3.Instance->CCR1 = comando1; //pwm do motor 1 de 0 a 250 (28800 KHz) M1+
				  htim3.Instance->CCR2 = 0;        //pwm do motor 2 de 0 a 250 (28800 KHz) M1-
				  htim3.Instance->CCR3 = comando2; //pwm do motor 1 de 0 a 250 (28800 KHz) M2+
				  htim3.Instance->CCR4 = 0;        //pwm do motor 2 de 0 a 250 (28800 KHz) M2-
				}
			if (logica == 18) //volocidade + -
				{
				  htim3.Instance->CCR1 = comando1; //pwm do motor 1 de 0 a 250 (28800 KHz) M1+
				  htim3.Instance->CCR2 = 0;        //pwm do motor 2 de 0 a 250 (28800 KHz) M1-
				  htim3.Instance->CCR3 = 0;        //pwm do motor 1 de 0 a 250 (28800 KHz) M2+
				  htim3.Instance->CCR4 = comando2; //pwm do motor 2 de 0 a 250 (28800 KHz) M2-
				}
			if (logica == 12) //velocidade - +
				{
				  htim3.Instance->CCR1 = 0;        //pwm do motor 1 de 0 a 250 (28800 KHz) M1+
				  htim3.Instance->CCR2 = comando1; //pwm do motor 2 de 0 a 250 (28800 KHz) M1-
				  htim3.Instance->CCR3 = comando2; //pwm do motor 1 de 0 a 250 (28800 KHz) M2+
				  htim3.Instance->CCR4 = 0;        //pwm do motor 2 de 0 a 250 (28800 KHz) M2-
				}
			if (logica == 20) //velocidade  - -
				{
				  htim3.Instance->CCR1 = 0;        //pwm do motor 1 de 0 a 250 (28800 KHz) M1+
				  htim3.Instance->CCR2 = comando1; //pwm do motor 2 de 0 a 250 (28800 KHz) M1-
				  htim3.Instance->CCR3 = 0;        //pwm do motor 1 de 0 a 250 (28800 KHz) M2+
				  htim3.Instance->CCR4 = comando2; //pwm do motor 2 de 0 a 250 (28800 KHz) M2-
				}
			}
	}
	 NRF24_stopListening();
	 myTxData[0] = enc1;
	 myTxData[1] = enc1>>8;
	 myTxData[2] = enc2;
	 myTxData[3] = enc2>>8;
	 myTxData[4] = encvel1;
	 myTxData[5] = encvel1>>8;
	 myTxData[6] = encvel2;
	 myTxData[7] = encvel2>>8;
	 NRF24_write(myTxData, buff_radio);
	 NRF24_startListening();

	 // Envia na serial tbm
	 for(cont = 0; cont < 8; cont++)
		{
		 saida[cont] = myTxData[cont];
		}
	 CDC_Transmit_FS(saida, 8);


/* // código para testar se tá funcionando o acionamento, sem usar o radio (old)
	  if (sp == 1)
	  {
		  htim3.Instance->CCR3 = comando1; //pwm do motor 1 de 0 a 250 (28800 KHz)
		  htim3.Instance->CCR4 = comando2; //pwm do motor 2 de 0 a 250 (28800 KHz)
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); //M1_a
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET); //M1_b
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); //M2_a
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET); //M2_b
		  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); //pisca led
	  }
	  if (sp == 0)
	  {
		  htim3.Instance->CCR3 = 250; //pwm do motor 1 de 0 a 250 (28800 KHz)
		  htim3.Instance->CCR4 = 250; //pwm do motor 2 de 0 a 250 (28800 KHz)
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET); //M1_a
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET); //M1_b
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET); //M2_a
		  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET); //M2_b
		  HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13); //pisca led
	  }   */

/* // comandos para enviar direto pela USB no lugar do radio (Raspberry)
    saida[0] = enc1;
	saida[1] = enc1>>8;
	saida[2] = enc2;
	saida[3] = enc2>>8;
	saida[4] = encvel1;
	saida[5] = encvel1>>8;
	saida[6] = encvel2;
	saida[7] = encvel2>>8;
	CDC_Transmit_FS(saida,8);

	*/

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }  // fecha o while(1)
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
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

  /** Initializes the CPU, AHB and APB buses clocks
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 9;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 249;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 71;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 9999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, CEpin_Pin|CSNpin_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : CEpin_Pin CSNpin_Pin */
  GPIO_InitStruct.Pin = CEpin_Pin|CSNpin_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
