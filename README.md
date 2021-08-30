# 24c16-EEPROM-
24c16 is i2c two wire serial 16kbit EEPROM ,it is  internally organized as 8 blocks ,each block contains 16 pages and each page contains 16 byte for read and write operations .

Unlike 24c256 EEPROM ,24c16 has different access mechanism .

In the case of 24c16 (16K EEPROM) A3,A2 and A1 left unconnected which means 24c16 limits the number of devices on a single bus to one ,and instead they are used to encode the accessing to every block inside the EEPROM

 * block 0 encoded by setting A2,A1,A1 with 000 by software

 * block 1 encoded by setting A2,A1,A1 with 001.....................

 * 				|

 * 				|

 * block 7 represented by setting A2,A1,A1 with 111...............

Despite 24c15 has 2048 bytes((16*1024)/8) which logically could be ranged from 0x000 to ox7FF,however the actual word location which is set during read or write transfer is an eight bit width and it is set relatively to each block .

for example if you want to wtite/read from address 0x102 ,so A2,A1,A0 must be set by software to 0b(010)  indicating block number 2 and the word location must be set to 0x02 or could be set relatively to page0 (ie,.. page0+2).



