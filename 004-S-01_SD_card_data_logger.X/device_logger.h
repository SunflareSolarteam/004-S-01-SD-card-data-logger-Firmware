#ifndef DEVICE_LOGGER_H
#define	DEVICE_LOGGER_H

#include <stdint.h>
#include "candrv.h"
#include "device_logger_descriptors.h"


#define DATA_LOGGING_RATE_MS    1000

// After every logging point the data can be cleared.
// Uncomment desired clearing method
#define DATA_LOGGING_CLEAR_TO_0X00
//#define DATA_LOGGING_CLEAR_TO_0XFF
//#define DATA_LOGGINF_DO_NOT_CLEAR


void device_logger_init(void);

void device_logger_clear_data(void);

void device_logger_decode_and_collect_can_message(can_msg_t rx_msg);

void device_logger_increase_time_since_boot(uint16_t time_ms);

void get_device_logger_collected_data(logging_buffer_t *buf_ptr);

#endif	/* DEVICE_LOGGER_H */

