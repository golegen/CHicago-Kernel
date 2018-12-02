// File author is Ítalo Lima Marconato Matias
//
// Created on July 14 of 2018, at 22:38 BRT
// Last edited on October 19 of 2018, at 18:22 BRT

#ifndef __CHICAGO_DEVICE_H__
#define __CHICAGO_DEVICE_H__

#include <chicago/types.h>

typedef struct DeviceStruct {
	PChar name;
	PVoid priv;
	Boolean (*read)(struct DeviceStruct *, UIntPtr, UIntPtr, PUInt8);
	Boolean (*write)(struct DeviceStruct *, UIntPtr, UIntPtr, PUInt8);
	Boolean (*control)(struct DeviceStruct *, UIntPtr, PUInt8, PUInt8);
} Device, *PDevice;

Void NullDeviceInit(Void);
Void ZeroDeviceInit(Void);
Void RawMouseDeviceInit(Void);
Void RawKeyboardDeviceInit(Void);
Void FrameBufferDeviceInit(Void);

Void RawMouseDeviceRead(UIntPtr len, PUInt8 buf);
Void RawMouseDeviceWrite(Int8 offx, Int8 offy, UInt8 buttons);

Void RawKeyboardDeviceRead(UIntPtr len, PUInt8 buf);
Void RawKeyboardDeviceWrite(UInt8 data);

Boolean FsReadDevice(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf);
Boolean FsWriteDevice(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf);
Boolean FsControlDevice(PDevice dev, UIntPtr cmd, PUInt8 ibuf, PUInt8 obuf);
Boolean FsAddDevice(PChar name, PVoid priv, Boolean (*read)(PDevice, UIntPtr, UIntPtr, PUInt8), Boolean (*write)(PDevice, UIntPtr, UIntPtr, PUInt8), Boolean (*control)(PDevice, UIntPtr, PUInt8, PUInt8));
Boolean FsRemoveDevice(PChar name);
PDevice FsGetDevice(PChar name);
PDevice FsGetDeviceByID(UIntPtr id);
UIntPtr FsGetDeviceID(PChar name);
Void FsSetBootDevice(PChar name);
PChar FsGetBootDevice(Void);
Void FsDbgListDevices(Void);
Void FsInitDeviceList(Void);
Void FsInitDevices(Void);

#endif		// __CHICAGO_DEVICE_H__
