// rdpfuzz-agent.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "rdpfuzz-agent.h"
#include "Windows.h"
#include <codecvt>
#include "time.h"
#include "VirtualChannel.h"

#include <string>
#include <fstream>
#include <streambuf>

int read_arguments(int argc, char * argv[], FuzzAgentData *data)
{
	int i;
	bool file_name_found = false;
	bool channel_name_found = false;
	bool msg_types_found = false;
	bool dynamic = false;

	std::string msgs_string;
	std::ifstream * msgs_file;


	// default values
	data->command = run;
	data->target = target_rdp_gfx;

	data->need_sleep = true;

	data->server_side = false;

	for (i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			switch (argv[i][1])
			{
			case 'f':
				i++;
				if (i == argc)
				{
					return bad_args;
				}

				data->file_name = argv[i];
				file_name_found = true;

				data->fuzz_directory = data->file_name.substr(0, data->file_name.find_last_of("\\"));
				data->fuzz_id = data->fuzz_directory.substr(data->fuzz_directory.find_last_of("\\") + 1);

				break;

			case 'd':
				dynamic = true;
				break;

			case 'c':
				i++;
				if (i == argc)
				{
					return bad_args;
				}

				data->channel_name = argv[i];
				channel_name_found = true;

				break;

			case 't':
				// get the target of the fuzzing
				i++;
				if (i == argc)
				{
					return bad_args;
				}
				if (_memicmp(argv[i], "GFX", strlen("GFX") + 1) == 0)
				{
					data->target = target_rdp_gfx;
				}
				else if (_memicmp(argv[i], "RDPDR", strlen("RDPDR") + 1) == 0)
				{
					data->target = target_rdp_dr;
				}
				else if (_memicmp(argv[i], "AUDIO", strlen("AUDIO") + 1) == 0)
				{
					data->target = target_rdp_audio;
				}
				else if (_memicmp(argv[i], "GEOMETRY", strlen("GEOMETRY") + 1) == 0)
				{
					data->target = target_rdp_geometry;
				}
				else if (_memicmp(argv[i], "VIDEO", strlen("VIDEO") + 1) == 0)
				{
					data->target = target_rdp_video;
				}
				else if (_memicmp(argv[i], "DWMPROX", strlen("DWMPROX") + 1) == 0)
				{
					data->target = target_rdp_dwmprox;
					printf("type is DWMPROX\r\n");
				}
				else if (_memicmp(argv[i], "SERVER_GFX", strlen("SERVER_GFX") + 1) == 0)
				{
					data->target = target_rdp_server_gfx;
					printf("type is SERVER_GFX\r\n");
				}
				else if (_memicmp(argv[i], "SERVER_INPUT", strlen("SERVER_INPUT") + 1) == 0)
				{
					data->target = target_rdp_server_input;
					printf("type is SERVER_INPUT\r\n");
				}
				else if (_memicmp(argv[i], "SERVER_USB", strlen("SERVER_USB") + 1) == 0)
				{
					data->target = target_rdp_server_usb;
					printf("type is SERVER_USB\r\n");
				}
				else if (_memicmp(argv[i], "SERVER", strlen("SERVER") + 1) == 0)
				{
					data->target = target_rdp_server;
					printf("type is SERVER\r\n");
				}
				else if (_memicmp(argv[i], "CLIENT", strlen("CLIENT") + 1) == 0)
				{
					data->target = target_rdp_client;
					printf("type is CLIENT\r\n");
				}
				else if (_memicmp(argv[i], "SERVER_RDPDR", strlen("SERVER_RDPDR") + 1) == 0)
				{
					data->target = target_rdp_server_rdpdr;
					printf("type is RDPDR\r\n");
				}
				else
				{
					return bad_args;
				}
				break;

			case 's':
				data->command = seed;
				break;

			case 'm':
				i++;
				if (i == argc)
				{
					return bad_args;
				}

				msgs_file = new std::ifstream(argv[i]);

				msgs_string.assign((std::istreambuf_iterator<char>(*msgs_file)),
					(std::istreambuf_iterator<char>()));

				delete msgs_file;

				msg_types_found = true;

				break;

			case 'r':
				data->raw_mode = true;

			case 'n':
				data->need_sleep = false;
				break;

			case 'b':
				data->server_side = true;
				break;

			case 'o':
				i++;
				if (i == argc)
				{
					return bad_args;
				}

				data->output_file = argv[i];
				break;

			default:
				break;
			}
		}
	}

	if (data->target == target_rdp_gfx)
	{
		// Create the msgs object
		data->msg_types = new CGfxMsgs(msgs_string);

		// Create the channel object
		data->sender = dynamic_cast<CSender *>(new CGfxVirtualChannel());
	}
	else if (data->target == target_rdp_audio)
	{
		// Create the msgs object
		data->msg_types = new CAudioMsgs(msgs_string);

		// Create the channel object
		data->sender = dynamic_cast<CSender *>(new CAudioVirtualChannel());
	}
	else if (data->target == target_rdp_geometry)
	{
		// Create the msgs object
		data->msg_types = new CGeometryMsgs(msgs_string);

		// Create the channel object
		data->sender = dynamic_cast<CSender *>(new CGeometryVirtualChannel());
	}
	else if (data->target == target_rdp_video)
	{
		// Create the msgs object
		data->msg_types = new CVideoMsgs(msgs_string);

		// Create the channel object
		data->sender = dynamic_cast<CSender *>(new CVideoVirtualChannel());
	}
	else if (data->target == target_rdp_dwmprox)
	{
		// Create the msgs object
		data->msg_types = new CdwmproxMsgs(msgs_string);

		// Create the channel object
		data->sender = dynamic_cast<CSender *>(new CdwmproxVirtualChannel());
	}
	else if (data->target == target_rdp_dr)
	{
		// Create the msgs object
		data->msg_types = new CServerRdpdrMsgs(msgs_string);

		// Create the channel object
		data->sender = dynamic_cast<CSender *>(new CRdpdrVirtualChannel(data->channel_name, dynamic));
	}
	else if (data->target == target_rdp_server)
	{
		// Create the msgs object
		data->msg_types = new CServerMsgs(msgs_string);

		// Create the channel object
		data->sender = dynamic_cast<CSender *>(new CServerVirtualChannel(data->output_file));
	}
	else if (data->target == target_rdp_client)
	{
		// Create the msgs object
		data->msg_types = new CClientMsgs(msgs_string);

		// Create the channel object
		data->sender = dynamic_cast<CSender *>(new CClientDynamicVirtualChannel(data->channel_name));
	}

	return success;
}

