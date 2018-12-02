// File author is Ítalo Lima Marconato Matias
//
// Created on November 15 of 2018, at 22:20 BRT
// Last edited on November 15 of 2018, at 22:59 BRT

#include <chicago/alloc.h>
#include <chicago/debug.h>
#include <chicago/ipc.h>
#include <chicago/mm.h>
#include <chicago/panic.h>
#include <chicago/string.h>

PList IpcPortList = Null;

PIpcPort IpcCreatePort(PChar name) {
	if (PsCurrentProcess == Null || IpcPortList == Null || name == Null) {				// Sanity checks
		return Null;																	// Nope...
	}
	
	PIpcPort port = (PIpcPort)MemAllocate(sizeof(IpcPort));								// Alloc the struct space
	
	if (port == Null) {
		return Null;																	// Failed
	}
	
	port->name = StrDuplicate(name);													// Duplicate the name
	
	if (port->name == Null) {
		MemFree((UIntPtr)port);															// Failed
		return Null;
	}
	
	port->queue.head = Null;															// Init the msg queue
	port->queue.tail = Null;
	port->queue.length = 0;
	port->queue.free = True;
	port->queue.user = False;
	port->proc = PsCurrentProcess;														// Save the port owner
	
	if (!ListAdd(IpcPortList, port)) {													// Add this port to the port list
		MemFree((UIntPtr)port->name);
		MemFree((UIntPtr)port);
		return Null;
	}
	
	return port;
}

Void IpcRemovePort(PChar name) {
	if (PsCurrentProcess == Null || IpcPortList == Null || name == Null) {				// Sanity checks
		return;
	}
	
	UIntPtr idx = 0;
	Boolean found = False;
	PIpcPort port = Null;
	
	for (; !found && idx < IpcPortList->length; idx++) {								// Let's search for this port in the port list!
		port = ListGet(IpcPortList, idx);
		
		if (StrGetLength(port->name) != StrGetLength(name)) {							// Same length?
			continue;																	// Nope
		} else if (StrCompare(port->name, name)) {										// Found it?
			found = True;																// Yes :)
		}
	}
	
	if (!found) {
		return;																			// Port not found...
	} else if (port->proc != PsCurrentProcess) {
		return;																			// Only the owner of this port can remove it
	}
	
	ListRemove(IpcPortList, idx);														// Remove this port from the list
	MemFree((UIntPtr)port->name);														// Free the name
	MemFree((UIntPtr)port);																// Free the struct itself
}

Void IpcSendMessage(PChar name, UInt32 msg, UIntPtr size, PUInt8 buf) {
	if (PsCurrentProcess == Null || IpcPortList == Null || name == Null) {				// Sanity checks
		return;
	}
	
	UIntPtr idx = 0;
	Boolean found = False;
	PIpcPort port = Null;
	
	for (; !found && idx < IpcPortList->length; idx++) {								// Let's search for this port in the port list!
		port = ListGet(IpcPortList, idx);
		
		if (StrGetLength(port->name) != StrGetLength(name)) {							// Same length?
			continue;																	// Nope
		} else if (StrCompare(port->name, name)) {										// Found it?
			found = True;																// Yes :)
		}
	}
	
	if (!found) {
		return;																			// Port not found...
	}
	
	PsLockTaskSwitch(old);																// Lock
	MmSwitchDirectory(port->proc->dir);													// Switch to the dir of the owner of this port
	
	PIpcMessage mes = (PIpcMessage)MmAllocUserMemory(sizeof(IpcMessage));				// Alloc space for the message struct
	
	if (mes == Null) {
		MmSwitchDirectory(PsCurrentProcess->dir);										// Failed
		PsUnlockTaskSwitch(old);
		return;
	}
	
	mes->msg = msg;																		// Setup it
	mes->src = PsCurrentProcess;
	mes->size = size;
	mes->buffer = buf;
	
	QueueAdd(&port->queue, mes);														// Add to the msg queue
	MmSwitchDirectory(PsCurrentProcess->dir);											// Switch back to the old dir
	PsUnlockTaskSwitch(old);															// Unlock
}

PIpcMessage IpcReceiveMessage(PChar name) {
	if (PsCurrentProcess == Null || IpcPortList == Null || name == Null) {				// Sanity checks
		return Null;
	}
	
	UIntPtr idx = 0;
	Boolean found = False;
	PIpcPort port = Null;
	
	for (; !found && idx < IpcPortList->length; idx++) {								// Let's search for this port in the port list!
		port = ListGet(IpcPortList, idx);
		
		if (StrGetLength(port->name) != StrGetLength(name)) {							// Same length?
			continue;																	// Nope
		} else if (StrCompare(port->name, name)) {										// Found it?
			found = True;																// Yes :)
		}
	}
	
	if (!found) {
		return Null;																	// Port not found...
	} else if (port->proc != PsCurrentProcess) {
		return Null;																	// Only the owner of this port can use the receive function
	}
	
	while (port->queue.length == 0) {													// Wait for anything to come in
		PsSwitchTask(Null);
	}
	
	return QueueRemove(&port->queue);													// And return!
}

Void IpcInit(Void) {
	IpcPortList = ListNew(True, False);													// Try to init the port list
	
	if (IpcPortList == Null) {
		DbgWriteFormated("PANIC! Failed to init IPC\r\n");								// Failed... but it's a critical component, so HALT
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
}
