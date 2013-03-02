#include "stdafx.h"
#include "FTCSPI.h"
#include "FTD2XX.H"
#include "wrapper.h"

FTC_HANDLE ftHandle;  
FTC_STATUS Status;
FTH_HIGHER_OUTPUT_PINS HighPinsWriteActiveStates;
FTC_CHIP_SELECT_PINS ChipSelectsDisableStates;
FTH_INPUT_OUTPUT_PINS HighInputOutputPins;
unsigned char byOutputBuffer[65535];
unsigned char dwLowPinsValue = 0;
unsigned long  dwNumbytesToSend = 0;

void Wrapper_ClearOutputBuffer()
{
	memset(byOutputBuffer, 0, 65535);
	dwNumbytesToSend = 0;
}

void Wrapper_AddbyteToOutputBuffer(unsigned char Databyte, bool bClearOutputBuffer)
{
	if(bClearOutputBuffer)
		Wrapper_ClearOutputBuffer();
	byOutputBuffer[dwNumbytesToSend++] = Databyte;
}

void Wrapper_AddWriteOutBuffer(unsigned long  dwNumControlBitsToWrite, unsigned char pWriteControlBuffer[])
{
	if (dwNumControlBitsToWrite <= 1)
		return;
	unsigned long  dwModNumControlBitsToWrite = dwNumControlBitsToWrite - 1;
	unsigned long  dwNumControlbytes = dwModNumControlBitsToWrite / 8;
	unsigned long  dwControlBufferIndex = 0;	
	if (dwNumControlbytes > 0)
	{
		dwNumControlbytes = dwNumControlbytes - 1;
		Wrapper_AddbyteToOutputBuffer(CLK_DATA_BYTES_OUT_ON_NEG_CLK_LSB_FIRST_CMD, false);
		Wrapper_AddbyteToOutputBuffer(dwNumControlbytes & 0xFF, false);
		Wrapper_AddbyteToOutputBuffer((dwNumControlbytes / 256) & 0xFF, false);
		do
		{
			Wrapper_AddbyteToOutputBuffer(pWriteControlBuffer[dwControlBufferIndex], false);
			dwControlBufferIndex = (dwControlBufferIndex + 1);
		}
		while (dwControlBufferIndex < (dwNumControlbytes + 1));
	}
	unsigned long  dwNumRemainingControlBits = dwModNumControlBitsToWrite % 8;
	if (dwNumRemainingControlBits <= 0)
		return;
	Wrapper_AddbyteToOutputBuffer(CLK_DATA_BITS_OUT_ON_NEG_CLK_LSB_FIRST_CMD, false);
	Wrapper_AddbyteToOutputBuffer(dwNumRemainingControlBits & 0xFF, false);
	Wrapper_AddbyteToOutputBuffer(pWriteControlBuffer[dwControlBufferIndex], false);
}

void Wrapper_AddReadOutBuffer(unsigned long  dwNumDataBitsToRead)
{
	unsigned long  dwModNumBitsToRead = dwNumDataBitsToRead - 1;
	unsigned long  dwNumDatabytes = dwModNumBitsToRead / 8;
	if (dwNumDatabytes > 0)
	{
		Wrapper_AddbyteToOutputBuffer(CLK_DATA_BYTES_IN_ON_NEG_CLK_LSB_FIRST_CMD, false);
		Wrapper_AddbyteToOutputBuffer((dwNumDatabytes & 0xFF), false);
		Wrapper_AddbyteToOutputBuffer(((dwNumDatabytes / 256) & 0xFF), false);
	}
	unsigned long  dwNumRemainingDataBits = dwModNumBitsToRead % 8;
	if (dwNumRemainingDataBits <= 0)
		return;
	Wrapper_AddbyteToOutputBuffer(CLK_DATA_BITS_IN_ON_NEG_CLK_LSB_FIRST_CMD, false);
	Wrapper_AddbyteToOutputBuffer(dwNumRemainingDataBits & 0xFF, false);
}

void Wrapper_SendbytesToDevice()
{
	Status = FTC_SUCCESS;
	unsigned long  dwNumbytesSent = 0;
	unsigned long  dwTotalNumbytesSent = 0;
	if (dwNumbytesToSend > MAX_NUM_BYTES_USB_WRITE)
	{
		unsigned long  dwNumDatabytesToSend = 0;
		do
		{
			if ((dwTotalNumbytesSent + MAX_NUM_BYTES_USB_WRITE) <= dwNumbytesToSend)
				dwNumDatabytesToSend = MAX_NUM_BYTES_USB_WRITE;
			else
				dwNumDatabytesToSend = (dwNumbytesToSend - dwTotalNumbytesSent);
			Status = FT_Write((FT_HANDLE)ftHandle, &byOutputBuffer[dwTotalNumbytesSent], dwNumDatabytesToSend, &dwNumbytesSent);
			dwTotalNumbytesSent = dwTotalNumbytesSent + dwNumbytesSent;
		}
		while ((dwTotalNumbytesSent < dwNumbytesToSend) && (Status == FTC_SUCCESS)); 
	}
	else
		Status = FT_Write((FT_HANDLE)ftHandle, byOutputBuffer, dwNumbytesToSend, &dwNumbytesSent);
	Wrapper_ClearOutputBuffer();
}

