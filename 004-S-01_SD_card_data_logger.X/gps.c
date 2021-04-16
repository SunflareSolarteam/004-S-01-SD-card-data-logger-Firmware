/*
 * File:   gps.c
 * Author: Hylke
 *
 * Created on March 7, 2019, 5:39 PM
 */

#include <xc.h>
#include <stdint.h>
#include "gps.h"
#include "softwaretimer.h"

// Defines
#define GPS_UART_DATA_RATE   115200
#define GPS_BUFFER_SIZE      1024

#define GPS_PIN_TRIS_TX      TRISCbits.TRISC2
#define GPS_PIN_TRIS_RX      TRISCbits.TRISC7
#define GPS_PIN_ANSEL_TX     ANSELCbits.ANSC2
//#define GPS_PIN_ANSEL_RX     
#define GPS_PIN_RP_TX        _RP50R
#define GPS_PIN_RP_RX        55

// Variables
static volatile struct {
    char data[GPS_BUFFER_SIZE];
    uint16_t in;
    uint16_t out;
} gps_tx_buffer = {.in = 0, .out = 0}, gps_rx_buffer = {.in = 0, .out = 0};

static gps_time_t gps_time = {};
static gps_coordinates_t gps_coordinates = {};
static gps_speed_t gps_speed = {};
static uint8_t gps_satellites = 0;
static uint8_t gps_tick = 0;
static uint32_t gps_time_stamp = 0;

// UART 1 interrupt
void __attribute__((interrupt(auto_psv))) _U1TXInterrupt(void){
    uint8_t data;
    
	// Fill the buffer till full or no more character are available
	while (!U1STAbits.UTXBF && (gps_tx_buffer.in != gps_tx_buffer.out)) {
		// Write character to transmit buffer
        data = gps_tx_buffer.data[gps_tx_buffer.out++];
		U1TXREG = data;
        if (gps_tx_buffer.out == GPS_BUFFER_SIZE) gps_tx_buffer.out = 0;
	}
	
    // Check if all data is send
    if (gps_tx_buffer.in == gps_tx_buffer.out) {
        // Disable interrupt but do not clear the flag.
        // This way when new data is put in buffer the interrupt will be re-enabled
        // and be triggered again.
        _U1TXIE = 0;
        return;
    }
    
	// Clear interrupt flag
	_U1TXIF = 0;
}

// UART 1 interrupt
void __attribute__((interrupt(auto_psv))) _U1RXInterrupt(void){
    uint16_t temp_in;
    
	// Fill the buffer till full or no more character are available
	while (U1STAbits.URXDA) {
        // Check if buffer has one more space
        temp_in = gps_rx_buffer.in + 1;
        if (temp_in == GPS_BUFFER_SIZE) temp_in -= GPS_BUFFER_SIZE;
        if (temp_in != gps_rx_buffer.out) {
            // Put the char in the buffer
            gps_rx_buffer.data[gps_rx_buffer.in] = U1RXREG;
            gps_rx_buffer.in++;
            if (gps_rx_buffer.in == GPS_BUFFER_SIZE) gps_rx_buffer.in = 0;
        } else {
            // Dummy read to discard data
            U1RXREG;
        }
	}
	
	// Clear interrupt flag
	_U1RXIF = 0;
}

void gps_send_char(char c) {
    uint16_t temp_in;
    
    // Disable interrupt
    _U1TXIE = 0;
    
    // Check if buffer has one more space
    temp_in = gps_tx_buffer.in + 1;
    if (temp_in == GPS_BUFFER_SIZE) temp_in -= GPS_BUFFER_SIZE;
    if (temp_in != gps_tx_buffer.out) {
        // Put the char in the buffer
        gps_tx_buffer.data[gps_tx_buffer.in] = c;
        gps_tx_buffer.in++;
        if (gps_tx_buffer.in == GPS_BUFFER_SIZE) gps_tx_buffer.in = 0;
    }
    
    // Enable interrupts again
    _U1TXIE = 1;
}

void gps_send_string(char *str) {
    // Fill the buffer until \0 char is found
    while (*str != '\0') {
        gps_send_char(*str++);
    }
}

static char gps_extract_char(char* s, uint8_t index) {
    uint8_t i = 0;
    
    //find the nth ,
    for (i = 0; i < index; i++) {
        while (*s != ',' && *s != '\0') {
            s++;
        }     
        s++;
    }
    
    if (*s == ',') {
        return '?';
    } else {
        return *s;
    }
}

