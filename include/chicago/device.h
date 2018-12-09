// File author is Ítalo Lima Marconato Matias
//
// Created on July 14 of 2018, at 22:38 BRT
// Last edited on December 09 of 2018, at 17:24 BRT

#ifndef __CHICAGO_DEVICE_H__
#define __CHICAGO_DEVICE_H__

#include <chicago/types.h>

typedef struct DeviceStruct {
	PWChar name;
	PVoid priv;
	Boolean (*read)(struct DeviceStruct *, UIntPtr, UIntPtr, PUInt8);
	Boolean (*write)(struct DeviceStruct *, UIntPtr, UIntPtr, PUInt8);
	Boolean (*control)(struct DeviceStruct *, UIntPtr, PUInt8, PUInt8);
} Device, *PDevice;

Void NullDeviceInit(Void);
Void ZeroDeviceInit(Void);
Void ConsoleDeviceInit(Void);
Void RawMouseDeviceInit(Void);
Void RawKeyboardDeviceInit(Void);
Void FrameBufferDeviceInit(Void);

Void RawMouseDeviceRead(UIntPtr len, PUInt8 buf);
Void RawMouseDeviceWrite(Int8 offx, Int8 offy, UInt8 buttons);

Void RawKeyboardDeviceRead(UIntPtr len, PUInt8 buf);
Void RawKeyboardDeviceWrite(UInt8 data);
Void RawKeyboardDeviceClear(Void);

Void ConsoleDeviceReadKeyboard(UIntPtr len, PWChar buf);
Void ConsoleDeviceWriteKeyboard(Char data);
Boolean ConsoleDeviceBackKeyboard(Void);
Void ConsoleDeviceClearKeyboard(Void);

Boolean FsReadDevice(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf);
Boolean FsWriteDevice(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf);
Boolean FsControlDevice(PDevice dev, UIntPtr cmd, PUInt8 ibuf, PUInt8 obuf);
Boolean FsAddDevice(PWChar name, PVoid priv, Boolean (*read)(PDevice, UIntPtr, UIntPtr, PUInt8), Boolean (*write)(PDevice, UIntPtr, UIntPtr, PUInt8), Boolean (*control)(PDevice, UIntPtr, PUInt8, PUInt8));
Boolean FsRemoveDevice(PWChar name);
PDevice FsGetDevice(PWChar name);
PDevice FsGetDeviceByID(UIntPtr id);
UIntPtr FsGetDeviceID(PWChar name);
Void FsSetBootDevice(PWChar name);
PWChar FsGetBootDevice(Void);
Void FsDbgListDevices(Void);
Void FsInitDeviceList(Void);
Void FsInitDevices(Void);

#endif		// __CHICAGO_DEVICE_H__
