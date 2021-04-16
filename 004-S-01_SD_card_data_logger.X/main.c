/*
 * File:   main.c
 * Author: Hylke
 *
 * Created on February 17, 2019, 4:46 PM
 */

#include <stdint.h>
#include "config.h"
#include "softwaretimer.h"
#include "debugprint.h"
#include "can.h"
#include "device_logger.h"
#include "device_logger_descriptors.h"
#include "flash.h"
#include "sd_logger.h"
#include "gps.h"

#define LED_PIN_TRIS_RED    TRISBbits.TRISB12
#define LED_PIN_TRIS_GREEN  TRISBbits.TRISB13
#define LED_PIN_LAT_RED     LATBbits.LATB12
#define LED_PIN_LAT_GREEN   LATBbits.LATB13


// Main application
int main(void) {
    int8_t one_sec_timer = SOFTWARETIMER_NONE, log_timer = SOFTWARETIMER_NONE;
    uint32_t time_since_boot_sec = 0;
    uint16_t i;
    enum {
        DATA_GATHERING,
        DATA_SAVING
    } logging_mode = DATA_GATHERING;
    can_msg_t rx_msg;
    logging_buffer_t logging_buffer;
    gps_time_t time;
    
    LED_PIN_TRIS_RED = 0;
    LED_PIN_TRIS_GREEN = 0;
    LED_PIN_LAT_RED = 1;
    LED_PIN_LAT_GREEN = 0;
    
    // Init clock
    // Set Fosc to 120Mhz
    // Configure PLL prescaler, PLL postscaler, PLL divisor
    CLKDIVbits.PLLPRE = 0;  // Input divided by 2
    CLKDIVbits.PLLPOST = 0; // Output divided by 2
    PLLFBDbits.PLLDIV = 28; // Feedback division by 30
    
    // Initiate Clock Switch to Primary Oscillator with PLL (NOSC=0b011)
    __builtin_write_OSCCONH(0x03);
    __builtin_write_OSCCONL(OSCCON | 0x01);
    
    // Wait for Clock switch to occur
    while (OSCCONbits.COSC!= 0b011);
    // Wait for PLL to lock
    while (OSCCONbits.LOCK!= 1);
    
    // Init software
    debugprint_init();
    softwaretimer_init();
    can_init();
    flash_init();
    device_logger_init();
    sd_logger_init();
    gps_init();
    
    // Create timers
    one_sec_timer = softwaretimer_create(SOFTWARETIMER_CONTINUOUS_MODE);
    softwaretimer_start(one_sec_timer, 1000);
    // Start logging timer
    log_timer = softwaretimer_create(SOFTWARETIMER_CONTINUOUS_MODE);
    softwaretimer_start(log_timer, DATA_LOGGING_RATE_MS);
    
    debugprint_string("Hello universe!\r\nBecause greeting the world is thinking too small...\r\n");
    
    while (1) {
        
        switch (logging_mode) {
            case DATA_GATHERING:
                gps_handler();
                // Transmit CAN bus messages
                can_transmit_process();
                
                // Check if a can bus message has been received
                if (can_receive_message(&rx_msg)) {
                    // Decode and collect data in local ram
                    device_logger_decode_and_collect_can_message(rx_msg);
                }
                
                // Check if data is ready to be stored
                if (softwaretimer_get_expired(log_timer)) {
                    LED_PIN_LAT_GREEN = !LED_PIN_LAT_GREEN;
                    device_logger_increase_time_since_boot(DATA_LOGGING_RATE_MS);
                    get_device_logger_collected_data(&logging_buffer);
                    device_logger_clear_data();
                    
                    // Store to flash
                    flash_store_logging_data(&logging_buffer);
                }
                
                // Check if we need to store to sd
                if (flash_get_flash_full()) {
                    logging_mode = DATA_SAVING;
                }
                break;
                
            case DATA_SAVING:
                // Save data
                LED_PIN_LAT_RED = 1;
                LED_PIN_LAT_GREEN = 0;
                for (i = 0; i < flash_get_flash_number_of_data(); i++) {
                    flash_get_flash_logging_data(&logging_buffer, i);
                    sd_logger_store_logging_buffer(&logging_buffer);
                    // Kick the dog
                    ClrWdt();
                }
                flash_clear_data();
                logging_mode = DATA_GATHERING;
                LED_PIN_LAT_RED = 0;
                break;
                
            default:
                logging_mode = DATA_GATHERING;
                break;
        }
        
        // Triggers every 1 sec
        if (softwaretimer_get_expired(one_sec_timer) == 1) {
            time_since_boot_sec++;
            debugprint_string("Time since bootup: ");
            debugprint_uint(time_since_boot_sec);
            debugprint_string("sec\r\n");
            
            time = get_gps_time();
            debugprint_string("Time: ");
            debugprint_uint(time.hour);
            debugprint_string(":");
            debugprint_uint(time.min);
            debugprint_string(":");
            debugprint_uint(time.sec);
            debugprint_string("\r\n");
        }
        
        // Kick the dog
        ClrWdt();
        
        // Modes:
        // 1 normal
        //      Receive messages and store in ram
        //      Store in flash every x ms
        // 2 saving
        //      Get messages from flash
        //      Store on sd card
        //      Clear all data
    }
    return 1; 
}
