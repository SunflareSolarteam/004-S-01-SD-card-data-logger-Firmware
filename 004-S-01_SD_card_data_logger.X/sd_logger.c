/*
 * File:   sd_logger.c
 * Author: Hylke
 *
 * Created on March 29, 2019, 7:33 PM
 */

#include <xc.h>
#include <stdint.h>
#include "sd_logger.h"
#include "mla_fileio/fileio.h"
#include "mla_fileio/sd_spi.h"
#include "debugprint.h"
#include <string.h>
#include "utl.h"
#include "device_logger_descriptors.h"

// ********************************************************
// * FILE IO AND SD CARD
// ********************************************************

void sd_logger_SdSpiConfigurePins (void);
inline void sd_logger_SdSpiSetCs(uint8_t a);
inline bool sd_logger_SdSpiGetCd(void);
inline bool sd_logger_SdSpiGetWp(void);

// The sdCardMediaParameters structure defines user-implemented functions needed by the SD-SPI fileio driver.
// The driver will call these when necessary.  For the SD-SPI driver, the user must provide
// parameters/functions to define which SPI module to use, Set/Clear the chip select pin,
// get the status of the card detect and write protect pins, and configure the CS, CD, and WP
// pins as inputs/outputs (as appropriate).
// For this demo, these functions are implemented in system.c, since the functionality will change
// depending on which demo board/microcontroller you're using.
// This structure must be maintained as long as the user wishes to access the specified drive.
FILEIO_SD_DRIVE_CONFIG sdCardMediaParameters =
{
    1,                                  // Use SPI module 2
    sd_logger_SdSpiSetCs,                    // User-specified function to set/clear the Chip Select pin.
    sd_logger_SdSpiGetCd,                    // User-specified function to get the status of the Card Detect pin.
    sd_logger_SdSpiGetWp,                    // User-specified function to get the status of the Write Protect pin.
    sd_logger_SdSpiConfigurePins             // User-specified function to configure the pins' TRIS bits.
};

// The gSDDrive structure allows the user to specify which set of driver functions should be used by the
// FILEIO library to interface to the drive.
// This structure must be maintained as long as the user wishes to access the specified drive.
const FILEIO_DRIVE_CONFIG gSdDrive =
{
    (FILEIO_DRIVER_IOInitialize)FILEIO_SD_IOInitialize,                      // Function to initialize the I/O pins used by the driver.
    (FILEIO_DRIVER_MediaDetect)FILEIO_SD_MediaDetect,                       // Function to detect that the media is inserted.
    (FILEIO_DRIVER_MediaInitialize)FILEIO_SD_MediaInitialize,               // Function to initialize the media.
    (FILEIO_DRIVER_MediaDeinitialize)FILEIO_SD_MediaDeinitialize,           // Function to de-initialize the media.
    (FILEIO_DRIVER_SectorRead)FILEIO_SD_SectorRead,                         // Function to read a sector from the media.
    (FILEIO_DRIVER_SectorWrite)FILEIO_SD_SectorWrite,                       // Function to write a sector to the media.
    (FILEIO_DRIVER_WriteProtectStateGet)FILEIO_SD_WriteProtectStateGet,     // Function to determine if the media is write-protected.
};

#define SD_PIN_ANSEL_CS     ANSELBbits.ANSB0
#define SD_PIN_ANSEL_MISO   ANSELAbits.ANSA0
#define SD_PIN_ANSEL_MOSI   ANSELAbits.ANSA2
#define SD_PIN_ANSEL_CLK    ANSELAbits.ANSA1
//#define SD_PIN_ANSEL_DETECT 

#define SD_PIN_TRIS_CS      TRISBbits.TRISB0
#define SD_PIN_TRIS_MISO    TRISAbits.TRISA0
#define SD_PIN_TRIS_MOSI    TRISAbits.TRISA2
#define SD_PIN_TRIS_CLK     TRISAbits.TRISA1
#define SD_PIN_TRIS_DETECT  TRISAbits.TRISA3

#define SD_PIN_LAT_CS       LATBbits.LATB0

#define SD_PIN_PORT_DETECT  PORTAbits.RA3

#define SD_PIN_RP_SDI       16
#define SD_PIN_RP_SDO       _RP18R
#define SD_PIN_RP_SCK       _RP17R


