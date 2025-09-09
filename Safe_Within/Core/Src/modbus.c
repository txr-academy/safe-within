/**
 * @file modbus.c
 * @brief Modbus RTU protocol frame processing implementation over UART
 *  Created on: Aug 6, 2025
 *      Author: abhiram and anandu
 */

#include "modbus.h"


uint16_t holding_registers[100];
uint8_t modbus_frame[8];
uint8_t reply[100]={0};
uint16_t values[100]={0};
uint16_t register_count = 0;


/*
 * @brief Function to calculate the CRC_16
 * @param buf points to the data buffer for the request/response frame
 * @param len gives the number of bytes in the request/response frame
 * @retval crc returns the 16 bit calculated crc value
 */
uint16_t Modbus_CRC16(uint8_t *buf, uint8_t len) {
    uint16_t crc = 0xFFFF;
    for (int pos = 0; pos < len; pos++) {
        crc ^= buf[pos];
        for (int i = 0; i < 8; i++) {
            if (crc & 0x0001)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc >>= 1;
        }
    }
    return crc;
}

/*
 * @brief Function that parses the request message frame and then generates and transmits the corresponding response frame
 * @param modbus_frame Points to the request message frame
 * @retval None
 */
void Process_Modbus_Frame(uint8_t * modbus_frame){
	// can be used to read stored values from holding registers
//	holding_registers[0] = 0x1234;
//	holding_registers[1] = 0x1001;
//	holding_registers[2] = 0x1212;

	uint8_t slave_address = modbus_frame[0];
	uint8_t function_code = modbus_frame[1];

	if (function_code == READ_SINGLE_REGISTER){
		uint16_t received_crc =  modbus_frame[6] | (modbus_frame[7] << 8);
		uint16_t calculated_crc = Modbus_CRC16(modbus_frame, 6);

		if (received_crc != calculated_crc) return;
		if (slave_address != 0x01) return;


		uint16_t start_address = (modbus_frame[2] << 8) | modbus_frame[3];
		register_count = (modbus_frame[4] << 8) | modbus_frame[5];

		if (register_count >10) return;
	    reply[0] = slave_address;
	    reply[1] = function_code;
	    reply[2] = register_count * 2; // byte count

	    for (int i = 0; i < register_count; i++) {	 // takes value from holding register and puts it into response frame
	        uint16_t val = holding_registers[start_address + i];
	        reply[3 + 2*i] = (val >> 8) & 0xFF;
	        reply[4 + 2*i] = val & 0xFF;
	    }

	    uint16_t crc = Modbus_CRC16(reply, 3 + 2*register_count); // calculates the crc from response data and puts it into response frame
	    reply[3 + 2*register_count] = crc & 0xFF;
	    reply[4 + 2*register_count] = (crc >> 8) & 0xFF;

	    HAL_UART_Transmit_IT(&huart2, reply, 5 + 2*register_count); // transmits response

	}

	else if (function_code == WRITE_SINGLE_REGISTER){
		uint16_t received_crc =  modbus_frame[6] | (modbus_frame[7] << 8);
		uint16_t calculated_crc = Modbus_CRC16(modbus_frame, 6);


		if (received_crc != calculated_crc) return;
		if (slave_address != 0x01) return;


		uint16_t start_address = (modbus_frame[2] << 8) | modbus_frame[3];
		values[0] = (modbus_frame[4] << 8) | modbus_frame[5];  // stores the data in buffer values
		holding_registers[start_address] = values[0];
		HAL_UART_Transmit_IT(&huart2, modbus_frame, 8);  // transmits back the request frame as the response frame
	}

	else if (function_code == WRITE_MULTIPLE_REGISTERS){

		uint16_t start_address = (modbus_frame[2] << 8) | modbus_frame[3];
		uint16_t register_count = (modbus_frame[4] << 8) | modbus_frame[5];

		uint16_t byte_count = modbus_frame[6];

		for (int i=0; i<register_count; i++){
			values[i] = (modbus_frame[7+2*i] << 8) | modbus_frame[8+2*i];  // stores each of the 16 bit data in values array
			holding_registers[start_address + i] = values[i];
		}

		uint16_t calculated_crc = Modbus_CRC16(modbus_frame, 7+byte_count);
		uint16_t received_crc =  modbus_frame[7+byte_count] | (modbus_frame[8+byte_count] << 8);
		if (received_crc != calculated_crc) return;
		if (slave_address != 0x01) return;


		reply[0] = slave_address;
		reply[1] = function_code;
		reply[2] = modbus_frame[2];
		reply[3] = modbus_frame[3];
		reply[4] = modbus_frame[4];
		reply[5] = modbus_frame[5];

		uint16_t crc = Modbus_CRC16(reply, 6);
	    reply[6] = crc & 0xFF;
	    reply[7] = (crc >> 8) & 0xFF;

	    HAL_UART_Transmit_IT(&huart2, reply, 8); // transmits response
	}
}



