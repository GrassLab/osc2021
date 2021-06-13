#ifndef __FS_FAT32_H_
#define __FS_FAT32_H_

#define RESERVED_SECTORS 32
#define SECTOR_SIZE 512

#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN 0x02
#define ATTR_SYSTEM 0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE 0x20
#define ATTR_LONG_NAME 0xf /* should be examined first */

#define FREE_CLUSTER 0x0
#define END_OF_CHAIN 0x0fffffff
#define END_OF_CHAIN_THRESHOLD 0x0ffffff8

struct chs_address {
    unsigned char head;
    unsigned short sectcylinder;
} __attribute__((packed));

struct partition_entry {
    unsigned char status;
    struct chs_address start;
    unsigned char type;
    struct chs_address end;
    unsigned int sector_start;
    unsigned int sectors_count;
} __attribute__((packed));

struct MBR {
    char bootstrap[446];
    struct partition_entry partition[4];
    char signature[2];
} __attribute__((packed));

struct FAT32_Bootsector {
    char jmp[3];
    char OemName[8];
    unsigned short BytesPerSector; // legal == { 512, 1024, 2048, 4096 }
    unsigned char SectorsPerCluster; // legal == { 1, 2, 4, 8, 16, 32, 64, 128 }
    unsigned short ReservedSectors; // must not be zero; legal for FAT12/16 == { 1 }, typically 32 for FAT32
    unsigned char NumberOfFatTables; // must not be zero; warn if this is not set to the value 1 or 2
    unsigned short MaxRootDirEntries; // legal for FAT12/16 == N * (BytesPerSector / 32), N is non-zero; must be {0} for FAT32
    unsigned short NumberOfSectors16; // must be {0} for FAT32; if {0}, then NumberOfSectors32 must be non-zero
    char MediaDescriptor; // legacy
    unsigned short SectorsPerFat16; // must be {0} for FAT32; must be non-zero for FAT12/16
    unsigned short SectorsPerTrack; // legacy
    unsigned short HeadsPerCylinder; // legacy
    unsigned int NumHiddenSectors; // legacy
    unsigned int NumberOfSectors32; // must be non-zero for FAT32; must be >= 0x10000 if NumberOfSectors16 is zero
    unsigned int SectorsPerFat32;
    unsigned short ActiveFatIndex : 4; // zero-based index of the active FAT
    unsigned short FlagsReserved1 : 3;
    unsigned short TransactionFat : 1; // 1 means only one FAT active, as indicated by ActiveFatIndex; 0 means both FATs are mirrored
    unsigned short FlagsReserved2 : 8;
    unsigned short Version; // must be 0
    unsigned int RootCluster; // cluster number (FAT index) for root.  usually 2.  preferably first non-bad sector.
    unsigned short InfoSector; // usually 1.
    unsigned short BootBackupStart; // usually 6.  No other value than 6 is recommended
    char Reserved[12]; // set to zero by formatting utility, no indicaton of other uses but shall be ignored/preserved
    char DriveNumber;
    char Unused;
    char ExtBootSignature;
    unsigned int SerialNumber; // only valid if ExtBootSignature == 0x29
    char VolumeLabel[11]; // only valid if ExtBootSignature == 0x29
    char FileSystemLabel[8]; // only valid if ExtBootSignature == 0x29
    unsigned char BootCode[420];
    unsigned char EndOfSectorMarker[2];
} __attribute__((packed));

struct FAT32_ShortEntry {
    unsigned char name[8];
    unsigned char ext[3];
    unsigned char attr;           /* attribute bits */
    unsigned char lcase;          /* Case for base and extension */
    unsigned char ctime_cs;       /* Creation time, centiseconds (0-199) */
    unsigned short ctime;          /* Creation time */
    unsigned short cdate;          /* Creation date */
    unsigned short adate;          /* Last access date */
    unsigned short start_hi;        /* High 16 bits of cluster in FAT32 */
    unsigned short time;
    unsigned short date;
    unsigned short start;
    unsigned int size;           /* file size (in bytes) */
} __attribute__((packed));

void init_fat32();

#endif