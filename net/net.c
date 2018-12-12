// File author is Ítalo Lima Marconato Matias
//
// Created on December 12 of 2018, at 12:36 BRT
// Last edited on December 12 of 2018, at 13:37 BRT

#include <chicago/alloc.h>
#include <chicago/device.h>
#include <chicago/list.h>
#include <chicago/net.h>
#include <chicago/string.h>

PWChar NetDeviceString = L"NetworkX";
PList NetDevices = Null;
UIntPtr NetLastID = 0;

static Boolean NetDeviceWrite(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	(Void)off;																						// Avoid compiler's unused parameter warning
	NetSendRawPacket((PNetworkDevice)dev->priv, len, buf);											// Redirect to NetSendRawPacket
	return True;
}

static Boolean NetDeviceControl(PDevice dev, UIntPtr cmd, PUInt8 ibuf, PUInt8 obuf) {
	(Void)ibuf;																						// Avoid compiler's unused parameter warning
	
	PNetworkDevice ndev = (PNetworkDevice)dev->priv;
	
	if (cmd == 0) {																					// Get MAC Address
		StrCopyMemory(obuf, ndev->mac_address, 6);
	} else {
		return False;																				// ...
	}
	
	return True;
}

PNetworkDevice NetAddDevice(PVoid priv, UInt8 mac[6], Void (*send)(PVoid, UIntPtr, PUInt8)) {
	if (NetDevices == Null) {																		// Init the net device list?
		NetDevices = ListNew(True, False);															// Yes :)
		
		if (NetDevices == Null) {
			return Null;																			// Failed :(
		}
	}
	
	if (mac == Null) {																				// Sanity checks
		return Null;
	}
	
	PNetworkDevice dev = (PNetworkDevice)MemAllocate(sizeof(NetworkDevice));						// Alloc space for our network device
	
	if (dev == Null) {
		return Null;																				// Failed
	}
	
	dev->id = NetLastID++;																			// Set the id
	dev->priv = priv;																				// The private data
	
	StrCopyMemory(dev->mac_address, mac, 6);														// The mac address
	
	dev->send = send;																				// And the send function
	
	if (!ListAdd(NetDevices, dev)) {																// Try to add!
		MemFree((UIntPtr)dev);																		// Failed, free the device
		NetLastID--;																				// And "free" the id
		return Null;
	}
	
	dev->dev_name = StrDuplicate(NetDeviceString);													// Duplicate the NetworkX string
	
	if (dev->dev_name == Null) {
		NetRemoveDevice(dev);																		// Failed...
		return Null;
	}
	
	dev->dev_name[7] = (WChar)('0' + dev->id);														// Set the num
	
	if (!FsAddDevice(dev->dev_name, dev, Null, NetDeviceWrite, NetDeviceControl)) {					// And try to add us to the device list!
		NetRemoveDevice(dev);																		// Failed...
		return Null;
	}
	
	return dev;
}

Void NetRemoveDevice(PNetworkDevice dev) {
	if ((NetDevices == Null) || (dev == Null)) {													// Sanity checks
		return;
	}
	
	UIntPtr idx = 0;
	Boolean found = False;
	
	ListForeach(NetDevices, i) {																	// Try to find it on the net device list
		if (i->data == dev) {
			found = True;																			// Found!
			break;
		} else {
			idx++;	
		}
	}
	
	if (!found) {
		return;																						// Not found...
	}
	
	ListRemove(NetDevices, idx);																	// Remove it from the net devices list
	FsRemoveDevice(dev->dev_name);																	// Remove it from the device list
	MemFree((UIntPtr)dev);																			// And free the device
}

Void NetSendRawPacket(PNetworkDevice dev, UIntPtr len, PUInt8 buf) {
	if ((dev == Null) || (dev->send == Null)) {														// Sanity checks
		return;
	}
	
	dev->send(dev->priv, len, buf);																	// SEND!
}
