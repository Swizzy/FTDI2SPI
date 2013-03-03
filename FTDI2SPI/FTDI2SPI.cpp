// This is the main DLL file.

#include "stdafx.h"

#include "FTDI2SPI.h"
#include "XNAND.h"

namespace FTDI2SPI {
#pragma region Status/Error Output
	void OutputEvents::SendStatus(int progress, int max) {
		StatusEventArgs^ args = gcnew StatusEventArgs(String::Format("Processing block 0x{0:X} of 0x{1:X}", progress, max), progress, max);
		OutputEvents::StatusUpdate(nullptr, args);
	}

	void OutputEvents::SendError(int block, unsigned int status, String^ type) {
		String^ errmsg = String::Format("ERROR 0x{0:X} {1} 0x{2:X}", status, type, block);
		ErrorMsgArgs^ args = gcnew ErrorMsgArgs(errmsg, status);
		Logger::WriteLine(errmsg);
		OutputEvents::ErrorUpdate(nullptr, args);
	}
#pragma endregion

#pragma region Logging
	void Logger::WriteLine(String^ entry) {
		if (!Enabled)
			return;
		Write(String::Format("{0}{1}", entry, Environment::NewLine));
	}

	void Logger::Write(String^ entry) {
		if (!Enabled)
			return;
		try
		{
			if (String::IsNullOrEmpty(LogPath))
				LogPath = String::Format("{0}\\{1}", System::IO::Path::GetDirectoryName(System::Reflection::Assembly::GetCallingAssembly()->FullName), DefaultName);
			System::IO::StreamWriter^ sw = gcnew System::IO::StreamWriter(System::IO::File::Open(LogPath, System::IO::FileMode::Append, System::IO::FileAccess::Write, System::IO::FileShare::ReadWrite));
			if (sw != nullptr)
			{
				sw->Write(entry);
				sw->Dispose();
			}
			GC::Collect();
		}
		catch (Exception^ e) { }
	};

	void Logger::WriteLogHead()
	{
		WriteLine("=========================================================================");
		System::Version^ version = System::Reflection::Assembly::GetExecutingAssembly()->GetName()->Version;
		WriteLine(String::Format("FTDI2SPI v{0}.{1} (Build: {2}) by Swizzy", version->Major, version->Minor, version->Build));
		WriteLine(String::Format("Log started: {0:yyyy-MM-dd HH:mm:ss}", DateTime::Now));
		WriteLine("=========================================================================");
		WriteLine("Any errors will be written below:");
	};
#pragma endregion

#pragma region XNAND
	void XNAND::RequestAbort() {
		XNAND::Abort = true;
	};

	XNANDInfo^ XNAND::GetInfo()	{
		return Info;
	};
	bool XNAND::InitDevice() {
		return XNAND_InitDevice();
	};
	void XNAND::StartRead(XNANDSettings args) {

	};

	void XNAND::StartWrite(XNANDSettings args) {

	};
	void XNAND::StartErase(XNANDSettings args) {

	};

#pragma endregion
}