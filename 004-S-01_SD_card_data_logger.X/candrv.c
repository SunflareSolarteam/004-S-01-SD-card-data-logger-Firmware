/*
 * File:   candrv.c
 * Author: Hylke
 *
 * Created on March 1, 2020, 8:24 PM
 */

#include "candrv.h"
#include <xc.h>
#include <stdint.h>
#include <p33EP128GS804.h>

// Valid options are 4, 6, 8, 12, 16, 24, or 32.
#define CAN1_MESSAGE_BUFFERS        32
#define CAN1_FIFO_STARTING_BUFFER   0x8
#define CAN1_TX_BUFFER_COUNT        8

#define CAN1_PIN_TRIS_TX            TRISCbits.TRISC10
#define CAN1_PIN_TRIS_RX            TRISCbits.TRISC1
#define CAN1_PIN_ANSEL_TX           ANSELCbits.ANSC10
#define CAN1_PIN_ANSEL_RX           ANSELCbits.ANSC1
#define CAN1_PIN_RP_TX              _RP58R
#define CAN1_PIN_RP_RX              49

// This alignment is required because of the DMA's peripheral indirect addressing mode.
static volatile uint16_t can1msgBuf [CAN1_MESSAGE_BUFFERS][8] __attribute__((aligned(CAN1_MESSAGE_BUFFERS * 16)));

static void can1_read_from_dma_ram_buffer(uint8_t buffer_number, can_msg_t *message)
{
    uint16_t ide=0;
    uint16_t rtr=0;
    uint32_t id=0;

    // read word 0 to see the message type
    ide = can1msgBuf[buffer_number][0] & 0x0001U;			

    // check to see what type of message it is
    // message is standard identifier
    if (ide == 0U) {
        message->frame.id = (can1msgBuf[buffer_number][0] & 0x1FFCU) >> 2U;		
        message->frame.idType = CAN_FRAME_STD;
        rtr=can1msgBuf[buffer_number][0] & 0x0002U;
    }
    // message is extended identifier
    else {
        id = can1msgBuf[buffer_number][0] & 0x1FFCU;		
        message->frame.id = id << 16U;
        message->frame.id += ( ((uint32_t)can1msgBuf[buffer_number][1] & (uint32_t)0x0FFF) << 6U );
        message->frame.id += ( ((uint32_t)can1msgBuf[buffer_number][2] & (uint32_t)0xFC00U) >> 10U );		
        message->frame.idType = CAN_FRAME_EXT;
        rtr = can1msgBuf[buffer_number][2] & 0x0200;
    }
    // check to see what type of message it is
    // RTR message
    if (rtr != 0U) {
        // to be defined ?
        message->frame.msgtype = CAN_MSG_RTR;	
    }
    // normal message
    else {
        message->frame.msgtype = CAN_MSG_DATA;
        message->frame.data0 = (uint8_t)can1msgBuf[buffer_number][3];
        message->frame.data1 = (uint8_t)((can1msgBuf[buffer_number][3] & 0xFF00U) >> 8U);
        message->frame.data2 = (uint8_t)can1msgBuf[buffer_number][4];
        message->frame.data3 = (uint8_t)((can1msgBuf[buffer_number][4] & 0xFF00U) >> 8U);
        message->frame.data4 = (uint8_t)can1msgBuf[buffer_number][5];
        message->frame.data5 = (uint8_t)((can1msgBuf[buffer_number][5] & 0xFF00U) >> 8U);
        message->frame.data6 = (uint8_t)can1msgBuf[buffer_number][6];
        message->frame.data7 = (uint8_t)((can1msgBuf[buffer_number][6] & 0xFF00U) >> 8U);
        message->frame.dlc = (uint8_t)(can1msgBuf[buffer_number][2] & 0x000FU);
    }
}

static void can1_write_to_dma_ram_buffer(volatile uint16_t* buffer, can_msg_t *message)
{   
    if(message->frame.idType == CAN_FRAME_STD) {
        buffer[0] = (message->frame.id & 0x000007FF) << 2;
        buffer[1] = 0;
        buffer[2] = message->frame.dlc & 0x0F;
    } else {
        buffer[0] = ( ( (uint16_t)(message->frame.id >> 16 ) & 0x1FFC ) ) | 0b1;
        buffer[1] = (uint16_t)(message->frame.id >> 6) & 0x0FFF;
        buffer[2] = (message->frame.dlc & 0x0F) + ( (uint16_t)(message->frame.id << 10) & 0xFC00);
    }

    buffer[3] = ((message->frame.data1)<<8) + message->frame.data0;
    buffer[4] = ((message->frame.data3)<<8) + message->frame.data2;
    buffer[5] = ((message->frame.data5)<<8) + message->frame.data4;
    buffer[6] = ((message->frame.data7)<<8) + message->frame.data6;
}

