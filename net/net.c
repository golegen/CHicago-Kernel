// File author is Ítalo Lima Marconato Matias
//
// Created on December 12 of 2018, at 12:36 BRT
// Last edited on December 13 of 2018, at 09:44 BRT

#include <chicago/alloc.h>
#include <chicago/device.h>
#include <chicago/list.h>
#include <chicago/net.h>
#include <chicago/string.h>

PWChar NetDeviceString = L"NetworkX";
PList NetDevices = Null;
UIntPtr NetLastID = 0;

static Boolean NetDeviceWrite(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	(Void)off;																													// Avoid compiler's unused parameter warning
	NetSendRawPacket((PNetworkDevice)dev->priv, len, buf);																		// Redirect to NetSendRawPacket
	return True;
}

static Boolean NetDeviceControl(PDevice dev, UIntPtr cmd, PUInt8 ibuf, PUInt8 obuf) {
	(Void)ibuf;																													// Avoid compiler's unused parameter warning
	
	PNetworkDevice ndev = (PNetworkDevice)dev->priv;
	
	if (cmd == 0) {																												// Get MAC Address
		StrCopyMemory(obuf, ndev->mac_address, 6);
	} else if (cmd == 1) {																										// Set the IP(v4) address
		StrCopyMemory(ndev->ipv4_address, ibuf, 4);
	} else {
		return False;																											// ...
	}
	
	return True;
}

PNetworkDevice NetAddDevice(PVoid priv, UInt8 mac[6], Void (*send)(PVoid, UIntPtr, PUInt8)) {
	if (NetDevices == Null) {																									// Init the net device list?
		NetDevices = ListNew(True, False);																						// Yes :)
		
		if (NetDevices == Null) {
			return Null;																										// Failed :(
		}
	}
	
	if (mac == Null) {																											// Sanity checks
		return Null;
	}
	
	PNetworkDevice dev = (PNetworkDevice)MemAllocate(sizeof(NetworkDevice));													// Alloc space for our network device
	
	if (dev == Null) {
		return Null;																											// Failed
	}
	
	dev->id = NetLastID++;																										// Set the id
	dev->priv = priv;																											// The private data
	
	StrSetMemory(dev->ipv4_address, 0, 4);																						// For now we don't have a IPv4 address :(
	StrCopyMemory(dev->mac_address, mac, 6);																					// The mac address
	
	dev->send = send;																											// And the send function
	
	if (!ListAdd(NetDevices, dev)) {																							// Try to add!
		MemFree((UIntPtr)dev);																									// Failed, free the device
		NetLastID--;																											// And "free" the id
		return Null;
	}
	
	dev->dev_name = StrDuplicate(NetDeviceString);																				// Duplicate the NetworkX string
	
	if (dev->dev_name == Null) {
		NetRemoveDevice(dev);																									// Failed...
		return Null;
	}
	
	dev->dev_name[7] = (WChar)('0' + dev->id);																					// Set the num
	
	if (!FsAddDevice(dev->dev_name, dev, Null, NetDeviceWrite, NetDeviceControl)) {												// And try to add us to the device list!
		NetRemoveDevice(dev);																									// Failed...
		return Null;
	}
	
	return dev;
}

Void NetRemoveDevice(PNetworkDevice dev) {
	if ((NetDevices == Null) || (dev == Null)) {																				// Sanity checks
		return;
	}
	
	UIntPtr idx = 0;
	Boolean found = False;
	
	ListForeach(NetDevices, i) {																								// Try to find it on the net device list
		if (i->data == dev) {
			found = True;																										// Found!
			break;
		} else {
			idx++;	
		}
	}
	
	if (!found) {
		return;																													// Not found...
	}
	
	ListRemove(NetDevices, idx);																								// Remove it from the net devices list
	FsRemoveDevice(dev->dev_name);																								// Remove it from the device list
	MemFree((UIntPtr)dev);																										// And free the device
}

