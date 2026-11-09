#include "stm32f411xe.h"

void TIM2_PWM_PA0_Init(void) {
  // Enable Clocks for GPIOA and TIM2
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

  // Configure PA0 as Alternate Function Mode (MODER = 10)
  GPIOA->MODER &= ~(3U << (0 * 2));
  GPIOA->MODER |= (2U << (0 * 2));

  // Select AF01 (TIM2) for PA0 in Alternate Function Low Register (AFR[0])
  GPIOA->AFR[0] &= ~(0xFU << (0 * 4));
  GPIOA->AFR[0] |= (0x01U << (0 * 4));

  // Timebase: 1 kHz PWM frequency at 100 MHz TIMCLK
  TIM2->PSC = 100 - 1;  // Timer ticks at 1 MHz
  TIM2->ARR = 1000 - 1; // Period = 1000 ticks = 1 ms (1 kHz)

  // Initial Duty Cycle (e.g., 25% -> 250 / 1000)
  TIM2->CCR1 = 250;

  // Set Channel 1 to PWM Mode 1 (OC1M = 110) & Enable Preload (OC1PE = 1)
  TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;
  TIM2->CCMR1 |= (6U << TIM_CCMR1_OC1M_Pos); // Mode 1: Active while CNT < CCR1
  TIM2->CCMR1 |= TIM_CCMR1_OC1PE;            // Preload register enabled

  // Enable Output for Channel 1 (active-high polarity by default)
  TIM2->CCER |= TIM_CCER_CC1E;

  // Enable Auto-reload preload (ARPE) and start counter
  TIM2->CR1 |= TIM_CR1_ARPE;
  TIM2->CR1 |= TIM_CR1_CEN;
}

int main(void) {
  TIM2_PWM_PA0_Init();

  while (1) {
    // CPU has zero work to do. Hardware handles the PWM continuously.
    __WFI(); // Wait for Interrupt / sleep mode to save power
  }
}