void candrv_init(void) {
    // Set pins
#ifdef CAN1_PIN_ANSEL_TX
    CAN1_PIN_ANSEL_TX = 0;
#endif
#ifdef CAN1_PIN_ANSEL_RX
    CAN1_PIN_ANSEL_RX = 0;
#endif
    CAN1_PIN_TRIS_TX = 0;
    CAN1_PIN_TRIS_RX = 1;
    
    // PPS unlock
    __builtin_write_OSCCONL(OSCCON & ~(1<<6));
    // PPS
    CAN1_PIN_RP_TX = _RPOUT_C1TX;    //RB14->ECAN1:C1TX
    _C1RXR = CAN1_PIN_RP_RX;    //RC1->ECAN1:C1RXR
    // PPS lock
    __builtin_write_OSCCONL(OSCCON | (1<<6));
    
    
    // Set can module in config mode
    C1CTRL1bits.REQOP = 4;
    while(C1CTRL1bits.OPMODE != 4);

    // Select buffer register window
    C1CTRL1bits.WIN = 0;
    
    // Setup can
    C1CTRL1bits.CANCAP = 0;     // No input capture on can receive
    C1CTRL1bits.CANCKS = 0;     // Fcan = Fp
    C1CTRL1bits.CSIDL = 0;      // Continue in idle mode
    C1CFG1bits.SJW = 0;         // Synchronization Jump Width = 1 TQ
    C1CFG1bits.BRP = 11;        // 250kbps with 10 TQ per bit -> (60000000/(12 * 2))/10 = 250000
    C1CFG2bits.PRSEG = 0;       // 1 TQ
    C1CFG2bits.SAM = 0;         // Sample once
    C1CFG2bits.SEG1PH = 5;      // 6 TQ
    C1CFG2bits.SEG2PH = 1;      // 2 TQ
    C1CFG2bits.SEG2PHTS = 1;    // Freely programmable
    C1CFG2bits.WAKFIL = 0;      // No wake up filter
    C1FCTRLbits.DMABS = 0b110;  // 32 buffers in ram
    C1FCTRLbits.FSA = 8;        // Start receive buffer at 8
    C1FEN1 = 0x01;              // Only filter 0 enabled
    
    // Filter configuration to pass all messages
    // enable window to access the filter configuration registers
    // use filter window
    C1CTRL1bits.WIN = 1;
    
    C1FMSKSEL1bits.F0MSK = 0x0; // Select Mask 0 for Filter 0

    // Configure mask as do not care
    C1RXM0SIDbits.SID = 0; 
    C1RXM0SIDbits.EID = 0; 
    C1RXM0SIDbits.MIDE = 0;
    C1RXM0EIDbits.EID = 0;     	   	
    
    // configure filter
    C1RXF0SIDbits.SID = 0; 
    C1RXF0SIDbits.EID = 0; 
    C1RXF0EIDbits.EID = 0; 
    C1RXF0SIDbits.EXIDE = 0;
    
    // FIFO Mode
    C1BUFPNT1bits.F0BP = 0xF; //Filter 0 uses FIFO

    // clear window bit to access ECAN control registers
    C1CTRL1bits.WIN = 0;
    
    // CAN1, Buffer 0 is a Transmit Buffer
    C1TR01CONbits.TXEN0 = 0x1; // Buffer 0 is a Transmit Buffer 
    C1TR01CONbits.TXEN1 = 0x1; // Buffer 1 is a Transmit Buffer 
    C1TR23CONbits.TXEN2 = 0x1; // Buffer 2 is a Transmit Buffer 
    C1TR23CONbits.TXEN3 = 0x1; // Buffer 3 is a Transmit Buffer 
    C1TR45CONbits.TXEN4 = 0x1; // Buffer 4 is a Transmit Buffer 
    C1TR45CONbits.TXEN5 = 0x1; // Buffer 5 is a Transmit Buffer 
    C1TR67CONbits.TXEN6 = 0x1; // Buffer 6 is a Transmit Buffer 
    C1TR67CONbits.TXEN7 = 0x1; // Buffer 7 is a Transmit Buffer 

    C1TR01CONbits.TX0PRI = 0x0; // Message Buffer 0 Priority Level
    C1TR01CONbits.TX1PRI = 0x0; // Message Buffer 1 Priority Level
    C1TR23CONbits.TX2PRI = 0x0; // Message Buffer 2 Priority Level
    C1TR23CONbits.TX3PRI = 0x0; // Message Buffer 3 Priority Level
    C1TR45CONbits.TX4PRI = 0x0; // Message Buffer 4 Priority Level
    C1TR45CONbits.TX5PRI = 0x0; // Message Buffer 5 Priority Level
    C1TR67CONbits.TX6PRI = 0x0; // Message Buffer 6 Priority Level
    C1TR67CONbits.TX7PRI = 0x0; // Message Buffer 7 Priority Level

    // clear the buffer and overflow flags  
    C1RXFUL1 = 0x0000;
    C1RXFUL2 = 0x0000;
    C1RXOVF1 = 0x0000;
    C1RXOVF2 = 0x0000;	

    // configure the device to interrupt on the receive buffer full flag
    // clear the buffer full flags
    C1INTFbits.RBIF = 0;  

    
    // DMA initialization
    DMA0CONbits.SIZE = 0;   // Word sized (16bits)
    DMA0CONbits.DIR = 1;    // Ram to peripheral
    DMA0CONbits.AMODE = 2;  // Indirect addressing
    DMA0CONbits.MODE = 0;   // Continues, ping pong disabled
    // FORCE disabled; IRQSEL CAN1 TX; 
    DMA0REQ = 70;
    DMA0STAH = 0;
    DMA0STAL = __builtin_dmaoffset(&can1msgBuf);
    DMA0STBH = 0x00;
    DMA0STBL = 0x00;
    DMA0PAD = (int16_t) &C1TXD;
    DMA0CNT= 0x07;
    // Clearing Channel 0 Interrupt Flag;
    IFS0bits.DMA0IF = 0;
    DMA0CONbits.CHEN = 1;
    
    DMA1CONbits.SIZE = 0;   // Word sized (16bits)
    DMA1CONbits.DIR = 0;    // peripheral to ram
    DMA1CONbits.AMODE = 2;  // Indirect addressing
    DMA1CONbits.MODE = 0;   // Continues, ping pong disabled
    // FORCE disabled; IRQSEL CAN1 RX; 
    DMA1REQ= 0x22;
    DMA1STAH= 0x00;
    DMA1STAL= __builtin_dmaoffset(&can1msgBuf);;
    DMA1STBH= 0x00;
    DMA1STBL= 0x00;
    DMA1PAD= (int16_t) &C1RXD;
    DMA1CNT= 0x07;
    // Clearing Channel 1 Interrupt Flag;
    IFS0bits.DMA1IF = 0;
    DMA1CONbits.CHEN = 1;
    
    
    // put the module in normal mode
    C1CTRL1bits.REQOP = 0;
    while(C1CTRL1bits.OPMODE != 0);
}

