// File author is Ítalo Lima Marconato Matias
//
// Created on November 15 of 2018, at 22:17 BRT
// Last edited on November 15 of 2018, at 22:53 BRT

#ifndef __CHICAGO_IPC_H__
#define __CHICAGO_IPC_H__

#include <chicago/process.h>
#include <chicago/queue.h>

typedef struct {
	UInt32 msg;
	PProcess src;
	UIntPtr size;
	PUInt8 buffer;
} IpcMessage, *PIpcMessage;

typedef struct {
	PChar name;
	Queue queue;
	PProcess proc;
} IpcPort, *PIpcPort;

PIpcPort IpcCreatePort(PChar name);
Void IpcRemovePort(PChar name);
Void IpcSendMessage(PChar port, UInt32 msg, UIntPtr size, PUInt8 buf);
PIpcMessage IpcReceiveMessage(PChar name);
Void IpcInit(Void);

#endif		// __CHICAGO_IPC_H__
