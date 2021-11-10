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
#include "string.h"

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

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#include <stdio.h>
#include "malloc.h"
#include "arm_math.h"
#include "nn.h"

#define ARM_MATH_CM7
#define __FPU_PRESENT_1
#define ADC_BUF_SIZE 1024
#define K ADC_BUF_SIZE/2+1

float MFCC[13];
uint16_t ADC_buff[ADC_BUF_SIZE];
int record_done = 0;
float mag_print;
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
	// Start TIM1
	if(HAL_TIM_Base_Start(&htim1) != HAL_OK){
		Error_Handler();
	}
	// FFT handler initialization
	arm_cfft_instance_f32 fft_handler;
	arm_cfft_init_f32(&fft_handler, ADC_BUF_SIZE);
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		HAL_Delay(1000);
		// Start sampling
		if(HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&ADC_buff, ADC_BUF_SIZE) != HAL_OK){
			Error_Handler();
		}

		while(1){
			if(record_done == 1){
				record_done = 0;
				break;
			}
		}
		/*
		// Print voice signal
		for (int i=0; i<ADC_BUF_SIZE; i++)
		{
			mag_print = ADC_buff[i];
			HAL_Delay(10);
		}
		 */
		/*
		 *  STEP 1: Window filtering
		 */
		// Subtract the average value
		float avg_val;
		for(int i=0; i<ADC_BUF_SIZE; i++)
		{
			avg_val += ADC_buff[i];
		}
		avg_val /= ADC_BUF_SIZE;
		// Window filtering
		float window[ADC_BUF_SIZE];
		for(int i=0; i<ADC_BUF_SIZE; i++)
		{
			window[i] = 0.54 - 0.46 * cos(2*M_PI*i / (ADC_BUF_SIZE-1));
		}
		/*
		 * STEP 2: Periodogram computation
		 */
		// Convert to complex array
		float* complex_input = (float*)malloc(ADC_BUF_SIZE*2*sizeof(float));
		for(int i = 0; i< ADC_BUF_SIZE; i++)
		{
			complex_input[2*i] = ((float)ADC_buff[i] - avg_val) * window[i];
			complex_input[2*i + 1] = 0;
		}
		// Perform FFT
		arm_cfft_f32(&fft_handler, complex_input, 0, 1);
		/*
		// Print FFT
		for(int i = 0; i < 2048; i++){
			printf("%.3f \n", complex_input[i]);
			HAL_Delay(5);
		}
		 */
		// K points of the magnitude squared
		float mag_sq_input[ADC_BUF_SIZE] = {0};
		for(int i=0; i<ADC_BUF_SIZE; i++)
		{
			mag_sq_input[i] = powf(complex_input[2*i],2)+powf(complex_input[2*i+1],2);
		}
		float K_input[K];
		for(int i=0; i<K; i++)
		{
			K_input[i] = mag_sq_input[i];
		}
		/*
		 *  STEP 3: Filter bank construction
		 */
		// Mel freq based on phi_0 and phi_(M+1)
		float f_mel[28] = {0};
		f_mel[0] 	= 2595 * log10f(1+250/700.f);
		f_mel[27] = 2595 * log10f(1+8e3/700.f);
		float f_mel_div = (f_mel[27]-f_mel[0])/27;
		// M frequencies and inverse mapping
		float f_phi[28] = {0};
		for(int i=0; i<28; i++)
		{
			f_mel[i] = f_mel[0] + f_mel_div * i;
			f_phi[i] = 700 * (powf(10, f_mel[i]/2595) - 1);
		}
		// Lambda calculation
		float lambda[28] = {0};
		for(int i=0; i<28; i++)
		{
			lambda[i] = (int)(513 * f_phi[i] / 8e3);
		}
		// Construction of the filter bank
		float filter[26][K] = {0};
		for(int filter_index=0; filter_index<26; filter_index++) // iteration per filter
		{
			float f_min_index = lambda[filter_index];
			float f_mid_index = lambda[filter_index+1];
			float f_max_index = lambda[filter_index+2];
			for(int f_PSD_index=0; f_PSD_index<K; f_PSD_index++) // iteration per PSD frequency
			{
				// f < lambda_(m-1) OR f > lambda_(m+1)
				if(f_PSD_index < f_min_index || f_PSD_index > f_max_index)
				{
					filter[filter_index][f_PSD_index] = 0;
				}
				// 0 to 1 linear interpolation lambda_(m-1) <= f <= lambda_(m)
				else if(f_PSD_index >= f_min_index && f_PSD_index <= f_mid_index)
				{
					filter[filter_index][f_PSD_index] = (f_PSD_index - f_min_index ) / (f_mid_index - f_min_index);
				}
				// 1 to 0 linear interpolation lambda_(m) <= f <= lambda_(m+1)
				else if(f_PSD_index >= f_mid_index && f_PSD_index <= f_max_index)
				{
					filter[filter_index][f_PSD_index] = 1 - (f_PSD_index - f_mid_index ) / (f_max_index - f_mid_index);
				}
			}
		}
		/*
		// Print filter bank
		for(int i=0; i<513; i++)
		{
			for(int j=0; j<26; j++)
			{
				mag_print = filter[j][i];
				HAL_Delay(1);
			}
			HAL_Delay(20);
		}
		 */
		/*
		 *  STEP 4 & 5: Filter bank application & Logarithmic energy output
		 */
		float K_energy[26] = {0};
		for(int i=0; i<26; i++) // filter
		{
			for(int j=0; j<K; j++) // K elements
			{
				K_energy[i] += K_input[j] * filter[i][j];
			}
			K_energy[i] = log10f(K_energy[i]);
		}
		/*
		 *  STEP 6: MFCC values
		 */
		for(int i=0; i<13; i++) // MFCC index
		{
			for(int j=0; j<26; j++) // Energy element
			{
				MFCC[i] += K_energy[j] * cos((i+1) * ((j+1) - 0.5) * M_PI / 26);
			}
		}
		/*
		// Print MFCCs
		for(int i=0; i<13; i++)
		{
			printf("%.6f", MFCC[i]);
			if (i < 12)
			{
				printf(", ");
			}
			HAL_Delay(5);
		}
		printf("\n");
		*/
		/*
		 *  NN Forward Propagation
		 */
		double hidden_1[SIZE_1] = {0};
		double probability[4] = {0};
		// preprocessing
		for (int i = 0; i < 13; i++)
			MFCC[i] = ((MFCC[i] - x1_offset[i]) * x1_gain[i]) + x1_ymin;
		// fully connected layers
		dense_Sig(MFCC, hidden_1, 13, SIZE_1);
		dense_S(hidden_1, probability, SIZE_1, 4);
		// print out prediction
		printf("Probability of 'ah': %.10f\n", probability[0]);
		printf("Probability of 'eh': %.10f\n", probability[1]);
		printf("Probability of 'ee': %.10f\n", probability[2]);
		printf("Probability of 'oo': %.10f\n", probability[3]);
		// Zero out the MFCC
		for(int i=0; i<13; i++)
		{
			MFCC[i] = 0;
		}
		// Free allocated memory
		free(complex_input);
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
	htim1.Init.Period = 6000-1;
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
	HAL_GPIO_WritePin(GPIOB, LD3_Pin, GPIO_PIN_SET);
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
