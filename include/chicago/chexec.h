// File author is Ítalo Lima Marconato Matias
//
// Created on November 10 of 2018, at 19:00 BRT
// Last edited on February 22 of 2019, at 21:24 BRT

#ifndef __CHICAGO_CHEXEC_H__
#define __CHICAGO_CHEXEC_H__

#include <chicago/types.h>

#ifndef CHEXEC_ARCH
#error "Sorry, this architecture doesn't support CHExec"
#endif

#define CHEXEC_HEADER_MAGIC 0x58454843

#define CHEXEC_HEADER_FLAGS_ARCH_X86 0x01
#define CHEXEC_HEADER_FLAGS_LIBRARY 0x02
#define CHEXEC_HEADER_FLAGS_EXECUTABLE 0x04
#define CHEXEC_HEADER_FLAGS_DRIVER 0x08
#define CHEXEC_HEADER_FLAGS_KERNEL 0x10

#define CHEXEC_SECTION_FLAGS_NONE 0x01
#define CHEXEC_SECTION_FLAGS_ZEROINIT 0x02

#define CHEXEC_SYMBOL_FLAGS_NONE 0x01
#define CHEXEC_SYMBOL_FLAGS_UNDEF 0x02

#define CHEXEC_REL_OP_ABS 0x00
#define CHEXEC_REL_OP_REL 0x01
#define CHEXEC_REL_OP_SYM 0x02
#define CHEXEC_REL_OP_REL_SYM 0x04
#define CHEXEC_REL_OP_BYTE 0x08
#define CHEXEC_REL_OP_WORD 0x10
#define CHEXEC_REL_OP_DWORD 0x20

typedef struct {
	UInt32 magic;
	UInt32 flags;
	UIntPtr entry;
	UIntPtr sh_count;
	UIntPtr sh_start;
	UIntPtr st_count;
	UIntPtr st_start;
	UIntPtr rel_count;
	UIntPtr rel_start;
	UIntPtr dep_count;
	UIntPtr dep_start;
} CHExecHeader, *PCHExecHeader;

typedef struct {
	UInt16 flags;
	UIntPtr offset;
	UIntPtr virt;
	UIntPtr size;
	UIntPtr name_len;
	WChar name[0];
} CHExecSection, *PCHExecSection;

typedef struct {
	UInt16 flags;
	UIntPtr virt;
	UIntPtr name_len;
	WChar name[0];
} CHExecSymbol, *PCHExecSymbol;

typedef struct {
	UInt8 op;
	UIntPtr incr;
	UIntPtr virt;
	UIntPtr name_len;
	WChar name[0];
} CHExecRelocation, *PCHExecRelocation;

typedef struct {
	UIntPtr name_len;
	WChar name[0];
} CHExecDependency, *PCHExecDependency;

Boolean CHExecValidateHeader(PUInt8 buf, UInt8 type);
UIntPtr CHExecLoadSections(PUInt8 buf);

#endif		// __CHICAGO_CHEXEC_H__
