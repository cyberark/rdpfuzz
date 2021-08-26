#ifndef TARGET_CHANNEL_H
#define TARGET_CHANNEL_H

#include <stdint.h>
#include <freerdp/dvc.h>


extern void load_fuzzer_parameters();
extern void start_fuzzer_thread();

#define MAX_FILE_SIZE				0x100000

typedef struct xf_fuzzer_params
{
	bool initialized;
	bool loaded_fuzzer_parameters;
	IWTSVirtualChannel* target_channel;
	IWTSVirtualChannelManager* GlobalChannelMgr;

	CHAR input_file_name[MAX_PATH];
	CHAR channel_name[MAX_PATH];
	CHAR * message;
	size_t message_size;
	bool inside_fuzz_msg;
} xf_fuzzer_params, *pxf_fuzzer_params;

extern xf_fuzzer_params fuzzer_params;

#endif /* TARGET_CHANNEL_H */