/***
 * @file	EEPROM.C
 * @brief	this file contains implementation for APIS used to interface 24c16 EEOROM
 */


/*INCLUDES*/
#include "EEPROM.h"

/*Enum to track data transfer_State*/
typedef enum{
	START,
	SEND_HW_ADD,
	SEND_1ST_ADD,
	RE_START_READ,
	RECIEVE_DATA,
	TRANSMITE_DATA,
	STOP,
	FAILED
}I2c_eeprom_State;


/*Defining  ret_st variable to be returned by APIs*/
static operation_state ret_st;

/*Defining  ret_st variable to be returned by APIs*/
static I2c_eeprom_State i2c_state_trans=START;


/**
 * @brief this function handles 24lc16(EEPROM) interface initialization
 * @param  None
 * @retval  None
 */
void eeprom_init (void){
	/*i2c bus initialization*/
	i2c_init();
	/* Set timer_inerruption to fire every 1ms to handle write_cycle timing */
	timer_interrupt_write_cycle_config(FIRE_1MS);
}

/**
 * @brief this function handles Wtiting byte to 24lc16(EEPROM)
 * @param 	data:data_byte to be written
 * @param 	byte_add:memory location at which data_byte will be written
 * 			important note about this param is that memory location value must be eight bit
 * 			its value should be between (0x00......0xfE) as 24lc16 is divided into eigh blocks
 * 			each block contains 256 byte with (0x00......0xfE) addresses relative to each block
 * @param	device_address_bn: defined at EEPROM.h ,it is device_address
 * 			which encoded as [fixed pattern(0b1010)+ (block_num)(0bxxx)+initial(0b0)].
 * @retval  ret_st: struct of type operation_state indicates the statues of transfer
 */
operation_state eeprom_byte_write(uint8_t data,uint8_t byte_add,uint8_t device_address_bn)
{
	/*check the validity of eeprom devic_address bit0 should be 0 */
	if((uint8_t)(device_address_bn<<7)==0)
	{

	}
	else
	{
		ret_st.check=DEVIC_ADDRESS_INVALID;
		return ret_st;
	}

	do{
		switch(i2c_state_trans)
		{	/*send start*/
			case START:
				if(i2c3_send_start()!=TIME_OUT_PORT)
				{	/*if no time_error then set state to send device_address_bn*/
					i2c_state_trans=SEND_HW_ADD;
					break;
				}
				/*move to FAILED if time_error is set*/
				i2c_state_trans=FAILED;
				break;

			/*send device_address_bn*/
			case SEND_HW_ADD:
				if(send_devic_address_write(device_address_bn)!=TIME_OUT_PORT)
				{
					i2c_state_trans=SEND_1ST_ADD;
					break;
				}
				i2c_state_trans=FAILED;
				break;

			/*send page_relative byte address with write command*/
			case SEND_1ST_ADD:
				if(send_devic_address_data(byte_add)!=TIME_OUT_PORT)
				{
					i2c_state_trans=TRANSMITE_DATA;
					break;
				}
				i2c_state_trans=FAILED;
				break;

			/*send data_byte to be sent*/
			case TRANSMITE_DATA:
				if(send_data(data)==TIME_OUT_PORT)
				{
					i2c_state_trans=FAILED;
					break;
				}
				i2c_state_trans=STOP;
				/*NO breaks statement add here to move directly to stop case
				 * */
			/*terminate the transfer by hitting stop*/
			case STOP:
				if(i2c3_send_stop_transmit()!=TIME_OUT_PORT)
				{
					break;
				}
				i2c_state_trans=FAILED;
				/*NO breaks statement add here to move directly to failed case
				 * if i2c3_send_stop_transmit()==TIME_OUT_PORT
				 */

			/*handles time_out occurrence*/
			case FAILED:
				ret_st.check=TIME_OUT;
				return ret_st;
		}

	}
	while(i2c_state_trans!=STOP);

	if(ret_st.check !=TIME_OUT && ret_st.check !=DEVIC_ADDRESS_INVALID)
	{	/*NO_ERROR during the transfer wait for 5ms write cycle*/
		enable_counter();
		stop_counter();
		ret_st.check=NO_ERROR;
	}
	i2c_state_trans=START;

	return ret_st; /* transfer ends successfully */
}
/**
 *@brief eeprom_page_write  writing array of bytes.
 *@param1 data:pointer to the arrayof bytes to be sent.
 *@param2 page_num:page number at with the array will be written.
 *@param2 device_address_bn:  device_address_bn is pattern of 0b(1010 xxx(block_n) 0).
 *@param4 size:length of array to be written
 *@retval ret_st: struct of type operation_state indicates the statues of transfer
 */
