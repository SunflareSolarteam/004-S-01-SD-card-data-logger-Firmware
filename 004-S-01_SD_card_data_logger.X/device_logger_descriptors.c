/*
 * File:   device_logger_descriptors.c
 * Author: Hylke
 *
 * Created on April 11, 2021, 10:59 AM
 */
#include "device_logger_descriptors.h"
#include <stdint.h>

const data_entry_descriptor_t sunflare_mppt_message_descriptor[SUNFLARE_MPPT_MESSAGE_COUNT] = {
    {.name = "mppt status",         .function_code = 0x180, .index = 0x2000, .subindex = 0x01, .start_byte = 0, .type = HEX32,  .unit = ""},
    //{.name = "solder jumper",       .function_code = 0x180, .index = 0x2001, .subindex = 0x01, .start_byte = 0, .type = HEX32,  .unit = ""},
    {.name = "solar voltage",       .function_code = 0x280, .index = 0x2000, .subindex = 0x01, .start_byte = 0, .type = UINT16, .unit = "mV"},
    {.name = "solar current",       .function_code = 0x280, .index = 0x2001, .subindex = 0x01, .start_byte = 0, .type = UINT16, .unit = "mA"},
    {.name = "ch1 current",         .function_code = 0x280, .index = 0x2002, .subindex = 0x01, .start_byte = 0, .type = UINT16, .unit = "mA"},
    {.name = "ch2 current",         .function_code = 0x280, .index = 0x2003, .subindex = 0x01, .start_byte = 0, .type = UINT16, .unit = "mA"},
    {.name = "solar power",         .function_code = 0x280, .index = 0x2004, .subindex = 0x01, .start_byte = 0, .type = UINT32, .unit = "mW"},
    {.name = "batt voltage",        .function_code = 0x280, .index = 0x2005, .subindex = 0x01, .start_byte = 0, .type = UINT16, .unit = "mV"}
    //{.name = "boost pd error",      .function_code = 0x380, .index = 0x2000, .subindex = 0x01, .start_byte = 0, .type = INT16,  .unit = "mV"},
    //{.name = "boost pd d",          .function_code = 0x380, .index = 0x2001, .subindex = 0x01, .start_byte = 0, .type = INT16,  .unit = "mV/dt"},
    //{.name = "boost pd p diff",     .function_code = 0x380, .index = 0x2002, .subindex = 0x01, .start_byte = 0, .type = INT32,  .unit = "mW"},
    //{.name = "boost req out p",     .function_code = 0x380, .index = 0x2003, .subindex = 0x01, .start_byte = 0, .type = UINT32, .unit = "mW"},
    //{.name = "boost req slr p",     .function_code = 0x380, .index = 0x2004, .subindex = 0x01, .start_byte = 0, .type = UINT16, .unit = "mA"},
    //{.name = "mppt delta slr p",    .function_code = 0x480, .index = 0x2000, .subindex = 0x01, .start_byte = 0, .type = INT32,  .unit = "mW"},
    //{.name = "mppt delta slr i",    .function_code = 0x480, .index = 0x2001, .subindex = 0x01, .start_byte = 0, .type = INT16,  .unit = "mA"},
    //{.name = "mppt step change",    .function_code = 0x480, .index = 0x2002, .subindex = 0x01, .start_byte = 0, .type = INT16,  .unit = "mA"},
    //{.name = "mppt req slr p",      .function_code = 0x480, .index = 0x2003, .subindex = 0x01, .start_byte = 0, .type = INT16,  .unit = "mA"}
};

