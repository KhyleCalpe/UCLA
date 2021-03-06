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
#include <arm_math.h>
#include <stdio.h>
#include "main.h"
#include "string.h"
#include "math.h"

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
#if defined ( __ICCARM__ ) /*!< IAR Compiler */

#pragma location=0x30040000
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
#pragma location=0x30040060
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */
#pragma location=0x30040200
uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE]; /* Ethernet Receive Buffers */

#elif defined ( __CC_ARM )  /* MDK ARM Compiler */

__attribute__((at(0x30040000))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
__attribute__((at(0x30040060))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */
__attribute__((at(0x30040200))) uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE]; /* Ethernet Receive Buffer */

#elif defined ( __GNUC__ ) /* GNU Compiler */

ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT] __attribute__((section(".RxDecripSection"))); /* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT] __attribute__((section(".TxDecripSection")));   /* Ethernet Tx DMA Descriptors */
uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE] __attribute__((section(".RxArraySection"))); /* Ethernet Receive Buffers */

#endif

ETH_TxPacketConfig TxConfig;
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

ETH_HandleTypeDef heth;

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart3;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ETH_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM1_Init(void);
/* USER CODE BEGIN PFP */
void 				convert_to_complex(float input[64], float32_t* arr, int sample_length);
void 				print_peak(float32_t* arr, int sample_length);
void 				print_magnitude(float32_t* arr, int length);
void 				set_magnitude(float32_t* input, float32_t* output, int length);
void				transpose128(float32_t input[128][128]);
void				transpose64(float32_t input[64][64]);
int					get_d_f(int sample_num, int sample_f);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define ARM_MATH_CM7
#define __FPU_PRESENT 1

#define ADC_BUF_SIZE_1024 1024
#define ADC_BUF_SIZE_256 	256
#define CMPLX_ARR_SIZE_0	2048
#define CMPLX_ARR_SIZE_1	512
#define CMPLX_ARR_SIZE_2	128

int record_done=0;

uint16_t 	ADC_buff_1024[ADC_BUF_SIZE_1024];
uint16_t 	ADC_buff_256[ADC_BUF_SIZE_256];
float32_t CMPLX_buff_1024[CMPLX_ARR_SIZE_0];
float32_t CMPLX_buff_256[CMPLX_ARR_SIZE_1];
float32_t CMPLX_buff_64[CMPLX_ARR_SIZE_2][CMPLX_ARR_SIZE_2];
float32_t MGNTD_buff_1024[ADC_BUF_SIZE_1024];
float32_t MGNTD_buff_256[ADC_BUF_SIZE_256];
float32_t MGNTD_buff_64[CMPLX_ARR_SIZE_2/2][CMPLX_ARR_SIZE_2/2];

