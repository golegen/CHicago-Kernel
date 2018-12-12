// File author is Ítalo Lima Marconato Matias
//
// Created on December 11 of 2018, at 18:02 BRT
// Last edited on December 12 of 2018, at 10:44 BRT

#include <chicago/arch/e1000.h>
#include <chicago/arch/idt.h>
#include <chicago/arch/pci.h>
#include <chicago/arch/port.h>

PCIInterruptHandler PCIInterruptHandlers[32];

UInt8 PCIReadByte(UInt16 bus, UInt8 slot, UInt8 func, UInt8 off) {
	PortOutLong(0xCF8, (bus << 16) | (slot << 11) | (func << 8) | (off & 0xFC) | 0x80000000);						// Write out the address
	return PortInByte(0xCFC + (off & 3));																			// And read the data
}

UInt16 PCIReadWord(UInt16 bus, UInt8 slot, UInt8 func, UInt8 off) {
	PortOutLong(0xCF8, (bus << 16) | (slot << 11) | (func << 8) | (off & 0xFC) | 0x80000000);						// Write out the address
	return PortInWord(0xCFC + (off & 2));																			// And read the data
}

UInt32 PCIReadLong(UInt16 bus, UInt8 slot, UInt8 func, UInt8 off) {
	PortOutLong(0xCF8, (bus << 16) | (slot << 11) | (func << 8) | (off & 0xFC) | 0x80000000);						// Write out the address
	return PortInLong(0xCFC);																						// And read the data
}

Void PCIWriteLong(UInt16 bus, UInt8 slot, UInt8 func, UInt8 off, UInt32 val) {
	PortOutLong(0xCF8, (bus << 16) | (slot << 11) | (func << 8) | (off & 0xFC) | 0x80000000);						// Write out the address
	PortOutLong(0xCFC, val);																						// And the data
}

static Void PCIHandler(PRegisters regs) {
	if (PCIInterruptHandlers[regs->int_num - 32].func != Null) {													// Call the handler for this IRQ
		PCIInterruptHandlers[regs->int_num - 32].func(PCIInterruptHandlers[regs->int_num - 32].priv);
	}
}

Void PCIRegisterIRQHandler(UInt16 bus, UInt8 slot, UInt8 func, PPCIInterruptHandlerFunc handler, PVoid priv) {
	UInt8 irq = PCIReadByte(bus, slot, func, PCI_INTERRUPT_LINE);													// Get the IRQ num
	
	PCIInterruptHandlers[irq].priv = priv;																			// Set the handler for this IRQ
	PCIInterruptHandlers[irq].func = handler;
	
	IDTRegisterIRQHandler(irq, PCIHandler);																			// And register the PCI global IRQ handler
}

static Void PCIHandleDevice(UInt16 bus, UInt8 slot, UInt8 func, UInt16 vendor, UInt16 device) {
	if ((vendor == PCI_VENDOR_INTEL) && (device == PCI_DEVICE_E1000)) {												// E1000?
		E1000Init(bus, slot, func);																					// Yes :)
	}
}

Void PCIInit(Void) {
	for (UInt16 i = 0; i < 256; i++) {																				// Brute force scan
		for (UInt8 j = 0; j < 32; j++) {
			UInt16 vendor = PCIReadWord(i, j, 0, PCI_VENDOR_ID);													// Let's try to get the vendor
			
			if (vendor == 0xFFFF) {
				continue;																							// This device doesn't exists
			} else {
				PCIHandleDevice(i, j, 0, vendor, PCIReadWord(i, j, 0, PCI_DEVICE_ID));								// Ok, let's handle it (if we need)!
			}
			
			if ((PCIReadByte(i, j, 0, PCI_HEADER_TYPE) & 0x80) == 0x80) {											// Multi-function device?
				for (UInt8 k = 1; k < 8; k++) {																		// Yes, scan the functions!
					vendor = PCIReadWord(i, j, k, PCI_VENDOR_ID);													// Try to get the vendor
					
					if (vendor != 0xFFFF) {																			// This device exists?
						PCIHandleDevice(i, j, k, vendor, PCIReadWord(i, j, k, PCI_DEVICE_ID));						// Yes, handle it :)
					}
				}
			}
		}
	}
}