const data_entry_descriptor_t motor_controller_message_descriptor[MOTOR_CONTROLLER_MESSAGE_COUNT] = {
    {.name = "sls status",          .function_code = 0x180, .index = 0x2000, .subindex = 0x01, .start_byte = 0, .type = HEX32,  .unit = ""},
    //{.name = "output limiting",     .function_code = 0x180, .index = 0x2001, .subindex = 0x01, .start_byte = 0, .type = HEX32,  .unit = ""},
    {.name = "temp power",          .function_code = 0x280, .index = 0x2000, .subindex = 0x01, .start_byte = 0, .type = INT16,  .unit = "100mdegC"},
    {.name = "temp electronics",    .function_code = 0x280, .index = 0x2000, .subindex = 0x02, .start_byte = 0, .type = INT16,  .unit = "100mdegC"},
    {.name = "temp motor 1",        .function_code = 0x280, .index = 0x2001, .subindex = 0x01, .start_byte = 0, .type = INT16,  .unit = "100mdegC"},
    {.name = "temp motor 2",        .function_code = 0x280, .index = 0x2001, .subindex = 0x02, .start_byte = 0, .type = INT16,  .unit = "100mdegC"},
    {.name = "UZK",                 .function_code = 0x380, .index = 0x2000, .subindex = 0x01, .start_byte = 0, .type = UINT16, .unit = "10mV"},
    {.name = "motor current",       .function_code = 0x380, .index = 0x2001, .subindex = 0x01, .start_byte = 0, .type = INT16,  .unit = "100mA"},
    {.name = "input current",       .function_code = 0x380, .index = 0x2002, .subindex = 0x01, .start_byte = 0, .type = INT16,  .unit = "100mA"},
    {.name = "rpm",                 .function_code = 0x380, .index = 0x2003, .subindex = 0x01, .start_byte = 0, .type = UINT16, .unit = "rpm"}
    //{.name = "max motor A lim",     .function_code = 0x480, .index = 0x2001, .subindex = 0x01, .start_byte = 0, .type = INT16,  .unit = "100mA"},
    //{.name = "max input A lim",     .function_code = 0x480, .index = 0x2002, .subindex = 0x01, .start_byte = 0, .type = INT16,  .unit = "100mA"},
    //{.name = "max rpm limit",       .function_code = 0x480, .index = 0x2003, .subindex = 0x01, .start_byte = 0, .type = UINT16, .unit = "rpm"}
};

const data_entry_descriptor_t hydrofoil_controller_message_descriptor[HYDROFOIL_CONTROLLER_MESSAGE_COUNT] = {
    {.name = "input pos 1",         .function_code = 0x280, .index = 0x2000, .subindex = 0x01, .start_byte = 0, .type = UINT16,  .unit = "raw"},
    //{.name = "input pos 2",         .function_code = 0x280, .index = 0x2000, .subindex = 0x02, .start_byte = 0, .type = UINT16,  .unit = "raw"},
    {.name = "output pos 1",        .function_code = 0x280, .index = 0x2001, .subindex = 0x01, .start_byte = 0, .type = UINT16,  .unit = "raw"}
    //{.name = "output pos 2",        .function_code = 0x280, .index = 0x2001, .subindex = 0x02, .start_byte = 0, .type = UINT16,  .unit = "raw"}
};

const data_entry_descriptor_t gps_message_descriptor[GPS_MESSAGE_COUNT] = {
    {.name = "time",           .function_code = 0x180, .index = 0x2000, .subindex = 0x01, .start_byte = 0, .type = UINT32,  .unit = ""},
    {.name = "latitude",            .function_code = 0x180, .index = 0x2001, .subindex = 0x01, .start_byte = 0, .type = UINT32,  .unit = "deg"},
    {.name = "latitude",            .function_code = 0x180, .index = 0x2001, .subindex = 0x02, .start_byte = 0, .type = UINT32,  .unit = "10umin"},
    {.name = "longitude",           .function_code = 0x180, .index = 0x2002, .subindex = 0x01, .start_byte = 0, .type = UINT32,  .unit = "deg"},
    {.name = "longitude",           .function_code = 0x180, .index = 0x2002, .subindex = 0x02, .start_byte = 0, .type = UINT32,  .unit = "10umin"},
    {.name = "speed",               .function_code = 0x280, .index = 0x2000, .subindex = 0x01, .start_byte = 0, .type = UINT16,  .unit = "10m/h"},
    {.name = "direction",           .function_code = 0x280, .index = 0x2001, .subindex = 0x01, .start_byte = 0, .type = UINT16,  .unit = "100mdeg"},
    {.name = "satellites",          .function_code = 0x380, .index = 0x2000, .subindex = 0x01, .start_byte = 0, .type = UINT16,  .unit = ""}
};

