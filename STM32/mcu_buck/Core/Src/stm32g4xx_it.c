/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32g4xx_it.c
  * @brief   Interrupt Service Routines.
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
#include "stm32g4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern DMA_HandleTypeDef hdma_adc3;
extern FMAC_HandleTypeDef hfmac;
/* USER CODE BEGIN EV */
extern HRTIM_HandleTypeDef hhrtim1;
extern volatile uint32_t new_adc_offset;
extern volatile uint8_t update_offset_flag;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32G4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 channel1 global interrupt.
  */
void DMA1_Channel1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel1_IRQn 0 */

  /* USER CODE END DMA1_Channel1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_adc3);
  /* USER CODE BEGIN DMA1_Channel1_IRQn 1 */

  /* USER CODE END DMA1_Channel1_IRQn 1 */
}

/**
  * @brief This function handles FMAC interrupt.
  */
void FMAC_IRQHandler(void)
{
  /* USER CODE BEGIN FMAC_IRQn 0 */
	// Sprawdzamy czy przerwanie pochodzi od gotowych danych (YEMPTY)
	if((FMAC->SR & FMAC_SR_YEMPTY) == 0)
	{
		// 1. Odczyt rejestru RDATA zdejmuje sprzętową flagę przerwania
		int16_t control_effort_q15 = (int16_t)READ_REG(FMAC->RDATA);
		//control_effort_q15 = 0;

		// BEZPIECZNE OKNO CZASOWE: ADSTART = 0. Dokonujemy aktualizacji!
		if (update_offset_flag)
		{
			ADC3->OFR1 &= ~ADC_OFR1_OFFSET1_EN;
			// Wrzucamy czyste bity bez przesunięcia, jak słusznie zauważyłeś!
			ADC3->OFR1 = (ADC3->OFR1 & ~ADC_OFR1_OFFSET1_Msk) | new_adc_offset;
			ADC3->OFR1 |= ADC_OFR1_OFFSET1_EN;

			update_offset_flag = 0; // Zadanie wykonane
		}

		// 2. Skalowanie
		int32_t pwm_base = 21760 * 0.2f; // 20%
		int32_t duty_change = ((int32_t)control_effort_q15 * 21760) >> 15;
		int32_t new_ton = pwm_base + duty_change;

		// 3. Saturacja
		if (new_ton < 0)   new_ton = 0;
		if (new_ton > 19584) new_ton = 19584;

/////////////////////////

//		//BALANSOWANIE FAZ
		int32_t current_ph1 = (int32_t)(ADC2->DR);
		int32_t current_ph2 = (int32_t)(ADC1->DR);
//		int32_t current_ph1 = 1000;
//		int32_t current_ph2 = 3000;

		// Błąd prądu
		int32_t i_error = current_ph1 - current_ph2;

		// Podział poprawki (jeszcze mniejsze wzmocnienie, np. >> 2 co daje Kp=0.25)
		int32_t balance_effort = i_error >> 2;

		if (balance_effort > 300) balance_effort = 300;
		if (balance_effort < -300) balance_effort = -300;

		// Faza 1 - Odejmujemy połowę błędu
		HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP1xR = new_ton - balance_effort;

		// Faza 2 - Dodajemy połowę błędu
		uint32_t reset_b = (21760 / 2) + new_ton + balance_effort;
		HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP1xR = reset_b;


////////////////////////

		// Wyliczenie połowy Toff pomiaru prądu
		uint32_t half_toff = (21760 - new_ton) / 2;
		HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_A].CMP3xR = new_ton + half_toff;
		HRTIM1->sTimerxRegs[HRTIM_TIMERINDEX_TIMER_B].CMP4xR = (reset_b + half_toff) % 21760;

	}
	return;
  /* USER CODE END FMAC_IRQn 0 */
  HAL_FMAC_IRQHandler(&hfmac);
  /* USER CODE BEGIN FMAC_IRQn 1 */

  /* USER CODE END FMAC_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
