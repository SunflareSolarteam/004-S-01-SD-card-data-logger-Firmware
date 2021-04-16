#include "device_logger.h"
#include "device_logger_descriptors.h"
#include <stdint.h>
#include "softwaretimer.h"
#include "utl.h"
#include "candrv.h"

logging_buffer_t logging_buffer = {};

void device_logger_init(void) {
    uint16_t index, device, length;
    // init logging buffer
    
    // Calculate data length of all devices combined
    length = 0;
    for (device = 0; device < DEVICE_LIST_COUNT; device++) {
        length += device_list[device].msg_count;
    }

    // Clear all buffer data
    for (index = 0; index < LOGGING_BUFFER_RAW_32_LEN; index++) {
        logging_buffer.raw_uint32[index] = 0;
    }
    logging_buffer.time_since_boot_ms = 0;
}

void device_logger_clear_data(void) {
    uint16_t i;
    // Clear buffer
#if defined(DATA_LOGGING_CLEAR_TO_0X00)
    for (i = 0; i < LOGGING_BUFFER_LEN; i++) {
        logging_buffer.data[i].uint32 = 0x00000000;
    }
#elif defined(DATA_LOGGING_CLEAR_TO_0XFF)
    for (i = 0; i < LOGGING_BUFFER_LEN; i++) {
        logging_buffer.data[i].uint32 = 0xFFFFFFFF;
    }
#elif defined(DATA_LOGGINF_DO_NOT_CLEAR)
    // Do nothing to the data
#else
#error At least one clearing method should be defined
#endif
}

void device_logger_decode_and_collect_can_message(can_msg_t rx_msg) {
    uint16_t cob_id, index, function_code;
    uint8_t sub_index, node_id;
    uint64_t data_raw, modified_data;
    uint16_t logging_buffer_index_offset = 0;
    uint16_t device, msg_i;
    union {
        uint32_t uint32;
        double double32;
    }double_uint32_conversion;
    
    cob_id = rx_msg.frame.id;
    function_code = cob_id & ~0x007F;
    node_id = cob_id & 0x7F;
    index = rx_msg.frame.data2 << 8 | rx_msg.frame.data1;
    sub_index = rx_msg.frame.data3;
    data_raw = (uint64_t)rx_msg.frame.data7 << 24 | (uint64_t)rx_msg.frame.data6 << 16 | (uint64_t)rx_msg.frame.data5 << 8 | (uint64_t)rx_msg.frame.data4 << 0;
    
    
    // Check for node id
    for (device = 0; device < DEVICE_LIST_COUNT; device++) {
        if (device_list[device].node_id == node_id && !(0x04 <= node_id && node_id <= 0x09)) {
            // Check for index and sub index
            for (msg_i = 0; msg_i < device_list[device].msg_count; msg_i++) {
                if (    (device_list[device].msg_descr + msg_i)->function_code == function_code &&
                        (device_list[device].msg_descr + msg_i)->index == index &&
                        (device_list[device].msg_descr + msg_i)->subindex == sub_index) {
                    // Get data from message into buffer
                    modified_data = data_raw >> ((device_list[device].msg_descr + msg_i)->start_byte * 8);
                    switch ((device_list[device].msg_descr + msg_i)->type) {
                        case UINT32:
                            logging_buffer.data[logging_buffer_index_offset + msg_i].uint32 = (uint32_t)(modified_data & 0xFFFFFFFF);
                            break;
                        case INT32:
                            logging_buffer.data[logging_buffer_index_offset + msg_i].int32 = (int32_t)(modified_data & 0xFFFFFFFF);
                            break;
                        case UINT16:
                            logging_buffer.data[logging_buffer_index_offset + msg_i].uint16 = (uint16_t)(modified_data & 0xFFFF);
                            break;
                        case INT16:
                            logging_buffer.data[logging_buffer_index_offset + msg_i].int16 = (int16_t)(modified_data & 0xFFFF);
                            break;
                        case UINT8:
                            logging_buffer.data[logging_buffer_index_offset + msg_i].uint8 = (uint8_t)(modified_data & 0xFF);
                            break;
                        case INT8:
                            logging_buffer.data[logging_buffer_index_offset + msg_i].int8 = (int8_t)(modified_data & 0xFF);
                            break;
                        case HEX32:
                            logging_buffer.data[logging_buffer_index_offset + msg_i].uint32 = (uint32_t)(modified_data & 0xFFFFFFFF);
                            break;
                        case HEX16:
                            logging_buffer.data[logging_buffer_index_offset + msg_i].uint16 = (uint16_t)(modified_data & 0xFFFF);
                            break;
                        case HEX8:
                            logging_buffer.data[logging_buffer_index_offset + msg_i].uint8 = (uint8_t)(modified_data & 0xFF);
                            break;
                            
                    }
                }
            }
        }
        // MG MPPT stuff
        if (device_list[device].node_id == node_id && (0x04 <= node_id && node_id <= 0x09)) {
            if (function_code == 0x180) {
                // Current in
                double_uint32_conversion.uint32 = (uint32_t)rx_msg.frame.data3 << 24 | (uint32_t)rx_msg.frame.data2 << 16 | (uint32_t)rx_msg.frame.data1 << 8 | (uint16_t)rx_msg.frame.data0;
                modified_data = double_uint32_conversion.double32;
                logging_buffer.data[logging_buffer_index_offset + 1].uint32 = (uint32_t)(modified_data & 0xFFFFFFFF);
                // voltage in
                double_uint32_conversion.uint32 = (uint32_t)rx_msg.frame.data7 << 24 | (uint32_t)rx_msg.frame.data6 << 16 | (uint32_t)rx_msg.frame.data5 << 8 | (uint16_t)rx_msg.frame.data4;
                modified_data = double_uint32_conversion.double32 * 1000;
                logging_buffer.data[logging_buffer_index_offset + 0].uint32 = (uint32_t)(modified_data & 0xFFFFFFFF);
            }
            if (function_code == 0x280) {
                // voltage out
                double_uint32_conversion.uint32 = (uint32_t)rx_msg.frame.data3 << 24 | (uint32_t)rx_msg.frame.data2 << 16 | (uint32_t)rx_msg.frame.data1 << 8 | (uint16_t)rx_msg.frame.data0;
                modified_data = double_uint32_conversion.double32 * 1000;
                logging_buffer.data[logging_buffer_index_offset + 3].uint32 = (uint32_t)(modified_data & 0xFFFFFFFF);
                // power in
                double_uint32_conversion.uint32 = (uint32_t)rx_msg.frame.data7 << 24 | (uint32_t)rx_msg.frame.data6 << 16 | (uint32_t)rx_msg.frame.data5 << 8 | (uint16_t)rx_msg.frame.data4;
                modified_data = double_uint32_conversion.double32 / 100;
                logging_buffer.data[logging_buffer_index_offset + 2].uint32 = (uint32_t)(modified_data & 0xFFFFFFFF);
            }
        }
        
        logging_buffer_index_offset += device_list[device].msg_count;
    }
}

void device_logger_increase_time_since_boot(uint16_t time_ms) {
    logging_buffer.time_since_boot_ms += time_ms;
}

void get_device_logger_collected_data(logging_buffer_t *buf_ptr) {
    uint16_t i;
    uint32_t crc;
    
    crc = utl_calc_crc(logging_buffer.raw_uint8, LOGGING_BUFFER_RAW_8_LEN - 4);
    logging_buffer.crc = crc;
    
    for (i = 0; i < LOGGING_BUFFER_RAW_32_LEN; i++) {
        buf_ptr->raw_uint32[i] = logging_buffer.raw_uint32[i];
    }
}
