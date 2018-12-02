// File author is Ítalo Lima Marconato Matias
//
// Created on July 14 of 2018, at 23:40 BRT
// Last edited on November 15 of 2018, at 16:00 BRT

#include <chicago/arch/ide.h>
#include <chicago/arch/idt.h>
#include <chicago/arch/port.h>

#include <chicago/alloc.h>
#include <chicago/debug.h>
#include <chicago/device.h>
#include <chicago/string.h>

IDEDevice IDEDevices[4];

UInt8 IDEBuffer[512] = { 0 };
Volatile Boolean IDEIRQInvoked = False;

PChar IDEHardDiskString = "HardDiskX";
PChar IDECDROMString = "CdRomX";

Void IDEHandler(PRegisters regs) {
	(Void)regs;
	IDEIRQInvoked = True;
}

Void IDEWaitIRQ(Void) {
	while (!IDEIRQInvoked) ;
	IDEIRQInvoked = False;
}

PChar IDEGetHardDiskString(Void) {
	return IDEHardDiskString;
}

PChar IDEGetCDROMString(Void) {
	return IDECDROMString;
}

Boolean IDEPolling(UInt16 io, Boolean adv) {
	while (PortInByte(io + ATA_REG_STATUS) & ATA_SR_BSY) ;														// Wait for BSY to be cleared
	
	if (adv) {																									// Advanced check?
		UInt8 status = PortInByte(io + ATA_REG_STATUS);															// Yes, read the status register
		
		if (status & ATA_SR_ERR || status & ATA_SR_DF || (!(status & ATA_SR_DRQ))) {							// Check for error
			return False;
		}
	}
	
	return True;
}

Void IDESelectDrive(UInt8 bus, UInt8 drive) {
	if (bus == 0) {																								// Primary?
		if (drive == 0) {																						// Yes! Master?
			PortOutByte(0x1F0 + ATA_REG_HDDEVSEL, 0xA0);														// Yes!
		} else {
			PortOutByte(0x1F0 + ATA_REG_HDDEVSEL, 0xB0);														// No, so it's slave
		}
	} else {
		if (drive == 0) {																						// Secundary... Master?
			PortOutByte(0x170 + ATA_REG_HDDEVSEL, 0xA0);														// Yes!
		} else {
			PortOutByte(0x170 + ATA_REG_HDDEVSEL, 0xB0);														// No, so it's slave
		}
	}
}

Boolean IDEATAReadSector(UInt8 bus, UInt8 drive, UInt32 lba, PUInt8 buf) {
	if (bus > 1 || drive > 1) {																					// Valid?
		return False;																							// No....
	}
	
	IDEDevice dev = IDEDevices[(bus * 2) + drive];																// Let's get our int device
	
	if (!dev.valid) {																							// Valid device?
		return False;																							// No
	} else if (dev.atapi) {																						// ATAPI device?
		return False;																							// Yes, in this function we only handle ATA
	}
	
	UInt16 io = dev.io;
	UInt8 slave = dev.slave;
	
	PortOutByte(io + ATA_REG_CONTROL, (IDEIRQInvoked = 0) + 2);													// Disable IRQs
	
	while (PortInByte(io + ATA_REG_STATUS) & ATA_SR_BSY) ;														// Poll until BSY is clear
	
	if (dev.addr48 && lba >= 0x10000000) {																		// We support, and need 48-bit addressing?
		PortOutByte(io + ATA_REG_HDDEVSEL, 0xE0 | (slave << 4));												// Yes!
	} else if (lba >= 0x10000000) {																				// We don't support, and need 48-bit addressing?
		return False;																							// No...
	} else {																									// So, it's normal addressing
		PortOutByte(io + ATA_REG_HDDEVSEL, 0xE0 | (slave << 4) | ((lba & 0xF0000000) >> 24));
	}
	
	if (lba >= 0x10000000) {																					// 48-bit addressing?
		PortOutByte(io + ATA_REG_SECCOUNT1, 0);																	// Yes, so setup 48-bit addressing mode
		PortOutByte(io + ATA_REG_LBA3, ((lba & 0xFF000000) >> 24));
		PortOutByte(io + ATA_REG_LBA4, 0);
		PortOutByte(io + ATA_REG_LBA5, 0);
	}
	
	PortOutByte(io + ATA_REG_SECCOUNT0, 1);																		// 1 sector per time
	PortOutByte(io + ATA_REG_LBA0, (lba & 0xFF));																// Let's send the LBA
	PortOutByte(io + ATA_REG_LBA1, ((lba & 0xFF00) >> 8));
	PortOutByte(io + ATA_REG_LBA2, ((lba & 0xFF0000) >> 16));
	
	if (lba >= 0x10000000) {																					// Send extended (48-bits addressing mode) command?
		PortOutByte(io + ATA_REG_COMMAND, ATA_CMD_READ_PIO_EXT);												// Yes
	} else {
		PortOutByte(io + ATA_REG_COMMAND, ATA_CMD_READ_PIO);													// No
	}
	
	if (!IDEPolling(io, True)) {																				// Poll, and return error if error
		return False;
	}
	
	PortInMultiple(io + ATA_REG_DATA, buf, 256);																// Read the data
	
	return True;
}

