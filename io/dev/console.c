// File author is Ítalo Lima Marconato Matias
//
// Created on December 07 of 2018, at 10:41 BRT
// Last edited on December 09 of 2018, at 17:33 BRT

#include <chicago/console.h>
#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/panic.h>
#include <chicago/process.h>

Queue ConsoleDeviceKeyboardQueue;
Lock ConsoleDeviceKeyboardQueueLock = False;

Void ConsoleDeviceReadKeyboard(UIntPtr len, PWChar buf) {
	if (len == 0) {
		return;
	}
	
	while (ConsoleDeviceKeyboardQueue.length < len) {																// Let's fill the queue with the chars that we need
		if (((UInt8)ListGet(&ConsoleDeviceKeyboardQueue, 0)) == '\n') {												// End?
			ListRemove(&ConsoleDeviceKeyboardQueue, 0);																// Yes
			break;
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

Boolean ConsoleDeviceBackKeyboard(Void) {
	Boolean ret = False;
	
	PsLock(&ConsoleDeviceKeyboardQueueLock);																		// Lock
	
	if (ConsoleDeviceKeyboardQueue.length != 0) {																	// We can do it?
		ListRemove(&ConsoleDeviceKeyboardQueue, 0);																	// Yes, remove the first entry!
		ret = True;
	}
	
	PsUnlock(&ConsoleDeviceKeyboardQueueLock);																		// Unlock!
	
	return ret;
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
	ConsoleDeviceReadKeyboard(len, (PWChar)buf);																	// Redirect to RawKeyboardDeviceRead
	return True;
}

Boolean ConsoleDeviceWrite(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	(Void)dev; (Void)off;																							// Avoid compiler's unused parameter warning
	
	if (buf == Null || len == 0) {																					// First, sanity check!
		return False;
	} else if (len == 1) {																							// We have only one character?
		ConWriteCharacter(buf[0]);																					// Yes!
	} else {
		ConWriteString((PWChar)buf);																				// No, so it's a string...
	}
	
	return True;
}

Void ConsoleDeviceInit(Void) {
	ConsoleDeviceKeyboardQueue.head = Null;																			// Init the keyboard queue
	ConsoleDeviceKeyboardQueue.tail = Null;
	ConsoleDeviceKeyboardQueue.length = 0;
	ConsoleDeviceKeyboardQueue.free = False;
	ConsoleDeviceKeyboardQueue.user = False;
	
	if (!FsAddDevice(L"Console", Null, ConsoleDeviceRead, ConsoleDeviceWrite, Null)) {								// Try to add the console device
		DbgWriteFormated("PANIC! Failed to add the Console device\r\n");											// Failed...
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
}