// ******************************************************************************
// *                                                                             
// *    Function:		CAN1_receive
// *    Description:       Receives the message from CAN buffer to user buffer 
// *                                                                             
// *    Arguments:		recCanMsg: pointer to the message object
// *                                             
// *    Return Value:      true - Receive successful
// *                       false - Receive failure                                                                              
// ******************************************************************************
int candrv_receive(can_msg_t *recCanMsg) 
{   
    uint_fast8_t currentBuffer;
    uint_fast8_t shiftAmount;
    uint8_t messageReceived = 0;
    uint16_t receptionFlags;

    // Check if message was received
    if(C1INTFbits.RBOVIF == 1)
    {
        C1INTFbits.RBOVIF = 0;
        return messageReceived;
    }
    
    currentBuffer = C1FIFObits.FNRB;
    if( currentBuffer < 16)
    {
	receptionFlags = C1RXFUL1;
        shiftAmount = currentBuffer;
    }
    else
    {
        receptionFlags = C1RXFUL2;
        shiftAmount = currentBuffer - 16;
    }
    
    if (((receptionFlags >> shiftAmount ) & 0x1) == 0x1)
    {           
       can1_read_from_dma_ram_buffer(currentBuffer, recCanMsg);

        if( currentBuffer < 16)
        {
            C1RXFUL1 &= ~(1 << shiftAmount);
        }
        else
        {
            C1RXFUL2 &= ~(1 << shiftAmount);
        }

        messageReceived = 1;
    }
        
    return (messageReceived);
}

// ******************************************************************************
// *                                                                             
// *    Function:		CAN1_transmit
// *    Description:       Transmits the message from user buffer to CAN buffer
// *                       as per the buffer number allocated.
// *                       Allocation of the buffer number is done by user 
// *                                                                             
// *    Arguments:		priority : priority of the message to be transmitted
// *                       sendCanMsg: pointer to the message object
// *                                             
// *    Return Value:      true - Transmit successful
// *                       false - Transmit failure                                                                              
// *****************************************************************************

typedef struct __attribute__((packed))
{
    unsigned priority                   :2;
    unsigned remote_transmit_enable     :1;
    unsigned send_request               :1;
    unsigned error                      :1;
    unsigned lost_arbitration           :1;
    unsigned message_aborted            :1;
    unsigned transmit_enabled           :1;
} can1_tx_controls_t;

int8_t candrv_transmit(can_msg_t *sendCanMsg) 
{
    can1_tx_controls_t * pTxControls = (can1_tx_controls_t*)&C1TR01CON;
    uint_fast8_t i;
    int8_t messageSent = 0;

    if(CAN1_TX_BUFFER_COUNT > 0)
    {
        for(i=0; i<CAN1_TX_BUFFER_COUNT; i++)
        {
            if(pTxControls->transmit_enabled == 1)
            {
                if (pTxControls->send_request == 0)
                {
                    can1_write_to_dma_ram_buffer( &can1msgBuf[i][0], sendCanMsg);

                    pTxControls->priority = 0;

                    // set the message for transmission
                    pTxControls->send_request = 1; 

                    messageSent = 1;
                    break;
                }
            }

            pTxControls++;
        }
    }

    return messageSent;
}