const data_entry_descriptor_t mg_battery_message_descriptor[MG_BATTERY_MESSAGE_COUNT] = {
    {.name = "voltage",             .function_code = 0x300, .index = 0x2005, .subindex = 0x01, .start_byte = 0, .type = UINT16,  .unit = "mV"},
    {.name = "current",             .function_code = 0x300, .index = 0x2005, .subindex = 0x02, .start_byte = 0, .type = INT16,   .unit = "10mA"},
    {.name = "discharge amps",      .function_code = 0x300, .index = 0x2005, .subindex = 0x03, .start_byte = 0, .type = INT16,   .unit = "10mA"},
    {.name = "charge amps",         .function_code = 0x300, .index = 0x2005, .subindex = 0x04, .start_byte = 0, .type = INT16,   .unit = "10mA"},
    {.name = "soc",                 .function_code = 0x300, .index = 0x2005, .subindex = 0x05, .start_byte = 0, .type = UINT8,   .unit = "%"},
    {.name = "time to go",          .function_code = 0x300, .index = 0x2005, .subindex = 0x07, .start_byte = 0, .type = UINT16,  .unit = "min"}
    
    //{.name = "cell temp high",      .function_code = 0x400, .index = 0x2005, .subindex = 0x09, .start_byte = 0, .type = INT8,    .unit = "degC"},
    //{.name = "cell temp low",       .function_code = 0x400, .index = 0x2005, .subindex = 0x0B, .start_byte = 0, .type = INT8,    .unit = "degC"},
    //{.name = "cell volt high",      .function_code = 0x400, .index = 0x2005, .subindex = 0x0C, .start_byte = 0, .type = UINT16,  .unit = "mV"},
    //{.name = "cell volt low",       .function_code = 0x400, .index = 0x2005, .subindex = 0x0D, .start_byte = 0, .type = UINT16,  .unit = "mV"},
    //{.name = "bms state",           .function_code = 0x400, .index = 0x2005, .subindex = 0x0E, .start_byte = 0, .type = UINT32,  .unit = "raw"},
    //{.name = "temp collection",     .function_code = 0x400, .index = 0x2005, .subindex = 0x0F, .start_byte = 0, .type = UINT32,  .unit = "raw"}
    /*
    {.name = "cell volt 01",        .function_code = 0x480, .index = 0x2000, .subindex = 0x01, .start_byte = 0, .type = UINT16,  .unit = "mV"},
    {.name = "cell volt 02",        .function_code = 0x480, .index = 0x2000, .subindex = 0x02, .start_byte = 0, .type = UINT16,  .unit = "mV"},
    {.name = "cell volt 03",        .function_code = 0x480, .index = 0x2000, .subindex = 0x03, .start_byte = 0, .type = UINT16,  .unit = "mV"},
    {.name = "cell volt 04",        .function_code = 0x480, .index = 0x2000, .subindex = 0x04, .start_byte = 0, .type = UINT16,  .unit = "mV"},
    {.name = "cell volt 05",        .function_code = 0x480, .index = 0x2000, .subindex = 0x05, .start_byte = 0, .type = UINT16,  .unit = "mV"},
    {.name = "cell volt 06",        .function_code = 0x480, .index = 0x2000, .subindex = 0x06, .start_byte = 0, .type = UINT16,  .unit = "mV"},
    {.name = "cell volt 07",        .function_code = 0x480, .index = 0x2000, .subindex = 0x07, .start_byte = 0, .type = UINT16,  .unit = "mV"},
    {.name = "cell volt 08",        .function_code = 0x480, .index = 0x2000, .subindex = 0x08, .start_byte = 0, .type = UINT16,  .unit = "mV"},
    {.name = "cell volt 09",        .function_code = 0x480, .index = 0x2000, .subindex = 0x09, .start_byte = 0, .type = UINT16,  .unit = "mV"},
    {.name = "cell volt 10",        .function_code = 0x480, .index = 0x2000, .subindex = 0x0A, .start_byte = 0, .type = UINT16,  .unit = "mV"},
    {.name = "cell volt 11",        .function_code = 0x480, .index = 0x2000, .subindex = 0x0B, .start_byte = 0, .type = UINT16,  .unit = "mV"},
    {.name = "cell volt 12",        .function_code = 0x480, .index = 0x2000, .subindex = 0x0C, .start_byte = 0, .type = UINT16,  .unit = "mV"},
    {.name = "cell volt 13",        .function_code = 0x480, .index = 0x2000, .subindex = 0x0D, .start_byte = 0, .type = UINT16,  .unit = "mV"}*/
};

