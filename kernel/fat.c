#include "fat.h"

FAT_TYPE get_FAT_type(FAT_BPB* bpb)
{
	unsigned int clusters = get_clusters(bpb);
	
	if (clusters < 4085)
		return FAT12;
	if (clusters < 65525)
		return FAT16;
	if (clusters < 268435445)
		return FAT32;
	else
		return exFAT;
}

void* get_EBPB(FAT_BPB* bpb)
{
	return ((void*)bpb) + EBPB_OFFSET;
}

unsigned int get_sectors(FAT_BPB* bpb)
{
	if (bpb->total_sectors != 0) // If the 16-bit BPB total sector count isn't zero, that's what we're looking for.
		return bpb->total_sectors;
	else
		return bpb->total_sectors_big; // Otherwise, return the bigger 32-bit value instead.
}

unsigned int get_fat_size(FAT_BPB* bpb)
{
	
}

unsigned int get_data(FAT_BPB* bpb)
{
	
}

unsigned int get_FAT(FAT_BPB* bpb)
{
	return bpb->reserved_sectors;
}

unsigned int get_data_count(FAT_BPB* bpb)
{
	
}

unsigned int get_clusters(FAT_BPB* bpb)
{
	return get_data_count(bpb) / bpb->sectors_cluster; // Just calculate the amount of clusters using given BPB.
}
