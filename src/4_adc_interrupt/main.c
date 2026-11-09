#include <stdint.h>

#include "cmsis_gcc.h"
#include "stm32f411xe.h"

volatile uint16_t adc_val = 0;

void ADC1_Interrupt_Init(void) {
  // Enable GPIOA Clock and ADC Clock
  RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

  // Sets bits [3:2] to 0b11 (Analog mode, disables digital Schmitt trigger)
  GPIOA->MODER |= (3U << (1 * 2));

  // PA1 mode: 0=input, 1=output, 3=analog
  GPIOC->MODER |= (3U << (1 * 2U));

  // Configuring ADC Prescaler Bits[17:16]
  // Set to 0b01: APB2 clock divided by 4 (100 MHz / 4 = 25 MHz <= 36 MHz limit)
  ADC->CCR &= ~ADC_CCR_ADCPRE;
  ADC->CCR |= ADC_CCR_ADCPRE_0;

  // Configuring ADC Regular Channel Sequence length
  // SQR1 bits [23:20] (L[3:0]): sequence length. 0b0000 = 1 conversion
  ADC1->SQR1 &= ~ADC_SQR1_L;

  // Configurinhg ADC Channel Sequence for 1st Conversion Sequence
  // SQR3 bits [4:0] (SQ1[4:0]): 1st conversion in sequence = Channel 1 (PA1)
  ADC1->SQR3 &= ~ADC_SQR3_SQ1;
  ADC1->SQR3 |= (1U << ADC_SQR3_SQ1_Pos);

  // Configuring Sample Time for Channel 1
  // SMPR2 bits [5:3] (SMP1[2:0]): Sample time for channel 1 0b100 = 84 ADC
  // clock cycles
  ADC1->SMPR2 |= (4U << ADC_SMPR2_SMP1_Pos);

  // Enable Interrupt for End of Conversion (EOC)
  ADC1->CR1 |= ADC_CR1_EOCIE;

  // Configure Cortex-M NVIC
  NVIC_SetPriority(ADC_IRQn, 2); // Set priority
  NVIC_EnableIRQ(ADC_IRQn);      // Enable ADC global interrupt line

  // Turn ON ADC Converter
  ADC1->CR2 |= ADC_CR2_ADON;
}

void ADC1_Start_Conversion(void) {
  // Start conversion by setting bit 30 (SWSTART = 1) in CR2
  ADC1->CR2 |= ADC_CR2_SWSTART;
}

// Global ADC Interrupt Handler
void ADC_IRQHandler(void) {
  // Check if End of Conversion (EOC) flag is set
  if (ADC1->SR & ADC_SR_EOC) {
    // Reading DR extracts the data and automatically clears the EOC flag
    uint16_t new_adc_val = (uint16_t)(ADC1->DR);
    if (new_adc_val != adc_val) {
      adc_val = new_adc_val;
    }
    ADC1_Start_Conversion();
  }
}

int main(void) {
  ADC1_Interrupt_Init();

  // Trigger initial conversion or trigger periodically from a timer
  ADC1_Start_Conversion();

  while (1) {
    // Waiting for Interrupt
    __WFI();
  }
}