float32_t sample;
float32_t mean;
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
	MX_ETH_Init();
	MX_USART3_UART_Init();
	MX_USB_OTG_FS_PCD_Init();
	MX_ADC1_Init();
	MX_TIM1_Init();
	/* USER CODE BEGIN 2 */
	if(HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET,ADC_SINGLE_ENDED) != HAL_OK){
		Error_Handler();
	}
	HAL_TIM_Base_Start(&htim1);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		/*
		 * 	Part 1
		 */

		// Record samples
		if(HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&ADC_buff_1024, ADC_BUF_SIZE_1024) != HAL_OK) {
			Error_Handler();
		}
		while (1)
		{
			if(record_done == 1){
				record_done = 0;
				break;
			}
		}

		// Print first 5 samples
		for (int i=0; i<5; i++)
		{
			printf("#%d: %d\n", i+1, ADC_buff_1024[i]);
			HAL_Delay(10);
		}
		// Plot sample
		for (int i=0; i<ADC_BUF_SIZE_1024; i++)
		{
			sample=ADC_buff_1024[i];
			HAL_Delay(10);
		}

		// Compute the array mean
		for (int i=0; i<ADC_BUF_SIZE_1024; i++)
		{
			mean += ADC_buff_1024[i];
		}
		mean /= ADC_BUF_SIZE_1024;
		// Subtract the array mean from each element
		for (int i=0; i<ADC_BUF_SIZE_1024; i++)
		{
			ADC_buff_1024[i] -= mean;
		}

		/*
		 * 	Part 2
		 */

		// FFT Instances
		arm_cfft_instance_f32 fft_handler_1024;
		arm_cfft_instance_f32 fft_handler_256;
		arm_cfft_init_f32(&fft_handler_1024, 1024);
		arm_cfft_init_f32(&fft_handler_256, 256);

		// Complex FFT of 3KHz sample
		convert_to_complex(ADC_buff_1024, CMPLX_buff_1024, 1024);
		// FFT of 3KHz sample
		arm_cfft_f32(&fft_handler_1024, CMPLX_buff_1024, 0, 1);
		// Print magnitude of FFT
		set_magnitude(CMPLX_buff_1024, MGNTD_buff_1024, 1024);
		print_magnitude(MGNTD_buff_1024, 1024);
		// Record sample of 4KHz sine wave
		if(HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&ADC_buff_1024, ADC_BUF_SIZE_1024) != HAL_OK) {
			Error_Handler();
		}
		while (1)
		{
			if(record_done == 1){
				record_done = 0;
				break;
			}
		}

		// Complex FFT of 4KHz sample
		convert_to_complex(ADC_buff_1024, CMPLX_buff_1024, 1024);
		// FFT of 4KHz sample
		arm_cfft_f32(&fft_handler_1024, CMPLX_buff_1024, 0, 1);
		// Print magnitude of FFT
		set_magnitude(CMPLX_buff_1024, MGNTD_buff_1024, 1024);
		print_magnitude(MGNTD_buff_1024, 1024);

		// Record sample of 4KHz sine wave for 256 samples
		if(HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&ADC_buff_256, ADC_BUF_SIZE_256) != HAL_OK) {
			Error_Handler();
		}
		while (1)
		{
			if(record_done == 1){
				record_done = 0;
				break;
			}
		}

		// Complex FFT of 4KHz sample for 256 samples
		convert_to_complex(ADC_buff_256, CMPLX_buff_256, 256);
		// FFT of 4KHz sample for 256 samples
		arm_cfft_f32(&fft_handler_256, CMPLX_buff_256, 0, 1);
		// Print magnitude of FFT
		set_magnitude(CMPLX_buff_256, MGNTD_buff_256, 256);
		print_magnitude(MGNTD_buff_256, 256);

		// Calculate delta f
		printf("delta f for %d samples: %d\n", 1024, get_d_f(1024, 48e3));
		printf("delta f for %d samples: %d\n", 1024, get_d_f(256, 48e3));

		// Print peak frequencies
		print_peak(MGNTD_buff_1024, 1024);
		print_peak(MGNTD_buff_256, 256);

		/*
		 * 	Part 3
		 */

		for (int i=0; i<2; i++)
		{
			// Record sample of 3/4KHz sine wave
			if(HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&ADC_buff_1024, ADC_BUF_SIZE_1024) != HAL_OK) {
				Error_Handler();
			}
			while (1)
			{
				if(record_done == 1){
					record_done = 0;
					break;
				}
			}

			// Zero out the last 124 complex points of the ADC_buff
			for (int i=0; i<124; i++)
			{
				ADC_buff_1024[1023-i] = 0;
			}
			// Complex FFT of 3/4KHz sample with zero padding
			convert_to_complex(ADC_buff_1024, CMPLX_buff_1024, 1024);
			// FFT of 3/4KHz sample with zero padding
			arm_cfft_f32(&fft_handler_1024, CMPLX_buff_1024, 0, 1);
			// Print magnitude of FFT
			set_magnitude(CMPLX_buff_1024, MGNTD_buff_1024, 1024);
			print_magnitude(MGNTD_buff_1024, 1024);
		}

		/*
		 * 	Part 4
		 */

		// Record sample of 1KHz square wave
		if(HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&ADC_buff_1024, ADC_BUF_SIZE_1024) != HAL_OK) {
			Error_Handler();
		}
		while (1)
		{
			if(record_done == 1){
				record_done = 0;
				break;
			}
		}

		// Complex FFT of 1KHz square wave
		convert_to_complex(ADC_buff_1024, CMPLX_buff_1024, 1024);
		// FFT of 1KHz square wave
		arm_cfft_f32(&fft_handler_1024, CMPLX_buff_1024, 0, 1);
		// Print magnitude of FFT
		set_magnitude(CMPLX_buff_1024, MGNTD_buff_1024, 1024);
		print_magnitude(MGNTD_buff_1024, 1024);

		/*
		 * 	Part 5
		 */

		// Image
		float img[64][64];
		// fill the image center (16x16) with ones
		for (int row=0; row<64; row++)
			for (int col=0; col<64; col++)
				img[row][col] = ((row > 23 && row < 40) && (col > 23 && col < 40)) ? 1 : 0;

		// FFT Instance
		arm_cfft_instance_f32 fft_handler_64;
		arm_cfft_init_f32(&fft_handler_64, 64);

		for (int row=0; row<64; row++)
		{
			// Perform row FFT
			convert_to_complex(img[row], CMPLX_buff_64[row], 64);
			arm_cfft_f32(&fft_handler_64, CMPLX_buff_64[row], 0, 1);
		}
		float32_t NEW[64][128];
		// Perform col FFT
		for (int col=0; col<64; col++)
		{
			float32_t arr[128];
			for (int i=0; i<64;i++)
			{
				arr[2*i]=CMPLX_buff_64[i][2*col];
				arr[2*i+1]=CMPLX_buff_64[i][2*col+1];
			}
			// Perform col FFT
			arm_cfft_f32(&fft_handler_64, arr, 0, 1);
			for (int i=0; i<64;i++)
			{
				NEW[col][2*i]=arr[2*i];
				NEW[col][2*i+1]=arr[2* 	i+1];
			}
		}
		// Assign magnitude values
		for (int row=0; row<64; row++)
		{
			// Set magnitude of FFT
			set_magnitude(NEW[row], MGNTD_buff_64[row], 64);
		}
		while(1);
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
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

	/** Supply configuration update enable
	 */
	HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
	/** Macro to configure the PLL clock source
	 */
	__HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 24;
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
			|RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3|RCC_PERIPHCLK_ADC
			|RCC_PERIPHCLK_USB;
	PeriphClkInitStruct.PLL2.PLL2M = 1;
	PeriphClkInitStruct.PLL2.PLL2N = 19;
	PeriphClkInitStruct.PLL2.PLL2P = 3;
	PeriphClkInitStruct.PLL2.PLL2Q = 2;
	PeriphClkInitStruct.PLL2.PLL2R = 2;
	PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
	PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOMEDIUM;
	PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
	PeriphClkInitStruct.Usart234578ClockSelection = RCC_USART234578CLKSOURCE_D2PCLK1;
	PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
	PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Enable USB Voltage detector
	 */
	HAL_PWREx_EnableUSBVoltageDetector();
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void)
{

	/* USER CODE BEGIN ADC1_Init 0 */

	/* USER CODE END ADC1_Init 0 */

	ADC_MultiModeTypeDef multimode = {0};
	ADC_ChannelConfTypeDef sConfig = {0};

	/* USER CODE BEGIN ADC1_Init 1 */

	/* USER CODE END ADC1_Init 1 */
	/** Common config
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc1.Init.Resolution = ADC_RESOLUTION_16B;
	hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc1.Init.LowPowerAutoWait = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.NbrOfConversion = 1;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIG_T1_TRGO;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;
	hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_ONESHOT;
	hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
	hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
	hadc1.Init.OversamplingMode = DISABLE;
	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		Error_Handler();
	}
	/** Configure the ADC multi-mode
	 */
	multimode.Mode = ADC_MODE_INDEPENDENT;
	if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
	{
		Error_Handler();
	}
	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_15;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	sConfig.OffsetSignedSaturation = DISABLE;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN ADC1_Init 2 */

	/* USER CODE END ADC1_Init 2 */

}

