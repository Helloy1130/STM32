#ifndef __SHT30_H
#define __SHT30_H

#include "main.h"

//struct for storing the data from sht30
typedef struct DATA_PROCESS_STRUCT {
    uint8_t sht30_data_buffer[6];
    int SHT30_temperature;
    int SHT30_humidity;
} data_process_struct;

extern data_process_struct data_process;

void SHT_Init(void);

void sht30_read_temp_humi(uint8_t *p);

int sht30_data_process(void);

int crc8_compute(uint8_t *check_data, uint8_t num_of_data);

int sht30_crc8_check(uint8_t *p, uint8_t num_of_data, uint8_t CrcData);

#endif  
	 
	 



