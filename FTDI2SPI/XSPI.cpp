#include "stdafx.h"
#include "wrapper.h"
#include "XSPI.h"

void XSPI_EnterFlashMode()
{
	Wrapper_SetGPIO(false, true);
	Wrapper_SetCS(true);
	Sleep(35);
	Wrapper_SetGPIO(false, false);
	Wrapper_SetCS(false);
	Sleep(35);
	Wrapper_SetGPIO(true, true);
	Sleep(35);
}

void XSPI_ReadSendReceive(DWORD len, unsigned char* data)
{
	Wrapper_SetAnswerFast();
	Wrapper_SendBytesToDevice();
	Wrapper_GetDataFromDevice(len, data);
}

void XSPI_Read(unsigned char reg, unsigned char* data, DWORD len, DWORD rlen, bool clear, bool sendReceive)
{
	unsigned char wbuf[2] = { (reg <<2) | 1, 0xFF };
	if (clear)
		Wrapper_ClearOutputBuffer();
	Wrapper_EnableSpiChip();
	Wrapper_AddWriteOutBuffer(0x10, wbuf);
	Wrapper_AddReadOutBuffer(rlen);
	Wrapper_DisableSPIChip();
	if (sendReceive)
		XSPI_ReadSendReceive(len, data);
}

void XSPI_ReadSync(unsigned char reg, unsigned char* data, DWORD len, bool SendReceive)
{
	XSPI_Read(reg, data, len, 0x20, true, SendReceive);
}

unsigned int XSPI_ReadWord(unsigned char reg, bool SendReceive)
{
	unsigned char data[2] = { 0, 0};
	XSPI_Read(reg, data, 2, 0x10, true, SendReceive);
	return data[0] | ((unsigned int)data[1]<<8);
}

unsigned char XSPI_ReadByte(unsigned char reg, bool SendReceive)
{
	unsigned char data;
	XSPI_Read(reg, &data, 1, 0x8, true, SendReceive);
	return data;
}

void XSPI_Write(unsigned char reg, XSPI_WriteData data, bool clear, bool send)
{
	if (data == NULL)
		return;
	unsigned char wbuf[5] = { (reg << 2) | 2, 0, 0, 0, 0 };
	memcpy(&wbuf[1], data, 4);
	if (clear)
		Wrapper_ClearOutputBuffer();
	Wrapper_EnableSpiChip();
	Wrapper_AddWriteOutBuffer(0x28, wbuf);
	Wrapper_DisableSPIChip();
	if (send)
		Wrapper_SendBytesToDevice();
}

void XSPI_WriteWord(unsigned char reg, unsigned int data, bool clear)
{
	XSPI_WriteData tmp = { 0, 0, 0, 0 };
	memcpy(tmp, &data, 4);
	XSPI_Write(reg, tmp, clear);
}

void XSPI_WriteByte(unsigned char reg, unsigned char data, bool send)
{
	XSPI_WriteData tmp = { data, 0, 0, 0 };
	XSPI_Write(reg, tmp, false, send);
}

void XSPI_WriteReg(unsigned char reg, bool clear, bool send)
{
	XSPI_WriteData tmp = { 0, 0, 0, 0 };
	XSPI_Write(reg, tmp, clear, send);
}