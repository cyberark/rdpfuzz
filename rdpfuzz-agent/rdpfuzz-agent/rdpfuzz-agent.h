#pragma once
#include "Logger.h"
#include "Sender.h"
#include "MsgStructures.h"

#define LOGS_FOLDER						"C:\\RDPFuzzAgent\\Logs\\"

#define SHUTDOWN_MESSAGE				"you may terminate process, if you so please"

#define MAX_FILE_BUFFER_SIZE			0x1000000u

#define CREATE_SURFACE_MSG				"\x00\x04\x09\x00\x00\x00\x0F\x00\x00\x00\x00\x00\x00\x01\x00\x01\x20"
#define CREATE_SURFACE_MSG_SIZE			17


enum RDPFuzzErrors
{
	success = 0,
	bad_args,
	failed_to_connect,
	insufficient_memory,
};

enum Commands
{
	run = 0,
	seed,
};


enum target_types
{
	target_rdp_gfx,
	target_rdp_dr,
	target_rdp_audio,
	target_rdp_geometry,
	target_rdp_video,
	target_rdp_dwmprox,
	target_rdp_server_gfx,
	target_rdp_server_input,
	target_rdp_server_usb,
	target_rdp_server,
	target_rdp_client,
	target_rdp_server_rdpdr
};


struct FuzzAgentData
{
	std::string file_name;
	std::string channel_name;
	DWORD command;

	std::string fuzz_directory;
	std::string fuzz_id;

	std::string output_file;

	CSender * sender;

	CHAR log_buffer[0x1000];
	CLogger * logger;

	DWORD target = 0;

	FORMAT_MSG raw_msg;
	FORMAT_MSG protocol_msg;

	std::vector<FORMAT_MSG> *msgs;
	DWORD msg_count;

	CMsgs *msg_types;

	DWORD last_caps_sent = 0;

	HANDLE case_event = NULL;

	bool raw_mode = false;

	bool need_sleep;

	bool server_side;
};


int main(int argc, char * argv[]);
int send_messages(FuzzAgentData *fuzz_data);
