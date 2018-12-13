// File author is Ítalo Lima Marconato Matias
//
// Created on December 12 of 2018, at 12:25 BRT
// Last edited on December 13 of 2018, at 15:33 BRT

#ifndef __CHICAGO_NET_H__
#define __CHICAGO_NET_H__

#include <chicago/process.h>
#include <chicago/queue.h>

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define ToNetByteOrder16(v) (v)
#define ToNetByteOrder32(v) (v)
#define FromNetByteOrder16(v) (v)
#define FromNetByteOrder32(v) (v)
#else
#define ToNetByteOrder16(v) ((v >> 8) | (v << 8))
#define ToNetByteOrder32(v) (((v >> 24) & 0xFF) | ((v << 8) & 0xFF0000) | ((v >> 8) & 0xFF00) | ((v << 24) & 0xFF000000))
#define FromNetByteOrder16(v) ((v >> 8) | (v << 8))
#define FromNetByteOrder32(v) (((v >> 24) & 0xFF) | ((v << 8) & 0xFF0000) | ((v >> 8) & 0xFF00) | ((v << 24) & 0xFF000000))
#endif

#define ETH_TYPE_IP 0x800
#define ETH_TYPE_ARP 0x806

#define ARP_OPC_REQUEST 0x01
#define ARP_OPC_REPLY 0x02

typedef struct {
	UInt8 dst[6];
	UInt8 src[6];
	UInt16 type;
} EthFrame, *PEthFrame;

typedef struct {
	UInt16 htype;
	UInt16 ptype;
	UInt8 hlen;
	UInt8 plen;
	UInt16 opcode;
	struct {
		UInt8 src_hw[6];
		UInt8 src_pr[4];
		UInt8 dst_hw[6];
		UInt8 dst_pr[4];
	} ipv4;
} ARPHeader, *PARPHeader;

typedef struct {
	UIntPtr id;
	PVoid priv;
	PWChar dev_name;
	UInt8 ipv4_address[4];
	UInt8 mac_address[6];
	Void (*send)(PVoid, UIntPtr, PUInt8);
} NetworkDevice, *PNetworkDevice;

typedef struct {
	Boolean user;
	PNetworkDevice dev;
	PQueue packet_queue;
	UInt8 mac_address[6];
	UInt8 ipv4_address[4];
	PProcess owner_process;
} ARPIPv4Socket, *PARPIPv4Socket;

PNetworkDevice NetAddDevice(PVoid priv, UInt8 mac[6], Void (*send)(PVoid, UIntPtr, PUInt8));
PNetworkDevice NetGetDevice(PFsNode dev);
Void NetRemoveDevice(PNetworkDevice dev);
Void NetHandlePacket(PNetworkDevice dev, UIntPtr len, PUInt8 buf);
Void NetSendRawPacket(PNetworkDevice dev, UIntPtr len, PUInt8 buf);
Void NetSendEthPacket(PNetworkDevice dev, UInt8 dest[6], UInt16 type, UIntPtr len, PUInt8 buf);
Void NetSendARPIPv4Packet(PNetworkDevice dev, UInt8 destm[6], UInt8 desti[4], UInt16 opcode);
PARPIPv4Socket NetAddARPIPv4Socket(PNetworkDevice dev, UInt8 mac[6], UInt8 ipv4[4], Boolean user);
Void NetRemoveARPIPv4Socket(PARPIPv4Socket sock);
Void NetSendARPIPv4Socket(PARPIPv4Socket sock, UInt16 opcode);
PARPHeader NetReceiveARPIPv4Socket(PARPIPv4Socket sock);

#endif		// __CHICAGO_NET_H__
