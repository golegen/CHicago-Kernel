// File author is Ítalo Lima Marconato Matias
//
// Created on October 28 of 2018, at 09:48 BRT
// Last edited on November 02 of 2018, at 11:43 BRT

#ifndef __CHICAGO_CHFS_H__
#define __CHICAGO_CHFS_H__

#include <chicago/file.h>

typedef struct {
	UInt8 jmp[2];
	Char magic[4];
	UInt32 block_count;
	UInt32 block_used_count;
	UInt32 root_directory_start;
	UInt8 code[494];
} Packed CHFsHeader, *PCHFsHeader;

typedef struct {
	UInt32 next_block;
	UInt8 type;
	UInt32 data_start;
	UInt32 data_length;
	UInt8 name_length;
	UInt8 name[];
} Packed CHFsINode, *PCHFsINode;

typedef struct {
	PFsNode dev;
	CHFsHeader hdr;
} CHFsMountInfo, *PCHFsMountInfo;

#endif		// __CHICAGO_CHFS_H__
