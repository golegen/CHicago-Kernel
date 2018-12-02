// File author is Ítalo Lima Marconato Matias
//
// Created on May 11 of 2018, at 23:50 BRT
// Last edited on October 27 of 2018, at 15:28 BRT

#include <chicago/arch/port.h>
#include <chicago/arch/serial.h>

Void DbgWriteCharacter(Char data) {
	while ((PortInByte(COM1_PORT + 5) & 0x20) == 0) ;
	PortOutByte(COM1_PORT, data);
}

Void SerialInit(UInt16 port) {
	PortOutByte(port + 1, 0x00);								// Disable all interrupts
	PortOutByte(port + 3, 0x80);								// Enable DLAB (set baud rate divisor)
	PortOutByte(port + 0, 0x03);								// Set divisor to 3 (lo byte) 38400
																// baud (hi byte)
	PortOutByte(port + 1, 0x00);
	PortOutByte(port + 3, 0x03);								// 8 bits, no parity, one stop bit
	PortOutByte(port + 2, 0xC7);								// Enable FIFO, clear then with
																// 14-byte threshold
	PortOutByte(port + 4, 0x0B);								// IRQs enables, RTS/DSR set
}
