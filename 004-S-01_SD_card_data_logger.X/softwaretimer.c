/*
 * File:   softwaretimer.c
 * Author: Hylke
 *
 * Created on March 1, 2020, 1:59 PM
 */

#include <xc.h>
#include <stdint.h>
#include "softwaretimer.h"

// Timer resources
static volatile struct{
    uint32_t set_value_ms;
    uint32_t time_left_ms;
    uint8_t used : 1;
    uint8_t mode : 1;
    uint8_t running : 1;
    uint8_t expired : 1;
} softwaretimers[SOFTWARETIMER_MAX_TIMERS] = {};

// Timer 1 interrupt. Triggers every 1 ms
void __attribute__ ( ( interrupt, no_auto_psv ) ) _T3Interrupt(void) {
    uint8_t timer_number;
    
    // Check all timers
    for (timer_number = 0; timer_number < SOFTWARETIMER_MAX_TIMERS; timer_number++) {
        // Skip non used and non running timers
        if (softwaretimers[timer_number].used == 0 || softwaretimers[timer_number].running == 0) {
            continue;
        }
        // Decrease time left
        if (softwaretimers[timer_number].time_left_ms != 0) {
            softwaretimers[timer_number].time_left_ms--;
        }
        // Check if expired
        if (softwaretimers[timer_number].time_left_ms == 0) {
            softwaretimers[timer_number].expired = 1;
            // Restart if continuous mode and stop if single
            if (softwaretimers[timer_number].mode == SOFTWARETIMER_CONTINUOUS_MODE) {
                softwaretimers[timer_number].time_left_ms = softwaretimers[timer_number].set_value_ms;
            } else {
                softwaretimers[timer_number].running = 0;
            }
        }
    }
    
    // Reset flag
    _T3IF = 0;
}

// Initializes the programmable software timers
void softwaretimer_init(void) {
    // Init timer 3
    T3CONbits.TON = 0;
    //TMR3 0; 
    TMR3 = 0x00;
    //Period = 0.001 s; Frequency = 60000000 Hz; PR1 7500; 
    PR3 = 7500;
    //TCKPS 1:8; TON enabled; TSIDL disabled; TCS FOSC/2; TSYNC disabled; TGATE disabled; 
    T3CONbits.TCS = 0;      // Source is FP
    T3CONbits.TCKPS = 0b01; // Pre-scaler 1:8
    T3CONbits.TGATE = 0;
    T3CONbits.TSIDL = 0;

    // Enable interrupt
    _T3IF = 0;
    _T3IP = 4; // Medium priority
    _T3IE = 1;
    
    // Start timer
    T3CONbits.TON = 1;
}

// Creates a new timer
// Parameters:
//  mode            Timer mode, single or continuous. In single mode the timer will be freed upon expire.
//                  In continuous the timer will reset and count again upon expire.
// Returns:
//  The timer number created. Returns -1 if no free timer is available.
int8_t softwaretimer_create(uint8_t mode) {
    uint8_t timer_number;
    
    // Check mode boundary
    if (!(SOFTWARETIMER_SINGLE_MODE <= mode && mode <= SOFTWARETIMER_CONTINUOUS_MODE)) {
        return -1;
    }
    
    // Get first free timer;
    for (timer_number = 0; timer_number < SOFTWARETIMER_MAX_TIMERS; timer_number++) {
        if (softwaretimers[timer_number].used == 0) {
            break;
        }
    }
    // Check if free timer is found
    if (timer_number == SOFTWARETIMER_MAX_TIMERS) {
        return -1;
    }
    // Create timer
    softwaretimers[timer_number].mode = mode;
    softwaretimers[timer_number].expired = 0;
    softwaretimers[timer_number].set_value_ms = 0;
    softwaretimers[timer_number].time_left_ms = 0;
    softwaretimers[timer_number].running = 0;
    softwaretimers[timer_number].used = 1;
    // Return timer no
    return timer_number;
}

// Deletes a timer and frees its recources.
// Parameters:
//  timer_number    The timer to delete. This was returned when creating the timer
// Returns:
//  Returns 0 if successfully deleted. Returns -1 if timer_number is out of range.
int8_t softwaretimer_delete(uint8_t timer_number) {
    // Check timer number boundary
    if (timer_number >= SOFTWARETIMER_MAX_TIMERS) {
        return -1;
    }
    // Delete timer
    softwaretimers[timer_number].used = 0;
    return 0;
}

// Start a software timer
// Parameters:
//  timer_number    The timer to start. This was returned when creating the timer
//  ms              time till expire in ms
// Returns:
//  Returns 0 if successfully started. Returns -1 if timer_number is out of range.
int8_t softwaretimer_start(uint8_t timer_number, uint32_t ms) {
    // Check timer number boundary
    if (timer_number >= SOFTWARETIMER_MAX_TIMERS) {
        return -1;
    }
    // Check if timer was created
    if (softwaretimers[timer_number].used == 0) {
        return -1;
    }
    // Start timer
    softwaretimers[timer_number].set_value_ms = ms;
    softwaretimers[timer_number].time_left_ms = ms;
    softwaretimers[timer_number].running = 1;
    return 0;
}

// Stops a running timer.
// Parameters:
//  timer_number    The timer to stop. This was returned when creating the timer
// Returns:
//  returns 0 if successful stop of the timer.
//  -1 if the timer number was not a running timer or out of range.
int8_t softwaretimer_stop(uint8_t timer_number) {
    // Check timer number boundary
    if (timer_number >= SOFTWARETIMER_MAX_TIMERS) {
        return -1;
    }
    // Check if timer was created
    if (softwaretimers[timer_number].used == 0) {
        return -1;
    }
    softwaretimers[timer_number].running = 0;
    return 0;
}

// Returns if a timer has expired and clears the expire mark.
// Parameters:
//  timer_number    The timer to check. This number was return when the timer was started.
// Returns:
//  True if the timer is expired, false is the timer is not expired.
//  -1 if the timer number was not a running timer or out of range.
int8_t softwaretimer_get_expired(uint8_t timer_number) {
    // Check timer number boundary
    if (timer_number >= SOFTWARETIMER_MAX_TIMERS) {
        return -1;
    }
    if (softwaretimers[timer_number].expired == 1) {
        softwaretimers[timer_number].expired = 0;
        return 1;
    } else {
        return 0;
    }
}
