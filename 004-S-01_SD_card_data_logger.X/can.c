/*
 * File:   can.c
 * Author: H. Veenstra
 *
 * Created on March 1, 2020, 3:11 PM
 */


#include "can.h"
#include <xc.h>
#include <stdint.h>
#include "candrv.h"
#include "softwaretimer.h"
#include "debugprint.h"
#include "device_logger.h"
#include "gps.h"

#define CAN_TRANSMIT_MESSAGES_NO 8

uint8_t can_transmit_message_pending[CAN_TRANSMIT_MESSAGES_NO] = {};
int8_t can_transmit_timer = SOFTWARETIMER_NONE;


void can_init(void) {
    // Init the can driver
    candrv_init();
    // Init timer
    can_transmit_timer = softwaretimer_create(SOFTWARETIMER_CONTINUOUS_MODE);
    softwaretimer_start(can_transmit_timer, CAN_TRANSMIT_PERIOD_MS);
}

void can_transmit_process(void) {
    uint8_t i;
    can_msg_t tx_msg;
    uint32_t temp_32;
    
    // Check transmit timer
    if (softwaretimer_get_expired(can_transmit_timer)) {
        for (i = 0; i < CAN_TRANSMIT_MESSAGES_NO; i++) {
            can_transmit_message_pending[i] = 1;
        }
    }
    
    for (i = 0; i < CAN_TRANSMIT_MESSAGES_NO; i++) {
        if (can_transmit_message_pending[i] == 1) {
            // transmit message
            tx_msg.frame.msgtype = CAN_MSG_DATA;
            tx_msg.frame.idType = CAN_FRAME_STD;
            tx_msg.frame.dlc = 8;
            
            switch(i) {
                case 0:
                    // Unix timestamp
                    tx_msg.frame.id = 0x180 | CAN_DEVICE_ID;
                    tx_msg.frame.data0 = CAN_DEVICE_ID;
                    tx_msg.frame.data1 = 0x00;
                    tx_msg.frame.data2 = 0x20;
                    tx_msg.frame.data3 = 0x01;
                    temp_32 = get_gps_time_stamp();
                    tx_msg.frame.data4 = temp_32 >> 0;
                    tx_msg.frame.data5 = temp_32 >> 8;
                    tx_msg.frame.data6 = temp_32 >> 16;
                    tx_msg.frame.data7 = temp_32 >> 24;
                    break;
                    
                case 1:
                    // lat degrees
                    tx_msg.frame.id = 0x180 | CAN_DEVICE_ID;
                    tx_msg.frame.data0 = CAN_DEVICE_ID;
                    tx_msg.frame.data1 = 0x01;
                    tx_msg.frame.data2 = 0x20;
                    tx_msg.frame.data3 = 0x01;
                    temp_32 = get_gps_lat_deg();
                    tx_msg.frame.data4 = temp_32 >> 0;
                    tx_msg.frame.data5 = temp_32 >> 8;
                    tx_msg.frame.data6 = temp_32 >> 16;
                    tx_msg.frame.data7 = temp_32 >> 24;
                    break;
                    
                case 2:
                    // lat min
                    tx_msg.frame.id = 0x180 | CAN_DEVICE_ID;
                    tx_msg.frame.data0 = CAN_DEVICE_ID;
                    tx_msg.frame.data1 = 0x01;
                    tx_msg.frame.data2 = 0x20;
                    tx_msg.frame.data3 = 0x02;
                    temp_32 = get_gps_lat_10u_min();
                    tx_msg.frame.data4 = temp_32 >> 0;
                    tx_msg.frame.data5 = temp_32 >> 8;
                    tx_msg.frame.data6 = temp_32 >> 16;
                    tx_msg.frame.data7 = temp_32 >> 24;
                    break;
                    
                case 3:
                    // long degrees
                    tx_msg.frame.id = 0x180 | CAN_DEVICE_ID;
                    tx_msg.frame.data0 = CAN_DEVICE_ID;
                    tx_msg.frame.data1 = 0x02;
                    tx_msg.frame.data2 = 0x20;
                    tx_msg.frame.data3 = 0x01;
                    temp_32 = get_gps_long_deg();
                    tx_msg.frame.data4 = temp_32 >> 0;
                    tx_msg.frame.data5 = temp_32 >> 8;
                    tx_msg.frame.data6 = temp_32 >> 16;
                    tx_msg.frame.data7 = temp_32 >> 24;
                    break;
                    
                case 4:
                    // long min
                    tx_msg.frame.id = 0x180 | CAN_DEVICE_ID;
                    tx_msg.frame.data0 = CAN_DEVICE_ID;
                    tx_msg.frame.data1 = 0x02;
                    tx_msg.frame.data2 = 0x20;
                    tx_msg.frame.data3 = 0x02;
                    temp_32 = get_gps_long_10u_min();
                    tx_msg.frame.data4 = temp_32 >> 0;
                    tx_msg.frame.data5 = temp_32 >> 8;
                    tx_msg.frame.data6 = temp_32 >> 16;
                    tx_msg.frame.data7 = temp_32 >> 24;
                    break;
                    
                case 5:
                    // speed
                    tx_msg.frame.id = 0x280 | CAN_DEVICE_ID;
                    tx_msg.frame.data0 = CAN_DEVICE_ID;
                    tx_msg.frame.data1 = 0x00;
                    tx_msg.frame.data2 = 0x20;
                    tx_msg.frame.data3 = 0x01;
                    temp_32 = get_gps_speed_10mtrph();
                    tx_msg.frame.data4 = temp_32 >> 0;
                    tx_msg.frame.data5 = temp_32 >> 8;
                    tx_msg.frame.data6 = 0;
                    tx_msg.frame.data7 = 0;
                    break;
                    
                case 6:
                    // direction
                    tx_msg.frame.id = 0x280 | CAN_DEVICE_ID;
                    tx_msg.frame.data0 = CAN_DEVICE_ID;
                    tx_msg.frame.data1 = 0x01;
                    tx_msg.frame.data2 = 0x20;
                    tx_msg.frame.data3 = 0x01;
                    temp_32 = get_gps_direction_100mdeg();
                    tx_msg.frame.data4 = temp_32 >> 0;
                    tx_msg.frame.data5 = temp_32 >> 8;
                    tx_msg.frame.data6 = 0;
                    tx_msg.frame.data7 = 0;
                    break;
                    
                case 7:
                    // number of satellites
                    tx_msg.frame.id = 0x380 | CAN_DEVICE_ID;
                    tx_msg.frame.data0 = CAN_DEVICE_ID;
                    tx_msg.frame.data1 = 0x00;
                    tx_msg.frame.data2 = 0x20;
                    tx_msg.frame.data3 = 0x01;
                    temp_32 = get_gps_number_of_satellites();
                    tx_msg.frame.data4 = temp_32 >> 0;
                    tx_msg.frame.data5 = temp_32 >> 8;
                    tx_msg.frame.data6 = 0;
                    tx_msg.frame.data7 = 0;
                    break;
                    
                default:
                    break;
            }
            
            if (candrv_transmit(&tx_msg)) {
                can_transmit_message_pending[i] = 0;
                // Send msg also to receive message function so we can log our own gps
                device_logger_decode_and_collect_can_message(tx_msg);
            }
            break;
        }
    }
    
}

uint8_t can_receive_message(can_msg_t *recCanMsg)  {
    return candrv_receive(recCanMsg);
}
