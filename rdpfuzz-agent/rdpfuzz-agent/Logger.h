#pragma once

#include "Windows.h"
#include <string>

#define MAX_LOG_FILE_SIZE				100000000

class CLogger
{
public:
	CLogger(std::string file_name, bool _to_screen = false, bool _log_off = false)
	{
		to_screen = _to_screen;
		log_off = _log_off;
		name = file_name;
		handle = NULL;
	}

	~CLogger() { }

	void log(std::string msg)
	{
		DWORD bytes_written;
		CHAR tick_count[16];
		DWORD file_size;
		DWORD file_size_high;

		if (log_off)
		{
			return;
		}

		sprintf_s(tick_count, 16, "%lld: ", GetTickCount64());


		std::string msg_to_print = tick_count;
		msg_to_print.append(msg);

		if (to_screen)
		{
			printf(msg_to_print.c_str());
		}
		else
		{
			handle = CreateFileA(name.c_str(), FILE_ALL_ACCESS, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if (handle != INVALID_HANDLE_VALUE)
			{
				file_size = GetFileSize(handle, &file_size_high);
				if (file_size > MAX_LOG_FILE_SIZE)
				{
					CloseHandle(handle);
					handle = CreateFileA(name.c_str(), FILE_ALL_ACCESS, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				}

				if (handle != INVALID_HANDLE_VALUE)
				{
					SetFilePointer(handle, 0, NULL, FILE_END);
					WriteFile(handle, (PVOID)msg_to_print.c_str(), (DWORD)msg_to_print.length(), &bytes_written, NULL);

					CloseHandle(handle);
				}
			}
		}
	}

	void log(PUCHAR data, DWORD size)
	{
		DWORD i;
		char current_byte[16];
		std::string msg = "";

		if (log_off)
		{
			return;
		}

		for (i = 0; i < size; i++)
		{
			sprintf_s(current_byte, 16, "%02x", data[i]);
			msg.append(current_byte);
			if ((i + 1) % 16 == 0)
			{
				msg.append("\r\n");
			}
			else if ((i + 1) % 8 == 0)
			{
				msg.append("  ");
			}
			else
			{
				msg.append(" ");
			}
		}

		log(msg);
	}

	void log(const char * format, va_list args)
	{
		char msg[0x1000];

		sprintf_s(msg, 0x1000, format, args);

		log(msg);
	}

private:
	std::string name;
	HANDLE handle;

	bool log_off;
	bool to_screen;
};