const data_entry_descriptor_t mg_mppt_message_descriptor[MG_MPPT_MESSAGE_COUNT] = {
    {.name = "voltage in",          .function_code = 0x001, .index = 0x0001, .subindex = 0x01, .start_byte = 0, .type = UINT32,  .unit = "mV"},
    {.name = "current in",          .function_code = 0x002, .index = 0x0002, .subindex = 0x02, .start_byte = 0, .type = UINT32,  .unit = "mA"},
    {.name = "power in",            .function_code = 0x003, .index = 0x0003, .subindex = 0x03, .start_byte = 0, .type = UINT32,  .unit = "mW"},
    {.name = "voltage out",         .function_code = 0x004, .index = 0x0004, .subindex = 0x04, .start_byte = 0, .type = UINT32,  .unit = "mV"}
};

// Device list to be logged

device_list_item_t device_list[DEVICE_LIST_COUNT] = {
    {.name = "MPPT01",  .node_id = 0x20, .msg_descr = sunflare_mppt_message_descriptor, .msg_count = SUNFLARE_MPPT_MESSAGE_COUNT},
    {.name = "MPPT02",  .node_id = 0x21, .msg_descr = sunflare_mppt_message_descriptor, .msg_count = SUNFLARE_MPPT_MESSAGE_COUNT},
    {.name = "MPPT05",  .node_id = 0x24, .msg_descr = sunflare_mppt_message_descriptor, .msg_count = SUNFLARE_MPPT_MESSAGE_COUNT},
    {.name = "MPPT06",  .node_id = 0x25, .msg_descr = sunflare_mppt_message_descriptor, .msg_count = SUNFLARE_MPPT_MESSAGE_COUNT},
    {.name = "MOTOR",   .node_id = 0x10, .msg_descr = motor_controller_message_descriptor, .msg_count = MOTOR_CONTROLLER_MESSAGE_COUNT},
    {.name = "HYDRO",   .node_id = 0x11, .msg_descr = hydrofoil_controller_message_descriptor, .msg_count = HYDROFOIL_CONTROLLER_MESSAGE_COUNT},
    {.name = "GPS",     .node_id = 0x30, .msg_descr = gps_message_descriptor, .msg_count = GPS_MESSAGE_COUNT},
    {.name = "BATT",    .node_id = 0x02, .msg_descr = mg_battery_message_descriptor, .msg_count = MG_BATTERY_MESSAGE_COUNT},
    {.name = "MPPT05",  .node_id = 0x04, .msg_descr = mg_mppt_message_descriptor, .msg_count = MG_MPPT_MESSAGE_COUNT},
    {.name = "MPPT06",  .node_id = 0x05, .msg_descr = mg_mppt_message_descriptor, .msg_count = MG_MPPT_MESSAGE_COUNT},
    {.name = "MPPT07",  .node_id = 0x06, .msg_descr = mg_mppt_message_descriptor, .msg_count = MG_MPPT_MESSAGE_COUNT}
};