void Wrapper_GetDataFromDevice(unsigned int dwNumbytesToRead, unsigned char ReadDataBuffer[])
{
	unsigned long  dwNumbytesRead = 0;
	unsigned long  dwbytesReadIndex = 0;
	int try_count = 10;	
	do {
		FT_Read((FT_HANDLE)ftHandle, &ReadDataBuffer[dwbytesReadIndex], dwNumbytesToRead, &dwNumbytesRead);
		dwbytesReadIndex += dwNumbytesRead;
		dwNumbytesToRead -= dwNumbytesRead;
	} while( dwNumbytesToRead > 0 && try_count-- > 0 );
	if( try_count <= 0 )
		throw "ERROR: NO DATA FROM DEVICE";	
}

bool Wrapper_InitDevice()
{
	unsigned long  dwNumHiSpeedDevices = 0, dwLocationID = 0, dwHiSpeedDeviceType = 0, dwClockFrequencyHz = 0;
	BYTE timerValue = 0;
	char szDeviceName[100];
	char szChannel[5];
	char szDllVersion[10];
	
	SPI_GetDllVersion(szDllVersion, 10);
	Status = SPI_GetNumHiSpeedDevices(&dwNumHiSpeedDevices);
	if ((Status == FTC_SUCCESS) && (dwNumHiSpeedDevices > 0))
	{
		unsigned long  dwHiSpeedDeviceIndex = 0;
		do
		{
			Status = SPI_GetHiSpeedDeviceNameLocIDChannel(dwHiSpeedDeviceIndex, szDeviceName, 100, &dwLocationID, szChannel, 5, &dwHiSpeedDeviceType);
			dwHiSpeedDeviceIndex = dwHiSpeedDeviceIndex + 1;
		}
		while ((Status == FTC_SUCCESS) && (dwHiSpeedDeviceIndex < dwNumHiSpeedDevices) && (strcmp(szChannel, "B") != 0));

		if (Status == FTC_SUCCESS)
		{
			if (strcmp(szChannel, "B") != 0)
				Status = FTC_DEVICE_IN_USE;
		}

		if (Status == FTC_SUCCESS) {
			Status = SPI_OpenHiSpeedDevice(szDeviceName, dwLocationID, szChannel, &ftHandle);
			if (Status == FTC_SUCCESS) {
				Status = SPI_GetHiSpeedDeviceType(ftHandle, &dwHiSpeedDeviceType);

				
			}
		}
	}

	if ((Status == FTC_SUCCESS) && (ftHandle != 0))
	{
		Status = SPI_InitDevice(ftHandle, MAX_FREQ_CLOCK_DIVISOR); //65536

		if (Status == FTC_SUCCESS) {
			if ((Status = SPI_GetDeviceLatencyTimer(ftHandle, &timerValue)) == FTC_SUCCESS) {
				if ((Status = SPI_SetDeviceLatencyTimer(ftHandle, 50)) == FTC_SUCCESS) {
					SPI_GetDeviceLatencyTimer(ftHandle, &timerValue);
					SPI_SetDeviceLatencyTimer(ftHandle, 1);
					SPI_GetDeviceLatencyTimer(ftHandle, &timerValue);
				}
			}
		}

		if (Status == FTC_SUCCESS)
		{
			if ((Status = SPI_GetHiSpeedDeviceClock(0, &dwClockFrequencyHz)) == FTC_SUCCESS)
			{
				if ((Status = SPI_TurnOnDivideByFiveClockingHiSpeedDevice(ftHandle)) == FTC_SUCCESS)
				{
					Status = SPI_GetHiSpeedDeviceClock(0, &dwClockFrequencyHz);

					if ((Status = SPI_SetClock(ftHandle, MAX_FREQ_CLOCK_DIVISOR, &dwClockFrequencyHz)) == FTC_SUCCESS)
					{
						if ((Status = SPI_TurnOffDivideByFiveClockingHiSpeedDevice(ftHandle)) == FTC_SUCCESS)
							Status = SPI_SetClock(ftHandle, MAX_FREQ_CLOCK_DIVISOR, &dwClockFrequencyHz);
					}
				}
			}
		}
		if (Status == FTC_SUCCESS)
		{
			// Must set the chip select disable states for all the SPI devices connected to a FT2232H hi-speed dual
			// device or FT4332H hi-speed quad device
			ChipSelectsDisableStates.bADBUS3ChipSelectPinState = true;
			ChipSelectsDisableStates.bADBUS4GPIOL1PinState = false;
			ChipSelectsDisableStates.bADBUS5GPIOL2PinState = false;
			ChipSelectsDisableStates.bADBUS6GPIOL3PinState = false;
			ChipSelectsDisableStates.bADBUS7GPIOL4PinState = false;
			
			HighInputOutputPins.bPin1InputOutputState = true;
			HighInputOutputPins.bPin1LowHighState = false;
			HighInputOutputPins.bPin2InputOutputState = true;
			HighInputOutputPins.bPin2LowHighState = true;
			HighInputOutputPins.bPin3InputOutputState = false;
			HighInputOutputPins.bPin3LowHighState = false;
			HighInputOutputPins.bPin4InputOutputState = false;
			HighInputOutputPins.bPin4LowHighState = false;
			HighInputOutputPins.bPin5InputOutputState = false;
			HighInputOutputPins.bPin5LowHighState = false;
			HighInputOutputPins.bPin6InputOutputState = false;
			HighInputOutputPins.bPin6LowHighState = false;
			HighInputOutputPins.bPin7InputOutputState = false;
			HighInputOutputPins.bPin7LowHighState = false;
			HighInputOutputPins.bPin8InputOutputState = false;
			HighInputOutputPins.bPin8LowHighState = false;

			Status = SPI_SetHiSpeedDeviceGPIOs(ftHandle, &ChipSelectsDisableStates, &HighInputOutputPins); //NULL
		}        
	}

	// Set the 8 general purpose higher input/output pins
	HighPinsWriteActiveStates.bPin1ActiveState = true;
	HighPinsWriteActiveStates.bPin1State = true;
	HighPinsWriteActiveStates.bPin2ActiveState = true;
	HighPinsWriteActiveStates.bPin2State = true;
	HighPinsWriteActiveStates.bPin3ActiveState = false;
	HighPinsWriteActiveStates.bPin3State = false;
	HighPinsWriteActiveStates.bPin4ActiveState = false;
	HighPinsWriteActiveStates.bPin4State = false;
	HighPinsWriteActiveStates.bPin5ActiveState = false;
	HighPinsWriteActiveStates.bPin5State = false;
	HighPinsWriteActiveStates.bPin6ActiveState = false;
	HighPinsWriteActiveStates.bPin6State = false;
	HighPinsWriteActiveStates.bPin7ActiveState = false;
	HighPinsWriteActiveStates.bPin7State = false;
	HighPinsWriteActiveStates.bPin8ActiveState = false;
	HighPinsWriteActiveStates.bPin8State = false;
	return (Status == FTC_SUCCESS && dwNumHiSpeedDevices > 0 ) ? true : false;
}