void sd_logger_SdSpiConfigurePins (void)
{
#ifdef SD_PIN_ANSEL_CS
    SD_PIN_ANSEL_CS = 0;
#endif
#ifdef SD_PIN_ANSEL_MISO
    SD_PIN_ANSEL_MISO = 0;
#endif
#ifdef SD_PIN_ANSEL_MOSI
    SD_PIN_ANSEL_MOSI = 0;
#endif
#ifdef SD_PIN_ANSEL_CLK
    SD_PIN_ANSEL_CLK = 0;
#endif
#ifdef SD_PIN_ANSEL_DETECT
    SD_PIN_ANSEL_DETECT = 0;
#endif
    
    SD_PIN_TRIS_CS = 0;
    SD_PIN_TRIS_MISO = 1;
    SD_PIN_TRIS_MOSI = 0;
    SD_PIN_TRIS_CLK = 0;
    SD_PIN_TRIS_DETECT = 1;
    
    SD_PIN_LAT_CS = 0;
    
    // PPS unlock
    __builtin_write_OSCCONL(OSCCON & ~(1<<6));
    // PPS
    _SDI1R = SD_PIN_RP_SDI;            // RP52 -> SPI1 SDI
    SD_PIN_RP_SDO = _RPOUT_SDO1;   // RP54 -> SPI1 SDO
    SD_PIN_RP_SCK = _RPOUT_SCK1;   // RP53 -> SPI1 SCK
    // PPS lock
    __builtin_write_OSCCONL(OSCCON | (1<<6));
}

inline void sd_logger_SdSpiSetCs(uint8_t a)
{
    if (a) {
        SD_PIN_LAT_CS = 1;
    } else {
        SD_PIN_LAT_CS = 0;
    }
}

inline bool sd_logger_SdSpiGetCd(void)
{
    if (!SD_PIN_PORT_DETECT) {
        return true;
    } else {
        return false;
    }
}

inline bool sd_logger_SdSpiGetWp(void)
{
    return false;
}

void GetTimestamp (FILEIO_TIMESTAMP * timeStamp)
{
    static uint16_t counter = 0;
    
    counter++;
    
    timeStamp->timeMs = 0;
    timeStamp->time.bitfield.hours = (counter / 1800) % 24;
    timeStamp->time.bitfield.minutes = (counter / 30) % 60;
    timeStamp->time.bitfield.secondsDiv2 = counter % 30;

    timeStamp->date.bitfield.day = 1;
    timeStamp->date.bitfield.month = 1;
    // Years in the FAT file system go from 1980-2108.
    timeStamp->date.bitfield.year = 20;
}

static int8_t sd_logger_fileio_init(void) {
    FILEIO_ERROR_TYPE error;
    // Initialize the library
    if (!FILEIO_Initialize()) {
        debugprint_string("Failed to init FILEIO\r\n");
        return -1;
    }
    
    FILEIO_RegisterTimestampGet (GetTimestamp);
    
    if (FILEIO_MediaDetect(&gSdDrive, &sdCardMediaParameters) != true) {
        debugprint_string("No media detected\r\n");
        return -1;
    } else {
        debugprint_string("Media detected\r\n");
    }
    if (FILEIO_SD_WriteProtectStateGet(&sdCardMediaParameters) == true) {
        debugprint_string("Media write protected\r\n");
        return -1;
    }
    error = FILEIO_DriveMount('A', &gSdDrive, &sdCardMediaParameters);
    if (error == FILEIO_ERROR_NONE) {
        debugprint_string("Successfully mounted the drive\r\n");
        return 0;
    } else {
        debugprint_string("Error mounting drive\r\n");
        debugprint_uint(error);
        return -1;
    }
    return -1;
}


// ********************************************************
// * LOGGING
// ********************************************************

static uint32_t sd_logger_file_number = 0;
static uint16_t sd_logger_file_bufs_written = 0;


static void sd_logger_find_free_file_number(void) {
    uint32_t i;
    char file_name[13];
    char temp[8];
    FILEIO_OBJECT file;
    
    // find next free number in filename
    for (i=0; i<99999; i++) {
        strcpy(file_name, "LOG");
        utl_uint32_to_string_len(i, temp, 10, 5);
        strcat(file_name, temp);
        strcat(file_name, ".CSV");
        // Try to open file
        if (FILEIO_Open(&file, file_name, FILEIO_OPEN_READ) != FILEIO_RESULT_SUCCESS) {
            // Could not open file. Means the file is not yet there and we can use this number.
            break;
        }
        FILEIO_Close (&file);
    }
    
    sd_logger_file_number = i;
}

static void sd_logger_write_to_file(char *buffer, uint16_t buffer_length) {
    FILEIO_OBJECT file;
    char file_name[13];
    static uint8_t write_errors = 0;
    char temp_string[16] = "";
    
    // Write to file
    strcpy(file_name, "LOG");
    utl_uint32_to_string_len(sd_logger_file_number, temp_string, 10, 5);
    strcat(file_name, temp_string);
    strcat(file_name, ".CSV");
    
    if (FILEIO_Open(&file, file_name, FILEIO_OPEN_WRITE | FILEIO_OPEN_APPEND | FILEIO_OPEN_CREATE) != FILEIO_RESULT_SUCCESS) {
        write_errors++;
        if (write_errors > 16) {
            asm("reset");
        }
        return;
    }else {
        write_errors = 0;
    }
    FILEIO_Write (buffer, 1, buffer_length, &file);
    FILEIO_Close (&file);
}

int8_t sd_logger_init(void) {
    // Init sd card until success
    int8_t res = sd_logger_fileio_init();
    if (res == -1) {
        return -1;
    } 
    // Successfully init filesystem
    else {
        sd_logger_find_free_file_number();
        
        debugprint_string("Using logfile ");
        debugprint_uint(sd_logger_file_number);
        debugprint_string("\r\n");
        
        return 0;
    }
}

