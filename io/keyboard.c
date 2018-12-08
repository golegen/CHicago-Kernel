// File author is Ítalo Lima Marconato Matias
//
// Created on December 07 of 2018, at 17:31 BRT
// Last edited on December 08 of 2018, at 10:19 BRT

#include <chicago/arch.h>
#include <chicago/console.h>
#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/keyboard.h>
#include <chicago/panic.h>
#include <chicago/process.h>

static Char KbdKeymapNormal[128] = {
	0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
	'\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
	0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
	0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0,
	0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static Char KbdKeymapShift[128] = {
	0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
	'\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
	0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
	0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
	'*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0,
	0, '+', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static Void KbdThread(Void) {
	ConsoleDeviceClearKeyboard();																	// Clear the console device (keyboard) buffer
	RawKeyboardDeviceClear();																		// Clear the raw keyboard device buffer
	
	Boolean ctrl = False;
	Boolean shft = False;
	Boolean alt = False;
	
	while (True) {
		UInt8 sc = 0;																				// Scancode "buffer"
		
		RawKeyboardDeviceRead(1, &sc);																// Read from the raw keyboard!
		
		if (sc & 0x80) {																			// Key release?
			if ((sc == 0xAA) || (sc == 0xB6)) {														// Yes! Shift?
				shft = !shft;
			} else if (sc == 0xB8) {																// Alt?
				alt = !alt;
			} else if (sc == 0x9D) {																// Control?
				ctrl = !ctrl;
			}
		} else if ((sc == 0x2A) || (sc == 0x36) || (sc == 0x3A)) {									// Shift (key press)?
			shft = !shft;
		} else if (sc == 0x38) {																	// Alt (key press)?
			alt  = !alt;
		} else if (sc == 0x1D) {																	// Control (key press)?
			ctrl = !ctrl;
		} else {
			Char ch = shft ? KbdKeymapShift[sc] : KbdKeymapNormal[sc];								// Ok, it's a normal key... get the character from the keymap
			
			if (ch == '\b') {																		// Backspace?
				if (ConsoleDeviceBackKeyboard()) {													// Yes, do it!
					ConWriteCharacter(ch);
				}
			} else if (ch == '\n') {																// New line?
				ConsoleDeviceWriteKeyboard(ch);														// Yes, convert the \n to \r\n
				ConWriteFormated("\r\n");
			} else if ((ch != '\t') && (ctrl && shft)) {											// Control+Key (with shift)?
				ConsoleDeviceWriteKeyboard(ch - 'A');												// Yes
				ConWriteFormated("^%c", ch);
			} else if (ch != '\t' && ctrl) {														// Control+Key (without shift)?
				ConsoleDeviceWriteKeyboard(ch - 'a');												// Yes
				ConWriteFormated("^%c", ch - 0x20);
			} else {
				ConsoleDeviceWriteKeyboard(ch);														// Normal character!
				ConWriteFormated("%c", ch);
			}
		}
	}
	
	ArchHalt();																						// Halt (should never get here!)
}

Void KbdInit(Void) {
	PThread th = PsCreateThread((UIntPtr)KbdThread, 0, False);										// Create the keyboard handler thread
	
	if (th == Null) {
		DbgWriteFormated("PANIC! Couldn't create the keybord handler thread\r\n");					// Failed...
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
	
	PsAddThread(th);																				// (Try to) add the keyboard handler thread
}
