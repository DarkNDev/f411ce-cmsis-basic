#include "stm32f411xe.h"

void delay(volatile uint32_t count) {
  while (count--) {
    // No Operation instruction for simple software delay
    __NOP();
  }
}

int main(void) {
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

  while (1) {
    // Toggle (XOR)
    GPIOC->ODR ^= GPIO_ODR_OD13;

    // random delay
    delay(500000);
  }
}