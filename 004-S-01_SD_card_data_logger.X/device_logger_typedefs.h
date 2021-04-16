#ifndef DEVICE_LOGGER_TYPEDEFS_H
#define	DEVICE_LOGGER_TYPEDEFS_H

#include <stdint.h> 

typedef enum {
    UINT32,
    INT32,
    UINT16,
    INT16,
    UINT8,
    INT8,
    HEX32,
    HEX16,
    HEX8
} data_entry_value_type_t;

typedef struct {
    char name[17];
    uint16_t function_code;
    uint16_t index;
    uint8_t subindex;
    uint8_t start_byte;
    data_entry_value_type_t type;
    char unit[9];
} data_entry_descriptor_t;

typedef union {
    uint32_t uint32;
    int32_t int32;
    uint16_t uint16;
    int16_t int16;
    uint8_t uint8;
    int8_t int8;
    uint8_t boolean;
    uint32_t hex32;
    uint16_t hex16;
    uint8_t hex8;
} logging_data_buffer_t;

typedef struct {
    char name[17];
    uint16_t node_id;
    const data_entry_descriptor_t *msg_descr;
    uint32_t msg_count;
} device_list_item_t;

#endif	/* DEVICE_LOGGER_TYPEDEFS_H */
