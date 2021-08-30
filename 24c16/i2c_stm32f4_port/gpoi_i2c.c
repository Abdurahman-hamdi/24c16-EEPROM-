#include "gpoi_i2c.h"


/**
  * @brief  This function initialize gpio_pins for i2c3 peripheral
  * @param  None
  * @retval None
  */
void gpio_i2c_3 (void){
	/*i2c3 --->SCL at PA8....SDA at PC9*/
	/* Enable port A and C clocks */
	  RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOCEN);

	  /* Select Alternate function mode */
	  GPIOA->MODER &= ~GPIO_MODER_MODER8;
	  GPIOA->MODER |= GPIO_MODER_MODER8_1;
	  GPIOC->MODER &= ~GPIO_MODER_MODER9;
	  GPIOC->MODER |= GPIO_MODER_MODER9_1;

	  /* Select output type open drain */
	  GPIOA->OTYPER |= GPIO_OTYPER_OT_8;
	  GPIOC->OTYPER |= GPIO_OTYPER_OT_9;

	  /* Select output speed very high */
	  GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR8;
	  GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR8_0 | GPIO_OSPEEDER_OSPEEDR8_1;
	  GPIOC->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR9;
	  GPIOC->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9_0 | GPIO_OSPEEDER_OSPEEDR9_1;

	  /* Select pull-up */
	  GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR8;
	  GPIOA->PUPDR |= GPIO_PUPDR_PUPDR8_0;
	  GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR9;
	  GPIOC->PUPDR |= GPIO_PUPDR_PUPDR9_0;

	  //alternative function for portA,C, pins 8 and 9
	   GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_I2C3);
	   GPIO_PinAFConfig(GPIOC, GPIO_PinSource9, GPIO_AF_I2C3);

}
