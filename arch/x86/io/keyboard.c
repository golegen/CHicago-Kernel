// File author is Ítalo Lima Marconato Matias
//
// Created on October 12 of 2018, at 23:10 BRT
// Last edited on October 19 of 2018, at 18:22 BRT

#include <chicago/arch/idt.h>
#include <chicago/arch/port.h>

#include <chicago/device.h>

static Void KeyboardHandler(PRegisters regs) {
	(Void)regs;
	
	UInt8 data = PortInByte(0x60);																										// Read the key
	
	if (data == 0xAA || data == 0xB6 || data == 0xB8 || data == 0x9D || (!(data & 0x80))) {												// We need it?
		RawKeyboardDeviceWrite(data);																									// Yes!
	}
}

Void ArchInitKeyboard(Void) {
	IDTRegisterIRQHandler(1, KeyboardHandler);																							// Register the keyboard IRQ handler
}