/**
 * @brief ETH Initialization Function
 * @param None
 * @retval None
 */
static void MX_ETH_Init(void)
{

	/* USER CODE BEGIN ETH_Init 0 */

	/* USER CODE END ETH_Init 0 */

	/* USER CODE BEGIN ETH_Init 1 */

	/* USER CODE END ETH_Init 1 */
	heth.Instance = ETH;
	heth.Init.MACAddr[0] =   0x00;
	heth.Init.MACAddr[1] =   0x80;
	heth.Init.MACAddr[2] =   0xE1;
	heth.Init.MACAddr[3] =   0x00;
	heth.Init.MACAddr[4] =   0x00;
	heth.Init.MACAddr[5] =   0x00;
	heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
	heth.Init.TxDesc = DMATxDscrTab;
	heth.Init.RxDesc = DMARxDscrTab;
	heth.Init.RxBuffLen = 1524;

	/* USER CODE BEGIN MACADDRESS */

	/* USER CODE END MACADDRESS */

	if (HAL_ETH_Init(&heth) != HAL_OK)
	{
		Error_Handler();
	}

	memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfig));
	TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
	TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
	TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
	/* USER CODE BEGIN ETH_Init 2 */

	/* USER CODE END ETH_Init 2 */

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

	/* USER CODE BEGIN TIM1_Init 1 */

	/* USER CODE END TIM1_Init 1 */
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 0;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 2000-1;
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
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM1_Init 2 */

	/* USER CODE END TIM1_Init 2 */

}

