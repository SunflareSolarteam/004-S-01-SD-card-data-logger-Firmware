/* THIS SOFTWARE IS SUPPLIED BY SUNFLARE SOLAR TEAM "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH SUNFLARE PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL SUNFLARE SOLAR TEAM BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF SUNFLARE SOLAR TEAM HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, SUNFLARE SOLAR TEAM'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO SUNFLARE SOLAR TEAM FOR THIS SOFTWARE.
 *
 * SUNFLARE SOLAR TEAM PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:        can.h
 * Author:      H. Veenstra
 * Comments:    Can bus functionality
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef CANDRV_H
#define	CANDRV_H


#include <stdint.h>

typedef union {
    struct {
        uint32_t id;
        uint8_t idType;
        uint8_t msgtype;
        uint8_t dlc;
        uint8_t data0;
        uint8_t data1;
        uint8_t data2;
        uint8_t data3;
        uint8_t data4;
        uint8_t data5;
        uint8_t data6;
        uint8_t data7;
    } frame;
    unsigned char array[16];
} can_msg_t;

// CAN message type identifiers
#define CAN_MSG_DATA    0x01
#define CAN_MSG_RTR     0x02
#define CAN_FRAME_EXT	0x03
#define CAN_FRAME_STD	0x04

void candrv_init(void);

int candrv_receive(can_msg_t *recCanMsg);

int8_t candrv_transmit(can_msg_t *sendCanMsg);

#endif	/* CANDRV_H */
