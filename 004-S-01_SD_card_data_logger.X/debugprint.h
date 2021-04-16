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
 * File:        debugprint.h
 * Author:      H. Veenstra
 * Comments:    uart debug output
 */

#ifndef DEBUGPRINT_H
#define	DEBUGPRINT_H

#include <stdint.h>

// Initializes the debugprint over uart
void debugprint_init(void);

// Prints a character onto the debug uart
// Parameters:
//  c            Character to be printed
void debugprint_char(char c);

// Prints a string onto the debug uart
// Parameters:
//  *str            Pointer to the string to send. The string needs to be \0 terminated
void debugprint_string(char *str);

// Prints a signed integer onto the debug uart
// Parameters:
//  value           The integer to be printed
void debugprint_int(int32_t value);

// Prints a signed integer onto the debug uart
// Parameters:
//  value           The integer to be printed
void debugprint_int_len(int32_t value, uint8_t len);

// Prints an unsigned integer onto the debug uart
// Parameters:
//  value           The integer to be printed
void debugprint_uint(uint32_t value);

// Prints an unsigned integer onto the debug uart
// Parameters:
//  value           The integer to be printed
void debugprint_uint_len(uint32_t value, uint8_t len);

// Prints a hex integer onto the debug uart
// Parameters:
//  value           The integer to be printed
void debugprint_hex(uint32_t value);

#endif	/* DEBUGPRINT_H */

