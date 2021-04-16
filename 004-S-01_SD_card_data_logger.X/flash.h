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
 * File:        flash.h
 * Author:      H. Veenstra
 * Comments:    dummy flash implementation
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef FLASH_H
#define	FLASH_H

#include <stdint.h>
#include "device_logger_descriptors.h"

void flash_init(void);

void flash_store_logging_data(logging_buffer_t *logging_buffer_ptr);

uint8_t flash_get_flash_full(void);

uint16_t flash_get_flash_number_of_data(void);

int8_t flash_get_flash_logging_data(logging_buffer_t *logging_buffer_ptr, uint16_t number);

void flash_clear_data(void);

#endif	/* FLASH_H */
