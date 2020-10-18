#include <stdint.h>
#include <stdbool.h>
#include "drv_spi.h"

#define DRV_SPI_CONFIG_V2_ENABLED

// The File I/O library requires the user to define the system clock frequency (Hz)
#define SYS_CLK_FrequencySystemGet()            60000000
// The File I/O library requires the user to define the peripheral clock frequency (Hz)
#define SYS_CLK_FrequencyPeripheralGet()        SYS_CLK_FrequencySystemGet()
// The File I/O library requires the user to define the instruction clock frequency (Hz)
#define SYS_CLK_FrequencyInstructionGet()       (SYS_CLK_FrequencySystemGet() / 2)