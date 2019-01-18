// File author is Ítalo Lima Marconato Matias
//
// Created on January 17 of 2019, at 21:06 BRT
// Last edited on January 18 of 2019, at 15:34 BRT

#include <chicago/arch/ahci.h>
#include <chicago/arch/ide.h>
#include <chicago/arch/pci.h>
#include <chicago/arch/port.h>

#include <chicago/alloc.h>
#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/mm.h>
#include <chicago/panic.h>
#include <chicago/string.h>

static UInt8 AHCICount = 0;
static PWChar AHCIHardDiskString = L"SataXHardDiskX";
static PWChar AHCICDROMString = L"SataXCdRomX";

static UInt8 AHCIGetType(PHBAPort port) {
	if ((port->ssts & 0x0F) != 0x03) {																					// Present?
		return 0;
	} else if (((port->ssts >> 8) & 0x0F) != 0x01) {																	// Active?
		return 0;
	} else if ((port->sig == 0xC33C0101 || port->sig == 0x96690101)) {													// Enclosure management bridge or Port multiplier?
		return 0;																										// Yes, we're not going to handle it (for now)
	} else if (port->sig == 0xEB140101) {																				// SATAPI?
		return 1;																										// Yes
	} else {
		return 2;																										// SATA
	}
}

static Void AHCIStartCMD(PHBAPort port) {
	while (port->cmd & 0x8000) ;																						// Wait until CR is cleared
	port->cmd |= 0x11;																									// Set the FRE and the ST bit
}

static Void AHCIStopCMD(PHBAPort port) {
	port->cmd &= ~0x01;																									// Clear the ST bit
	while ((port->cmd & 0xC000) == 0xC000) ;																			// Wait until FR bit and CR bit are cleared
	port->cmd &= ~0x10;																									// Clear the FRE bit
}

static Void AHCIRebase(PHBAPort port, UIntPtr virt) {
	AHCIStopCMD(port);																									// Stop the command engine
	
	UIntPtr phys = MmGetPhys(virt);																						// Get the physical address of virt
	PHBACmd cmd = (PHBACmd)virt;
	
	port->clb = phys;																									// Set the command list PHYSICAL location
	port->clbu = 0;
	StrSetMemory(cmd, 0, sizeof(HBACmd) * 32);																			// Clear the command list (using the VIRTUAL address)
	
	port->fb = phys + (sizeof(HBACmd) * 32);																			// Set the FIS PHYSICAL location
	port->fbu = 0;
	StrSetMemory((PUInt8)(virt + sizeof(HBACmd)), 0, 256);																// Clear the FIS (using the VIRTUAL address)
	
	for (UIntPtr i = 0; i < 32; i++, cmd++) {
		cmd->prdtl = 8;																									// 8 PRDT entries
		cmd->ctba = phys + (sizeof(HBACmd) * 32) + 256 + (i * sizeof(HBACmdTbl));										// Set the command table PHYSICAL location
		cmd->ctbau = 0;
		StrSetMemory((PUInt8)(virt + (sizeof(HBACmd) * 32) + 256 + (i * sizeof(HBACmdTbl))), 0, sizeof(HBACmdTbl));		// Clear the command table (using the VIRTUAL address)
	}
	
	AHCIStartCMD(port);																									// Start the command engine again*/
}

static Boolean AHCIDeviceRead(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	(Void)dev; (Void)off; (Void)len; (Void)buf;																			// TODO
	return False;
}

static Boolean AHCIDeviceWrite(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	(Void)dev; (Void)off; (Void)len; (Void)buf;																			// TODO
	return False;
}

static Boolean AHCIDeviceControl(PDevice dev, UIntPtr cmd, PUInt8 ibuf, PUInt8 obuf) {
	(Void)dev; (Void)cmd; (Void)ibuf; (Void)obuf;																		// TODO
	return False;
}