static int32_t gps_extract_value(char* s, uint8_t index, uint8_t res) {
    uint8_t i = 0;
    int32_t value = 0;
    uint8_t neg = 0;
    
    //find the nth ,
    for (i = 0; i < index; i++) {
        while (*s != ',' && *s != '\0') {
            s++;
        }     
        s++;
    }
    
    //Get value before dot
    while (*s != '.' && *s != ',' && *s != '\0') {
        if (*s == '-') {
            neg = 1;
        }
        if (*s >= '0' && *s <= '9') {
            value *= 10;
            value += (uint32_t)*s - '0';
        }
        s++;
    }
    
    // Get value after dot if present
    if (*s == '.' && res != 0) {
        s++;
        while (*s != ',' && res != 0 && *s != '\0') {
            if (*s >= '0' && *s <= '9') {
                value *= 10;
                value += (uint32_t)*s - '0';
            }
            s++;
            res--;
        }
    }
    
    // Finish value by resolution
    while (res != 0) {
        value *= 10;
        res--;
    }
    if (neg) {
        value *= -1;
    }
    
    return value;
}

void gps_init(void) {
    // Set pins
#ifdef GPS_PIN_ANSEL_TX
    GPS_PIN_ANSEL_TX = 0;
#endif
#ifdef GPS_PIN_ANSEL_RX
    GPS_PIN_ANSEL_RX = 0;
#endif
    GPS_PIN_TRIS_TX = 0;
    GPS_PIN_TRIS_RX = 1;
    
    // PPS unlock
    __builtin_write_OSCCONL(OSCCON & ~(1<<6));
    // PPS
    GPS_PIN_RP_TX = _RPOUT_U1TX;
    _U1RXR = GPS_PIN_RP_RX;
    // PPS lock
    __builtin_write_OSCCONL(OSCCON | (1<<6));
    
    // Disable uart module
    U1MODEbits.UARTEN = 0;
    
    U1MODEbits.BRGH = 1;
    U1MODEbits.PDSEL = 0b00;    // 8-bit no parity
    U1MODEbits.STSEL = 0;       // one stop bit
    U1MODEbits.ABAUD = 0;       // Auto-Baud disabled
    U1STAbits.UTXISEL0 = 0;
    U1STAbits.UTXISEL1 = 0;     // Interrupt when space becomes available in tx buffer
    
    // BaudRate = 115200; Frequency = 60000000 Hz; BRG 129; 
    //U1BRG = 129;
    U1BRG = 1561;
    
    // Interrupt settings
    _U1TXIF = 0;
    _U1TXIP = 2;    // Normal priority
    _U1TXIE = 1;
    
    _U1RXIF = 0;
    _U1RXIP = 2;    // Normal priority
    _U1RXIE = 1;
    
    U1MODEbits.UARTEN = 1;
    U1STAbits.UTXEN = 1;        // Enable TX
}

