// File author is Ítalo Lima Marconato Matias
//
// Created on November 15 of 2018, at 22:17 BRT
// Last edited on December 15 of 2018, at 09:08 BRT

#ifndef __CHICAGO_IPC_H__
#define __CHICAGO_IPC_H__

#include <chicago/process.h>
#include <chicago/queue.h>

typedef struct {
	UInt32 msg;
	UIntPtr src;
	UIntPtr size;
	PUInt8 buffer;
} IpcMessage, *PIpcMessage;

typedef struct {
	PWChar name;
	Queue queue;
	PProcess proc;
} IpcPort, *PIpcPort;

#ifndef __CHICAGO_IPC__
extern PList IpcPortList;
#endif

PIpcPort IpcCreatePort(PWChar name);
Void IpcRemovePort(PWChar name);
Void IpcSendMessage(PWChar port, UInt32 msg, UIntPtr size, PUInt8 buf);
PIpcMessage IpcReceiveMessage(PWChar name);
Void IpcInit(Void);

#endif		// __CHICAGO_IPC_H__
