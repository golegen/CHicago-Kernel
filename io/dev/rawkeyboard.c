// File author is Ítalo Lima Marconato Matias
//
// Created on October 12 of 2018, at 21:08 BRT
// Last edited on October 27 of 2018, at 22:25 BRT

#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/panic.h>
#include <chicago/process.h>
#include <chicago/queue.h>

Queue RawKeyboardDeviceQueue;
Lock RawKeyboardDeviceQueueLock = False;

static Boolean RawKeyboardDeviceReadInt(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	(Void)dev; (Void)off;
	RawKeyboardDeviceRead(len, buf);														// Redirect to RawKeyboardDeviceRead
	return True;
}

Void RawKeyboardDeviceWrite(UInt8 data) {
	PsLock(&RawKeyboardDeviceQueueLock);													// Lock
	
	while (RawKeyboardDeviceQueue.length >= 1024) {											// Don't let the queue grow too much
		QueueRemove(&RawKeyboardDeviceQueue);
	}
	
	QueueAdd(&RawKeyboardDeviceQueue, (PVoid)data);											// Add to the queue
	PsUnlock(&RawKeyboardDeviceQueueLock);													// Unlock!
}

Void RawKeyboardDeviceRead(UIntPtr len, PUInt8 buf) {
	if (len == 0) {
		return;
	}
	
	while (RawKeyboardDeviceQueue.length < len) {											// Let's fill the stack with the chars that we need
		PsSwitchTask(Null);
	}
	
	PsLock(&RawKeyboardDeviceQueueLock);													// Lock
	
	for (UIntPtr i = 0; i < len; i++) {														// Fill the buffer!
		buf[i] = (UInt8)QueueRemove(&RawKeyboardDeviceQueue);
	}
	
	PsUnlock(&RawKeyboardDeviceQueueLock);													// Unlock!
}

Void RawKeyboardDeviceInit(Void) {
	RawKeyboardDeviceQueue.head = Null;														// Init the keyboard stack
	RawKeyboardDeviceQueue.tail = Null;
	RawKeyboardDeviceQueue.length = 0;
	RawKeyboardDeviceQueue.free = False;
	RawKeyboardDeviceQueue.user = False;
	
	if (!FsAddDevice("RawKeyboard", Null, RawKeyboardDeviceReadInt, Null, Null)) {			// Try to add the keyboard device
		DbgWriteFormated("PANIC! Failed to add the RawKeyboard device\r\n");				// Failed...
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
}
