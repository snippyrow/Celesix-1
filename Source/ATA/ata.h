#define ATA_BASE 0x1F0   // Base I/O port for primary ATA controller
#define ATA_DATA 0x00    // Data register
#define ATA_ERROR 0x01   // Error register
#define ATA_SECTOR_COUNT 0x02 // Sector count register
#define ATA_LBA_LOW 0x03  // LBA low register
#define ATA_LBA_MID 0x04  // LBA mid register
#define ATA_LBA_HIGH 0x05 // LBA high register
#define ATA_DRIVE 0x06    // Drive register
#define ATA_COMMAND 0x07  // Command register

#define ATA_CMD_READ_SECTORS 0x20 // Command to read sectors