#include "stm32f411xe.h"

volatile uint16_t adc_val = 0;

void ADC1_Init(void) {
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

  // Turn ON ADC Converter
  ADC1->CR2 |= ADC_CR2_ADON;
}

uint16_t ADC1_Read(void) {
  // 1. Start conversion by setting bit 30 (SWSTART = 1) in CR2
  ADC1->CR2 |= ADC_CR2_SWSTART;

  // 2. Wait until conversion is complete (poll bit 1, EOC in SR)
  while (!(ADC1->SR & ADC_SR_EOC))
    ;

  // 3. Return 12-bit result (0 to 4095) from DR
  // Reading ADC1->DR automatically clears the EOC bit in ADC1->SR
  return (uint16_t)(ADC1->DR);
}

int main() {
  ADC1_Init();

  while (1) {
    // Read conversion value
    adc_val = ADC1_Read();

    // Small random delay between reads
    for (volatile int i = 0; i < 100000; i++)
      ;
  }
}