Boolean IDEATAWriteSector(UInt8 bus, UInt8 drive, UInt32 lba, PUInt8 buf) {
	if (bus > 1 || drive > 1) {																					// Valid?
		return False;																							// No....
	}
	
	IDEDevice dev = IDEDevices[(bus * 2) + drive];																// Let's get our int device
	
	if (!dev.valid) {																							// Valid device?
		return False;																							// No
	} else if (dev.atapi) {																						// ATAPI device?
		return False;																							// Yes, in this function we only handle ATA
	}
	
	UInt16 io = dev.io;
	UInt8 slave = dev.slave;
	
	PortOutByte(io + ATA_REG_CONTROL, (IDEIRQInvoked = 0) + 2);													// Disable IRQs
	
	while (PortInByte(io + ATA_REG_STATUS) & ATA_SR_BSY) ;														// Poll until BSY is clear
	
	if (dev.addr48 && lba >= 0x10000000) {																		// We support, and need 48-bit addressing?
		PortOutByte(io + ATA_REG_HDDEVSEL, 0xE0 | (slave << 4));												// Yes!
	} else if (lba >= 0x10000000) {																				// We don't support, and need 48-bit addressing?
		return False;																							// No...
	} else {																									// So, it's normal addressing
		PortOutByte(io + ATA_REG_HDDEVSEL, 0xE0 | (slave << 4) | ((lba & 0xF0000000) >> 24));
	}
	
	if (lba >= 0x10000000) {																					// 48-bit addressing?
		PortOutByte(io + ATA_REG_SECCOUNT1, 0);																	// Yes, so setup 48-bit addressing mode
		PortOutByte(io + ATA_REG_LBA3, ((lba & 0xFF000000) >> 24));
		PortOutByte(io + ATA_REG_LBA4, 0);
		PortOutByte(io + ATA_REG_LBA5, 0);
	}
	
	PortOutByte(io + ATA_REG_SECCOUNT0, 1);																		// 1 sector per time
	PortOutByte(io + ATA_REG_LBA0, (lba & 0xFF));																// Let's send the LBA
	PortOutByte(io + ATA_REG_LBA1, ((lba & 0xFF00) >> 8));
	PortOutByte(io + ATA_REG_LBA2, ((lba & 0xFF0000) >> 16));
	
	if (lba >= 0x10000000) {																					// Send extended (48-bits addressing mode) command?
		PortOutByte(io + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO_EXT);												// Yes
	} else {
		PortOutByte(io + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);													// No
	}
	
	if (!IDEPolling(io, True)) {																				// Poll, and return error if error
		return False;
	}
	
	PortOutMultiple(io + ATA_REG_DATA, buf, 256);																// Write the data
	
	if (lba >= 0x10000000) {																					// Send extended (48-bits addressing mode) FLUSH command?
		PortOutByte(io + ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH_EXT);												// Yes
	} else {
		PortOutByte(io + ATA_REG_COMMAND, ATA_CMD_CACHE_FLUSH);													// No
	}
	
	IDEPolling(io, False);																						// Do some polling
	
	return True;
}