void sd_logger_store_logging_buffer(logging_buffer_t *buf) {
    char log_string[256] = "";
    char temp_string[16] = "";
    uint16_t device_index, entry_index, data_index;
    
    // If this is first line of this file, write units
    if (sd_logger_file_bufs_written == 0) {
        strcpy(log_string, ";");
        // Device names
        for (device_index = 0; device_index < DEVICE_LIST_COUNT; device_index++) {
            strcat(log_string, device_list[device_index].name);
            for (entry_index = 0; entry_index < device_list[device_index].msg_count; entry_index++) {
                strcat(log_string, ";");
                // Names are max 16 chars long + ; char + null char
                if (strlen(log_string) >= (256 - 18)) {
                    sd_logger_write_to_file(log_string, strlen(log_string));
                    strcpy(log_string, "");
                }
            }
        }
        strcat(log_string, "\r\nTimeSinceBoot;");
        // Names are max 16 chars long + ; char + null char
        if (strlen(log_string) >= (256 - 18)) {
            sd_logger_write_to_file(log_string, strlen(log_string));
            strcpy(log_string, "");
        }
        // Data names
        for (device_index = 0; device_index < DEVICE_LIST_COUNT; device_index++) {
            for (entry_index = 0; entry_index < device_list[device_index].msg_count; entry_index++) {
                strcat(log_string, (device_list[device_index].msg_descr + entry_index)->name);
                strcat(log_string, ";");
                // Names are max 16 chars long + ; char + null char
                if (strlen(log_string) >= (256 - 18)) {
                    sd_logger_write_to_file(log_string, strlen(log_string));
                    strcpy(log_string, "");
                }
            }
        }
        strcat(log_string, "\r\nms;");
        // Names are max 16 chars long + ; char + null char
        if (strlen(log_string) >= (256 - 18)) {
            sd_logger_write_to_file(log_string, strlen(log_string));
            strcpy(log_string, "");
        }
        // Units
        for (device_index = 0; device_index < DEVICE_LIST_COUNT; device_index++) {
            for (entry_index = 0; entry_index < device_list[device_index].msg_count; entry_index++) {
                strcat(log_string, (device_list[device_index].msg_descr + entry_index)->unit);
                strcat(log_string, ";");
                // Names are max 16 chars long + ; char + null char
                if (strlen(log_string) >= (256 - 18)) {
                    sd_logger_write_to_file(log_string, strlen(log_string));
                    strcpy(log_string, "");
                }
            }
        }
        strcat(log_string, "\r\n");
        sd_logger_write_to_file(log_string, strlen(log_string));
    }
    
    // Write buffer
    strcpy(log_string, "");
    // Time since boot
    utl_uint32_to_string(buf->time_since_boot_ms, temp_string, 10);
    strcat(log_string, temp_string);
    strcat(log_string, ";");
    // Data
    data_index = 0;
    for (device_index = 0; device_index < DEVICE_LIST_COUNT; device_index++) {
        for (entry_index = 0; entry_index < device_list[device_index].msg_count; entry_index++) {
            switch ((device_list[device_index].msg_descr + entry_index)->type) {
                case UINT32:
                    utl_uint32_to_string(buf->data[data_index].uint32, temp_string, 10);
                    break;
                case INT32:
                    utl_int32_to_string(buf->data[data_index].int32, temp_string, 10);
                    break;
                case UINT16:
                    utl_uint32_to_string(buf->data[data_index].uint16, temp_string, 10);
                    break;
                case INT16:
                    utl_int32_to_string(buf->data[data_index].int16, temp_string, 10);
                    break;
                case UINT8:
                    utl_uint32_to_string(buf->data[data_index].uint8, temp_string, 10);
                    break;
                case INT8:
                    utl_int32_to_string(buf->data[data_index].int8, temp_string, 10);
                    break;
                case HEX32:
                    utl_uint32_to_string(buf->data[data_index].hex32, temp_string, 16);
                    break;
                case HEX16:
                    utl_uint32_to_string(buf->data[data_index].hex16, temp_string, 16);
                    break;
                case HEX8:
                    utl_uint32_to_string(buf->data[data_index].hex8, temp_string, 16);
                    break;
            }
            data_index++;
            strcat(log_string, temp_string);
            strcat(log_string, ";");
            // Data are max 10 chars long + ; char + null char
            if (strlen(log_string) >= (256 - 18)) {
                sd_logger_write_to_file(log_string, strlen(log_string));
                strcpy(log_string, "");
            }
        }
    }
    strcat(log_string, "\r\n");
    sd_logger_write_to_file(log_string, strlen(log_string));
    
    // Increment buffers written to this file counter
    sd_logger_file_bufs_written++;
    // When 100 buffers written, start new file
    if (sd_logger_file_bufs_written >= 252) {
        sd_logger_file_bufs_written = 0;
        sd_logger_file_number++;
    }
}
