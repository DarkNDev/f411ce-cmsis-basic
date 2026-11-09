#include "stm32f411xe.h"

// HSE = 25MHz, SCLK = 100MHz, PLL used
void Clock_HSE_100MHz(void) {
  // Enable HSE Clock and wait for it to be ready
  RCC->CR |= RCC_CR_HSEON;
  while (!(RCC->CR & RCC_CR_HSERDY))
    ;

  // Enable Power Interface Clock
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;

  // Configure Regulator Voltage Scaling Output
  // Bits[15:14] = 0b11 (Scale 1 Mode <=100 MHz)
  PWR->CR |= PWR_CR_VOS;

  // Configure Flash Memory Latency, Prefetch, Instruction Cache, Data Cache
  // Flash Memory Latency: Bits[3:0] = 0b0011 (3 Wait States)
  FLASH->ACR = FLASH_ACR_LATENCY_3WS | FLASH_ACR_PRFTEN | FLASH_ACR_ICEN |
               FLASH_ACR_DCEN;

  // Configure AHB Prescaler, APB High and Low Speed Prescalers
  // AHB Prescaler Bits[7:4], APB High Speed Prescaler Bits[15:13]
  // APB Low Speed Prescaler Bits[12:10]
  RCC->CFGR &= ~RCC_CFGR_HPRE;
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
  RCC->CFGR &= ~RCC_CFGR_PPRE2;

  // Configure PLLM, PLLN, PLLP, PLLQ, Enable PLL Source as HSE
  // PLLM Bits[5:0], PLLN Bits[14:6], PLLP Bits[17:16], PLLQ Bits[27:24]
  RCC->PLLCFGR = 0x00000000;
  RCC->PLLCFGR = (25U << RCC_PLLCFGR_PLLM_Pos) |
                 (200U << RCC_PLLCFGR_PLLN_Pos) | (0U << RCC_PLLCFGR_PLLP_Pos) |
                 (4U << RCC_PLLCFGR_PLLQ_Pos) | RCC_PLLCFGR_PLLSRC_HSE;

  // Enable PLL and wait for it to be ready
  RCC->CR |= RCC_CR_PLLON;
  while (!(RCC->CR & RCC_CR_PLLRDY))
    ;

  // Configure PLL as System Clock
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;

  // Wait till PLL switches as System Clock
  while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL)
    ;
}

int main(void) {
  Clock_HSE_100MHz();

  while (1) {
    // loop code
  }
}