Boolean IDEATAPIReadSector(UInt8 bus, UInt8 drive, UInt32 lba, PUInt8 buf) {
	if (bus > 1 || drive > 1) {																					// Valid?
		return False;																							// No....
	}
	
	IDEDevice dev = IDEDevices[(bus * 2) + drive];																// Let's get our int device
	
	if (!dev.valid) {																							// Valid device?
		return False;																							// No
	} else if (!dev.atapi) {																					// ATAPI device?
		return False;																							// No, in this function we only handle ATAPI
	}
	
	UInt16 io = dev.io;
	UInt8 slave = dev.slave;
	UInt8 packet[12] = { 0 };
	
	PortOutByte(io + ATA_REG_CONTROL, IDEIRQInvoked = 0);														// This time, enable the IRQs
	
	packet[0] = ATAPI_CMD_READ;																					// Setup SCSI packet
	packet[1] = 0x00;
	packet[2] = ((lba >> 24) & 0xFF);
	packet[3] = ((lba >> 16) & 0xFF);
	packet[4] = ((lba >> 8) & 0xFF);
	packet[5] = (lba & 0xFF);
	packet[6] = 0x00;
	packet[7] = 0x00;
	packet[8] = 0x00;
	packet[9] = 0x01;
	packet[10] = 0x00;
	packet[11] = 0x00;
	
	PortOutByte(io + ATA_REG_HDDEVSEL, slave << 4);																// Select the drive
	
	for (UInt32 i = 0; i < 4; i++) {																			// 400 nanoseconds delay
		PortInByte(io + ATA_REG_ALTSTATUS);
	}
	
	PortOutByte(io + ATA_REG_FEATURES, 0);																		// We're going to use PIO mode
	PortOutByte(io + ATA_REG_LBA1, 2048 & 0xFF);																// Tell the size of the buffer
	PortOutByte(io + ATA_REG_LBA2, 2048 >> 8);
	PortOutByte(io + ATA_REG_COMMAND, ATA_CMD_PACKET);															// Send the PACKET command
	
	if (!IDEPolling(io, True)) {																				// Poll and return error if error
		return False;
	}
	
	PortOutMultiple(io + ATA_REG_DATA, packet, 6);																// Send the packet
	
	IDEWaitIRQ();																								// Wait for an IRQ
	
	if (!IDEPolling(io, True)) {																				// Poll, and... alright let's stop this
		return False;
	}
	
	PortInMultiple(io + ATA_REG_DATA, buf, 1024);																// Read the data
	
	IDEWaitIRQ();																								// And wait for another IRQ
	
	while (PortInByte(io + ATA_REG_STATUS) & (ATA_SR_BSY | ATA_SR_DRQ)) ;										// Wait for BSY and DRQ to clear
	
	return True;
}

Boolean IDEReadSectors(UInt8 bus, UInt8 drive, UInt8 count, UInt32 lba, PUInt8 buf) {
	if (bus > 1 || drive > 1) {																					// Valid?
		return False;																							// No....
	}
	
	IDEDevice dev = IDEDevices[(bus * 2) + drive];																// Let's get our int device
	
	if (!dev.valid) {																							// Valid device?
		return False;																							// No
	}
	
	Boolean ret = False;
	
	if (dev.atapi) {																							// ATAPI?
		for (UInt32 i = 0; i < count; i++) {																	// Yes, so use the ATAPI read sector function
			ret = IDEATAPIReadSector(bus, drive, lba + i, buf + (i * 2048));
		}
	} else {
		for (UInt32 i = 0; i < count; i++) {																	// No, so use the ATA read sector function
			ret = IDEATAReadSector(bus, drive, lba + i, buf + (i * 512));
		}
	}
	
	return ret;
}

