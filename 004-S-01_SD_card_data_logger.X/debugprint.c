/*
 * File:   debugprint.c
 * Author: Hylke
 *
 * Created on March 1, 2020, 2:18 PM
 */

#include "debugprint.h"
#include <xc.h>
#include <stdint.h>

// Defines
#define DEBUGPRINT_UART_DATA_RATE   115200
#define DEBUGPRINT_BUFFER_SIZE      1028

#define DEBUGPRINT_PIN_TRIS_TX      TRISBbits.TRISB3
#define DEBUGPRINT_PIN_TRIS_RX      TRISBbits.TRISB4
#define DEBUGPRINT_PIN_ANSEL_TX     ANSELBbits.ANSB3
//#define DEBUGPRINT_PIN_ANSEL_RX
#define DEBUGPRINT_PIN_RP_TX        _RP35R
#define DEBUGPRINT_PIN_RP_RX        36

// Variables
static volatile struct {
    char data[DEBUGPRINT_BUFFER_SIZE];
    uint16_t in;
    uint16_t out;
} debugprint_buffer = {.in = 0, .out = 0};

// UART 2 interrupt
void __attribute__((interrupt(auto_psv))) _U2TXInterrupt(void){
    uint8_t data;
    
	// Fill the buffer till full or no more character are available
	while (!U2STAbits.UTXBF && (debugprint_buffer.in != debugprint_buffer.out)) {
		// Write character to transmit buffer
        data = debugprint_buffer.data[debugprint_buffer.out++];
		U2TXREG = data;
        if (debugprint_buffer.out == DEBUGPRINT_BUFFER_SIZE) debugprint_buffer.out = 0;
	}
	
    // Check if all data is send
    if (debugprint_buffer.in == debugprint_buffer.out) {
        // Disable interrupt but do not clear the flag.
        // This way when new data is put in buffer the interrupt will be re-enabled
        // and be triggered again.
        _U2TXIE = 0;
        return;
    }
    
	// Clear interrupt flag
	_U2TXIF = 0;
}

void debugprint_init(void) {
    // Set pins
#ifdef DEBUGPRINT_PIN_ANSEL_TX
    DEBUGPRINT_PIN_ANSEL_TX = 0;
#endif
#ifdef DEBUGPRINT_PIN_ANSEL_RX
    DEBUGPRINT_PIN_ANSEL_RX = 0;
#endif
    DEBUGPRINT_PIN_TRIS_TX = 0;
    DEBUGPRINT_PIN_TRIS_RX = 1;
    
    // PPS unlock
    __builtin_write_OSCCONL(OSCCON & ~(1<<6));
    // PPS
    DEBUGPRINT_PIN_RP_TX = _RPOUT_U2TX;
    _U2RXR = DEBUGPRINT_PIN_RP_RX;
    // PPS lock
    __builtin_write_OSCCONL(OSCCON | (1<<6));
    
    // Disable uart module
    U2MODEbits.UARTEN = 0;
    
    U2MODEbits.BRGH = 1;
    U2MODEbits.PDSEL = 0b00;    // 8-bit no parity
    U2MODEbits.STSEL = 0;       // one stop bit
    U2MODEbits.ABAUD = 0;       // Auto-Baud disabled
    U2STAbits.UTXISEL0 = 0;
    U2STAbits.UTXISEL1 = 0;     // Interrupt when space becomes available in tx buffer
    
    // BaudRate = 115200; Frequency = 60000000 Hz; BRG 129; 
    U2BRG = 129;
    
    // Interrupt settings
    _U2TXIF = 0;
    _U2TXIP = 1;    // Low priority
    _U2TXIE = 1;
    
    U2MODEbits.UARTEN = 1;
    U2STAbits.UTXEN = 1;        // Enable TX
}

void debugprint_char(char c) {
    uint16_t temp_in;
    
    // Disable interrupt
    _U2TXIE = 0;
    
    // Check if buffer has one more space
    temp_in = debugprint_buffer.in + 1;
    if (temp_in == DEBUGPRINT_BUFFER_SIZE) temp_in -= DEBUGPRINT_BUFFER_SIZE;
    if (temp_in != debugprint_buffer.out) {
        // Put the char in the buffer
        debugprint_buffer.data[debugprint_buffer.in] = c;
        debugprint_buffer.in++;
        if (debugprint_buffer.in == DEBUGPRINT_BUFFER_SIZE) debugprint_buffer.in = 0;
    }
    
    // Enable interrupts again
    _U2TXIE = 1;
}

void debugprint_string(char *str) {
    // Fill the buffer until \0 char is found
    while (*str != '\0') {
        debugprint_char(*str++);
    }
}

void debugprint_int(int32_t value) {
    char result_str[12];    // minus sign, 10 char value, zero termination
    char *ptr = result_str, *ptr1 = result_str;
    char tmp_char;
    int32_t tmp_value;
    
    do {
        tmp_value = value;
        value /= 10;
        *ptr++ = "9876543210123456789" [9 + (tmp_value - value * 10)];
    } while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    
    debugprint_string(result_str);
}

void debugprint_int_len(int32_t value, uint8_t len) {
    char result_str[12];    // minus sign, 10 char value, zero termination
    char *ptr = result_str, *ptr1 = result_str;
    char tmp_char;
    int32_t tmp_value;
    
    do {
        tmp_value = value;
        value /= 10;
        *ptr++ = "9876543210123456789" [9 + (tmp_value - value * 10)];
        len--;
    } while (value && len);

    // Apply negative sign
    if (tmp_value < 0 && len != 0) { 
        *ptr++ = '-';
        len--;
    }

    do {
        *ptr++ = ' ';
        len--;
    } while (len);
    
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    
    debugprint_string(result_str);
}

void debugprint_uint(uint32_t value) {
    char result_str[11];     // 10 char value, zero termination
    char *ptr = result_str, *ptr1 = result_str;
    char tmp_char;
    uint32_t tmp_value;
    
    do {
        tmp_value = value;
        value /= 10;
        *ptr++ = "9876543210123456789" [9 + (tmp_value - value * 10)];
    } while ( value );

    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    
    debugprint_string(result_str);
}

void debugprint_uint_len(uint32_t value, uint8_t len) {
    char result_str[11];     // 10 char value, zero termination
    char *ptr = result_str, *ptr1 = result_str;
    char tmp_char;
    uint32_t tmp_value;
    
    do {
        tmp_value = value;
        value /= 10;
        *ptr++ = "9876543210123456789" [9 + (tmp_value - value * 10)];
        len--;
    } while (value && len);

    do {
        *ptr++ = ' ';
        len--;
    } while (len);
    
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    
    debugprint_string(result_str);
}

void debugprint_hex(uint32_t value) {
    char result_str[9];     // 8 char value, zero termination
    char *ptr = result_str, *ptr1 = result_str;
    char tmp_char;
    uint32_t tmp_value;
    uint8_t i = 0;
    
    do {
        tmp_value = value;
        value /= 16;
        *ptr++ = "FEDCBA9876543210123456789ABCDEF" [15 + (tmp_value - value * 16)];
        i++;
    } while ( value );

    if ((i & 0b1) != 0) {
        *ptr++ = '0';
    }
    
    *ptr-- = '\0';
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    
    debugprint_string(result_str);
}