static Void NetHandleARPPacket(PNetworkDevice dev, PARPHeader hdr) {
	if ((dev == Null) || (hdr == Null)) {																						// Sanity checks
		return;
	} else if (hdr->htype == ToNetByteOrder16(hdr->htype)) {
		return;
	} else if (FromNetByteOrder16(hdr->ptype) != ETH_TYPE_IP) {
		return;
	}
	
	if (StrCompareMemory(dev->ipv4_address, hdr->ipv4.dst_pr, 4)) {																// For us?
		if (hdr->opcode == FromNetByteOrder16(ARP_OPC_REQUEST)) {																// Yes, was a request?
			NetSendARPIPv4Packet(dev, hdr->ipv4.src_hw, hdr->ipv4.src_pr, ARP_OPC_REPLY);										// Yes, so let's reply :)
		}
	}
}

static Void NetHandleEthPacket(PNetworkDevice dev, UInt8 src[6], UInt16 type, UIntPtr len, PUInt8 buf) {
	if ((dev == Null) || (src == Null) || (len == 0) || (buf == Null)) {														// Sanity checks
		return;
	}
	
	if (type == ETH_TYPE_ARP) {																									// ARP?
		NetHandleARPPacket(dev, (PARPHeader)buf);																				// Yes, handle it!
	}
}

Void NetHandlePacket(PNetworkDevice dev, UIntPtr len, PUInt8 buf) {
	if ((dev == Null) || (len < sizeof(EthFrame)) || (buf == Null)) {															// Sanity checks
		return;
	}
	
	PEthFrame frame = (PEthFrame)buf;
	
	NetHandleEthPacket(dev, frame->src, FromNetByteOrder16(frame->type), len - sizeof(EthFrame), buf + sizeof(EthFrame));		// Handle this eth packet
}

Void NetSendRawPacket(PNetworkDevice dev, UIntPtr len, PUInt8 buf) {
	if ((dev == Null) || (dev->send == Null)) {																					// Sanity checks
		return;
	}
	
	dev->send(dev->priv, len, buf);																								// SEND!
}

Void NetSendEthPacket(PNetworkDevice dev, UInt8 dest[6], UInt16 type, UIntPtr len, PUInt8 buf) {
	if ((dev == Null) || (dev->send == Null) || (dest == Null)) {																// Sanity checks
		return;
	}
	
	PEthFrame frame = (PEthFrame)MemAllocate(sizeof(EthFrame) + len);															// Let's build our ethernet frame!
	
	if (frame == Null) {
		return;																													// Failed :(
	}
	
	StrCopyMemory(frame->dst, dest, 6);																							// Copy the dest mac address
	StrCopyMemory(frame->src, dev->mac_address, 6);																				// The src mac address (our mac address)
	StrCopyMemory((PUInt8)(((UIntPtr)frame) + sizeof(EthFrame)), buf, len);														// Copy the data/payload
	
	frame->type = ToNetByteOrder16(type);																						// Set the type
	
	dev->send(dev->priv, sizeof(EthFrame) + len, (PUInt8)frame);																// SEND!
	MemFree((UIntPtr)frame);																									// Free our eth frame
}


Void NetSendARPIPv4Packet(PNetworkDevice dev, UInt8 destm[6], UInt8 desti[4], UInt16 opcode) {
	if ((dev == Null) || (destm == Null)) {																						// Sanity checks
		return;
	}
	
	PARPHeader hdr = (PARPHeader)MemAllocate(sizeof(ARPHeader));																// Let's build our ARP header
	
	if (hdr == Null) {
		return;																													// Failed :(
	}
	
	hdr->htype = 0x100;																											// Fill the hardware type (1 = ETHERNET)
	hdr->ptype = 8;																												// The protocol type (IP)
	hdr->hlen = 6;																												// The hardware length (MAC ADDRESS = 6)
	hdr->plen = 4;																												// The protocol length (IPV4 ADDRESS = 4)
	hdr->opcode = ToNetByteOrder16(opcode);																						// The opcode
	
	StrCopyMemory(hdr->ipv4.dst_hw, destm, 6);																					// The destination mac address
	StrCopyMemory(hdr->ipv4.dst_pr, desti, 4);																					// The destination ipv4 address
	StrCopyMemory(hdr->ipv4.src_hw, dev->mac_address, 6);																		// The source mac address
	StrCopyMemory(hdr->ipv4.src_pr, dev->ipv4_address, 4);																		// And the source ipv4 address
	NetSendEthPacket(dev, destm, ETH_TYPE_ARP, sizeof(ARPHeader), (PUInt8)hdr);													// Send!
	MemFree((UIntPtr)hdr);																										// And free the header	
}
