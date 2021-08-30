#ifndef __i2c_desk__
#define __i2c_desk__

#include "gpoi_i2c.h"
#include "stm32f4xx_i2c.h"

/*fire_1ms is a constant used to set systick to overflow every 1ms to handle t_wr cycle */
#define FIRE_1MS   (uint32_t)168000

typedef enum{
	NO_ERROR_PORT,
	TIME_OUT_PORT


}ret_state_port;

typedef struct{
	uint8_t data;
	ret_state_port state_port;
}ret_state_data_port;

void i2c_init (void);
ret_state_port i2c3_send_start(void);
ret_state_port i2c3_send_REstart(void);
void i2c3_send_stop(void);
ret_state_port send_devic_address_write(uint8_t);
ret_state_port send_data(uint8_t);
ret_state_data_port i2c3_data_read(void);
ret_state_port send_devic_address_read(uint8_t);
ret_state_port send_devic_address_data(uint8_t);
void enable_ack(void);
void stop_ack(void);
ret_state_port i2c3_send_stop_transmit(void);
void i2c3_send_stop_reciever(void);
void enable_counter(void);
void stop_counter(void);
void Time_out_off(void);
void timer_interrupt_write_cycle_config(uint32_t);


#endif
