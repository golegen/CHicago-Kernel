// File author is Ítalo Lima Marconato Matias
//
// Created on December 11 of 2018, at 18:02 BRT
// Last edited on January 23 of 2019, at 13:35 BRT

#include <chicago/arch/idt.h>
#include <chicago/arch/pci.h>
#include <chicago/arch/port.h>

#include <chicago/alloc.h>
#include <chicago/debug.h>
#include <chicago/panic.h>

PCIInterruptHandler PCIInterruptHandlers[32];
UIntPtr PCIDevicesCount = 0;
PPCIDevice PCIDevices;

static UInt8 PCIReadByte(UInt16 bus, UInt8 slot, UInt8 func, UInt8 off) {
	PortOutLong(0xCF8, (bus << 16) | (slot << 11) | (func << 8) | (off & 0xFC) | 0x80000000);																					// Write out the address
	return PortInByte(0xCFC + (off & 3));																																		// And read the data
}

static UInt16 PCIReadWord(UInt16 bus, UInt8 slot, UInt8 func, UInt8 off) {
	PortOutLong(0xCF8, (bus << 16) | (slot << 11) | (func << 8) | (off & 0xFC) | 0x80000000);																					// Write out the address
	return PortInWord(0xCFC + (off & 2));																																		// And read the data
}

static UInt32 PCIReadLong(UInt16 bus, UInt8 slot, UInt8 func, UInt8 off) {
	PortOutLong(0xCF8, (bus << 16) | (slot << 11) | (func << 8) | (off & 0xFC) | 0x80000000);																					// Write out the address
	return PortInLong(0xCFC);																																					// And read the data
}

static Void PCIWriteLong(UInt16 bus, UInt8 slot, UInt8 func, UInt8 off, UInt32 val) {
	PortOutLong(0xCF8, (bus << 16) | (slot << 11) | (func << 8) | (off & 0xFC) | 0x80000000);																					// Write out the address
	PortOutLong(0xCFC, val);																																					// And the data
}

static Void PCIHandler(PRegisters regs) {
	if (PCIInterruptHandlers[regs->int_num - 32].func != Null) {																												// Call the handler for this IRQ
		PCIInterruptHandlers[regs->int_num - 32].func(PCIInterruptHandlers[regs->int_num - 32].priv);
	}
}

Void PCIRegisterIRQHandler(PPCIDevice dev, PPCIInterruptHandlerFunc handler, PVoid priv) {
	if (dev == Null) {																																							// Check if the device isn't Null
		return;
	}
	
	PCIInterruptHandlers[dev->iline].priv = priv;																																// Set the handler for this IRQ
	PCIInterruptHandlers[dev->iline].func = handler;
	
	IDTRegisterIRQHandler(dev->iline, PCIHandler);																																// And register the PCI global IRQ handler
}

PPCIDevice PCIFindDevice1(PUIntPtr last, UInt16 vendor, UInt16 device) {
	if ((PCIDevicesCount == 0) || (PCIDevices == Null)) {																														// We have any device to check?/The device list is initialized?
		return Null;
	}
	
	for (UIntPtr i = (last == Null) ? 0 : *last; i < PCIDevicesCount; i++) {																									// Let's search!
		if ((PCIDevices[i].vendor == vendor) && (PCIDevices[i].device == device)) {																								// Found?
			if (last != Null) {																																					// Yes! Save the i to the "last" parameter?
				*last = i + 1;																																					// Yes
			}
			
			return &PCIDevices[i];
		}
	}
	
	return Null;
}

PPCIDevice PCIFindDevice2(PUIntPtr last, UInt8 class, UInt8 subclass) {
	if ((PCIDevicesCount == 0) || (PCIDevices == Null)) {																														// We have any device to check?/The device list is initialized?
		return Null;
	}
	
	for (UIntPtr i = (last == Null) ? 0 : *last; i < PCIDevicesCount; i++) {																									// Let's search!
		if ((PCIDevices[i].class == class) && (PCIDevices[i].subclass == subclass)) {																							// Found?
			if (last != Null) {																																					// Yes! Save the i to the "last" parameter?
				*last = i + 1;																																					// Yes
			}
			
			return &PCIDevices[i];
		}
	}
	
	return Null;
}

