// File author is Ítalo Lima Marconato Matias
//
// Created on July 14 of 2018, at 22:35 BRT
// Last edited on October 27 of 2018, at 22:21 BRT

#include <chicago/alloc.h>
#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/list.h>
#include <chicago/panic.h>
#include <chicago/string.h>

PList FsDeviceList = Null;
PChar FsBootDevice = Null;

Boolean FsReadDevice(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	if (dev->read != Null) {													// We can call the device's function?
		return dev->read(dev, off, len, buf);									// Yes!
	} else {
		return False;															// Nope, so return False
	}
}

Boolean FsWriteDevice(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	if (dev->write != Null) {													// We can call the device's function?
		return dev->write(dev, off, len, buf);									// Yes!
	} else {
		return False;															// Nope, so return False
	}
}

Boolean FsControlDevice(PDevice dev, UIntPtr cmd, PUInt8 ibuf, PUInt8 obuf) {
	if (dev->control != Null) {													// We can call the device's function?
		return dev->control(dev, cmd, ibuf, obuf);								// Yes!
	} else {
		return False;															// Nope, so return False
	}
}

Boolean FsAddDevice(PChar name, PVoid priv, Boolean (*read)(PDevice, UIntPtr, UIntPtr, PUInt8), Boolean (*write)(PDevice, UIntPtr, UIntPtr, PUInt8), Boolean (*control)(PDevice, UIntPtr, PUInt8, PUInt8)) {
	if (FsDeviceList == Null) {													// Device list was initialized?
		return False;															// No...
	}
	
	PDevice dev = (PDevice)MemAllocate(sizeof(Device));							// Allocate memory for the dev struct
	
	if (dev == Null) {															// Failed?
		return False;															// Yes...
	}
	
	dev->name = name;
	dev->priv = priv;
	dev->read = read;
	dev->write = write;
	dev->control = control;
	
	if (!ListAdd(FsDeviceList, dev)) {											// Try to add to the list
		MemFree((UIntPtr)dev);													// Failed, so let's free the dev struct
		return False;															// And return False
	}
	
	return True;
}

Boolean FsRemoveDevice(PChar name) {
	if (FsDeviceList == Null) {													// Device list was initialized?
		return False;															// No...
	}
	
	PDevice dev = FsGetDevice(name);											// Try to get the device
	UIntPtr idx = 0;
	Boolean found = False;
	
	if (dev == Null) {															// Failed?
		return False;															// Yes, so this device doesn't exists
	}
	
	for (; !found && idx < FsDeviceList->length; idx++) {
		if (ListGet(FsDeviceList, idx) == dev) {								// Found?
			found = True;														// Yes!
		}
	}
	
	if (!found) {																// Found?
		return False;															// No (What? But we found it earlier)
	}
	
	if (ListRemove(FsDeviceList, idx) == Null) {								// Try to remove it
		return False;															// Failed...
	}
	
	if (FsGetDevice(name) != Null) {											// We really removed it?
		return False;															// No (for some reason)
	}
	
	MemFree((UIntPtr)(dev->name));												// Free the name
	MemFree((UIntPtr)dev);														// And the dev struct
	
	return True;																// AND RETURN TRUE!
}

PDevice FsGetDevice(PChar name) {
	if (FsDeviceList == Null) {													// Device list was initialized?
		return Null;															// No...
	}
	
	ListForeach(FsDeviceList, i) {												// Let's do an for in each (foreach) list entry
		PChar dname = ((PDevice)(i->data))->name;								// Save the entry name
		
		if (StrGetLength(dname) != StrGetLength(name)) {						// Same length?
			continue;															// No, so we don't even need to compare this entry
		} else if (StrCompare(dname, name)) {									// dname == name?
			return (PDevice)(i->data);											// YES!
		}
	}
	
	return Null;
}

PDevice FsGetDeviceByID(UIntPtr id) {
	if (FsDeviceList == Null) {
		return Null;
	} else if (id >= FsDeviceList->length) {
		return Null;
	}
	
	return (PDevice)ListGet(FsDeviceList, id);
}

UIntPtr FsGetDeviceID(PChar name) {
	if (FsDeviceList == Null) {
		return 0;
	}
	
	UIntPtr idx = 0;
	
	ListForeach(FsDeviceList, i) {
		PChar dname = ((PDevice)(i->data))->name;
		
		if (StrGetLength(dname) != StrGetLength(name)) {
			idx++;
			continue;
		} else if (StrCompare(dname, name)) {
			return idx;
		} else {
			idx++;
		}
	}
	
	return (UIntPtr)-1;
}

Void FsSetBootDevice(PChar name) {
	if (FsGetDevice(name) != Null) {												// Device with this name exists?
		FsBootDevice = name;														// Yes! So set it as boot device (just like the user asked)
	}
}

PChar FsGetBootDevice(Void) {
	return FsBootDevice;
}

Void FsDbgListDevices(Void) {
	if (FsDeviceList == Null) {
		DbgWriteFormated("[FsDbgListDevices] Device list isn't initialized!\r\n");
	} else if (FsDeviceList->length == 0) {
		DbgWriteFormated("[FsDbgListDevices] No devices avaliable.\r\n");
	} else {
		ListForeach(FsDeviceList, i) {
			DbgWriteFormated("[FsDbgListDevices] %s\r\n", ((PDevice)(i->data))->name);
		}
	}
}

Void FsInitDeviceList(Void) {
	FsDeviceList = ListNew(True, False);											// Try to init the device list
	
	if (FsDeviceList == Null) {														// Failed?
		DbgWriteFormated("PANIC! Couldn't init the device list\r\n");				// Yes, so halt
		Panic(PANIC_KERNEL_INIT_FAILED);
	}
}

Void FsInitDevices(Void) {
	NullDeviceInit();																// Add the Null device
	ZeroDeviceInit();																// Add the Zero device
	RawMouseDeviceInit();															// Add the RawMouse device
	RawKeyboardDeviceInit();														// Add the RawKeyboard device
	FrameBufferDeviceInit();														// Add the FrameBuffer device
}