Boolean IDEWriteSectors(UInt8 bus, UInt8 drive, UInt8 count, UInt32 lba, PUInt8 buf) {
	if (bus > 1 || drive > 1) {																					// Valid?
		return False;																							// No....
	}
	
	IDEDevice dev = IDEDevices[(bus * 2) + drive];																// Let's get our int device
	
	if (!dev.valid) {																							// Valid device?
		return False;																							// No
	}
	
	Boolean ret = False;
	
	if (dev.atapi) {																							// ATAPI?
		ret = False;																							// Yes... but we don't support ATAPI write...
	} else {
		for (UInt32 i = 0; i < count; i++) {																	// No, so yse the ATA write sector function
			ret = IDEATAWriteSector(bus, drive, lba + i, buf + (i * 512));
		}
	}
	
return ret;
}

UIntPtr IDEGetBlockSize(UInt8 bus, UInt8 drive) {
	if (bus > 1 || drive > 1) {
		return 0;
	}
	
	IDEDevice dev = IDEDevices[(bus * 2) + drive];
	
	if (!dev.valid) {
		return 0;
	}
	
	if (dev.atapi) {
		return 2048;
	} else {
		return 512;
	}
}

Void IDEInitializeInt(UInt32 bus, UInt32 drive)
{
	UInt16 io = 0;
	Boolean atapi = False;
	
	IDEDevices[(bus * 2) + drive].valid = False;																// For now, let's assume that there is no drive here
	
	IDESelectDrive(bus, drive);																					// Select drive
	
	if (bus == 0) {																								// Primary or secundary drive?
		io = 0x1F0;																								// Primary
	} else {
		io = 0x170;																								// Secundary
	}
	
	PortOutByte(io + ATA_REG_SECCOUNT0, 0);																		// These values should be zero before sending IDENTIFY
	PortOutByte(io + ATA_REG_LBA0, 0);
	PortOutByte(io + ATA_REG_LBA1, 0);
	PortOutByte(io + ATA_REG_LBA2, 0);
	
	PortOutByte(io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);														// Send IDENTIFY
	
	if (!(PortInByte(io + ATA_REG_STATUS))) {																	// Read the status port
		return;
	}
	
	Boolean err = False;
	
	while (True) {																									// Poll until BSY is clear
		UInt8 status = PortInByte(io + ATA_REG_STATUS);
		
		if (status & ATA_SR_ERR) {																				// ERR set?
			err = True;																							// So we can break and... well... ERROR (or, maybe, it's an ATAPI drive)
			break;
		}
		
		if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)) {													// Everyting is alright?
			break;																								// So we can break and continue
		}
	}
	
	if (err) {																									// ERR set?
		UInt8 cl = PortInByte(io + ATA_REG_LBA1);																// This drive may be an ATAPI drive
		UInt8 ch = PortInByte(io + ATA_REG_LBA2);
		
		if ((cl == 0x14 && ch == 0xEB) || (cl == 0x69 && ch == 0x96)) {											// ATAPI?
			PortOutByte(io + ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);											// Yes, so send ATAPI IDENTIFY PACKET
			atapi = True;
		} else {
			return;																								// No... so just return
		}
	}
	
	for (UInt32 i = 0; i < 256; i++) {																			// Read the data
		*(PUInt16)(IDEBuffer + i * 2) = PortInWord(io + ATA_REG_DATA);
	}
	
	IDEDevices[(bus * 2) + drive].valid = True;																	// We're valid!
	IDEDevices[(bus * 2) + drive].io = io;																		// Set the IO (base port)
	IDEDevices[(bus * 2) + drive].slave = drive;																// Set if we're slave
	IDEDevices[(bus * 2) + drive].atapi = atapi;																// Set if this is an atapi drive
	IDEDevices[(bus * 2) + drive].addr48 = ((*((PUInt32)(IDEBuffer + ATA_IDENT_COMMANDSETS))) & (1 << 26));		// And use command set to get if we have 48-bit addressing
	
	for (UInt32 i = 0; i < 40; i += 2)	{																		// Get disk model
		IDEDevices[(bus * 2) + drive].model[i] = IDEBuffer[ATA_IDENT_MODEL + i + 1];
		IDEDevices[(bus * 2) + drive].model[i + 1] = IDEBuffer[ATA_IDENT_MODEL + i];
	}
	
	IDEDevices[(bus * 2) + drive].model[40] = 0;																// End disk model string
}

