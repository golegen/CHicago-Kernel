// File author is Ítalo Lima Marconato Matias
//
// Created on July 17 of 2018, at 16:11 BRT
// Last edited on July 17 of 2018, at 16:11 BRT

#ifndef __CHICAGO_ISO9660__
#define __CHICAGO_ISO9660__

#include <chicago/types.h>

typedef struct {
	UInt8 directory_record_size;
	UInt8 extended_directory_record_size;
	UInt32 extent_lba_lsb;
	UInt32 extent_lba_msb;
	UInt32 extent_length_lsb;
	UInt32 extent_length_msb;
	UInt8 record_date[7];
	UInt8 flags;
	UInt8 interleave_units;
	UInt8 interleave_gap;
	UInt16 volume_sequence_number_lsb;
	UInt16 volume_sequence_number_msb;
	UInt8 name_length;
	UInt8 name[];
} Packed Iso9660DirEntry, *PIso9660DirEntry;

typedef struct {
	UInt8 type;
	UInt8 cd001[5];
	UInt8 version;
	UInt8 unused1;
	UInt8 system_identifier[32];
	UInt8 volume_identifier[32];
	UInt8 unused2[8];
	UInt32 volume_size_lsb;
	UInt32 volume_size_msb;
	UInt8 unused3[32];
	UInt16 volume_set_size_lsb;
	UInt16 volume_set_size_msb;
	UInt16 volume_sequence_number_lsb;
	UInt16 volume_sequence_number_msb;
	UInt16 block_size_lsb;
	UInt16 block_size_msb;
	UInt32 path_table_size_lsb;
	UInt32 path_table_size_msb;
	UInt32 path_table_location_lsb;
	UInt32 optional_path_table_location_lsb;
	UInt32 path_table_location_msb;
	UInt32 optional_path_table_location_msb;
	UInt8 root_directory[34];
	UInt8 volume_set_identifier[128];
	UInt8 publisher[128];
	UInt8 data_preparer[128];
	UInt8 application_identifier[128];
	UInt8 copyright_file[38];
	UInt8 abstract_file[36];
	UInt8 bibliographic_file[37];
	UInt8 volume_creation[17];
	UInt8 volume_modification[17];
	UInt8 volume_expiration[17];
	UInt8 volume_effective[17];
	UInt8 file_structure_version;
	UInt8 unused4;
	UInt8 application_used[512];
	UInt8 reserved[653];
} Packed Iso9660PVD, *PIso9660PVD;

#endif