Void AHCIInit(UInt16 bus, UInt8 slot, UInt8 func) {
	PHBAMem abar = (PHBAMem)(PCIReadLong(bus, slot, func, PCI_BAR5) & 0xFFFFFFF0);										// Read the BAR5 (ABAR)
	
	for (UIntPtr i = 0; i < sizeof(HBAMem); i += MM_PAGE_SIZE) {														// Let's map the ABAR!
		if (!MmMap(((UIntPtr)abar) + i, ((UIntPtr)abar) + i, MM_MAP_KDEF)) {
			DbgWriteFormated("PANIC! Couldn't init a detected AHCI controller\r\n");									// Failed to map, so halt
			Panic(PANIC_KERNEL_INIT_FAILED);
		}
	}
	
	UInt32 port = abar->pi;																								// Let's search for the amount of SATA/SATAPI devices
	UInt32 pcnt = 0;
	UInt8 hdc = 0;
	UInt8 cdc = 0;
	
	for (UInt32 i = 0; i < 32; i++) {
		if ((port & 1) && (AHCIGetType(&abar->ports[i]) != 0)) {														// Valid?
			pcnt++;																										// Yes
		}
		
		port >>= 1;																										// Go to the next one
	}
	
	if (pcnt == 0) {																									// No devices here?
		for (UIntPtr i = 0; i < sizeof(HBAMem); i += MM_PAGE_SIZE) {													// Yeah, so let's unmap everything and return
			MmUnmap(((UIntPtr)abar) + i);
		}
		
		return;
	}
	
	UIntPtr virt = MemAAllocate(pcnt * (sizeof(HBACmd) + sizeof(HBACmdTbl) + 256) * 32, MM_PAGE_SIZE);					// Alloc memory for all the devices
	
	if (virt == 0) {
		DbgWriteFormated("PANIC! Couldn't init a detected AHCI controller\r\n");										// Failed, so halt
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
	
	port = abar->pi;																									// Let's search for all the SATA/SATAPI devices!
	pcnt = 0;
	
	for (UInt32 i = 0; i < 32; i++) {
		if (port & 1) {																									// Anything on this port?
			UInt8 type = AHCIGetType(&abar->ports[i]);																	// Yes, get the type
			
			if (type == 0) {																							// Handle it?
				goto nope;																								// Nope
			}
			
			AHCIRebase(&abar->ports[i], virt + (pcnt * (sizeof(HBACmd) + sizeof(HBACmdTbl) + 256) * 32));				// Rebase the port
			
			pcnt++;																										// Increase the pcnt
			
			PAHCIDevice dev = (PAHCIDevice)MemAllocate(sizeof(AHCIDevice));												// Alloc memory for this device
			
			if ((dev == Null) && (type == 1)) {
				DbgWriteFormated("[x86] Failed to add Sata%dCdRom%d device\r\n", AHCICount, cdc++);						// Failed (CdRom)
				goto nope;
			} else if (dev == Null) {
				DbgWriteFormated("[x86] Failed to add Sata%dHardDisk%d device\r\n", AHCICount, hdc++);					// Failed (HardDisk)
				goto nope;
			}
			
			dev->port = &abar->ports[i];																				// Set the port of this device
			dev->atapi = type == 1;																						// Set if this device is ATAPI
			
			PWChar name = Null;																							// Let's allocate memory for the name
			
			if (type == 1) {																							// SATAPI?
				name = StrDuplicate(AHCICDROMString);																	// Yes, so duplicate the SataXCdRomX string
				
				if (name == Null) {																						// Failed?
					DbgWriteFormated("[x86] Failed to add Sata%dCdRom%d device\r\n", AHCICount, cdc++);					// Yes...
					MemFree((UIntPtr)dev);
					goto nope;
				}
				
				name[10] = (WChar)(cdc++ + '0');																		// And set the num
			} else {
				name = StrDuplicate(AHCIHardDiskString);																// No, so duplicate the SataXHardDiskX string
				
				if (name == Null) {																						// Failed?
					DbgWriteFormated("[x86] Failed to add Sata%dCdRom%d device\r\n", AHCICount, hdc++);					// Yes...
					MemFree((UIntPtr)dev);
					goto nope;
				}
				
				name[13] = (WChar)(hdc++ + '0');																		// And set the num
			}
			
			name[4] = (WChar)(AHCICount + '0');																			// And set the SATA controller "special" number
			
			if (!FsAddDevice(name, dev, AHCIDeviceRead, AHCIDeviceWrite, AHCIDeviceControl)) {							// At end try to add us to the device list!
				DbgWriteFormated("[x86] Failed to add %s device\r\n", name);
				MemFree((UIntPtr)name);
				MemFree((UIntPtr)dev);
			}
		}
		
nope:;	port >>= 1;																										// Go to the next port
	}
	
	AHCICount++;
}
