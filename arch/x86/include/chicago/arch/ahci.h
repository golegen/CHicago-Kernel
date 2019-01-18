// File author is Ítalo Lima Marconato Matias
//
// Created on January 17 of 2019, at 21:07 BRT
// Last edited on January 18 of 2019, at 15:33 BRT

#ifndef __CHICAGO_ARCH_AHCI_H__
#define __CHICAGO_ARCH_AHCI_H__

#include <chicago/types.h>

typedef volatile struct {
	UInt32 clb;
	UInt32 clbu;
	UInt32 fb;
	UInt32 fbu;
	UInt32 is;
	UInt32 ie;
	UInt32 cmd;
	UInt32 res0;
	UInt32 tfd;
	UInt32 sig;
	UInt32 ssts;
	UInt32 sctl;
	UInt32 serr;
	UInt32 sact;
	UInt32 ci;
	UInt32 sntf;
	UInt32 fbs;
	UInt32 res1[11];
	UInt32 vendor[4];
} HBAPort, *PHBAPort;

typedef volatile struct {
	UInt32 cap;
	UInt32 ghc;
	UInt32 is;
	UInt32 pi;
	UInt32 vs;
	UInt32 cccctl;
	UInt32 cccpts;
	UInt32 emloc;
	UInt32 emctl;
	UInt32 cap2;
	UInt32 bohc;
	UInt8 res[0x74];
	UInt8 vendor[0x60];
	HBAPort ports[32];
} HBAMem, *PHBAMem;

typedef struct {
	UInt8 cfl : 5;
	UInt8 atapi : 1;
	UInt8 h2d : 1;
	UInt8 prefetch : 1;
	UInt8 reset : 1;
	UInt8 bist : 1;
	UInt8 clear : 1;
	UInt8 res0 : 1;
	UInt8 pmp : 4;
	UInt16 prdtl;
	Volatile UInt32 prdbc;
	UInt32 ctba;
	UInt32 ctbau;
	UInt32 res1[4];
} HBACmd, *PHBACmd;

typedef struct {
	UInt32 dba;
	UInt32 dbau;
	UInt32 res0;
	UInt32 dbc : 22;
	UInt32 res1 : 9;
	UInt32 i : 1;
} HBAPRDTEntry, *PHBAPRDTEntry;

typedef struct {
	UInt8 cfis[64];
	UInt8 acmd[16];
	UInt8 res[48];
	HBAPRDTEntry prdt[8];
} HBACmdTbl, *PHBACmdTbl;

typedef struct {
	PHBAPort port;
	Boolean atapi;
} AHCIDevice, *PAHCIDevice;

Void AHCIInit(UInt16 bus, UInt8 slot, UInt8 func);

#endif		// __CHICAGO_ARCH_AHCI_H__
