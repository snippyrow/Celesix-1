#include "ata.h"

// SPECS: https://www.scs.stanford.edu/23wi-cs212/pintos/specs/ata-3-std.pdf
// EX: https://github.com/dthain/basekernel/blob/master/kernel/ata.c#L100
// OTHER: https://gitlab.com/monarrk/untrust/-/blob/master/src/ata/mod.rs?ref_type=heads

void read_sectors(uint32_t lba, uint8_t *buffer, uint32_t sector_count) {
    outb(ATA_BASE + ATA_DRIVE, currentATA | 0x00); // Select drive
    outb(ATA_BASE + ATA_SECTOR_COUNT, sector_count); // Set sector count
    outb(ATA_BASE + ATA_LBA_LOW, lba & 0xFF); // Set LBA low byte
    outb(ATA_BASE + ATA_LBA_MID, (lba >> 8) & 0xFF); // Set LBA mid byte
    outb(ATA_BASE + ATA_LBA_HIGH, (lba >> 16) & 0xFF); // Set LBA high byte
    outb(ATA_BASE + ATA_COMMAND, ATA_CMD_READ_SECTORS); // Issue read command

    // Wait for the drive to be ready
    while ((inb(ATA_BASE + ATA_COMMAND) & 0x08) == 0) {}

    // Read data from the data register
    for (uint32_t i = 0; i < (sector_count * 512); i += 2) {
        uint16_t data = inw(ATA_BASE + ATA_DATA);
        buffer[i] = data & 0xFF;
        buffer[i + 1] = (data >> 8) & 0xFF;
    }
}