void Wrapper_SetAnswerFast()
{
	Wrapper_AddbyteToOutputBuffer(SEND_ANSWER_BACK_IMMEDIATELY_CMD, false);	
}

void Wrapper_DisableSPIChip()
{
	Wrapper_AddbyteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);
	dwLowPinsValue |= CHIP_SELECT_PIN;
	Wrapper_AddbyteToOutputBuffer(dwLowPinsValue, false);
	Wrapper_AddbyteToOutputBuffer(0xFB, false);
}

void Wrapper_EnableSpiChip()
{
	Wrapper_AddbyteToOutputBuffer(SET_LOW_BYTE_DATA_BITS_CMD, false);
	dwLowPinsValue &= ~CHIP_SELECT_PIN;
	Wrapper_AddbyteToOutputBuffer(dwLowPinsValue, false);
	Wrapper_AddbyteToOutputBuffer(0xFB, false);
}

void Wrapper_SetCS(bool ChipSelect)
{
	Wrapper_ClearOutputBuffer();
	unsigned long  dwNumbytesSent = 0;
	byOutputBuffer[dwNumbytesToSend++] = 0x80;
	dwLowPinsValue &= ~0x08;
	dwLowPinsValue |= ChipSelect ? 0x08 : 0x00;
	byOutputBuffer[dwNumbytesToSend++] = dwLowPinsValue;
	byOutputBuffer[dwNumbytesToSend++] = 0x3E;
	FT_STATUS ftStatus = FT_Write( (FT_HANDLE)ftHandle, byOutputBuffer, dwNumbytesToSend, &dwNumbytesSent);
	Wrapper_ClearOutputBuffer();
}

void Wrapper_SetGPIO(bool xxLO, bool ejLO)
{
	Wrapper_ClearOutputBuffer();
	unsigned long  dwNumbytesSent = 0;
	byOutputBuffer[dwNumbytesToSend++] = 0x80;
	dwLowPinsValue &= ~0x30;
	dwLowPinsValue |= (xxLO ? 0x10 : 0x00) | (ejLO ? 0x20 : 0x00);
	byOutputBuffer[dwNumbytesToSend++] = dwLowPinsValue; 
	byOutputBuffer[dwNumbytesToSend++] = 0x3E;
	FT_STATUS ftStatus = FT_Write( (FT_HANDLE)ftHandle, byOutputBuffer, dwNumbytesToSend, &dwNumbytesSent);
	Wrapper_ClearOutputBuffer();
}