operation_state eeprom_page_write(uint8_t*data,uint8_t page_num,uint8_t device_address_bn,uint8_t size)
{
		/*check the validity of eeprom_address */
	/*check the validity of eeprom_device address */
		if((uint8_t)(device_address_bn<<7)==0)
		{

		}
		else
		{
			ret_st.check=DEVIC_ADDRESS_INVALID;
			return ret_st;
		}

		 do
		 {
			 switch(i2c_state_trans)
			 {
			 	 case START:
			 		 if(i2c3_send_start()!=TIME_OUT_PORT)
			 		 {
			 			 i2c_state_trans=SEND_HW_ADD;
			 			 break;
			 		 }
			 		 i2c_state_trans=FAILED;
			 		 break;

			 	 case SEND_HW_ADD:
			 		 if(send_devic_address_write(device_address_bn)!=TIME_OUT_PORT)
			 		 {
			 			 i2c_state_trans=SEND_1ST_ADD;
			 			 break;
			 		 }
			 		 i2c_state_trans=FAILED;
			 		 break;

			 	 case SEND_1ST_ADD:
			 		 if(send_devic_address_data(page_num)!=TIME_OUT_PORT)
			 		 {
			 			 i2c_state_trans=TRANSMITE_DATA;
			 			 break;
			 		 }
			 		 i2c_state_trans=FAILED;
			 		 break;

			 	 /*Writing to eeprom */
			 	 case TRANSMITE_DATA:
			 		 if(send_data(*data++)!=TIME_OUT_PORT)
			 		 {
			 			 if(size-->1)
			 			 {
			 				 i2c_state_trans=TRANSMITE_DATA;
			 				 break;
			 			 }
			 			 /*once the array is wrote move to stop state */
			 			 i2c_state_trans=STOP;
			 		 }
			 		 else
			 		 {
			 			 i2c_state_trans=FAILED;
			 			 break;
			 		 }

			 	 case STOP:
			 		 if(i2c3_send_stop_transmit()!=TIME_OUT_PORT)
			 		 {
			 			 break;
			 		 }
			 		 i2c_state_trans=FAILED;
			 		 /*NO breaks statement add here to move directly to failed case
			 		  * if i2c3_send_stop_transmit()==TIME_OUT_PORT
			 		  */

			 	/*handles time_out occurrence*/
			 	 case FAILED:
			 		 ret_st.check=TIME_OUT;
			 		 return ret_st;
			 }

		 }
		 while(i2c_state_trans!=STOP);

		 if(ret_st.check !=TIME_OUT && ret_st.check !=DEVIC_ADDRESS_INVALID)
		 {	/*NO_ERROR during the transfer wait for 5ms write cycle*/
			 enable_counter();
			 stop_counter();
			 ret_st.check=NO_ERROR;
		 }
		 i2c_state_trans=START;
		 return ret_st;
}
/**
 *@brief eeprom_byte_read handles read byte from eeprom operation
 * @param 	byte_add:memory location at which data_byte will be read
 * 			important note about this param is that memory location value must be eight bit
 * 			its value should be between (0x00......0xfE) as 24lc16 is divided into eigh blocks
 * 			each block contains 256 byte with (0x00......0xfE) addresses relative to each block
 * @param	device_address_bn: defined at EEPROM.h ,it is device_address
 * 			which encoded as [fixed pattern(0b1010)+ (block_num)(0bxxx)+initial(0b0)].
 * @retval  ret_st: struct of type operation_state indicates the statues of transfer and data that is read from eeprom
 */

operation_state eeprom_byte_read(uint8_t byte_add,uint8_t device_address_bn){
		ret_state_data_port received_info ;
	   /* check the validity of eeprom_address*/
		if((uint8_t)(device_address_bn<<7)==0)
		{

		}
		else{
			ret_st.check=DEVIC_ADDRESS_INVALID;
			return ret_st;
		}

		do{
			 switch(i2c_state_trans)
			 {
			 	 case START:
			 		 if(i2c3_send_start()!=TIME_OUT_PORT)
			 		 {
			 			 i2c_state_trans=SEND_HW_ADD;
			 			 break;
			 		 }
			 		 i2c_state_trans=FAILED;
			 		 break;

			 	 case SEND_HW_ADD:
			 		 if(send_devic_address_write(device_address_bn)!=TIME_OUT_PORT)
			 		 {
			 			 i2c_state_trans=SEND_1ST_ADD;
			 			 break;
			 		 }
			 		 i2c_state_trans=FAILED;
			 		 break;

			 	 case SEND_1ST_ADD:
			 		 if(send_devic_address_data(byte_add)!=TIME_OUT_PORT)
			 		 {
			 			 i2c_state_trans=RE_START_READ;
			 			 break;
			 		 }
			 		 i2c_state_trans=FAILED;
			 		 break;

			 	 case RE_START_READ:
			 		 if(i2c3_send_REstart()!=TIME_OUT_PORT)
			 		 {	/*stop sending ack to eeprom before receiving the byte ,this stops eeprom transmitting any byte after the byte the ack is disabled before it arrives to the master*/
			 			 stop_ack();
			 			 if(send_devic_address_read(device_address_bn)!=TIME_OUT_PORT)
			 			 {
			 				i2c_state_trans=RECIEVE_DATA;
			 				 break;
			 			 }

			 		 }
			 		i2c_state_trans=FAILED;
			 		break;
			 	 case RECIEVE_DATA:
			 		 /*read the byte and transfer state*/
			 		 received_info=i2c3_data_read();
			 		 if(received_info.state_port!=TIME_OUT_PORT)
			 		 {	/*if no time_out then read the byte*/
			 			 ret_st.data_read=received_info.data;
			 			 i2c_state_trans=STOP;
			 		 }
			 		 else
			 		 {
			 			 i2c_state_trans=FAILED;
			 			 break;
			 		 }

			 	 case STOP:
			 		 /*terminate transfer by hitting stop*/
			 		 i2c3_send_stop_reciever();
			 		 /*re_enable master_receiver acknowledge*/
			 		 enabe_ack();
			 		 break;

			 		 /*handles time_out occurrence*/
			 	 case FAILED:
			 		 ret_st.check=TIME_OUT;
			 		 return ret_st;

			 }

		}
		while(i2c_state_trans!=STOP);

		if(ret_st.check !=TIME_OUT && ret_st.check !=DEVIC_ADDRESS_INVALID)
		{	/*NO_ERROR during the transfer wait for 5ms write cycle*/
			enable_counter();
			stop_counter();
			ret_st.check=NO_ERROR;
		}
		i2c_state_trans=START;
		return ret_st;
}

