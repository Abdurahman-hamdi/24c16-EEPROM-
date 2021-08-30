#ifndef  __EEPROM__
#define  __EEPROM__

/*Includes*/
#include <i2c_port.h>

/**
 * Definitions for device_address_word
 * 24c16 internally organized as eight blocks of 256_bytes and each of these blocks contains 16 pages of 16_bytes..=8*16*16=4048byte=16kbit
 * At the beginning /mid of transfer master hits the slave(EEPROM) with this pattern 1010 A3A2A1 r/w where A3A2A1 used to indicates slave address within the network.
 * In the case of 24c16 (16K EEPROM) A3,A2 and A1 left unconnected which means 24c16 limits the number of devices on a single bus to one,
 * and instead they used to encode the accessing to every block inside the  EEPROM
 * block 0 encoded by setting A2,A1,A1 with 000 by software
 * block 1 encoded by setting A2,A1,A1 with 001.....................
 * 				|
 * 				|
 * block 7 	represented by setting A2,A1,A1 with 111...............
 *
 */
#define      device_address_b0               (uint8_t)0XA0
#define      device_address_b1	             (uint8_t)0xA2
#define      device_address_b2               (uint8_t)0XA4
#define      device_address_b3               (uint8_t)0xA6
#define      device_address_b4               (uint8_t)0XA8
#define      device_address_b5               (uint8_t)0XAA
#define      device_address_b6               (uint8_t)0XAC
#define      device_address_b7               (uint8_t)0XAE

/**
 * Definitions for all 16_pages
 * All pages addresses(0x00,0x10....0xF0) are 8_bit width and relative to each block
 *
 */
#define      PAGE_0               (uint8_t)0X00
#define      PAGE_1	              (uint8_t)0x10
#define      PAGE_2               (uint8_t)0X20
#define      PAGE_3               (uint8_t)0x30
#define      PAGE_4               (uint8_t)0X40
#define      PAGE_5               (uint8_t)0X50
#define      PAGE_6               (uint8_t)0X60
#define      PAGE_7               (uint8_t)0X70
#define      PAGE_8               (uint8_t)0X80
#define      PAGE_9	              (uint8_t)0x90
#define      PAGE_10              (uint8_t)0XA0
#define      PAGE_11              (uint8_t)0xB0
#define      PAGE_12              (uint8_t)0XC0
#define      PAGE_13              (uint8_t)0XD0
#define      PAGE_14              (uint8_t)0XE0
#define      PAGE_15              (uint8_t)0XF0

/**
 *
 * Declare enum to check transfer
 */


typedef enum{
	NO_ERROR,
	TIME_OUT,
	DEVIC_ADDRESS_INVALID



}Error_State;
/**
 *
 */
typedef struct{
	Error_State check;
	uint8_t data_read;
} operation_state;


void eeprom_init (void);
operation_state eeprom_byte_write(uint8_t ,uint8_t ,uint8_t );

operation_state eeprom_page_write(uint8_t*,uint8_t ,uint8_t ,uint8_t );

operation_state eeprom_byte_read(uint8_t ,uint8_t );

operation_state eeprom_page_read(uint8_t*,uint8_t data_add,uint8_t ,uint8_t );





#endif
