// File author is Ítalo Lima Marconato Matias
//
// Created on December 07 of 2018, at 10:41 BRT
// Last edited on December 07 of 2018, at 13:18 BRT

#include <chicago/console.h>
#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/panic.h>
#include <chicago/process.h>

Queue ConsoleDeviceKeyboardQueue;
Lock ConsoleDeviceKeyboardQueueLock = False;

Void ConsoleDeviceReadKeyboard(UIntPtr len, PChar buf) {
	if (len == 0) {
		return;
	}
	
	while (ConsoleDeviceKeyboardQueue.length < len) {																// Let's fill the queue with the chars that we need
		if (((UInt8)ListGet(&ConsoleDeviceKeyboardQueue, ConsoleDeviceKeyboardQueue.length - 1)) == '\n') {			// End?
			break;																									// Yes
		}
		
		PsSwitchTask(Null);
	}
	
	PsLock(&ConsoleDeviceKeyboardQueueLock);																		// Lock
	
	UIntPtr alen = ConsoleDeviceKeyboardQueue.length;																// Save the avaliable length
	
	for (UIntPtr i = 0; i < len && i < alen; i++) {																	// Fill the buffer!
		buf[i] = (UInt8)QueueRemove(&ConsoleDeviceKeyboardQueue);
	}
	
	if (alen < len) {																								// Put a string terminator (NUL) in the end!
		buf[alen] = 0;
	} else {
		buf[len] = 0;
	}
	
	PsUnlock(&ConsoleDeviceKeyboardQueueLock);																		// Unlock!
}

Void ConsoleDeviceWriteKeyboard(Char data) {
	PsLock(&ConsoleDeviceKeyboardQueueLock);																		// Lock
	QueueAdd(&ConsoleDeviceKeyboardQueue, (PVoid)data);																// Add to the queue
	PsUnlock(&ConsoleDeviceKeyboardQueueLock);																		// Unlock!
}

Void ConsoleDeviceClearKeyboard(Void) {
	PsLock(&ConsoleDeviceKeyboardQueueLock);																		// Lock
	
	while (ConsoleDeviceKeyboardQueue.length != 0) {																// Clean!
		QueueRemove(&ConsoleDeviceKeyboardQueue);
	}
	
	PsUnlock(&ConsoleDeviceKeyboardQueueLock);																		// Unlock!
}

Boolean ConsoleDeviceRead(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	(Void)dev; (Void)off;																							// Avoid compiler's unused parameter warning
	ConsoleDeviceReadKeyboard(len, (PChar)buf);																		// Redirect to RawKeyboardDeviceRead
	return True;
}

Boolean ConsoleDeviceWrite(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	(Void)dev; (Void)off;																							// Avoid compiler's unused parameter warning
	
	if (buf == Null || len == 0) {																					// First, sanity check!
		return False;
	} else if (len == 1) {																							// We have only one character?
		ConWriteCharacter(buf[0]);																					// Yes!
	} else {
		ConWriteString((PChar)buf);																					// No, so it's a string...
	}
	
	return True;
}

Void ConsoleDeviceInit(Void) {
	ConsoleDeviceKeyboardQueue.head = Null;																			// Init the keyboard queue
	ConsoleDeviceKeyboardQueue.tail = Null;
	ConsoleDeviceKeyboardQueue.length = 0;
	ConsoleDeviceKeyboardQueue.free = False;
	ConsoleDeviceKeyboardQueue.user = False;
	
	if (!FsAddDevice("Console", Null, ConsoleDeviceRead, ConsoleDeviceWrite, Null)) {								// Try to add the console device
		DbgWriteFormated("PANIC! Failed to add the Console device\r\n");											// Failed...
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
}