/**
 * @brief   eeprom_byte_read handles read multiple bytes from eeprom operation.
 * @param1 data: pointer to array to hold data read from eeprom.
 * @param2 page_num:page number from which the  array will be read.
 * @param3 device_address_bn: defined at EEPROM.h ,it is device_address
 * 			which encoded as [fixed pattern(0b1010)+ (block_num)(0bxxx)+initial(0b0)].
 * @param4 size:length of byte read from eeprom
 * @retval  ret_st: struct of type operation_state indicates the statues of transfer .
 */

operation_state eeprom_page_read(uint8_t*data,uint8_t page_num,uint8_t device_address_bn,uint8_t size)
{
	ret_state_data_port received_info ;
	//check the validity of eeprom_address
	if((uint8_t)(device_address_bn<<7)==0)
	{

	}
	else
	{
		ret_st.check=DEVIC_ADDRESS_INVALID;
		return ret_st;
	}

	do
	{
		switch(i2c_state_trans)
		{
			case START:
				if (i2c3_send_start()!=TIME_OUT_PORT)
				{
					i2c_state_trans=SEND_HW_ADD;
					break;
				}
				i2c_state_trans=FAILED;
				break;

			case SEND_HW_ADD:
				if(send_devic_address_write(device_address_bn)!=TIME_OUT_PORT)
				{
					i2c_state_trans=SEND_1ST_ADD;
					break;
				}
				i2c_state_trans=FAILED;
				break;

			case SEND_1ST_ADD:
				if(send_devic_address_data(page_num)!=TIME_OUT_PORT)
				{
					i2c_state_trans=RE_START_READ;
					break;
				}
				i2c_state_trans=FAILED;
				break;

			case RE_START_READ:

				if(i2c3_send_REstart()!=TIME_OUT_PORT)
				{

					if(send_devic_address_read(device_address_bn)!=TIME_OUT_PORT)
					{
						i2c_state_trans=RECIEVE_DATA;
						break;
					}

				}
				i2c_state_trans=FAILED;
				break;

			case RECIEVE_DATA:
				if(size-->0)
				{
					received_info=i2c3_data_read();
					if(received_info.state_port != TIME_OUT_PORT)
					{
						*(data++)=received_info.data;
						if(size==1)
						{   /*stop master reciver ack before reading last byte this stops eeprom transmitting any byte after the byte the ack is disabled before it arrives to the master */
							stop_ack();
						}
						i2c_state_trans=RECIEVE_DATA;
						break;
					}
					else/*if time_out occurred*/
					{
						i2c_state_trans=FAILED;
						break;
					}

				}
				/*if the whole array is read move to stop state to terminate the transfer and to re enable ack*/
				i2c_state_trans=STOP;

			case STOP:
				i2c3_send_stop_reciever();
				enabe_ack();
				break;

				/*handles time_out occurrence*/
			case FAILED:
				ret_st.check=TIME_OUT;
				return ret_st;

		}

	}

	while(i2c_state_trans!=STOP);
	if(ret_st.check !=TIME_OUT && ret_st.check !=DEVIC_ADDRESS_INVALID)
	{	/*NO_ERROR during the transfer wait for 5ms write cycle*/
		enable_counter();
		stop_counter();
		ret_st.check=NO_ERROR;
	}
	i2c_state_trans=START;
	return ret_st;
}


