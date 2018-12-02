// File author is Ítalo Lima Marconato Matias
//
// Created on July 16 of 2018, at 18:18 BRT
// Last edited on November 02 of 2018, at 01:07 BRT

#ifndef __CHICAGO_FILE_H__
#define __CHICAGO_FILE_H__

#include <chicago/device.h>
#include <chicago/list.h>

#define FS_FLAG_DIR 0x01
#define FS_FLAG_FILE 0x02

typedef struct FsNodeStruct {
	PChar name;
	PVoid priv;
	UIntPtr flags;
	UIntPtr inode;
	UIntPtr length;
	UIntPtr offset;
	Boolean (*read)(struct FsNodeStruct *, UIntPtr, UIntPtr, PUInt8);
	Boolean (*write)(struct FsNodeStruct *, UIntPtr, UIntPtr, PUInt8);
	Boolean (*open)(struct FsNodeStruct *);
	Void (*close)(struct FsNodeStruct *);
	PChar (*readdir)(struct FsNodeStruct *, UIntPtr);
	struct FsNodeStruct *(*finddir)(struct FsNodeStruct *, PChar);
	Boolean (*create)(struct FsNodeStruct *, PChar, UIntPtr);
	Boolean (*control)(struct FsNodeStruct *, UIntPtr, PUInt8, PUInt8);
} FsNode, *PFsNode;

typedef struct {
	PChar path;
	PChar type;
	PFsNode root;
} FsMountPoint, *PFsMountPoint;

typedef struct {
	PChar name;
	Boolean (*probe)(PFsNode);
	PFsMountPoint (*mount)(PFsNode, PChar);
	Boolean (*umount)(PFsMountPoint);
} FsType, *PFsType;

Void CHFsInit(Void);
Void DevFsInit(Void);
Void Iso9660Init(Void);

PList FsTokenizePath(PChar path);
PChar FsCanonicalizePath(PChar path);
PChar FsJoinPath(PChar src, PChar incr);
PChar FsGetRandomPath(PChar prefix);
Boolean FsReadFile(PFsNode file, UIntPtr off, UIntPtr len, PUInt8 buf);
Boolean FsWriteFile(PFsNode file, UIntPtr off, UIntPtr len, PUInt8 buf);
PFsNode FsOpenFile(PChar path);
Void FsCloseFile(PFsNode node);
Boolean FsMountFile(PChar path, PChar file, PChar type);
Boolean FsUmountFile(PChar path);
PChar FsReadDirectoryEntry(PFsNode dir, UIntPtr entry);
PFsNode FsFindInDirectory(PFsNode dir, PChar name);
Boolean FsCreateFile(PFsNode dir, PChar name, UIntPtr type);
Boolean FsControlFile(PFsNode file, UIntPtr cmd, PUInt8 ibuf, PUInt8 obuf);
PFsMountPoint FsGetMountPoint(PChar path, PChar *outp);
PFsType FsGetType(PChar type);
Boolean FsAddMountPoint(PChar path, PChar type, PFsNode root);
Boolean FsRemoveMountPoint(PChar path);
Boolean FsAddType(PChar name, Boolean (*probe)(PFsNode), PFsMountPoint (*mount)(PFsNode, PChar), Boolean (*umount)(PFsMountPoint));
Boolean FsRemoveType(PChar name);
Void FsDbgListMountPoints(Void);
Void FsDbgListTypes(Void);
Void FsInit(Void);

#endif		// __CHICAGO_FILE_H__
