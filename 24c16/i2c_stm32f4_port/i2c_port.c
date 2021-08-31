/**@file	i2c_port.c
 * @brief   this file contain specific_port function to accelerate driver_design process
 *
 */

/*Includes*/
#include "i2c_port.h"

/*definition Ack_port as pattern used to enable master_receiver Ack during transfer */
#define ACK   ((uint16_t)0x0400)

/*definition Ack_port as pattern used to disable master_receiver Ack during transfer */
#define NACK  ((uint16_t)0xFBFF)

/*timer_cnt is a counter to handle (24c16) timing cycle between two successive stop -> start conditions */
volatile int timer_cnt =0;


/**
  * @brief  This function sets systick timer to fire every 1ms which handles writing_cycle timing between every stop and start conditions  .
  * @param  32_bit fie rate
  * @retval None
  */
void timer_interrupt_write_cycle_config(uint32_t fire_rate)
{
	SysTick_Config(fire_rate);
}

/**
  * @brief  This function serves systick interrupt handler  .
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	timer_cnt++;

}

void enable_counter(void)
{	timer_cnt=0;
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;
}
void stop_counter(void){
	while(timer_cnt!=5);
	timer_cnt=0;
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;
}
/**
 * @brief this function is used to wait for 2ms to monitor event occurrence
 * @param None
 * @revalue 1: return 1 if 2ms waiting duration has expired
 */
void Time_out_off(void)
{

	timer_cnt=0;
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;


}


/*
  * @brief  This function handles enable acknowledge .
  * @param  None
  * @retval None
 */
void enabe_ack(void){
	/*enable ack*/
	I2C3->CR1 |= ACK;

}
/*
  * @brief  This function handles stop/disable acknowledge and called before reception last word from receiver.
  * @param  None
  * @retval None
 */
void stop_ack(void){
	/*disable ack*/
	I2C3->CR1 &= NACK;

}
/**
  * @brief  This function handles the gpio_pins and i2c3 bus initialization .
  * @param  None
  * @retval None
  */
void i2c_init(void){
	/*gpio_pins initialize for SDA SCL*/
	 gpio_i2c_3();
	 /*Enable I2C peripheral clk*/
	 RCC->APB1ENR |= RCC_APB1ENR_I2C3EN;
	 /*Create i2c_struct variable to hold master configuration*/
	 I2C_InitTypeDef i2c_struct;
	 /*master run in simple mode with 100khz,with 0x00 own address,Ack is enabled,acknowledged 7bit_addressing*/
	 i2c_struct.I2C_ClockSpeed=100000;
	 i2c_struct.I2C_Mode      =I2C_Mode_I2C;
	 i2c_struct.I2C_OwnAddress1=0x00;
	 i2c_struct.I2C_Ack        =I2C_Ack_Enable;
	 i2c_struct.I2C_AcknowledgedAddress =I2C_AcknowledgedAddress_7bit;
	 /*Initialize i2c3 with these configurations*/
	 I2C_Init(I2C3,&i2c_struct);
	 /* Clock stretching enabled to avoid runtime error like overrun/underrun error*/
	 I2C3->CR1 &= ~I2C_CR1_NOSTRETCH;
	 /* Event interrupt enabled*/
	   I2C3->CR2 |= I2C_CR2_ITEVTEN;

 }

/**
  * @brief  This function handles checking master byte_ transmit flag  to send data_word.
  * @param  None
  * @retval ret_state
  */
static ret_state_port i2c3_check_TXE(void)
{

	 /*enable counter to serve time_out*/
	enable_counter();
	/* Wait TXE flag is set or 2ms as time_out */
	while((I2C_SR1_TXE != (I2C_SR1_TXE & I2C3->SR1))&&timer_cnt!=2)
	 {
		 	    /* Do nothing */
	 }
	if(timer_cnt==2)
	{
		Time_out_off();
		return TIME_OUT_PORT;
	}
	else
	{
		return NO_ERROR_PORT;

	}

}
/**
  * @brief  This function handles checking  byte_ receiver flag  to receive data_word from slave.
  * @param  None
  * @retval ret_state
  */
