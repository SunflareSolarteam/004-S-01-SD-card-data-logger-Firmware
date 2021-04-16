#ifndef DEVICE_LOGGER_DESCRIPTORS_H
#define	DEVICE_LOGGER_DESCRIPTORS_H

#include <stdint.h>
#include "device_logger_typedefs.h"

// Device messages descriptors

#define SUNFLARE_MPPT_MESSAGE_COUNT 7
extern const data_entry_descriptor_t sunflare_mppt_message_descriptor[SUNFLARE_MPPT_MESSAGE_COUNT];

#define MOTOR_CONTROLLER_MESSAGE_COUNT 9
extern const data_entry_descriptor_t motor_controller_message_descriptor[MOTOR_CONTROLLER_MESSAGE_COUNT];

#define HYDROFOIL_CONTROLLER_MESSAGE_COUNT 2
extern const data_entry_descriptor_t hydrofoil_controller_message_descriptor[HYDROFOIL_CONTROLLER_MESSAGE_COUNT];

#define GPS_MESSAGE_COUNT 8
extern const data_entry_descriptor_t gps_message_descriptor[GPS_MESSAGE_COUNT];

#define MG_BATTERY_MESSAGE_COUNT 6
extern const data_entry_descriptor_t mg_battery_message_descriptor[MG_BATTERY_MESSAGE_COUNT];

#define MG_MPPT_MESSAGE_COUNT 4
extern const data_entry_descriptor_t mg_mppt_message_descriptor[MG_MPPT_MESSAGE_COUNT];

#define DEVICE_LIST_COUNT 11
extern device_list_item_t device_list[DEVICE_LIST_COUNT];

// We must know the total number of message entries to create buffers
#define TOTAL_MESSAGE_COUNT (4 * SUNFLARE_MPPT_MESSAGE_COUNT +\
                             1 * MOTOR_CONTROLLER_MESSAGE_COUNT +\
                             1 * HYDROFOIL_CONTROLLER_MESSAGE_COUNT +\
                             1 * GPS_MESSAGE_COUNT +\
                             1 * MG_BATTERY_MESSAGE_COUNT +\
                             3 * MG_MPPT_MESSAGE_COUNT\
                            )

// ****************************************************************************
// * Do not modify anything below  
// ****************************************************************************

#define LOGGING_BUFFER_LEN         TOTAL_MESSAGE_COUNT
#define LOGGING_BUFFER_RAW_8_LEN   ((LOGGING_BUFFER_LEN + 2) * 4)
#define LOGGING_BUFFER_RAW_32_LEN  (LOGGING_BUFFER_LEN + 2)

typedef struct {
    union {
        struct {
            uint32_t time_since_boot_ms;
            logging_data_buffer_t data[LOGGING_BUFFER_LEN];
            uint32_t crc;
        };
        uint8_t raw_uint8[LOGGING_BUFFER_RAW_8_LEN];
        uint32_t raw_uint32[LOGGING_BUFFER_RAW_32_LEN];
    };
} logging_buffer_t;

#endif	/* DEVICE_LOGGER_DESCRIPTORS_H */
