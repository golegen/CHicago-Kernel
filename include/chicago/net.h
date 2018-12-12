// File author is Ítalo Lima Marconato Matias
//
// Created on December 12 of 2018, at 12:25 BRT
// Last edited on December 12 of 2018, at 13:34 BRT

#ifndef __CHICAGO_NET_H__
#define __CHICAGO_NET_H__

#include <chicago/types.h>

typedef struct {
	UIntPtr id;
	PVoid priv;
	PWChar dev_name;
	UInt8 mac_address[6];
	Void (*send)(PVoid, UIntPtr, PUInt8);
} NetworkDevice, *PNetworkDevice;

PNetworkDevice NetAddDevice(PVoid priv, UInt8 mac[6], Void (*send)(PVoid, UIntPtr, PUInt8));
Void NetRemoveDevice(PNetworkDevice dev);
Void NetSendRawPacket(PNetworkDevice dev, UIntPtr len, PUInt8 buf);

#endif		// __CHICAGO_NET_H__