static ret_state_port i2c3_check_RXE(void)
{

	/*enable counter to serve time_out*/
		 enable_counter();
	   /* Wait RXE flag is set */
	 	 while(I2C_SR1_RXNE != (I2C_SR1_RXNE & I2C3->SR1)&&timer_cnt!=2)
	 	 {
	 	    /* Do nothing */
	 	 }
	 	if(timer_cnt==2)
	 	{
	 	 	Time_out_off();
	 	 	return TIME_OUT_PORT;
	 	}
	 	 else
	 	{
	 	 	return NO_ERROR_PORT;

	 	}

}

/**
  * @brief  This function handles checking Acknowledge from certain slave after master hits this slave with its own_address at the beginning of transfer  .
  * @param  None
  * @retval ret_state
  */
 static ret_state_port i2c3_check_slave_own_address_flag(void)
 {

	 /*enable counter to serve time_out*/
	 enable_counter();
 	/* Wait ADDR flag is set */
 	  while(I2C_SR1_ADDR != (I2C_SR1_ADDR & I2C3->SR1)&&timer_cnt!=2)
 	  {
 	    /* Do nothing */
 	  }
 	 if(timer_cnt==2)
 	 {
 	 	 Time_out_off();
 	 	 return TIME_OUT_PORT;
 	 }
 	 else
 	 {
 		/* Read SR1 */
 		(void)I2C3->SR1;
 		/* Read SR2 */
 		(void)I2C3->SR2;

 		 return NO_ERROR_PORT;

 	 }




 }

 /**
   * @brief  This function handles last byte transmitted   .
   * @param  None
   * @retval ret_state
   */
static ret_state_port i2c3_check_BTF(void)
{

	/*enable counter to serve time_out*/
	 enable_counter();

	 /* Wait BTF flag is set */
	   while(I2C_SR1_BTF != (I2C_SR1_BTF & I2C3->SR1)&&timer_cnt!=2)
	   {
	     /* Do nothing */
	   }
	  if(timer_cnt==2)
	   {
		  Time_out_off();
		  return TIME_OUT_PORT;
	   }
	  else
	   {
		  return NO_ERROR_PORT;

	   }

 }

/**
  * @brief  This function handles start_condition event   .
  * @param  None
  * @retval ret_state
  */
 static ret_state_port i2c3_check_start_flag(void)
 { uint16_t valx;
	 /*enable counter to serve time_out*/
	 enable_counter();
	 valx=I2C3->SR1;
	 /* Wait SB flag is set */
 	  while( I2C_SR1_SB != (I2C_SR1_SB & I2C3->SR1)&&timer_cnt!=2)
 	  {	   /**/
 	     //Do nothing
 	  }

 	 if(timer_cnt==2)
 	  {
 		 	 Time_out_off();
 	  	 	 return TIME_OUT_PORT;
 	  }
 	 else
 	  {		Time_out_off();
 	  	    /* Read SR1 */
 	  		(void)I2C3->SR1;
 	  		valx=I2C3->SR1;
 	  		 return NO_ERROR_PORT;

 	  	 }
 }
 /**
   * @brief  This function handles start_condition generation at the beginning of transfer.   .
   * @param  None
   * @retval None
   */
 ret_state_port i2c3_send_start(void)
 {
	 /* Generate START */
	   I2C3->CR1 |= I2C_CR1_START;
	   /*check start_condition occurrence event*/
	   if(i2c3_check_start_flag()!=TIME_OUT_PORT)
	   { /*if it set  */
		   return NO_ERROR_PORT;
	   }
	   /*if it not set*/
	   return TIME_OUT_PORT;
}
 /**
   * @brief  This function handles re-start_condition generation at  any point of transfer.   .
   * @param  None
   * @retval None
   */
 ret_state_port i2c3_send_REstart(void)
 {
	 /*waiting for last byte transfer*/
	 if(i2c3_check_BTF()==TIME_OUT_PORT)
	 {
		 return TIME_OUT_PORT;
	 }
	 /*generate start condition*/
	 if(i2c3_send_start()==TIME_OUT_PORT)
	 {
		 return TIME_OUT_PORT;
	 }
	 return NO_ERROR_PORT;

 }
 /**
   * @brief  This function handles sending own device_address(EEprom) with write cmd .   .
   * @param  device_own address
   * @retval ret_state_port
   */
 ret_state_port send_devic_address_write(uint8_t add){
	/*send address with wite cmd represented by reseting bit 0 of add*/
	I2C3->DR =  (uint8_t)add;
	/*wait for ack*/
	if(i2c3_check_slave_own_address_flag()!=TIME_OUT_PORT)
	{
		return NO_ERROR_PORT;
	}
	return TIME_OUT_PORT;
}

