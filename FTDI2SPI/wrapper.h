#pragma once
#define MAX_NUM_BYTES_USB_WRITE			4096
#define MAX_READ_DATA_WORDS_BUFFER_SIZE 65536    // 64k bytes
#define MAX_FREQ_CLOCK_DIVISOR			1
#define CHIP_SELECT_PIN					0x08

const BYTE SET_LOW_BYTE_DATA_BITS_CMD					= 0x80;
const BYTE SET_HIGH_BYTE_DATA_BITS_CMD					= 0x82;
const BYTE SEND_ANSWER_BACK_IMMEDIATELY_CMD				= 0x87;
const BYTE CLK_DATA_BYTES_OUT_ON_NEG_CLK_LSB_FIRST_CMD	= 0x19;
const BYTE CLK_DATA_BITS_OUT_ON_NEG_CLK_LSB_FIRST_CMD	= 0x1B;
const BYTE CLK_DATA_BYTES_IN_ON_NEG_CLK_LSB_FIRST_CMD	= 0x2D;
const BYTE CLK_DATA_BITS_IN_ON_NEG_CLK_LSB_FIRST_CMD	= 0x2F;

void Wrapper_ClearOutputBuffer();
void Wrapper_AddByteToOutputBuffer(BYTE DataByte, bool bClearOutputBuffer);
void Wrapper_AddWriteOutBuffer(DWORD dwNumControlBitsToWrite, unsigned char pWriteControlBuffer[]);
void Wrapper_AddReadOutBuffer(DWORD dwNumDataBitsToRead);
void Wrapper_SendbytesToDevice();
void Wrapper_GetDataFromDevice(unsigned int dwNumBytesToRead, unsigned char ReadDataBuffer[]);
bool Wrapper_InitDevice();
void Wrapper_SetAnswerFast();
void Wrapper_DisableSPIChip();
void Wrapper_EnableSpiChip();
void Wrapper_SetCS(bool ChipSelect);
void Wrapper_SetGPIO(bool xxLO, bool ejLO);