Boolean IDEDeviceRead(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	UInt8 bus = (((UInt32)dev->priv) >> 8) & 0xFF;
	UInt8 drive = ((UInt32)dev->priv) & 0xFF;
	UIntPtr bsize = IDEGetBlockSize(bus, drive);																// Get the block size
	
	if (bsize == 0) {
		return False;																							// Invalid device!
	}
	
	UIntPtr cur = off / bsize;
	UIntPtr end = (off + len) / bsize;
	
	if ((off % bsize) != 0) {																					// "Align" the start
		PUInt8 buff = (PUInt8)MemAllocate(bsize);																// Alloc memory for reading the disk
		
		if (buff == Null) {
			return False;																						// Failed...
		} else if (!IDEReadSectors(bus, drive, 1, cur, buff)) {													// Read this block
			MemFree((UIntPtr)buff);																				// Failed...
			return False;
		}
		
		StrCopyMemory(buf, buff + (off % bsize), bsize - (off % bsize));										// Copy it into the user buffer
		MemFree((UIntPtr)buff);
		cur++;
	}
	
	if (((off + len) % bsize) != 0) {																			// "Align" the end
		PUInt8 buff = (PUInt8)MemAllocate(bsize);																// Alloc memory for reading the disk
		
		if (buff == Null) {
			return False;																						// Failed...
		} else if (!IDEReadSectors(bus, drive, 1, end, buff)) {													// Read this block
			MemFree((UIntPtr)buff);																				// Failed...
			return False;
		}
		
		StrCopyMemory(buf + len - ((off + len) % bsize), buff, (off + len) % bsize);							// Copy it into the user buffer
		MemFree((UIntPtr)buff);
		
		if (end != 0) {																							// Only decrease the end if it isn't 0
			end--;
		}
	}
	
	if (cur < end) {																							// Let's read!
		if (!IDEReadSectors(bus, drive, end - cur, cur, buf + (off % bsize))) {
			return False;																						// Failed...
		}
	}
	
	return True;
}

Boolean IDEDeviceWrite(PDevice dev, UIntPtr off, UIntPtr len, PUInt8 buf) {
	UInt8 bus = (((UInt32)dev->priv) >> 8) & 0xFF;
	UInt8 drive = ((UInt32)dev->priv) & 0xFF;
	UIntPtr bsize = IDEGetBlockSize(bus, drive);																// Get the block size
	
	if (bsize == 0) {
		return False;																							// Invalid device!
	}
	
	UIntPtr cur = off / bsize;
	UIntPtr end = (off + len) / bsize;
	
	if ((off % bsize) != 0) {																					// "Align" the start
		PUInt8 buff = (PUInt8)MemAllocate(bsize);																// Alloc memory for reading the disk
		
		if (buff == Null) {
			return False;																						// Failed...
		} else if (!IDEReadSectors(bus, drive, 1, cur, buff)) {													// Read this block
			MemFree((UIntPtr)buff);																				// Failed...
			return False;
		}
		
		StrCopyMemory(buff + (off % bsize), buf + (off % bsize), bsize - (off % bsize));						// Write back to the buffer
		
		if (!IDEWriteSectors(bus, drive, 1, cur, buff)) {														// Write this block back
			MemFree((UIntPtr)buff);																				// Failed...
			return False;
		}
		
		MemFree((UIntPtr)buff);
		cur++;
	}
	
	if (((off + len) % bsize) != 0) {																			// "Align" the end
		PUInt8 buff = (PUInt8)MemAllocate(bsize);																// Alloc memory for reading the disk
		
		if (buff == Null) {
			return False;																						// Failed...
		} else if (!IDEReadSectors(bus, drive, 1, end, buff)) {													// Read this block
			MemFree((UIntPtr)buff);																				// Failed...
			return False;
		}
		
		StrCopyMemory(buff + ((off + len) % bsize), buf + len - ((off + len) % bsize), (off + len) % bsize);	// Write back to the buffer
		
		if (!IDEWriteSectors(bus, drive, 1, end, buff)) {														// Write this block back
			MemFree((UIntPtr)buff);																				// Failed...
			return False;
		}
		
		MemFree((UIntPtr)buff);
		
		if (end != 0) {																							// Only decrease the end if it isn't 0
			end--;
		}
	}
	
	if (cur < end) {																							// Let's write!
		if (!IDEWriteSectors(bus, drive, end - cur, cur, buf + (off % bsize))) {
			return False;																						// Failed...
		}
	}
	
	return True;
}