/**
  * @brief  This function handles sending data( entire memory address  ) to slave during the transfer.
  * @param  data(memory location_address) to be sent
  * @retval ret_state_port
  */
ret_state_port send_devic_address_data(uint8_t data_add){
	/*check sending flag(data transfered from data register to shift_register) before assign data_add to be sent  to data register(DR)*/
	if(i2c3_check_TXE()!=TIME_OUT_PORT)
	{
		/*assign data_add to be sent  to data register(DR)*/
		I2C3->DR = data_add;
		return NO_ERROR_PORT;
	}

	return TIME_OUT_PORT;
}

/**
  * @brief  This function handles sending data( data_word  ) to slave during the transfer.
  * @param  data(data_word) to be sent
  * @retval None
  */

ret_state_port send_data(uint8_t data){
	/*check sending flag(data transfered from data register to shift_register) before assign data_add to be sent  to data register(DR)*/
	if(i2c3_check_TXE()!=TIME_OUT_PORT)
	{
		/*assign data_add to be sent  to data register(DR)*/
		I2C3->DR = data;
		return NO_ERROR_PORT;
	}

	return TIME_OUT_PORT;
}

/**
  * @brief  This function handles sending own device_address(EEprom) with READ cmd to be used at the beginning.of transfer or at mid of combined frame.
  * @param  device_own address
  * @retval ret_state_port
  */
ret_state_port send_devic_address_read(uint8_t add){
	/*setting the first bit of the address indicating READ from slave cmd*/
	add|=1;
	I2C3->DR =  add;
	/*wait for ack*/
	if(i2c3_check_slave_own_address_flag()!=TIME_OUT_PORT)
	{

		return NO_ERROR_PORT;
	}

	return TIME_OUT_PORT;


}

/**
  * @brief  This function handles master stop generation after transmitting operation get finished .
  * @param  None
  * @retval ret_state_port
 */
ret_state_port i2c3_send_stop_transmit(void){
	/*check if data register empty*/
	if(i2c3_check_TXE()==TIME_OUT_PORT)
	{
		return TIME_OUT_PORT;
	}
	/*check if last byte has been transmitted and no date in data register*/
	if(i2c3_check_BTF()==TIME_OUT_PORT)
	{
		return TIME_OUT_PORT;
	}
	/*hit stop condition to terminate the transfer*/
	I2C3->CR1 |= I2C_CR1_STOP;
	return NO_ERROR_PORT;
}

/**
  * @brief  This function handles master stop generation after receiving operation get finished .
  * @param  None
  * @retval None
  */
void i2c3_send_stop_reciever(void){

	I2C3->CR1 |= I2C_CR1_STOP;
}

/**
  * @brief  This function handles receiving data_word from slave .
  * @param  None
  * @retval 8bit data_wart
  */

ret_state_data_port i2c3_data_read(void)
{
	ret_state_data_port reciver;
	/*check receiving flag/transferring data_word from shift reg to data red*/
	if(i2c3_check_RXE()!=TIME_OUT_PORT)
	{
		/*read DR register an return its value*/
		reciver.data=(uint8_t)I2C3->DR;
		reciver.state_port=NO_ERROR_PORT;
		return reciver;
	}
	reciver.state_port=TIME_OUT_PORT;
	return reciver;

}
