#include "sht30.h"
#include "myiic.h"
#include <stdio.h>

data_process_struct data_process;

void SHT_Init(void) {
    HAL_Delay(250);
    IIC_Start();
    IIC_Send_Byte(0x88);
    IIC_Wait_Ack();
    IIC_Send_Byte(0x21);
    IIC_Wait_Ack();
    IIC_Send_Byte(0x30);
    IIC_Wait_Ack();
    IIC_Stop();

    HAL_Delay(150);
}

void sht30_read_temp_humi(uint8_t *p) {
//send a command to sht30 for asking a visit to result register
    IIC_Start();
    IIC_Send_Byte(0x88);
    IIC_Wait_Ack();
    IIC_Send_Byte(0xe0);//0xE000 fetch data
    IIC_Wait_Ack();
    IIC_Send_Byte(0x00);
    IIC_Wait_Ack();
//starting read temp data, p for result, 3 bits in the font is for temp, 3 in the back is for the huminity
    IIC_Start();
    IIC_Send_Byte(0x89);
    IIC_Wait_Ack();

    p[0] = IIC_Read_Byte(1);//high 8 bits of temperature
    p[1] = IIC_Read_Byte(1);//low 8 bits of temp
    p[2] = IIC_Read_Byte(1);//crc
    p[3] = IIC_Read_Byte(1);//for the same
    p[4] = IIC_Read_Byte(1);
    p[5] = IIC_Read_Byte(0);
    IIC_Stop();
}

int sht30_data_process(void) {
    uint8_t temporary[3];
    uint16_t data;
    uint8_t crc_result;

    sht30_read_temp_humi(data_process.sht30_data_buffer);
    //processing the temp data first
    temporary[0] = data_process.sht30_data_buffer[0];
    temporary[1] = data_process.sht30_data_buffer[1];
    temporary[2] = data_process.sht30_data_buffer[2];
    //crc checkout
    crc_result = sht30_crc8_check(temporary, 2, temporary[2]);
    //if crc checkout turns out wrong, return 1, and stop updating temp
    if (crc_result == 0) {
        //combining two 8 bits data to one 16bits data
        data = ((uint16_t) temporary[0] << 8) | temporary[1];
        //temp transfromation
        data_process.SHT30_temperature = (int) ((175.0 * ((double) data) / 65535.0 - 45.0) * 10.0);
        printf("temperature is %.1f\r\n", data_process.SHT30_temperature / 10.0);
    } else {
        return 1;
    }
    temporary[0] = data_process.sht30_data_buffer[3];
    temporary[1] = data_process.sht30_data_buffer[4];
    temporary[2] = data_process.sht30_data_buffer[5];
    //crc checkout
    crc_result = sht30_crc8_check(temporary, 2, temporary[2]);
    if (crc_result == 0) {
        //the same as temperature process
        data = ((uint16_t) temporary[0] << 8) | temporary[1];
        data_process.SHT30_humidity = (int) ((100.0 * (double) data / 65535.0) * 10.0);
        printf("humidity is %.1f\r\n", data_process.SHT30_humidity / 10.0);
        return 0;
    } else {
        return 2;
    }
}

int crc8_compute(uint8_t *check_data, uint8_t num_of_data) {
    uint8_t bit;        // bit mask
    uint8_t crc = 0xFF; // calculated checksum
    uint8_t byteCtr;    // byte counter

    // calculates 8-Bit checksum with given polynomial
    for (byteCtr = 0; byteCtr < num_of_data; byteCtr++) {
        crc ^= (check_data[byteCtr]);
        //crc checkout, the highest bit is 1, for ^0x31
        for (bit = 8; bit > 0; --bit) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x31;
            } else {
                crc = (crc << 1);
            }
        }
    }
    return crc;
}

int sht30_crc8_check(uint8_t *p, uint8_t num_of_data, uint8_t CrcData) {
    uint8_t crc;
    crc = crc8_compute(p, num_of_data);// calculates 8-Bit checksum
    if (crc != CrcData) {
        return 1;
    }
    return 0;
}