/**
 * @brief USART3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART3_UART_Init(void)
{

	/* USER CODE BEGIN USART3_Init 0 */

	/* USER CODE END USART3_Init 0 */

	/* USER CODE BEGIN USART3_Init 1 */

	/* USER CODE END USART3_Init 1 */
	huart3.Instance = USART3;
	huart3.Init.BaudRate = 115200;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
	huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart3) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART3_Init 2 */

	/* USER CODE END USART3_Init 2 */

}

/**
 * @brief USB_OTG_FS Initialization Function
 * @param None
 * @retval None
 */
static void MX_USB_OTG_FS_PCD_Init(void)
{

	/* USER CODE BEGIN USB_OTG_FS_Init 0 */

	/* USER CODE END USB_OTG_FS_Init 0 */

	/* USER CODE BEGIN USB_OTG_FS_Init 1 */

	/* USER CODE END USB_OTG_FS_Init 1 */
	hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
	hpcd_USB_OTG_FS.Init.dev_endpoints = 9;
	hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
	hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
	hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
	hpcd_USB_OTG_FS.Init.Sof_enable = ENABLE;
	hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
	hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
	hpcd_USB_OTG_FS.Init.battery_charging_enable = ENABLE;
	hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
	hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
	if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USB_OTG_FS_Init 2 */

	/* USER CODE END USB_OTG_FS_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void)
{

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Stream0_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);

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
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(USB_OTG_FS_PWR_EN_GPIO_Port, USB_OTG_FS_PWR_EN_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : LD1_Pin LD3_Pin */
	GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : USB_OTG_FS_PWR_EN_Pin */
	GPIO_InitStruct.Pin = USB_OTG_FS_PWR_EN_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(USB_OTG_FS_PWR_EN_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : USB_OTG_FS_OVCR_Pin */
	GPIO_InitStruct.Pin = USB_OTG_FS_OVCR_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(USB_OTG_FS_OVCR_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : LD2_Pin */
	GPIO_InitStruct.Pin = LD2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void convert_to_complex(float input[64], float32_t* output, int sample_length)
{
	for (int i=0; i<sample_length*2; i++)
	{
		output[i] = (i%2 == 0) ? input[i/2] : 0;
	}
}

void print_peak(float32_t* input, int sample_length)
{
	int curr_val = 0, max_val = 0, peak = 0;
	for (int i=1; i<sample_length/2; i++)
	{
		curr_val = input[2*i];
		if (curr_val > max_val)
		{
			max_val = curr_val;
			peak = i;
		}
	}
	printf("Array length(%d): %.0f\n", sample_length, (48e3/sample_length*2*peak));
}

void print_magnitude(float32_t* input, int length)
{
	// Print magnitude of fft
	for (int i=1; i<length; i++)
	{
		sample=input[i];
		HAL_Delay(10);
	}
}

void set_magnitude(float32_t* input, float32_t* output, int length)
{
	// Set magnitude of fft
	for (int i=0; i<length; i++)
	{
		output[i]=sqrtf(powf(input[2*i],2)+powf(input[2*i+1],2));
	}
}

void transpose128(float32_t input[128][128])
{
	// Copy of input matrix
	float32_t copy[128][128];
	// Copy the input elements
	for (int row=0; row<128; row++)
	{
		for (int col=0; col<128; col++)
		{
			copy[row][col]=input[row][col];
		}
	}
	// Transpose operation
	for (int row=0; row<64; row++)
	{
		for (int col=0; col<64; col++)
		{
			// Real
			input[2*row][2*col]=copy[2*col][2*row];
			// Complex
			input[2*row][2*col+1]=copy[2*col+1][2*row];
		}
	}
}

void transpose64(float32_t input[64][64])
{
	// Copy of input matrix
	float32_t copy[64][64];
	// Copy the input elements
	for (int row=0; row<64; row++)
	{
		for (int col=0; col<64; col++)
		{
			copy[row][col]=input[row][col];
		}
	}
	// Transpose operation
	for (int row=0; row<64; row++)
	{
		for (int col=0; col<64; col++)
		{
			input[row][col]=copy[col][row];
		}
	}
}

int get_d_f(int sample_num, int sample_f)
{
	return sample_f/sample_num;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc){
	if(HAL_ADC_Stop_DMA(&hadc1) != HAL_OK){
		Error_Handler();
	}
	record_done = 1;
}

int _write(int file, char *ptr, int len)
{
	/* Implement your write code here, this is used by puts and printf for example */
	int i=0;
	for(i=0 ; i<len ; i++)
		ITM_SendChar((*ptr++));
	return len;
}
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