int main(int argc, char * argv[])
{
	int rc = success;
	FuzzAgentData fuzz_data;
	std::string command_line;
	std::vector<std::string> names;

	char log_path[MAX_PATH];

	sprintf_s(log_path, MAX_PATH, "C:\\RDPFuzz\\Logs\\rdpfuzz-agent-%ld-log.txt", GetCurrentProcessId());
	fuzz_data.logger = new CLogger(log_path);

	rc = read_arguments(argc, argv, &fuzz_data);
	if (rc != success)
	{
		return rc;
	}

	CHAR event_name[1000];
	char *id_string = (char *)strstr(fuzz_data.file_name.c_str(), "input\\") + 6;

	// create the event used to synchronize afl-fuzz with the agent
	sprintf_s(event_name, 1000, "Global\\case_event_fuzz%s", id_string);
	event_name[24] = 0;
	fuzz_data.logger->log(event_name);
	fuzz_data.logger->log("\r\n");

	fuzz_data.case_event = CreateEventA(NULL, 0, 0, event_name);
	if (fuzz_data.case_event == NULL)
	{
		sprintf_s(fuzz_data.log_buffer, 0x1000, "failed to create event %s last_error=%08x\r\n", event_name, GetLastError());
		fuzz_data.logger->log(fuzz_data.log_buffer);
	}
	else
	{
		sprintf_s(fuzz_data.log_buffer, 0x1000, "created event %s\r\n", event_name);
		fuzz_data.logger->log(fuzz_data.log_buffer);
	}

	switch (fuzz_data.command)
	{
	case run:
	default:
		rc = send_messages(&fuzz_data);
		break;
	}

	if (fuzz_data.sender)
	{
		delete fuzz_data.sender;
	}

	return rc;
}

