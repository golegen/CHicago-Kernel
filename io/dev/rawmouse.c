// File author is Ítalo Lima Marconato Matias
//
// Created on October 19 of 2018, at 21:40 BRT
// Last edited on October 27 of 2018, at 22:25 BRT

#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/panic.h>
#include <chicago/process.h>
#include <chicago/queue.h>

Queue RawMouseDeviceQueue;
Lock RawMouseDeviceQueueLock = False;

static Boolean RawMouseDeviceReadInt(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	(Void)dev; (Void)off;
	RawMouseDeviceRead(len, buf);															// Redirect to RawMouseDeviceRead
	return True;
}

Void RawMouseDeviceWrite(Int8 offx, Int8 offy, UInt8 buttons) {
	PsLock(&RawMouseDeviceQueueLock);														// Lock
	
	while (RawMouseDeviceQueue.length >= 1024) {											// Don't let the queue grow too much
		QueueRemove(&RawMouseDeviceQueue);
	}
	
	QueueAdd(&RawMouseDeviceQueue, (PVoid)(offx | (offy << 8) | (buttons << 16)));			// Add to the queue
	PsUnlock(&RawMouseDeviceQueueLock);														// Unlock!
}

Void RawMouseDeviceRead(UIntPtr len, PUInt8 buf) {
	if (len == 0) {
		return;
	}
	
	while (RawMouseDeviceQueue.length < len) {												// Let's fill the queue with what we need
		PsSwitchTask(Null);
	}
	
	PsLock(&RawMouseDeviceQueueLock);														// Lock
	
	for (UIntPtr i = 0; i < len; i++) {														// Fill the buffer!
		UIntPtr cmd = (UIntPtr)QueueRemove(&RawMouseDeviceQueue);
		
		buf[i * 3] = (UInt8)(cmd & 0xFF);
		buf[i * 3 + 1] = (UInt8)((cmd >> 8) & 0xFF);
		buf[i * 3 + 2] = (UInt8)((cmd >> 16) & 0xFF);
	}
	
	PsUnlock(&RawMouseDeviceQueueLock);														// Unlock!
}

Void RawMouseDeviceInit(Void) {
	RawMouseDeviceQueue.head = Null;
	RawMouseDeviceQueue.tail = Null;
	RawMouseDeviceQueue.length = 0;
	RawMouseDeviceQueue.free = False;
	RawMouseDeviceQueue.user = False;
	
	if (!FsAddDevice("RawMouse", Null, RawMouseDeviceReadInt, Null, Null)) {
		DbgWriteFormated("PANIC! Failed to add the RawMouse device\r\n");
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
}
