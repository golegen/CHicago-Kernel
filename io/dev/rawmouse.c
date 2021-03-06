// File author is Ítalo Lima Marconato Matias
//
// Created on October 19 of 2018, at 21:40 BRT
// Last edited on December 14 of 2018, at 18:27 BRT

#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/panic.h>
#include <chicago/process.h>
#include <chicago/queue.h>

Queue RawMouseDeviceQueue;
Lock RawMouseDeviceQueueReadLock = { False, Null };
Lock RawMouseDeviceQueueWriteLock = { False, Null };

static Boolean RawMouseDeviceReadInt(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	(Void)dev; (Void)off;
	RawMouseDeviceRead(len, buf);															// Redirect to RawMouseDeviceRead
	return True;
}

Void RawMouseDeviceWrite(Int8 offx, Int8 offy, UInt8 buttons) {
	PsLock(&RawMouseDeviceQueueWriteLock);													// Lock
	
	while (RawMouseDeviceQueue.length >= 1024) {											// Don't let the queue grow too much
		QueueRemove(&RawMouseDeviceQueue);
	}
	
	QueueAdd(&RawMouseDeviceQueue, (PVoid)(offx | (offy << 8) | (buttons << 16)));			// Add to the queue
	PsUnlock(&RawMouseDeviceQueueWriteLock);												// Unlock!
}

Void RawMouseDeviceRead(UIntPtr len, PUInt8 buf) {
	if (len == 0) {
		return;
	}
	
	while (RawMouseDeviceQueue.length < len) {												// Let's fill the queue with what we need
		PsSwitchTask(Null);
	}
	
	PsLock(&RawMouseDeviceQueueReadLock);													// Lock
	
	for (UIntPtr i = 0; i < len; i++) {														// Fill the buffer!
		UIntPtr cmd = (UIntPtr)QueueRemove(&RawMouseDeviceQueue);
		
		buf[i * 3] = (UInt8)(cmd & 0xFF);
		buf[i * 3 + 1] = (UInt8)((cmd >> 8) & 0xFF);
		buf[i * 3 + 2] = (UInt8)((cmd >> 16) & 0xFF);
	}
	
	PsUnlock(&RawMouseDeviceQueueReadLock);													// Unlock!
}

Void RawMouseDeviceInit(Void) {
	RawMouseDeviceQueue.head = Null;
	RawMouseDeviceQueue.tail = Null;
	RawMouseDeviceQueue.length = 0;
	RawMouseDeviceQueue.free = False;
	RawMouseDeviceQueue.user = False;
	
	if (!FsAddDevice(L"RawMouse", Null, RawMouseDeviceReadInt, Null, Null)) {
		DbgWriteFormated("PANIC! Failed to add the RawMouse device\r\n");
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
}