int send_messages(FuzzAgentData *fuzz_data)
{
	bool file_rc = true;
	HANDLE file_handle;

	bool first = true;
	bool stop = false;

	DWORD test_counter = 0;

	bool msg_valid = false;

	DWORD fuzz_iteration = 0;

	// allocate raw msg buffer
	fuzz_data->raw_msg.msg = (PUCHAR)malloc(MAX_FILE_BUFFER_SIZE);
	if (fuzz_data->raw_msg.msg == NULL)
	{
		fuzz_data->logger->log("failed to allocate raw msg buffer\r\n");
		return insufficient_memory;
	}
	fuzz_data->logger->log("raw msg buffer allocated\r\n");

	// allocate protocol msg buffer
	fuzz_data->protocol_msg.msg = (PUCHAR)malloc(MAX_FILE_BUFFER_SIZE);
	if (fuzz_data->protocol_msg.msg == NULL)
	{
		fuzz_data->logger->log("failed to allocate protocol msg buffer\r\n");
		return insufficient_memory;
	}
	fuzz_data->logger->log("raw msg buffer allocated\r\n");

	fuzz_data->msgs = new std::vector<FORMAT_MSG>;

	// main loop - periodically read data from file and send over the channel
	while (!stop)
	{
		Sleep(1);

		// wait until afl-fuzz wakes us up
		WaitForSingleObject(fuzz_data->case_event, INFINITE);

		fuzz_iteration++;

		// to do - this is only true for RDP - move to sender (treat_first_case function) 
		if (first)
		{
			first = false;
			Sleep(STARTUP_SLEEP);
			fuzz_data->logger->log("after first sleep\r\n");
		}

		do
		{
			// open fuzz input file 
			file_handle = CreateFileA(fuzz_data->file_name.c_str(), FILE_ALL_ACCESS, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (file_handle == INVALID_HANDLE_VALUE)
			{
				Sleep(1);
			}
			else
			{
				break;
			}
		} while (file_handle == INVALID_HANDLE_VALUE);

		// delete old data
		memset(fuzz_data->raw_msg.msg, 0, MAX_FILE_BUFFER_SIZE);

		// read fraw fuzz input
		file_rc = ReadFile(file_handle, fuzz_data->raw_msg.msg, MAX_FILE_BUFFER_SIZE, &fuzz_data->raw_msg.msg_size, NULL);
		CloseHandle(file_handle);

		if (file_rc == false)
		{
			sprintf_s(fuzz_data->log_buffer, 0x1000, "failed to read file - continue last_error=%08x\r\n", GetLastError());
			fuzz_data->logger->log(fuzz_data->log_buffer);
			continue;
		}

		// check if we are told to shut down
		if (strcmp((PCHAR)&fuzz_data->raw_msg.msg, SHUTDOWN_MESSAGE) == 0)
		{
			stop = true;
		}
		else
		{
			// this is it! we have a real input to send :)

			fuzz_data->msgs[0].clear();

			// transform the fuzzer msg to a protocol valid one
			if (fuzz_data->raw_mode)
			{
				// just take the raw msg as is
				memcpy(fuzz_data->protocol_msg.msg, fuzz_data->raw_msg.msg, fuzz_data->raw_msg.msg_size);
				fuzz_data->protocol_msg.msg_size = fuzz_data->raw_msg.msg_size;

				sprintf_s(fuzz_data->log_buffer, 0x1000, "[%ld] z=%ld data=\r\n",
					fuzz_iteration, fuzz_data->protocol_msg.msg_size);

				fuzz_data->logger->log(fuzz_data->log_buffer);
				fuzz_data->logger->log(fuzz_data->protocol_msg.msg, fuzz_data->protocol_msg.msg_size);

				// send data over channel
				if (fuzz_data->sender->write(fuzz_data->protocol_msg.msg, fuzz_data->protocol_msg.msg_size) == false)
				{
					fuzz_data->logger->log("not sent\r\n");
				}
				else
				{
					fuzz_data->logger->log("sent\r\n");
				}
			}
			else
			{
				// let the sender reset if needed in every iteration
				fuzz_data->sender->reset();

				// split the fuzzer input into the protocol messages
				fuzz_data->msg_count = fuzz_data->msg_types->split((PCHAR)fuzz_data->raw_msg.msg, fuzz_data->raw_msg.msg_size, fuzz_data->msgs);

				// for each message enforce protocol rules and send
				for (DWORD msg_index = 0; msg_index < fuzz_data->msg_count; msg_index++)
				{
					// translate the fuzzer format to the protocol format
					fuzz_data->protocol_msg.msg_size = fuzz_data->msg_types->fix((PCHAR)fuzz_data->msgs[0][msg_index].msg, fuzz_data->msgs[0][msg_index].msg_size, (PCHAR)fuzz_data->protocol_msg.msg);

					// wrap - compress, add flags, etc.
					fuzz_data->protocol_msg.msg_size = fuzz_data->msg_types->wrap((PCHAR)fuzz_data->protocol_msg.msg, fuzz_data->protocol_msg.msg_size);

					// add the marker - let the fuzzer know this is our message
					fuzz_data->msg_types->add_marker((PCHAR)fuzz_data->protocol_msg.msg, fuzz_data->protocol_msg.msg_size, msg_index == (fuzz_data->msg_count - 1));

					if (fuzz_data->protocol_msg.msg_size)
					{
						sprintf_s(fuzz_data->log_buffer, 0x1000, "[%ld] z=%ld data=\r\n",
							fuzz_iteration, fuzz_data->protocol_msg.msg_size);

						fuzz_data->logger->log(fuzz_data->log_buffer);
						fuzz_data->logger->log(fuzz_data->protocol_msg.msg, fuzz_data->protocol_msg.msg_size);

						// send data over channel
						if (fuzz_data->sender->write(fuzz_data->protocol_msg.msg, fuzz_data->protocol_msg.msg_size) == false)
						{
							fuzz_data->logger->log("not sent\r\n");
						}
						else
						{
							fuzz_data->logger->log("sent\r\n");
						}
					}
					else
					{
						fuzz_data->logger->log("format_message failed\r\n");
					}

					free(fuzz_data->msgs[0][msg_index].msg);
				}
			}
		}
	}

	if (fuzz_data->raw_msg.msg)
	{
		free(fuzz_data->raw_msg.msg);
	}

	fuzz_data->sender->disconnect();

	return success;
}
