#ifndef KERNEL_FAT
#define KERNEL_FAT

struct FAT_BPB // The BIOS Parameter Block structure, to overlay onto the boot sector. Here, regardless of FAT_type.
{
	unsigned char startjmp[3];		// The first three bytes of the BPB are always a JMP opcode to the end of it.
	unsigned char oem_name[8];		// OEM string, although generally useless.
	unsigned short bytes_sector;	// Number of bytes per sector.
	unsigned char sectors_cluster;	// Number of sectors per cluster.
	unsigned short reserved_sectors;// Number of reserved sectors (including boot sector). OS goes in here.
	unsigned char fat_count;		// Number of FATs on the disk. Usually 2.
	unsigned short directories;		// Number of directory entries.
	unsigned short total_sectors;	// Total sectors in the logical volume. 
									// If it's 0, there are more than 65535 and are stored elsewhere.
	unsigned char media_descriptor;	// The media descriptor type.
	unsigned short sectors_per_fat;	// Number of sectors per allocation table. (FAT 12/16 only, not FAT32)
	unsigned short sectors_track;	// Number of sectors per track.
	unsigned short head_side_count;	// Number of heads (or sides) of the disk.
	unsigned int hidden_sectors;	// Number of hidden sectors (not reserved) to offset the beginning of the partition to.
	unsigned int total_sectors_big;	// Large amount of sectors on disk. If it doesn't fit in total_sectors, it does here.
} __attribute__ ((packed));

struct FAT_EBPB // The Extended BIOS Parameter Block for FAT 12 and FAT 16 partitions.
{
	unsigned char drive_number;		// The BIOS-type drive number. Usually 0x0 for floppies, and 0x80 for HDDs.
	unsigned char reserved_flags;	// Not used here, usually flags for Windows NT.
	unsigned char signature;		// Should be 0x28 or 0x29. Not used.
	unsigned int serial_number;		// Volume serial number, probably ignored.
	unsigned char volume_label[11];	// The 11-byte volume label.
	unsigned char system_id[8];		// The string representation of the FAT file system, probably unused.
} __attribute__ ((packed));

struct FAT32_EBPB // The Extended BIOS Parameter Block for FAT 32 partitions only.
{
	unsigned int sectors_per_fat;	// The FAT32 version of the above BPB sectors_per_fat. Size of the FAT in sectors.
	unsigned short flags;			// General FAT32 flags.
	unsigned short fat_version;		// FAT version number, high byte is the major version, low byte is the minor.
	unsigned int root_clusters;		// Number of clusters of the root directory, usually two.
	unsigned short info_sector;		// Sector number of the file system info.
	unsigned short backup_sector;	// Sector number of the backup boot sector.
	unsigned char reserved[12];		// 12 reserved bytes, if the drive is formatted these SHOULD all be zero.
	unsigned char drive_number;		// The BIOS-type drive number, same as the above FAT_EBPB. The next few will be the same, too.
	unsigned char reserved_flags;	// Not used here, usually flags for Windows NT.
	unsigned char signature;		// Should be 0x28 or 0x29.
	unsigned int serial_number;		// Volume serial number, probably ignored.
	unsigned char volume_label[11];	// The 11-byte volume label.
	unsigned char system_id[8];		// The string representation of the FAT file system, probably unused.
} __attribute__ ((packed));

#endif