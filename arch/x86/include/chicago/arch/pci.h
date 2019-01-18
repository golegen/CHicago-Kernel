// File author is Ítalo Lima Marconato Matias
//
// Created on December 11 of 2018, at 18:17 BRT
// Last edited on January 17 of 2019, at 21:14 BRT

#ifndef __CHICAGO_ARCH_PCI_H__
#define __CHICAGO_ARCH_PCI_H__

#include <chicago/types.h>

#define PCI_VENDOR_ID 0x00
#define PCI_DEVICE_ID 0x02
#define PCI_COMMAND 0x04
#define PCI_STATUS 0x06
#define PCI_REVISION_ID 0x08
#define PCI_PROG_IF 0x09
#define PCI_SUBCLASS 0x0A
#define PCI_CLASS 0x0B
#define PCI_CACHE_LINE_SIZE 0x0C
#define PCI_LATENCY_TIMER 0x0D
#define PCI_HEADER_TYPE 0x0E
#define PCI_BIST 0x0F
#define PCI_BAR0 0x10
#define PCI_BAR1 0x14
#define PCI_BAR2 0x18
#define PCI_BAR3 0x1C
#define PCI_BAR4 0x20
#define PCI_BAR5 0x24
#define PCI_INTERRUPT_LINE 0x3C
#define PCI_SECONDARY_BUS 0x19

#define PCI_VENDOR_INTEL 0x8086

#define PCI_DEVICE_E1000 0x100E

#define PCI_CLASS_MASS 0x01

#define PCI_SUBCLASS_SATA 0x06

typedef Void (*PPCIInterruptHandlerFunc)(PVoid);

typedef struct {
	PVoid priv;
	PPCIInterruptHandlerFunc func;
} PCIInterruptHandler, *PPCIInterruptHandler;

UInt8 PCIReadByte(UInt16 bus, UInt8 slot, UInt8 func, UInt8 off);
UInt16 PCIReadWord(UInt16 bus, UInt8 slot, UInt8 func, UInt8 off);
UInt32 PCIReadLong(UInt16 bus, UInt8 slot, UInt8 func, UInt8 off);
Void PCIWriteLong(UInt16 bus, UInt8 slot, UInt8 func, UInt8 off, UInt32 val);
Void PCIRegisterIRQHandler(UInt16 bus, UInt8 slot, UInt8 func, PPCIInterruptHandlerFunc handler, PVoid priv);
Void PCIInit(Void);

#endif		// __CHICAGO_ARCH_PCI_H__