Boolean IDEDeviceControl(PDevice dev, UIntPtr cmd, PUInt8 ibuf, PUInt8 obuf) {
	(Void)dev; (Void)ibuf;																						// Avoid compiler's unused parameter warning
	
	UInt8 bus = (((UInt32)dev->priv) >> 8) & 0xFF;
	UInt8 drive = ((UInt32)dev->priv) & 0xFF;
	PUIntPtr out = (PUIntPtr)obuf;
	
	if (cmd == 0) {																								// Get block size?
		*out = IDEGetBlockSize(bus, drive);
	} else {
		return False;																							// ...
	}
	
	return True;
}

Void IDEInit(Void) {
	PortOutByte(0x1F0 + ATA_REG_CONTROL, 2);																	// Disable IRQs
	PortOutByte(0x170 + ATA_REG_CONTROL, 2);
	
	IDTRegisterIRQHandler(0x0E, IDEHandler);																	// Register our IRQ handler
	IDTRegisterIRQHandler(0x0F, IDEHandler);
	
	for (UInt32 i = 0; i < 2; i++) {																			// Init our IDE devices
		for (UInt32 j = 0; j < 2; j++) {
			IDEInitializeInt(i, j);
		}
	}
	
	for (UInt32 i = 0, hdc = 0, cdc = 0; i < 2; i++) {															// And let's add them (the valid ones) to the device list!
		for (UInt32 j = 0; j < 2; j++) {
			if (IDEDevices[(i * 2) + j].valid) {																// Valid?
				PChar name = Null;																				// Yes, let's allocate memory for the name
				PVoid devbd = (PVoid)((i << 8) | j);
				
				if (IDEDevices[(i * 2) + j].atapi) {															// ATAPI?
					name = StrDuplicate(IDECDROMString);														// Yes, so duplicate the CdRomX string
					
					if (name == Null) {																			// Failed?
						DbgWriteFormated("[x86] Failed to add CdRom%d device\r\n", cdc++);						// Yes...
						goto next;
					}
					
					name[5] = (Char)(cdc++ + '0');																// And set the num
				} else {
					name = StrDuplicate(IDEHardDiskString);														// No, so duplicate the HardDiskX string
					
					if (name == Null) {																			// Failed?
						DbgWriteFormated("[x86] Failed to add HardDisk%d device\r\n", hdc++);					// Yes...
						goto next;
					}
					
					name[8] = (Char)(hdc++ + '0');																// And set the num
				}
				
				if (!FsAddDevice(name, devbd, IDEDeviceRead, IDEDeviceWrite, IDEDeviceControl)) {				// At end try to add us to the device list!
					DbgWriteFormated("[x86] Failed to add %s device\r\n", name);
					MemFree((UIntPtr)name);
				}
				
next:			;
			}
		}
	}
}
