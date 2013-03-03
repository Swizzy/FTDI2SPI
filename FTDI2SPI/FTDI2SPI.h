// FTDI2SPI.h

#pragma once

using namespace System;
namespace FTDI2SPI {
#pragma region EventArgs
	public ref class StatusEventArgs : public EventArgs
	{
	public:
		StatusEventArgs(String^ msg, int progress, int maximum) 
		{
			this -> Status = msg; 
			this -> Progress = progress;
			this -> Max = maximum;
		}
		String^ Status;
		int Progress;
		int Max;
	};

	public ref class ErrorMsgArgs : public EventArgs
	{
	public:
		ErrorMsgArgs(String^ msg, unsigned int status)
		{
			this -> Message = msg;
			this -> Status = status;
		}
		String^ Message;
		unsigned int Status;
	};
#pragma endregion

	public ref class OutputEvents
	{
	public:
		delegate void StatusDelegate(Object^ sender, StatusEventArgs^ e);
		delegate void ErrorDelegate(Object^ sender, ErrorMsgArgs^ e);

		static event StatusDelegate^ StatusUpdate;
		static event ErrorDelegate^ ErrorUpdate;

		static void SendStatus(int progress, int max);
		static void SendError(int block, unsigned int status, String^ type);
	};

	public ref class Logger
	{
	public:
		static bool Enabled = false;
		static String^ LogPath;
	internal:
		static void WriteLine(String^ entry);
		static void Write(String^ entry);
		static void WriteLogHead();
	private:
		static String^ DefaultName = "debug.log";
	};

	public ref class XNAND
	{
	public:
		static bool InitDevice();
		static void StartRead(XNANDSettings args);
		static void StartWrite(XNANDSettings args);
		static void StartErase(XNANDSettings args);
		static void RequestAbort();
		static XNANDInfo^ GetInfo();
	private:
		static bool Initalized = false;
		static bool Abort = false;
		static XNANDInfo^ Info;
		static System::Diagnostics::Stopwatch^ swatch = gcnew System::Diagnostics::Stopwatch();
	};

	public ref class XNANDSettings
	{
	public:
		XNANDSettings(String^ filename, unsigned int pages, unsigned int start, unsigned int count, unsigned int writemode, bool erasewrite)
		{
			this -> File = filename;
			this -> TotalPages = pages;
			this -> StartBlock = start;
			this -> BlockCount = count;
			this -> WriteMode = writemode;
			this -> EraseBeforeWrite = erasewrite;
		}
		XNANDSettings(String^ filename, unsigned int pages, unsigned int start, unsigned int count)
		{
			XNANDSettings(filename, pages, start, count, XNANDEnums::Raw, true);
		}
		String^ File;
		unsigned int TotalPages;
		unsigned int StartBlock;
		unsigned int BlockCount;
		unsigned int WriteMode;
		bool EraseBeforeWrite;
	};

	public ref class XNANDEnums
	{
	public:
		enum {
			Raw,
			Glitch
		}WriteModes;
		enum {
			Auto = 0,
			SB16 = 0x8000,
			BB64 = 0x20000,
			BB256 = 0x80000,
			BB512 = 0x100000,
		}PageCounts;
	};

	public ref class XNANDInfo
	{
	public:
		int MetaType;
		int PageSz;
		int MetaSz;
		int PageSzPhys;
		int PagesInBlock;
		int BlockSz;
		int BlockSzPhys;
		int SizeMB;
		int SizeBytes;
		int SizeBytesPhys;
		int SizePages;
		int SizeBlocks;
		int BlocksPerLgBlock;
		int SizeUsableFs;
		int AddrConfig;
		int LenConfig;
		unsigned int Config;
		int Startblock;
		int Lastblock;
	};
}