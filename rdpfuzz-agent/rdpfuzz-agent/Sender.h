#pragma once
#include "windows.h"

// this class connects to the target channel and sends the protocol valid messages on it
class CSender
{
public:
	virtual bool connect() = 0;
	virtual void disconnect() = 0;
	virtual bool reconnect(bool sleep = true) = 0;
	virtual bool read() = 0;
	virtual bool write(PVOID data, DWORD data_size) = 0;
	virtual bool reset()
	{
		{
			return true;
		}
	}
};