Void PCIEnableBusMaster(PPCIDevice dev) {
	if (dev == Null) {																																							// Check if the device isn't Null
		return;
	}
	
	UInt16 cmd = PCIReadWord(dev->bus, dev->slot, dev->func, PCI_COMMAND);																										// Let's enable bus mastering!
	
	if ((cmd & 0x04) != 0x04) {																																					// We really need to do it?
		cmd |= 0x04;																																							// Yes, set the bus mastering bit
		PCIWriteLong(dev->bus, dev->slot, dev->func, PCI_COMMAND, cmd);																											// And write back
	}
}

static Void PCIAddDevice(UInt16 bus, UInt8 slot, UInt8 func, UInt16 vendor) {
	PCIDevices[PCIDevicesCount].bus = bus;																																		// Just fill the info about this PCI device
	PCIDevices[PCIDevicesCount].slot = slot;
	PCIDevices[PCIDevicesCount].func = func;
	PCIDevices[PCIDevicesCount].vendor = vendor;
	PCIDevices[PCIDevicesCount].device = PCIReadWord(bus, slot, func, PCI_DEVICE_ID);
	PCIDevices[PCIDevicesCount].class = PCIReadByte(bus, slot, func, PCI_CLASS);
	PCIDevices[PCIDevicesCount].subclass = PCIReadByte(bus, slot, func, PCI_SUBCLASS);
	PCIDevices[PCIDevicesCount].bar0 = PCIReadLong(bus, slot, func, PCI_BAR0);
	PCIDevices[PCIDevicesCount].bar1 = PCIReadLong(bus, slot, func, PCI_BAR1);
	PCIDevices[PCIDevicesCount].bar2 = PCIReadLong(bus, slot, func, PCI_BAR2);
	PCIDevices[PCIDevicesCount].bar3 = PCIReadLong(bus, slot, func, PCI_BAR3);
	PCIDevices[PCIDevicesCount].bar4 = PCIReadLong(bus, slot, func, PCI_BAR4);
	PCIDevices[PCIDevicesCount].bar5 = PCIReadLong(bus, slot, func, PCI_BAR5);
	PCIDevices[PCIDevicesCount].iline = PCIReadByte(bus, slot, func, PCI_INTERRUPT_LINE);
	PCIDevices[PCIDevicesCount++].ipin = PCIReadByte(bus, slot, func, PCI_INTERRUPT_PIN);
}

Void PCIInit(Void) {
	PCIDevices = (PPCIDevice)MemZAllocate(sizeof(PCIDevice) * 8192);																											// Alloc space for the PCI device list
	
	if (PCIDevices == Null) {
		DbgWriteFormated("PANIC! Couldn't init the PCI controller\r\n");																										// Failed...
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
	
	for (UInt16 i = 0; i < 256; i++) {																																			// Brute force scan
		for (UInt8 j = 0; j < 32; j++) {
			UInt16 vendor = PCIReadWord(i, j, 0, PCI_VENDOR_ID);																												// Let's try to get the vendor
			
			if (vendor == 0xFFFF) {
				continue;																																						// This device doesn't exists
			} else {
				PCIAddDevice(i, j, 0, vendor);																																	// Exists! Add it
			}
			
			
			if ((PCIReadByte(i, j, 0, PCI_HEADER_TYPE) & 0x80) == 0x80) {																										// Multi-function device?
				for (UInt8 k = 1; k < 8; k++) {																																	// Yes, scan the functions!
					vendor = PCIReadWord(i, j, k, PCI_VENDOR_ID);																												// Try to get the vendor
					
					if (vendor != 0xFFFF) {																																		// This device exists?
						PCIAddDevice(i, j, k, vendor);																															// Yes! Add it :)
					}
				}
			}
		}
	}
}