void gps_handler(void) {
    /*
    char c;
    while (gps_rx_buffer.in != gps_rx_buffer.out) {
		// Write character to transmit buffer
        c = (char)gps_rx_buffer.data[gps_rx_buffer.out++];
		debugprint_char(c);
        if (gps_rx_buffer.out == GPS_BUFFER_SIZE) gps_rx_buffer.out = 0;
	}*/
    
    
    char gps_line_buffer[128];
    uint16_t index, i;
    uint32_t value;
    char ch;

    // Skip until the $ sign, indicating new gps data line
    while (gps_rx_buffer.in != gps_rx_buffer.out) {
        if (gps_rx_buffer.data[gps_rx_buffer.out] == '$') {
            // $ found
            break;
        }
        gps_rx_buffer.out++;
        if (gps_rx_buffer.out == GPS_BUFFER_SIZE) gps_rx_buffer.out = 0; 
	}
    // Return when no valid start has been found
    if (gps_rx_buffer.data[gps_rx_buffer.out] != '$') {
        return;
    }
    
    // Read until a newline is found
    index = gps_rx_buffer.out;
    while (1) {
        // Return when no valid end has been found
        if (gps_rx_buffer.in == index) {
            return;
        }
        if (gps_rx_buffer.data[index] == '\r') {
            // $ found
            break;
        }
        index++;
        if (index == GPS_BUFFER_SIZE) index = 0; 
	}
    
    // Copy to temp buffer
    gps_rx_buffer.out++; // Skip $
    i = 0;
    while (1) {
        gps_line_buffer[i] = gps_rx_buffer.data[gps_rx_buffer.out];
        if (gps_rx_buffer.out == index) {
            break;
        }
        gps_rx_buffer.out++;
        if (gps_rx_buffer.out == GPS_BUFFER_SIZE) gps_rx_buffer.out = 0; 
        i++;
        if (i == 128) {
            return;
        }
    }
    gps_line_buffer[i+1] = '\0';

    // Data consists of string indicating the type of data and then the data separated by commas
    /*
    $GPRMC,095241.00,A,5306.68774,N,00604.15290,E,0.006,,050617,,,A*7F
    $GPVTG,,T,,M,0.006,N,0.012,K,A*26
    $GPGGA,095241.00,5306.68774,N,00604.15290,E,1,08,0.93,-2.7,M,45.7,M,,*7C
    $GPGSA,A,3,14,19,32,12,15,25,24,17,,,,,1.59,0.93,1.29*03
    $GPGSV,4,1,13,02,09,124,07,06,18,083,29,10,03,262,25,12,89,302,46*78
    $GPGSV,4,2,13,14,23,316,42,15,09,180,39,17,15,039,34,19,32,050,39*7C
    $GPGSV,4,3,13,22,03,352,31,24,62,134,41,25,43,252,43,29,02,197,30*7F
    $GPGSV,4,4,13,32,37,298,39*47
    $GPGLL,5306.68774,N,00604.15290,E,095241.00,A,A*65
    */
    
    // GGA
    if (    gps_line_buffer[0] == 'G' &&
            gps_line_buffer[1] == 'P' &&
            gps_line_buffer[2] == 'G' &&
            gps_line_buffer[3] == 'G' &&
            gps_line_buffer[4] == 'A') {
        
        // Number of satellites
        value = gps_extract_value(gps_line_buffer, 7, 0);
        gps_satellites = value;
        
        //Height
        value = gps_extract_value(gps_line_buffer, 9, 1);
        gps_coordinates.height_m = value;
    }
    
    // RMC
    if (    gps_line_buffer[0] == 'G' &&
            gps_line_buffer[1] == 'P' &&
            gps_line_buffer[2] == 'R' &&
            gps_line_buffer[3] == 'M' &&
            gps_line_buffer[4] == 'C') {
        
        
        
        // Time stamp
        value = gps_extract_value(gps_line_buffer, 1, 0);
        gps_time.hour = value / 10000;
        gps_time.min = (value / 100) % 100;
        gps_time.sec = value % 100;
        gps_time_stamp = value;
        
        // Latitude
        value = gps_extract_value(gps_line_buffer, 3, 5);
        gps_coordinates.latitude_degrees = value / 10000000;
        gps_coordinates.latitude_minutes = value % 10000000;
        ch = gps_extract_char(gps_line_buffer, 4);
        if (ch == 'W') {
            gps_coordinates.latitude_degrees *= -1;
        }
        //debugprint_uint(value);
        //debugprint_string("\r\n");
        
        // Longitude
        value = gps_extract_value(gps_line_buffer, 5, 5);
        gps_coordinates.longitude_degrees = value / 10000000;
        gps_coordinates.longitude_minutes = value % 10000000;
        ch = gps_extract_char(gps_line_buffer, 6);
        if (ch == 'S') {
            gps_coordinates.longitude_degrees *= -1;
        }
        //debugprint_uint(value);
        //debugprint_string("\r\n");
        
        // Date
        value = gps_extract_value(gps_line_buffer, 9, 0);
        gps_time.day = value / 10000;
        gps_time.month = (value/ 100) % 100;
        gps_time.year = value % 100;
        
        // Set gps tick if we received a valid timestamp
        if (gps_time.day != 0) {
            gps_tick = 1;
        }
    }
    
    // VTG
    if (    gps_line_buffer[0] == 'G' &&
            gps_line_buffer[1] == 'P' &&
            gps_line_buffer[2] == 'V' &&
            gps_line_buffer[3] == 'T' &&
            gps_line_buffer[4] == 'G') {
        
        // Track in degrees
        value = gps_extract_value(gps_line_buffer, 1, 1);
        gps_speed.direction_degrees = value;
        
        // Speed km
        value = gps_extract_value(gps_line_buffer, 7, 2);
        gps_speed.speed_kmh = value;
    }
    
}

// Get functions
gps_time_t get_gps_time(void) {
    return gps_time;
}

gps_coordinates_t get_gps_coordinates(void) {
    return gps_coordinates;
}

gps_speed_t get_gps_speed(void) {
    return gps_speed;
}

uint8_t get_gps_satellites(void) {
    return gps_satellites;
}

uint8_t get_gps_tick(void) {
    if (gps_tick) {
        gps_tick = 0;
        return 1;
    } else {
        return 0;
    }
}

uint32_t get_gps_time_stamp(void) {
    return gps_time_stamp;
}

uint32_t get_gps_lat_deg(void) {
    return gps_coordinates.latitude_degrees;
}

uint32_t get_gps_lat_10u_min(void) {
    return gps_coordinates.latitude_minutes;
}

uint32_t get_gps_long_deg(void) {
    return gps_coordinates.longitude_degrees;
}

uint32_t get_gps_long_10u_min(void) {
    return gps_coordinates.longitude_minutes;
}

uint16_t get_gps_speed_10mtrph(void) {
    return gps_speed.speed_kmh;
}

uint16_t get_gps_direction_100mdeg(void) {
    return gps_speed.direction_degrees;
}

uint16_t get_gps_number_of_satellites(void) {
    return gps_satellites;
}
