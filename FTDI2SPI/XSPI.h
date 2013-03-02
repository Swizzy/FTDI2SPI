#pragma once
#include "stdafx.h"

typedef unsigned char XSPI_WriteData[4];

void XSPI_EnterFlashMode();

void XSPI_ReadSendReceive(DWORD len, unsigned char* data);
void XSPI_Read(unsigned char reg, unsigned char* data, DWORD len, DWORD rlen = 0x20, bool clear = false, bool sendReceive = true);
void XSPI_ReadSync(unsigned char reg, unsigned char* data, DWORD len, bool SendReceive = true);
unsigned int XSPI_ReadWord(unsigned char reg, bool SendReceive = true);
unsigned char XSPI_Readbyte(unsigned char reg, bool SendReceive = true);

void XSPI_Write(unsigned char reg, XSPI_WriteData data, bool clear = true, bool send = true);
void XSPI_WriteWord(unsigned char reg, unsigned int data, bool clear = true);
void XSPI_Writebyte(unsigned char reg, unsigned char data, bool send = false);
void XSPI_WriteReg(unsigned char reg, bool clear = false, bool send = false);