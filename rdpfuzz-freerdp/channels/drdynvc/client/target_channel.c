#include <channels/drdynvc/client/target_channel.h>
#include <sys/stat.h>
#include <unistd.h>

#include <errno.h>

#include <winpr/crt.h>
#include <winpr/stream.h>

#include "drdynvc_main.h"

xf_fuzzer_params fuzzer_params;


void read_fuzzer_params_from_file()
{
	// currently just hardcoded, change channel_name for other channels
	strcpy(fuzzer_params.channel_name, "AUDIO_INPUT");
	strcpy(fuzzer_params.input_file_name, "/mnt/rdpfuzz/input/01/fuzz_file_to_send");
}

void load_fuzzer_parameters()
{
	read_fuzzer_params_from_file();
	fuzzer_params.loaded_fuzzer_parameters = true;
}

inline bool file_exists(const CHAR * name)
{
  struct stat buffer;   
  return (stat (name, &buffer) == 0); 
}

void print_msg(unsigned char * msg, UINT size)
{
	char *print_msg = (char *)malloc(size * 10);
	char current[4];

	print_msg[0] = 0;
	for (UINT i = 0; i < size; i++)
	{
		if ((i % 16) == 8)
		{
			strcat(print_msg, " ");
		}
		else if ((i % 16) == 0)
		{
			strcat(print_msg, "\n");
		}

		sprintf_s(current, 4, "%02x ", msg[i]);
		strcat(print_msg, current);
	}

	WLog_INFO("FUZZ", "sending msg:%s", print_msg);

	free(print_msg);
}

void send_message_on_channel()
{
	UINT rc;
	WLog_INFO("FUZZ", "send_message_on_channel starting");

	if (fuzzer_params.message_size != 0)
	{
		WLog_INFO("FUZZ", "message_size = %ld != 0", fuzzer_params.message_size);
		if (fuzzer_params.target_channel != NULL)
		{
			WLog_INFO("FUZZ", "Target_channel != NULL");
			print_msg((unsigned char *)fuzzer_params.message, fuzzer_params.message_size);
			fuzzer_params.inside_fuzz_msg = true;
			rc = dvcman_write_channel(fuzzer_params.target_channel, fuzzer_params.message_size, (const BYTE *)fuzzer_params.message, NULL);
			fuzzer_params.inside_fuzz_msg = false;
			WLog_INFO("FUZZ", "rc=%ld", rc);
		}
	}	
}

static DWORD WINAPI xf_fuzzer_thread(LPVOID param)
{
	bool terminate = false;
	FILE * input_file;
	UINT result = 0;
	UINT loop_counter = 0;

	WLog_INFO("FUZZ", "xf_fuzzer_thread starting");

	fuzzer_params.message = (CHAR *)malloc(MAX_FILE_SIZE);
	if (fuzzer_params.message == NULL)
	{
		return -1;
	}

	sleep(1);	

	// main fuzz loop - wait until the input file exists and send it on the channel
	while (!terminate)
	{
		if (file_exists(fuzzer_params.input_file_name))
		{
			input_file = fopen(fuzzer_params.input_file_name, "r");
			fuzzer_params.message_size = fread(fuzzer_params.message, sizeof(BYTE), MAX_FILE_SIZE, input_file);
			fclose(input_file);
			if (unlink(fuzzer_params.input_file_name) < 0)
			{
				WLog_INFO("FUZZ", "failed to delete file %s errno=%ld", fuzzer_params.input_file_name, errno);
			}

			// this is it! send the fuzzing message
			send_message_on_channel();
		} 

		usleep(10000);

		if ((loop_counter % 1000) == 0)
		{
			WLog_INFO("FUZZ", "%ld loops performed", loop_counter);			
		}
		loop_counter++;
	}

    return result;
}

void start_fuzzer_thread()
{
    if (!CreateThread(NULL, 0, xf_fuzzer_thread, NULL, 0, NULL))
	{
		WLog_ERR("FUZZ", "failed to create fuzzer thread");
	}
}