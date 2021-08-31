#include"EEPROM.h"



int main(void)
{
	operation_state ret_state;

	uint8_t a=1;

	static uint8_t re_arr[16];



    eeprom_init();

    /*write to location to address 10 in page n.2 included in block_0 */
    ret_state= eeprom_byte_write('W',PAGE_2+10,device_address_b0);
    if (ret_state.check !=NO_ERROR)
    {
    	/*add code here to handle error occurrence*/
    }
    /*write 5 byte "hello" string to page n.10 in block 0*/
    ret_state= eeprom_page_write((uint8_t*)"hello", PAGE_10,device_address_b0,5);

    if (ret_state.check !=NO_ERROR)
    {
        	/*add code here to handle error occurrence*/
    }
    /*read byte from address 4 in page no_10 in block no_0*/
    ret_state= eeprom_byte_read(PAGE_10+4,device_address_b0);
    if (ret_state.check !=NO_ERROR)
    {
          /*add code here to handle error occurrence*/
    }
    else
    {
    	/*data read from eeprom*/
    	a=ret_state.data_read;
    }

    /*read array of 5byte from  page no_10 in block no_0*/
    ret_state=eeprom_page_read(re_arr,PAGE_10,device_address_b0,5);
    if (ret_state.check !=NO_ERROR)
    {
              /*add code here to handle error occurrence*/
    }

}
