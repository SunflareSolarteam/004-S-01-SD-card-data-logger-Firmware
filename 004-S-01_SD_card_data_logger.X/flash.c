/*
 * File:   flash.c
 * Author: Hylke
 *
 * Created on April 11, 2021, 11:30 AM
 */

#include "flash.h"
#include <stdint.h>

#define FLASH_BUFFER_SIZE 4
logging_buffer_t flash_buffer[FLASH_BUFFER_SIZE];
uint16_t flash_buffer_index;

void flash_init(void) {
    flash_clear_data();
}

void flash_store_logging_data(logging_buffer_t *logging_buffer_ptr) {
    uint16_t i;
    
    if (flash_buffer_index == FLASH_BUFFER_SIZE) {
        return;
    }
    
    for (i = 0; i < LOGGING_BUFFER_RAW_32_LEN; i++) {
        flash_buffer[flash_buffer_index].raw_uint32[i] = logging_buffer_ptr->raw_uint32[i];
    }
    flash_buffer_index++;
}

uint8_t flash_get_flash_full(void) {
    if (flash_buffer_index == FLASH_BUFFER_SIZE) {
        return 1;
    } else {
        return 0;
    }
}

uint16_t flash_get_flash_number_of_data(void) {
    return flash_buffer_index;
}

int8_t flash_get_flash_logging_data(logging_buffer_t *logging_buffer_ptr, uint16_t number) {
    uint16_t i;
    
    if (number < FLASH_BUFFER_SIZE) {
        for (i = 0; i < LOGGING_BUFFER_RAW_32_LEN; i++) {
            logging_buffer_ptr->raw_uint32[i] = flash_buffer[number].raw_uint32[i];
        }
        return 1;
    } else {
        return 0;
    }
}

void flash_clear_data(void) {
    uint16_t i, j;
    
    for (i = 0; i < FLASH_BUFFER_SIZE; i++) {
        for (j = 0; j < LOGGING_BUFFER_RAW_32_LEN; j++) {
            flash_buffer[i].raw_uint32[j] = 0;
        }
    }
    flash_buffer_index = 0;
}
