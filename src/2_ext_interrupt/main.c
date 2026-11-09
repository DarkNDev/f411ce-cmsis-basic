#include "stm32f411xe.h"

void GPIO_Input_Init(void) {
  // Enable GPIOA clock
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

  // PA0 mode: 0=input, 1=output, 3=analog
  GPIOA->MODER &= ~(0x3U << (0 * 2U));
  GPIOA->MODER |= (0U << (0 * 2U));

  // Output type: 0=push-pull, 1=open-drain
  GPIOA->OTYPER &= ~(1U << 0);
  GPIOA->OTYPER |= (0U << 0);

  // Speed: 0=low, 1=medium, 2=fast, 3=high
  GPIOA->OSPEEDR &= ~(0x3U << (0 * 2U));
  GPIOA->OSPEEDR |= (0U << (0 * 2U));

  // Pull: 0=none, 1=pull-up, 2=pull-down
  GPIOA->PUPDR &= ~(0x3U << (0 * 2U));
  GPIOA->PUPDR |= (1U << (0 * 2U));
}

void GPIO_Output_Init(void) {
  // Enable GPIOC clock
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

  // PC13 mode: 0=input, 1=output, 3=analog
  GPIOC->MODER &= ~(0x3U << (13 * 2U));
  GPIOC->MODER |= (1U << (13 * 2U));

  // Output type: 0=push-pull, 1=open-drain
  GPIOC->OTYPER &= ~(1U << 13);
  GPIOC->OTYPER |= (0U << 13);

  // Speed: 0=low, 1=medium, 2=fast, 3=high
  GPIOC->OSPEEDR &= ~(0x3U << (13 * 2U));
  GPIOC->OSPEEDR |= (0U << (13 * 2U));

  // Pull: 0=none, 1=pull-up, 2=pull-down
  GPIOC->PUPDR &= ~(0x3U << (13 * 2U));
  GPIOC->PUPDR |= (0U << (13 * 2U));
}

void EXTI0_Init(void) {
  // Enable System Config Clock
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  // Configure System Config Controller to use PA0 as External Interrupt
  // Pin PA0: 0b0000 at Bits[3:0]
  SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;

  // Unmask the Interrupt Request Line 0 (0:Masked, 1:Unmasked)
  EXTI->IMR |= EXTI_IMR_MR0;

  // Enable Trigger at Falling Edge and Disable at Rising Edge
  EXTI->FTSR |= EXTI_FTSR_TR0;
  EXTI->RTSR &= ~EXTI_RTSR_TR0;

  // Enable the Interrupt
  NVIC_SetPriority(EXTI0_IRQn, 2);
  NVIC_EnableIRQ(EXTI0_IRQn);
}

void EXTI0_IRQHandler(void) {
  // Checking Pending Bit for Trigger Request occured
  if (EXTI->PR & EXTI_PR_PR0) {
    // Clearing Pending Bit by setting it to 1
    EXTI->PR |= EXTI_PR_PR0;
    // Interrupt Task
    GPIOC->ODR ^= (1U << 13);
  }
}

int main(void) {
  GPIO_Input_Init();
  GPIO_Output_Init();
  EXTI0_Init();

  while (1) {
    // Waiting for Interrupt
    __WFI();
  }
}