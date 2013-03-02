// FTDI2SPI.h

#pragma once

using namespace System;

namespace FTDI2SPI {
	public delegate void StatusDelegate(Object^ sender, StatusEventArgs^ e);
	
	public ref class Class1
	{
	public:
		static event StatusDelegate^ StatusUpdate;
	};

	public ref class StatusEventArgs : public EventArgs
	{
	public:
		StatusEventArgs(String^ msg, int progress) 
		{
			this -> Status = msg; 
			this -> Progress = progress;
		}
		String^ Status;
		int Progress;
	};
}
