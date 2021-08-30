/*
   WinAFL - DynamoRIO client (instrumentation) code
   ------------------------------------------------

   Written and maintained by Ivan Fratric <ifratric@google.com>

   Copyright 2016 Google Inc. All Rights Reserved.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

	   http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*/

#define _CRT_SECURE_NO_WARNINGS

#define MAP_SIZE 65536


#define CREATE_TASK_OFFSET			0x341b30
#define EXECUTE_TASK_OFFSET			0x33fb80
#define TASK_RASTERIZE_OFFSET		0x3021a0
#define SETUP_TILES_RETURN_OFFSET	0x7e8c8

#define CMDID_BUFFER_SIZE			100

#include "dr_api.h"
#include "drmgr.h"
#include "drx.h"
#include "drreg.h"
#include "drwrap.h"
#include "dr_tools.h"

#ifdef USE_DRSYMS
#include "drsyms.h"
#endif

#include "modules.h"
#include "utils.h"
#include "hashtable.h"
#include "drtable.h"
#include "limits.h"
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#include <psapi.h>

#define UNKNOWN_MODULE_ID USHRT_MAX

#ifndef PF_FASTFAIL_AVAILABLE
#define PF_FASTFAIL_AVAILABLE 23
#endif

#ifndef STATUS_FATAL_APP_EXIT
#define STATUS_FATAL_APP_EXIT ((DWORD)0x40000015L)
#endif

#ifndef STATUS_HEAP_CORRUPTION
#define STATUS_HEAP_CORRUPTION 0xC0000374
#endif

static uint verbose;

#define NOTIFY(level, fmt, ...) do {          \
    if (verbose >= (level))                   \
        dr_fprintf(STDERR, fmt, __VA_ARGS__); \
} while (0)

#define FUZZ_LOG(log_name, fmt, ...) do {									\
	file_t fuzz_log_handle;													\
	fuzz_log_handle = dr_open_file(log_name, DR_FILE_WRITE_APPEND);			\
	dr_fprintf(fuzz_log_handle, "%lld:", GetTickCount64());					\
	dr_fprintf(fuzz_log_handle, fmt, __VA_ARGS__);							\
	dr_close_file(fuzz_log_handle);											\
} while (0)

#define FUZZ_LOG_IF_ON(log_name, fmt, ...) do {								\
	if (options.fuzz_log)													\
	{																		\
		FUZZ_LOG(log_name, fmt, __VA_ARGS__);								\
	}																		\
} while (0)

#define OPTION_MAX_LENGTH MAXIMUM_PATH

#define COVERAGE_BB 0
#define COVERAGE_EDGE 1

//fuzz modes
enum persistence_mode_t { native_mode = 0, in_app = 1, };

typedef struct _target_module_t {
	char module_name[MAXIMUM_PATH];
	struct _target_module_t *next;
} target_module_t;

typedef struct _winafl_option_t {
	/* Use nudge to notify the process for termination so that
	 * event_exit will be called.
	 */
	bool nudge_kills;
	bool debug_mode;
	int persistence_mode;
	int coverage_kind;
	char logdir[MAXIMUM_PATH];
	target_module_t *target_modules;
	bool target_all_modules;
	char fuzz_module[MAXIMUM_PATH];
	char fuzz_method[MAXIMUM_PATH];
	char pipe_name[MAXIMUM_PATH];
	char shm_name[MAXIMUM_PATH];
	unsigned long fuzz_offset;
	int fuzz_iterations;
	void **func_args;
	int num_fuz_args;
	drwrap_callconv_t callconv;
	bool thread_coverage;
	bool no_loop;
	bool dr_persist_cache;
	bool fuzz_log;
	//bool fuzz_wrapper;
	//unsigned long fuzz_wrapper_offset;
	//unsigned long wrapper_data_param_number;
	//unsigned long wrapper_data_size_param_number;
	//unsigned long wrapper_marker_offset;
	//char wrapper_identifier[MAXIMUM_PATH];
	//unsigned long inside_timeout;
	unsigned long data_param_number;
	unsigned long data_size_param_number;
	unsigned long marker_offset;
	unsigned char marker;
	unsigned char last_msg_marker;
	bool marker_less_than;
	bool marker_is_bits;
	unsigned long offset_to_replace;
	unsigned long offset_to_replace_with;
	bool no_marker;
} winafl_option_t;
static winafl_option_t options;

#define NUM_THREAD_MODULE_CACHE 4

typedef struct _winafl_data_t {
	module_entry_t *cache[NUM_THREAD_MODULE_CACHE];
	file_t  log;
	unsigned char *fake_afl_area; //used for thread_coverage
	unsigned char *afl_area;
} winafl_data_t;
static winafl_data_t winafl_data;
//unsigned char *joint_afl_area;

typedef struct coverage_map_entry
{
	unsigned long long last_accessed_iteration;
	unsigned long long thread_mask;
} coverage_map_entry;

CRITICAL_SECTION coverage_sync;
//CRITICAL_SECTION print_sync;

unsigned long long global_thread_mask;

static coverage_map_entry * coverage_map;
static unsigned char * accumulative_coverage_map;
static int coverage_map_size;

PUCHAR accumulated_msgs = NULL;

bool last = false;
bool wrap_last = false;
bool first = false;
bool inside = false;
bool inside_wrapper = false;

DWORD last_msg_tick_count = 0;

CRITICAL_SECTION fuzz_msg_handling_lock;
CRITICAL_SECTION wrapper_msg_handling_lock;

HANDLE log_mutex_handle;

int msg_count = 0;
int wrap_msg_count = 0;

DWORD session_id;

UCHAR cmd_id;

bool treat_cmd_id;

module_data_t d3d10;

PROCESS_MEMORY_COUNTERS_EX ppm_before;
PROCESS_MEMORY_COUNTERS_EX ppm_after;

long long bb_counter;

DWORD *this_for_reconnect_wrapper;
DWORD prev_off_46;
DWORD prev_off_47;
DWORD prev_off_48;

DWORD *this_for_fuzz_wrapper;

HANDLE thread_handle_table[1000];
DWORD thread_handle_counter;

HANDLE *pthread_handle;

volatile long long num_closed_handles;
volatile long long num_threads_opened;

static int winafl_tls_coverage_map_field;
static int winafl_tls_coverage_map_registered;

static int winafl_tls_thread_mask_field;
static int winafl_tls_thread_mask_registered;

static int winafl_tls_cmdid_buffer_field;
static int winafl_tls_cmdid_buffer_registered;

static int winafl_tls_cmdid_index_field;
static int winafl_tls_cmdid_index_registered;

static int winafl_tls_field;
static int winafl_tls_field_registered;
static int inside_winafl_tls_field;
static int inside_winafl_tls_field_registered;

static int thread_afl_map_tls_field;
static int thread_afl_map_tls_field_reistered;

static module_entry_t *target_entry;

//DWORD restore_data[4];

typedef struct _fuzz_target_t {
	reg_t xsp;            /* stack level at entry to the fuzz target */
	app_pc func_pc;
	int iteration;
} fuzz_target_t;
static fuzz_target_t fuzz_target;

typedef struct _debug_data_t {
	int pre_hanlder_called;
	int post_handler_called;
} debug_data_t;
static debug_data_t debug_data;

static module_table_t *module_table;
static client_id_t client_id;

static volatile bool go_native;

static void
event_exit(void);

static void
event_thread_exit(void *drcontext);

static HANDLE pipe;

/****************************************************************************
 * Nudges
 */

enum {
	NUDGE_TERMINATE_PROCESS = 1,
};

static void
event_nudge(void *drcontext, uint64 argument)
{
	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[event_nudge] starting %lld\r\n", argument);

	int nudge_arg = (int)argument;
	int exit_arg = (int)(argument >> 32);
	if (nudge_arg == NUDGE_TERMINATE_PROCESS) {
		static int nudge_term_count;
		/* handle multiple from both NtTerminateProcess and NtTerminateJobObject */
		uint count = dr_atomic_add32_return_sum(&nudge_term_count, 1);
		if (count == 1) {
			dr_exit_process(exit_arg);
		}
	}
	ASSERT(nudge_arg == NUDGE_TERMINATE_PROCESS, "unsupported nudge");
	ASSERT(false, "should not reach"); /* should not reach */
}

static bool
event_soft_kill(process_id_t pid, int exit_code)
{
	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[event_soft_kill] starting\r\n");

	/* we pass [exit_code, NUDGE_TERMINATE_PROCESS] to target process */
	dr_config_status_t res;
	res = dr_nudge_client_ex(pid, client_id,
		NUDGE_TERMINATE_PROCESS | (uint64)exit_code << 32,
		0);
	if (res == DR_SUCCESS) {
		/* skip syscall since target will terminate itself */
		return true;
	}
	/* else failed b/c target not under DR control or maybe some other
	 * error: let syscall go through
	 */
	return false;
}

/****************************************************************************
 * Event Callbacks
 */

char ReadCommandFromPipe()
{
	char fuzz_log_name[MAX_PATH];

	dr_snprintf(fuzz_log_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\winafl-start-log-%i.txt", GetCurrentProcessId());
	FUZZ_LOG_IF_ON(fuzz_log_name, "[ReadCommandFromPipe] start\r\n");

	DWORD num_read;
	char result;
	ReadFile(pipe, &result, 1, &num_read, NULL);
	FUZZ_LOG_IF_ON(fuzz_log_name, "[ReadCommandFromPipe] read %c\r\n", result);
	return result;
}

void WriteCommandToPipe(char cmd)
{
	char fuzz_log_name[MAX_PATH];

	dr_snprintf(fuzz_log_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\winafl-start-log-%i.txt", GetCurrentProcessId());
	FUZZ_LOG_IF_ON(fuzz_log_name, "[WriteCommandToPipe] start cmd=%c\r\n", cmd);

	DWORD num_written;
	WriteFile(pipe, &cmd, 1, &num_written, NULL);
	FUZZ_LOG_IF_ON(fuzz_log_name, "[WriteCommandToPipe] wrote %c\r\n", cmd);

	last_msg_tick_count = GetTickCount();
}

static void
dump_winafl_data()
{
	dr_write_file(winafl_data.log, winafl_data.afl_area, MAP_SIZE);
}

#define MAX_ACCUMULATED_MSGS_SIZE 0x10000

static void accumulate_fuzz_msgs(PUCHAR msg, DWORD size, bool is_first)
{
	UCHAR current[4];

	if (!accumulated_msgs)
	{
		accumulated_msgs = (PUCHAR)dr_global_alloc(MAX_ACCUMULATED_MSGS_SIZE);
	}

	if (accumulated_msgs)
	{
		if (is_first)
		{
			accumulated_msgs[0] = 0;
		}

		strcat(accumulated_msgs, "\r\nmsg:\r\n");

		for (DWORD i = 0; i < size; i++)
		{
			if ((i % 16) == 8)
			{
				strcat(accumulated_msgs, " ");
			}
			else if ((i % 16) == 0)
			{
				strcat(accumulated_msgs, "\r\n");
			}

			dr_snprintf(current, 3, "%02x", msg[i]);
			strcat(accumulated_msgs, current);
		}

		strcat(accumulated_msgs, "\r\n");
	}
}

#define MAX_COVERAGE_MAP_SIZE 10000000

static void accumulate_coverage_map(void *drcontext)
{
	return;

	int i;
	unsigned long long thread_mask = (ULONGLONG)drmgr_get_tls_field(drcontext, winafl_tls_thread_mask_field);

	if (first)
	{
		memset(accumulative_coverage_map, 0, MAX_COVERAGE_MAP_SIZE);
	}

	for (i = 0; i < coverage_map_size / sizeof(coverage_map_entry); i++)
	{
		if (coverage_map[i].thread_mask & thread_mask)
		{
			accumulative_coverage_map[i] = 1;
		}
	}
}

static void dump_accumulate_coverage_map(void *drcontext)
{
	return;

	int i;
	//file_t log_file;
	char file_name[1000];
	dr_snprintf(file_name, 1000, "C:\\RDPFuzz\\Logs\\winafl-log-%i-%i.txt", GetCurrentProcessId(), msg_count);
	FUZZ_LOG_IF_ON(file_name, "[dump_accumulate_coverage_map] starting:\r\n");

	for (i = 0; i < coverage_map_size / sizeof(coverage_map_entry); i++)
	{
		if (accumulative_coverage_map[i])
		{
			FUZZ_LOG_IF_ON(file_name, "%s+%x ; %lld\r\n", options.fuzz_module, i, coverage_map[i].last_accessed_iteration);

			//log_file = dr_open_file(file_name, DR_FILE_WRITE_APPEND);
			//dr_fprintf(log_file, "%s+%x ; %lld\r\n", options.fuzz_module, i, coverage_map[i].last_accessed_iteration);
			//dr_close_file(log_file);
		}
	}
}

static void exception_dump(void *drcontext, dr_exception_t *excpt)
{
	file_t log_file;
	unsigned long long thread_mask;

	STARTUPINFOA si = { 0 };
	si.cb = sizeof(STARTUPINFOA);
	PROCESS_INFORMATION pi = { 0 };
	//CHAR procdump_cmdline[0x1000];
	CHAR create_dump_file_name[MAX_PATH];

	DWORD exception_code = excpt->record->ExceptionCode;

	thread_mask = (unsigned long long)drmgr_get_tls_field(drcontext, winafl_tls_thread_mask_field);

	//accumulate_coverage_map(drcontext);

	//dump_accumulate_coverage_map(drcontext);

	dr_snprintf(create_dump_file_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\create_crash_dump_%ld", GetCurrentProcessId());
	log_file = dr_open_file(create_dump_file_name, DR_FILE_WRITE_APPEND);
	dr_fprintf(log_file, "%ld\r\n", GetCurrentProcessId());
	dr_close_file(log_file);

	WriteCommandToPipe('C');

	WaitForSingleObject(log_mutex_handle, INFINITE);

	//EnterCriticalSection(&print_sync);

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", "CRASH! exception_code=%08x, msg_count=%ld, thread_mask=%lld, pid=%ld tid=%ld trace follows:\r\n", 
		exception_code, msg_count, thread_mask, GetCurrentProcessId(), GetCurrentThreadId());

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "CRASH! exception_code=%08x, msg_count=%ld, thread_mask=%lld, pid=%ld tid=%ld trace follows:\r\n", exception_code, msg_count, thread_mask, GetCurrentProcessId(), GetCurrentThreadId());
	//dr_close_file(log_file);

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", " excpt=%016llx code=%08x \r\n flags=%08x \r\n address=%016llx \r\n num_params=%ld \r\n record=%016llx \r\n"
		, excpt
		, excpt->record->ExceptionCode
		, excpt->record->ExceptionFlags
		, excpt->record->ExceptionAddress
		, excpt->record->ExceptionRecord
		, excpt->record->NumberParameters);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, " excpt=%016llx code=%08x \r\n flags=%08x \r\n address=%016llx \r\n num_params=%ld \r\n record=%016llx \r\n"
	//	, excpt
	//	, excpt->record->ExceptionCode
	//	, excpt->record->ExceptionFlags
	//	, excpt->record->ExceptionAddress
	//	, excpt->record->ExceptionRecord
	//	, excpt->record->NumberParameters);
	//dr_close_file(log_file);

	for (DWORD j = 0; j < excpt->record->NumberParameters; j++)
	{
		FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", " param[%i]=%016llx \r\n", j, excpt->record->ExceptionInformation[j]);
		//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", DR_FILE_WRITE_APPEND);
		//dr_fprintf(log_file, " param[%i]=%016llx \r\n", j, excpt->record->ExceptionInformation[j]);
		//dr_close_file(log_file);
	}

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", "Fuzz msgs:\r\n%s", accumulated_msgs);

	for (DWORD i = 0; i < coverage_map_size / sizeof(coverage_map_entry); i++)
	{
		if (coverage_map[i].thread_mask & thread_mask)
		{
			FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", "mstscax+%x ; %lld; %lld \r\n", i, coverage_map[i].last_accessed_iteration, coverage_map[i].thread_mask);
			//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", DR_FILE_WRITE_APPEND);
			//dr_fprintf(log_file, "mstscax+%x ; %lld; %lld \r\n", i, coverage_map[i].last_accessed_iteration, coverage_map[i].thread_mask);
			//dr_close_file(log_file);
		}
	}

	ReleaseMutex(log_mutex_handle);

	//LeaveCriticalSection(&print_sync);

	//dr_snprintf(procdump_cmdline, 0x1000, "\"C:\\Users\\Fuzz\\Desktop\\Sysinternals Suite\\procdump.exe\" -ma -accepteula %i C:\\RdpFuzz\\Logs\\PID_HHMMSS_%i", GetCurrentProcessId(), msg_count);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "before CreateProcessA cmdline=%s\r\n", procdump_cmdline);
	//dr_close_file(log_file);



	//dr_snprintf(create_dump_file_name, MAX_PATH, "G:\\trace\\crash\\%ld", GetCurrentProcessId());
	//log_file = dr_open_file(create_dump_file_name, DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "%ld\r\n", GetCurrentProcessId());
	//dr_close_file(log_file);



	Sleep(60000);

	//if (CreateProcessA(NULL, (LPSTR)procdump_cmdline, NULL, NULL, false, 0, NULL, NULL, &si, &pi))
	//{
	//	log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", DR_FILE_WRITE_APPEND);
	//	dr_fprintf(log_file, "inside CreateProcessA\r\n");
	//	dr_close_file(log_file);


	//	WaitForSingleObject(pi.hProcess, INFINITE);
	//	CloseHandle(pi.hProcess);
	//	CloseHandle(pi.hThread);
	//}

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "after CreateProcessA\r\n");
	//dr_close_file(log_file);

}

static bool
onexception(void *drcontext, dr_exception_t *excpt) {
	DWORD exception_code = excpt->record->ExceptionCode;

	//file_t log_file;

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", "[onexception] received code %ld", excpt->record->ExceptionCode);

	//return true;

	DWORD before = 0x55555555u;

	void *dr_exception_record = excpt;
	EXCEPTION_RECORD * exception_record = excpt->record;

	DWORD after = 0x66666666u;


	if (options.debug_mode)
		dr_fprintf(winafl_data.log, "Exception caught: %x\n", exception_code);

	if ((exception_code == EXCEPTION_ACCESS_VIOLATION) ||
		(exception_code == EXCEPTION_ILLEGAL_INSTRUCTION) ||
		(exception_code == EXCEPTION_PRIV_INSTRUCTION) ||
		(exception_code == EXCEPTION_INT_DIVIDE_BY_ZERO) ||
		(exception_code == STATUS_HEAP_CORRUPTION) ||
		(exception_code == EXCEPTION_STACK_OVERFLOW) ||
		(exception_code == STATUS_STACK_BUFFER_OVERRUN) ||
		(exception_code == STATUS_FATAL_APP_EXIT)) {
		if (options.debug_mode) {
			dr_fprintf(winafl_data.log, "crashed\n");
		}
		else {

			exception_dump(drcontext, excpt);
		}
		dr_exit_process(1);
	}

	before = before * (DWORD)(ULONGLONG)drcontext + after;
	after ^= before;

	if (after == 17)
	{
		FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", "NO WAY!");
		//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", DR_FILE_WRITE_APPEND);
		//dr_fprintf(log_file, "NO WAY!");
		//dr_close_file(log_file);
	}

	return true;
}

static void event_thread_init(void *drcontext)
{
	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[event_thread_init] starting %ld\r\n", GetCurrentThreadId());

	void **thread_data;
	bool *inside_winafl;
	unsigned char *thread_afl_map;
	unsigned long long i;
	unsigned long long current_thread_mask;
	UCHAR *cmdid_buffer;
	DWORD *cmdid_index;

	if (winafl_tls_field_registered)
	{
		thread_data = (void **)dr_thread_alloc(drcontext, 2 * sizeof(void *));
		thread_data[0] = 0;
		if (options.thread_coverage) {
			thread_data[1] = winafl_data.fake_afl_area;
		}
		else {
			thread_data[1] = winafl_data.afl_area;
		}
		drmgr_set_tls_field(drcontext, winafl_tls_field, thread_data);
	}

	if (inside_winafl_tls_field_registered)
	{
		inside_winafl = dr_thread_alloc(drcontext, sizeof(bool));
		inside_winafl[0] = false;

		drmgr_set_tls_field(drcontext, inside_winafl_tls_field, (void *)inside_winafl);
	}

	if (thread_afl_map_tls_field_reistered)
	{
		thread_afl_map = dr_thread_alloc(drcontext, MAP_SIZE);
		memset(thread_afl_map, 0, MAP_SIZE);

		drmgr_set_tls_field(drcontext, thread_afl_map_tls_field, (void *)thread_afl_map);
	}

	if (winafl_tls_coverage_map_registered)
	{
		drmgr_set_tls_field(drcontext, winafl_tls_coverage_map_field, coverage_map);
	}

	EnterCriticalSection(&coverage_sync);

	for (i = 0; i < 64; i++)
	{
		current_thread_mask = (unsigned long long)1 << i;
		if ((global_thread_mask & current_thread_mask) == 0)
		{
			break;
		}
	}

	global_thread_mask |= current_thread_mask;

	if (winafl_tls_thread_mask_registered)
	{
		drmgr_set_tls_field(drcontext, winafl_tls_thread_mask_field, (void *)current_thread_mask);
	}

	LeaveCriticalSection(&coverage_sync);

	if (winafl_tls_cmdid_buffer_registered)
	{
		cmdid_buffer = dr_thread_alloc(drcontext, CMDID_BUFFER_SIZE);
		memset(cmdid_buffer, 0, CMDID_BUFFER_SIZE);

		drmgr_set_tls_field(drcontext, winafl_tls_cmdid_buffer_field, cmdid_buffer);
	}

	if (winafl_tls_cmdid_index_registered)
	{
		cmdid_index = dr_thread_alloc(drcontext, sizeof(DWORD));
		*cmdid_index = 0;

		drmgr_set_tls_field(drcontext, winafl_tls_cmdid_index_field, cmdid_index);
	}

	//file_t log_file;
	//char file_name[1000];

	//dr_snprintf(file_name, 1000, "C:\\RDPFuzz\\Logs\\winafl-thread-%i-log.txt", GetCurrentThreadId());
	//log_file = dr_open_file(file_name, DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "thread start\r\n");
	//dr_close_file(log_file);
}

static void event_thread_exit(void *drcontext)
{
	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[event_thread_exit] starting %ld\r\n", GetCurrentThreadId());

	void *data;
	void *inside_winafl;
	void *thread_afl_map;

	unsigned long long thread_mask;

	UCHAR *cmdid_buffer;
	DWORD *cmdid_index;

	if (winafl_tls_field_registered)
	{
		data = drmgr_get_tls_field(drcontext, winafl_tls_field);
		dr_thread_free(drcontext, data, 2 * sizeof(void *));
	}

	if (inside_winafl_tls_field_registered)
	{
		inside_winafl = drmgr_get_tls_field(drcontext, inside_winafl_tls_field);
		if (inside_winafl && *(bool *)inside_winafl)
		{
			char fuzz_log_name[MAX_PATH];

			dr_snprintf(fuzz_log_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\winafl-start-log-%i.txt", GetCurrentProcessId());

			// thread terminating while in our function, free the lock 
			FUZZ_LOG_IF_ON(fuzz_log_name, "terminating while inside\r\n");

			if (last)
			{
				// mark that we are outside!
				inside = false;

				//let server know we finished a cycle, redundunt on first cycle.
				WriteCommandToPipe('K');
			}

			LeaveCriticalSection(&fuzz_msg_handling_lock);
		}
		dr_thread_free(drcontext, inside_winafl, sizeof(bool));
	}

	if (thread_afl_map_tls_field_reistered)
	{
		thread_afl_map = drmgr_get_tls_field(drcontext, thread_afl_map_tls_field);
		dr_thread_free(drcontext, thread_afl_map, MAP_SIZE);
	}

	thread_mask = (unsigned long long)drmgr_get_tls_field(drcontext, winafl_tls_thread_mask_field);

	EnterCriticalSection(&coverage_sync);
	global_thread_mask &= (~thread_mask);
	LeaveCriticalSection(&coverage_sync);

	if (winafl_tls_cmdid_buffer_registered)
	{
		cmdid_buffer = drmgr_get_tls_field(drcontext, winafl_tls_cmdid_buffer_field);
		dr_thread_free(drcontext, cmdid_buffer, CMDID_BUFFER_SIZE);
	}

	if (winafl_tls_cmdid_index_registered)
	{
		cmdid_index = drmgr_get_tls_field(drcontext, winafl_tls_cmdid_index_field);
		dr_thread_free(drcontext, cmdid_index, sizeof(DWORD));
	}


	//dr_thread_free(drcontext, prev_coverage_map, coverage_map_size);
	//dr_thread_free(drcontext, current_coverage_map, coverage_map_size);

	//file_t log_file;
	//char file_name[1000];

	//dr_snprintf(file_name, 1000, "C:\\RDPFuzz\\Logs\\winafl-thread-%i-log.txt", GetCurrentThreadId());
	//log_file = dr_open_file(file_name, DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "thread stop\r\n");
	//dr_close_file(log_file);
}

static dr_emit_flags_t
app_instrument_bb_coverage(void *drcontext, void *tag, instrlist_t *bb, bool for_trace, bool translating)
{
	module_entry_t **mod_entry_cache;
	module_entry_t *mod_entry;
	app_pc start_pc;
	target_module_t *target_modules;
	const char *module_name;
	int i = 0;
	uint offset;

	//FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[app_instrument_bb_coverage] starting\r\n");

	start_pc = dr_fragment_app_pc(tag);

	mod_entry_cache = winafl_data.cache;
	mod_entry = module_table_lookup(mod_entry_cache,
		NUM_THREAD_MODULE_CACHE,
		module_table, start_pc);

	if (mod_entry == NULL || mod_entry->data == NULL) return DR_EMIT_DEFAULT;

	module_name = dr_module_preferred_name(mod_entry->data);

	target_modules = options.target_modules;

	// this is to skip noisy bug in rascustom.dll
	if (_stricmp(module_name, "RASCUSTOM.DLL") == 0)
	{
		offset = (uint)(start_pc - mod_entry->data->start);

		//FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "\r\ninstrumenting module %s, offset=%08x\r\n", module_name, offset);

		if (offset == 0x4C4E9)
		{
			FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "\r\nfound target bb for deletion\r\n");

			instr_t *instr_to_delete;

			instr_to_delete = instrlist_first(bb);
			instr_to_delete = instr_get_next(instr_to_delete);

			instrlist_remove(bb, instr_to_delete);
		}
	}

	return DR_EMIT_DEFAULT | DR_EMIT_PERSISTABLE;
}

static dr_emit_flags_t
instrument_bb_coverage(void *drcontext, void *tag, instrlist_t *bb, instr_t *inst,
	bool for_trace, bool translating, void *user_data)
{
	static bool debug_information_output = false;
	app_pc start_pc;
	module_entry_t **mod_entry_cache;
	module_entry_t *mod_entry;
	const char *module_name;
	uint offset;
	uint real_offset;
	target_module_t *target_modules;
	bool should_instrument;
	unsigned char *afl_map;
	dr_emit_flags_t ret;

	if (!drmgr_is_first_instr(drcontext, inst))
		return DR_EMIT_DEFAULT;

	start_pc = dr_fragment_app_pc(tag);

	mod_entry_cache = winafl_data.cache;
	mod_entry = module_table_lookup(mod_entry_cache,
		NUM_THREAD_MODULE_CACHE,
		module_table, start_pc);

	if (mod_entry == NULL || mod_entry->data == NULL) return DR_EMIT_DEFAULT;

	module_name = dr_module_preferred_name(mod_entry->data);

	//FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "\r\nmodule_name=%s\r\n", module_name);


	should_instrument = options.target_all_modules;

	target_modules = options.target_modules;
	while (target_modules) {
		if (_stricmp(module_name, target_modules->module_name) == 0) {

			// this is my hack!
			// we only have 1 target module, so just save it to a global variable
			target_entry = mod_entry;

			should_instrument = true;
			if (options.debug_mode && debug_information_output == false) {
				dr_fprintf(winafl_data.log, "Instrumenting %s with the 'bb' mode\n", module_name);
				debug_information_output = true;
			}
			break;
		}
		target_modules = target_modules->next;
	}
	if (!should_instrument) return DR_EMIT_DEFAULT | DR_EMIT_PERSISTABLE;

	offset = (uint)(start_pc - mod_entry->data->start);
	real_offset = offset;
	offset &= MAP_SIZE - 1;

	afl_map = winafl_data.afl_area;

	drreg_reserve_aflags(drcontext, bb, inst);

	if (options.thread_coverage || options.dr_persist_cache) {
		reg_id_t reg, reg2;
		opnd_t opnd1, opnd2;
		instr_t *new_instr;

		drreg_reserve_register(drcontext, bb, inst, NULL, &reg);
		drreg_reserve_register(drcontext, bb, inst, NULL, &reg2);

		drmgr_insert_read_tls_field(drcontext, winafl_tls_field, bb, inst, reg);

		opnd1 = opnd_create_reg(reg);
		opnd2 = OPND_CREATE_MEMPTR(reg, sizeof(void *));
		new_instr = INSTR_CREATE_mov_ld(drcontext, opnd1, opnd2);
		instrlist_meta_preinsert(bb, inst, new_instr);

		opnd1 = OPND_CREATE_MEM8(reg, offset);
		new_instr = INSTR_CREATE_inc(drcontext, opnd1);
		instrlist_meta_preinsert(bb, inst, new_instr);


		// create the pointer to bb_counter
		opnd1 = OPND_CREATE_ABSMEM(&bb_counter, OPSZ_8);

		// increase bb_counter
		new_instr = INSTR_CREATE_inc(drcontext, opnd1);

		// add the instruction
		instrlist_meta_preinsert(bb, inst, new_instr);

		// use another register
		opnd1 = opnd_create_reg(reg2);

		// pointer to the counter
		opnd2 = OPND_CREATE_ABSMEM(&bb_counter, OPSZ_8);

		// move to the register
		new_instr = INSTR_CREATE_movq(drcontext, opnd1, opnd2);

		// add the instruction
		instrlist_meta_preinsert(bb, inst, new_instr);

		//// read the coverage map to reg
		//drmgr_insert_read_tls_field(drcontext, winafl_tls_coverage_map_field, bb, inst, reg);

		//// add the offset
		//opnd1 = OPND_CREATE_MEMPTR(reg, real_offset * sizeof(long long));

		//opnd2 = OPND_CREATE_INT64(0);

		//new_instr = INSTR_CREATE_mov_imm(drcontext, opnd1, opnd2);
		//// add the instruction
		//instrlist_meta_preinsert(bb, inst, new_instr);

		// read the coverage map to reg
		drmgr_insert_read_tls_field(drcontext, winafl_tls_coverage_map_field, bb, inst, reg);

		// add the offset
		opnd1 = OPND_CREATE_MEMPTR(reg, real_offset * sizeof(coverage_map_entry));

		// use another register
		opnd2 = opnd_create_reg(reg2);

		//new_instr = instr_create_1dst_2src(drcontext, OP_add, opnd1, opnd1, opnd2);

		new_instr = INSTR_CREATE_movq(drcontext, opnd1, opnd2);
		// add the instruction
		instrlist_meta_preinsert(bb, inst, new_instr);

		// read the thread mask
		drmgr_insert_read_tls_field(drcontext, winafl_tls_thread_mask_field, bb, inst, reg2);

		// point to the location od the thread mask
		opnd1 = OPND_CREATE_MEMPTR(reg, real_offset * sizeof(coverage_map_entry) + sizeof(long long));

		opnd2 = opnd_create_reg(reg2);
		
		// set the thread_mask bit
		new_instr = INSTR_CREATE_or(drcontext, opnd1, opnd2);

		// add the instruction
		instrlist_meta_preinsert(bb, inst, new_instr);

		// create the increase instruction
		// to do - change to move opnd1, 1
		//new_instr = INSTR_CREATE_inc(drcontext, opnd1);



		drreg_unreserve_register(drcontext, bb, inst, reg2);
		drreg_unreserve_register(drcontext, bb, inst, reg);

		ret = DR_EMIT_DEFAULT | DR_EMIT_PERSISTABLE;

	}
	else {

		instrlist_meta_preinsert(bb, inst,
			INSTR_CREATE_inc(drcontext, OPND_CREATE_ABSMEM
			(&(afl_map[offset]), OPSZ_1)));

		ret = DR_EMIT_DEFAULT;
	}


	drreg_unreserve_aflags(drcontext, bb, inst);

	return ret;
}

static dr_emit_flags_t
instrument_edge_coverage(void *drcontext, void *tag, instrlist_t *bb, instr_t *inst,
	bool for_trace, bool translating, void *user_data)
{
	static bool debug_information_output = false;
	app_pc start_pc;
	module_entry_t **mod_entry_cache;
	module_entry_t *mod_entry;
	reg_id_t reg, reg2, reg3;
	opnd_t opnd1, opnd2;
	instr_t *new_instr;
	const char *module_name;
	uint offset;
	target_module_t *target_modules;
	bool should_instrument;
	dr_emit_flags_t ret;

	if (!drmgr_is_first_instr(drcontext, inst))
		return DR_EMIT_DEFAULT;

	start_pc = dr_fragment_app_pc(tag);

	mod_entry_cache = winafl_data.cache;
	mod_entry = module_table_lookup(mod_entry_cache,
		NUM_THREAD_MODULE_CACHE,
		module_table, start_pc);

	if (mod_entry == NULL || mod_entry->data == NULL) return DR_EMIT_DEFAULT;

	module_name = dr_module_preferred_name(mod_entry->data);

	should_instrument = false;
	target_modules = options.target_modules;
	while (target_modules) {
		if (_stricmp(module_name, target_modules->module_name) == 0) {

			// this is my hack!
			// we only have 1 target module, so just save it to a global variable
			target_entry = mod_entry;

			should_instrument = true;
			if (options.debug_mode && debug_information_output == false) {
				dr_fprintf(winafl_data.log, "Instrumenting %s with the 'edge' mode\n", module_name);
				debug_information_output = true;
			}
			break;
		}
		target_modules = target_modules->next;
	}
	if (!should_instrument) return DR_EMIT_DEFAULT | DR_EMIT_PERSISTABLE;

	offset = (uint)(start_pc - mod_entry->data->start);
	offset &= MAP_SIZE - 1;

	drreg_reserve_aflags(drcontext, bb, inst);
	drreg_reserve_register(drcontext, bb, inst, NULL, &reg);
	drreg_reserve_register(drcontext, bb, inst, NULL, &reg2);
	drreg_reserve_register(drcontext, bb, inst, NULL, &reg3);

	//reg2 stores AFL area, reg 3 stores previous offset

	//load the pointer to previous offset in reg3
	drmgr_insert_read_tls_field(drcontext, winafl_tls_field, bb, inst, reg3);

	//load address of shm into reg2
	if (options.thread_coverage || options.dr_persist_cache) {
		opnd1 = opnd_create_reg(reg2);
		opnd2 = OPND_CREATE_MEMPTR(reg3, sizeof(void *));
		new_instr = INSTR_CREATE_mov_ld(drcontext, opnd1, opnd2);
		instrlist_meta_preinsert(bb, inst, new_instr);

		ret = DR_EMIT_DEFAULT | DR_EMIT_PERSISTABLE;

	}
	else {
		opnd1 = opnd_create_reg(reg2);
		opnd2 = OPND_CREATE_INTPTR((uint64)winafl_data.afl_area);
		new_instr = INSTR_CREATE_mov_imm(drcontext, opnd1, opnd2);
		instrlist_meta_preinsert(bb, inst, new_instr);

		ret = DR_EMIT_DEFAULT;
	}

	//load previous offset into register
	opnd1 = opnd_create_reg(reg);
	opnd2 = OPND_CREATE_MEMPTR(reg3, 0);
	new_instr = INSTR_CREATE_mov_ld(drcontext, opnd1, opnd2);
	instrlist_meta_preinsert(bb, inst, new_instr);

	//xor register with the new offset
	opnd1 = opnd_create_reg(reg);
	opnd2 = OPND_CREATE_INT32(offset);
	new_instr = INSTR_CREATE_xor(drcontext, opnd1, opnd2);
	instrlist_meta_preinsert(bb, inst, new_instr);

	//increase the counter at reg + reg2
	opnd1 = opnd_create_base_disp(reg2, reg, 1, 0, OPSZ_1);
	new_instr = INSTR_CREATE_inc(drcontext, opnd1);
	instrlist_meta_preinsert(bb, inst, new_instr);

	//store the new value
	offset = (offset >> 1)&(MAP_SIZE - 1);
	opnd1 = OPND_CREATE_MEMPTR(reg3, 0);
	opnd2 = OPND_CREATE_INT32(offset);
	new_instr = INSTR_CREATE_mov_st(drcontext, opnd1, opnd2);
	instrlist_meta_preinsert(bb, inst, new_instr);

	drreg_unreserve_register(drcontext, bb, inst, reg3);
	drreg_unreserve_register(drcontext, bb, inst, reg2);
	drreg_unreserve_register(drcontext, bb, inst, reg);
	drreg_unreserve_aflags(drcontext, bb, inst);

	return ret;
}

//static bool is_ours(PUCHAR ours)
//{
//	//WriteCommandToPipe(*ours);
//
//	// to do - verify that this follows through
//	if (*ours == 0 || *ours == 1)
//	{
//		MessageBoxA(NULL, "fail", NULL, MB_OK);
//
//		*ours += 0xE0u;
//		return true;
//	}
//
//	return false;
//}

static void logger(PCHAR log_name, PCHAR log_msg)
{
	file_t log_file;
	DWORD file_size_low;
	DWORD file_size_high;
	char log_name_pid[1000];

	dr_snprintf(log_name_pid, 1000, "%s-%s.txt", log_name, options.shm_name);

	log_file = dr_open_file(log_name_pid, DR_FILE_WRITE_APPEND);
	file_size_low = GetFileSize(log_file, &file_size_high);
	if (file_size_high || file_size_low > 100000000)
	{
		dr_close_file(log_file);
		log_file = dr_open_file(log_name_pid, DR_FILE_WRITE_OVERWRITE);
	}

	dr_fprintf(log_file, log_msg);
	dr_close_file(log_file);
}

static void print_mem_diff(bool before)
{
	//file_t log_file;
	//char log_msg[1000];

	if (before)
	{
		memset(&ppm_before, 0, sizeof(ppm_before));
		ppm_before.cb = sizeof(ppm_before);

		GetProcessMemoryInfo(GetCurrentProcess(), (PPROCESS_MEMORY_COUNTERS)&ppm_before, sizeof(ppm_before));

		//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-memory-counters-log.txt", DR_FILE_WRITE_APPEND);
		//dr_fprintf(log_file,
		//	"%lld: [%i] before: PrivateUsage=%016llx\r\n", GetTickCount64(), msg_count, ppm_before.PrivateUsage);
		//dr_close_file(log_file);


	}
	else
	{
		memset(&ppm_after, 0, sizeof(ppm_after));
		ppm_after.cb = sizeof(ppm_after);

		GetProcessMemoryInfo(GetCurrentProcess(), (PPROCESS_MEMORY_COUNTERS)&ppm_after, sizeof(ppm_after));

		//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-memory-counters-log.txt", DR_FILE_WRITE_APPEND);
		//dr_fprintf(log_file,
		//	"%lld: [%i] after:  PrivateUsage=%016llx\r\n", GetTickCount64(), msg_count, ppm_after.PrivateUsage);
		//dr_close_file(log_file);
		if ((LONGLONG)(ppm_after.PrivateUsage - ppm_before.PrivateUsage) > 0x100000)
		{
			FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-memory-counters-diff-log", "%lld: [%i] diff=%016llx\r\n", GetTickCount64(), msg_count, ppm_after.PrivateUsage - ppm_before.PrivateUsage);

			//dr_snprintf(log_msg, 1000, "%lld: [%i] diff=%016llx\r\n", GetTickCount64(), msg_count, ppm_after.PrivateUsage - ppm_before.PrivateUsage);
			//logger("C:\\RDPFuzz\\Logs\\winafl-memory-counters-diff-log", log_msg);
		}

		if (ppm_after.PrivateUsage > 10240000000)
		{
			FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-memory-restarts-log", "%lld: [%i] pid=%ld\r\n", GetTickCount64(), msg_count, GetCurrentProcessId());

			//dr_snprintf(log_msg, 1000, "%lld: [%i] pid=%ld\r\n", GetTickCount64(), msg_count, GetCurrentProcessId());
			//logger("C:\\RDPFuzz\\Logs\\winafl-memory-restarts-log", log_msg);

			WriteCommandToPipe('M');
		}
	}
}

static void zero_accumulate_coverage_map_for_current_thread(void *drcontext)
{
	int i;
	unsigned long long thread_mask = (ULONGLONG)drmgr_get_tls_field(drcontext, winafl_tls_thread_mask_field);

	for (i = 0; i < coverage_map_size / sizeof(coverage_map_entry); i++)
	{
		coverage_map[i].thread_mask &= ~thread_mask;
	}
}


static void dump_afl_map(void *drcontext, bool before)
{
	//unsigned char **afl_map;
	int i;
	//file_t log_file;
	char file_name[1000];

	FILETIME current_time = { 0 };

	//FILETIME thread_creation_time;
	//FILETIME thread_exit_time;
	//FILETIME thread_kernel_time;
	//FILETIME thread_user_time;

	//ULONGLONG handle;

	unsigned long long thread_mask = (ULONGLONG)drmgr_get_tls_field(drcontext, winafl_tls_thread_mask_field);

	//bool first_non_zero = true;
	//bool in_first_non_zero = false;

	//afl_map = drmgr_get_tls_field(drcontext, winafl_tls_field);

	//if (before)
	//{
	//	memcpy(fake_afl_area_copy, afl_map[1], MAP_SIZE);
	//}
	//else
	//{
	//	log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-log.txt", DR_FILE_WRITE_APPEND);
	//	dr_fprintf(log_file, "changes:\r\n");
	//	dr_close_file(log_file);


	//	for (i = 0; i < MAP_SIZE; i++)
	//	{
	//		if (fake_afl_area_copy[i] != afl_map[1][i])
	//		{
	//			log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-log.txt", DR_FILE_WRITE_APPEND);
	//			dr_fprintf(log_file, "offset=%ld prev=%ld current=%ld num_hits=%ld\r\n", i, fake_afl_area_copy[i], afl_map[1][i], afl_map[1][i] - fake_afl_area_copy[i]);
	//			dr_close_file(log_file);
	//		}
	//	}
	//}


	if (before)
	{
		//if (first)
		//{
		//	for (i = 0; i < coverage_map_size / sizeof(coverage_map_entry); i++)
		//	{
		//		coverage_map[i].thread_mask = 0;
		//	}
		//}

		//for (i = 0; i < coverage_map_size / sizeof(coverage_map_entry); i++)
		//{
			//if (coverage_map[i].thread_mask)
			//{
			//	if (msg_count == 10)
			//	{
			//		if (first_non_zero)
			//		{
			//			first_non_zero = false;
			//			in_first_non_zero = true;

			//			dr_snprintf(file_name, 1000, "C:\\RDPFuzz\\Logs\\winafl-log-%i-%i-%i.txt", GetCurrentProcessId(), GetCurrentThreadId(), msg_count);
			//			log_file = dr_open_file(file_name, DR_FILE_WRITE_APPEND);
			//			dr_fprintf(log_file, "before ANDing coverage_map[i].thread_mask=%016llx ~thread_mask=%016llx\r\n", coverage_map[i].thread_mask, ~thread_mask);
			//			dr_close_file(log_file);
			//		}
			//	}
			//}

			//coverage_map[i].thread_mask &= (~thread_mask);

			//if (msg_count == 10)
			//{
			//	if (in_first_non_zero)
			//	{
			//		in_first_non_zero = false;

			//		dr_snprintf(file_name, 1000, "C:\\RDPFuzz\\Logs\\winafl-log-%i-%i-%i.txt", GetCurrentProcessId(), GetCurrentThreadId(), msg_count);
			//		log_file = dr_open_file(file_name, DR_FILE_WRITE_APPEND);
			//		dr_fprintf(log_file, "after ANDing coverage_map[i].thread_mask=%016x ~thread_mask=%016x\r\n", coverage_map[i].thread_mask, ~thread_mask);
			//		dr_close_file(log_file);
			//	}
			//}
		//}

		if (first)
		{
			memset(coverage_map, 0, coverage_map_size);
		}
		//for (i = 0; i < 110; i++)
		//{
		//	dr_snprintf(file_name, 1000, "C:\\RDPFuzz\\Logs\\winafl-before-fuzz-loop-log-%i.txt", msg_count);
		//	log_file = dr_open_file(file_name, DR_FILE_WRITE_APPEND);
		//	dr_fprintf(log_file, "[%i]=%x\r\n", i, this_for_fuzz_wrapper[i]);
		//	dr_close_file(log_file);
		//}


		//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-log.txt", DR_FILE_WRITE_APPEND);
		//dr_fprintf(log_file, "before module_size=%lld \r\n", target_entry->data->module_internal_size);
		//dr_close_file(log_file);
	}
	else
	{
		if (last)
		{
			dr_snprintf(file_name, 1000, "C:\\RDPFuzz\\Logs\\winafl-log-%i-%i-%i.txt", GetCurrentProcessId(), GetCurrentThreadId(), msg_count);
			for (i = 0; i < coverage_map_size / sizeof(coverage_map_entry); i++)
			{
				if (coverage_map[i].last_accessed_iteration)
				{
					if (coverage_map[i].thread_mask & thread_mask)
					{
						FUZZ_LOG_IF_ON(file_name, "RDPSERVERBASE+%x ; last_iteration=%lld ; thread_mask=%lld\r\n", i, coverage_map[i].last_accessed_iteration, coverage_map[i].thread_mask);

						//log_file = dr_open_file(file_name, DR_FILE_WRITE_APPEND);
						//dr_fprintf(log_file, "RDPSERVERBASE+%x ; last_iteration=%lld ; thread_mask=%lld\r\n", i, coverage_map[i].last_accessed_iteration, coverage_map[i].thread_mask);
						//dr_close_file(log_file);
					}
				}
			}
		}

		//if ((msg_count % 100) == 0)
		//{
		//	GetSystemTimeAsFileTime(&current_time);
		//
		//	for (handle = 0; handle < 0x10000u; handle += 4)
		//	{
		//		if (GetThreadId((HANDLE)handle))
		//		{
		//			if (GetThreadTimes((HANDLE)handle, &thread_creation_time, &thread_exit_time, &thread_kernel_time, &thread_user_time))
		//			{
		//				if (current_time.dwHighDateTime > thread_creation_time.dwHighDateTime || current_time.dwLowDateTime >= thread_creation_time.dwLowDateTime + 300000000)
		//				{
		//					if (WaitForSingleObject((HANDLE)handle, 0) == WAIT_OBJECT_0)
		//					{
		//						// its been over ten secons since the thread was created, and it was already terminated
		//						// let's hope it is safe to close the handle
		//						CloseHandle((HANDLE)handle);
		//					}
		//				}
		//			}
		//			//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-thread-termination-duplicate-handles-log.txt", DR_FILE_WRITE_APPEND);
		//			//dr_fprintf(log_file, "first = %lld second = %lld\r\n", *pthread_handle, i);
		//			//dr_close_file(log_file);
		//		}
		//	}
		//}

		//exception_dump(drcontext);


		//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-log.txt", DR_FILE_WRITE_APPEND);
		//dr_fprintf(log_file, "changes:\r\n");
		//dr_close_file(log_file);

		//for (i = 0; i < 110; i++)
		//{
		//	dr_snprintf(file_name, 1000, "C:\\RDPFuzz\\Logs\\winafl-after-fuzz-loop-log-%i.txt", msg_count);
		//	log_file = dr_open_file(file_name, DR_FILE_WRITE_APPEND);
		//	dr_fprintf(log_file, "[%i]=%x\r\n", i, this_for_fuzz_wrapper[i]);
		//	dr_close_file(log_file);
		//}


		//for (i = 0; i < coverage_map_size / sizeof(long long); i++)
		//{
		//	if (coverage_map[i])
		//	{
		//		accumulative_coverage_map[i] = true;
		//		
		//		dr_snprintf(file_name, 1000, "C:\\RDPFuzz\\Logs\\winafl-log-%i.txt", msg_count);
		//		log_file = dr_open_file(file_name, DR_FILE_WRITE_APPEND);
		//		dr_fprintf(log_file, "mstscax+%x ; %lld\r\n", i, coverage_map[i]);
		//		dr_close_file(log_file);
		//	}
		//}


		//if ((msg_count % 1000) == 0)
		//{
		//	for (i = 0; i < coverage_map_size / sizeof(long); i++)
		//	{
		//		if (accumulative_coverage_map[i])
		//		{
		//			log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-log.txt", DR_FILE_WRITE_APPEND);
		//			dr_fprintf(log_file, "mstscax+%x\r\n", i);
		//			dr_close_file(log_file);
		//		}
		//	}
		//}

		//if (cmd_id == 6)
		//{
		//	for (i = 0; i < coverage_map_size / sizeof(long); i++)
		//	{
		//		if (coverage_map[i])
		//		{
		//			log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-log-cmd_id06.txt", DR_FILE_WRITE_APPEND);
		//			dr_fprintf(log_file, "mstscax+%x\r\n", i);
		//			dr_close_file(log_file);
		//		}
		//	}
		//}

		//if (coverage_map[0x377a9c])
		//{
		//	log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-log-CAPS.txt", DR_FILE_WRITE_APPEND);
		//	dr_fprintf(log_file, "[%ld]CAPS_OFF\r\n", msg_count);
		//	dr_close_file(log_file);

		//	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\restart.txt", DR_FILE_WRITE_OVERWRITE);
		//	//dr_fprintf(log_file, "%ld\r\n", msg_count);
		//	//dr_close_file(log_file);
		//}
		//if (coverage_map[0x377ae2])
		//{
		//	log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-log-CAPS.txt", DR_FILE_WRITE_APPEND);
		//	dr_fprintf(log_file, "[%ld]CAPS_ON\r\n", msg_count);
		//	dr_close_file(log_file);


		//	//for (i = 0; i < coverage_map_size / sizeof(long); i++)
		//	//{
		//	//	if (coverage_map[i])
		//	//	{
		//	//		log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-log-CAPS_ON_TO_OFF.txt", DR_FILE_WRITE_APPEND);
		//	//		dr_fprintf(log_file, "mstscax+%x\r\n", i);
		//	//		dr_close_file(log_file);
		//	//	}
		//	//}
		//}

		//if (coverage_map[0x382e50])
		//{
		//	log_file = dr_open_file("C:\\RDPFuzz\\Logs\\restart.txt", DR_FILE_WRITE_OVERWRITE);
		//	dr_fprintf(log_file, "%ld\r\n", msg_count);
		//	dr_close_file(log_file);
		//}
	}
}

static void
pre_error_log_handler(void *wrapcxt, INOUT void **user_data)
{
	//file_t log_file;
	void *drcontext = drwrap_get_drcontext(wrapcxt);
	DWORD error_code = (DWORD)(ULONGLONG)drwrap_get_arg(wrapcxt, 2);

	FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-error-log.txt", "%lld: error=%x\r\n", GetTickCount64(), error_code);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-error-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "%lld: error=%x\r\n", GetTickCount64(), error_code);
	//dr_close_file(log_file);
}

static void pre_ReconnectSoftwareMode(void *wrapcxt, INOUT void **user_data)
{
	//file_t log_file;
	this_for_reconnect_wrapper = (DWORD *)drwrap_get_arg(wrapcxt, 0);

	for (DWORD i = 0; i < 110; i++)
	{
		FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-before-reconnect-log.txt", "[%i]=%x\r\n", i, this_for_reconnect_wrapper[i]);
		//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-before-reconnect-log.txt", DR_FILE_WRITE_APPEND);
		//dr_fprintf(log_file, "[%i]=%x\r\n", i, this_for_reconnect_wrapper[i]);
		//dr_close_file(log_file);
	}


	//prev_off_46 = this_for_reconnect_wrapper[46];
	//prev_off_47 = this_for_reconnect_wrapper[47];
	//prev_off_48 = this_for_reconnect_wrapper[48];


	//this_for_reconnect_wrapper[49] = 1;

	//restore_data[0] = this_pointer[34];
}

static void post_ReconnectSoftwareMode(void *wrapcxt, INOUT void **user_data)
{
	//file_t log_file;

	for (DWORD i = 0; i < 110; i++)
	{
		FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-after-reconnect-log.txt", "[%i]=%x\r\n", i, this_for_reconnect_wrapper[i]);

		//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-after-reconnect-log.txt", DR_FILE_WRITE_APPEND);
		//dr_fprintf(log_file, "[%i]=%x\r\n", i, this_for_reconnect_wrapper[i]);
		//dr_close_file(log_file);
	}

	//this_for_reconnect_wrapper[46] = prev_off_46;
	//this_for_reconnect_wrapper[47] = prev_off_47;
	//this_for_reconnect_wrapper[48] = prev_off_48;
}

//static void pre_thread_alloc_0x2a53a0(void *wrapcxt, INOUT void **user_data)
//{
//	file_t log_file;
//
//	log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-thread-0x2a53a0-log.txt", DR_FILE_WRITE_APPEND);
//	dr_fprintf(log_file, "thread started\r\n");
//	dr_close_file(log_file);
//}
//
//static void pre_thread_alloc_0x2c1260(void *wrapcxt, INOUT void **user_data)
//{
//	file_t log_file;
//
//	log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-thread-0x2c1260-log.txt", DR_FILE_WRITE_APPEND);
//	dr_fprintf(log_file, "thread started\r\n");
//	dr_close_file(log_file);
//}

static void pre_d3d10warp(void *wrapcxt, INOUT void **user_data)
{
	//file_t log_file;
	//unsigned long long thread_mask;
	//CHAR create_dump_file_name[MAX_PATH];

	//void *drcontext = drwrap_get_drcontext(wrapcxt);

	//thread_mask = (unsigned long long)drmgr_get_tls_field(drcontext, winafl_tls_thread_mask_field);

	//EnterCriticalSection(&print_sync);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-d3d10warp-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "d3d10warp!UMContext::ScheduleRasterization reached\r\n");
	//dr_close_file(log_file);

	//for (DWORD i = 0; i < coverage_map_size / sizeof(coverage_map_entry); i++)
	//{
	//	if (coverage_map[i].thread_mask & thread_mask)
	//	{
	//		log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-d3d10warp-log.txt", DR_FILE_WRITE_APPEND);
	//		dr_fprintf(log_file, "mstscax+%x ; %lld ; %lld \r\n", i, coverage_map[i].last_accessed_iteration, coverage_map[i].thread_mask);
	//		dr_close_file(log_file);
	//	}
	//}

	//LeaveCriticalSection(&print_sync);


	//dr_snprintf(create_dump_file_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\create_d3d10warp_dump_%ld", GetCurrentProcessId());
	//log_file = dr_open_file(create_dump_file_name, DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "%ld\r\n", GetCurrentProcessId());
	//dr_close_file(log_file);

	//Sleep(60000);
}

static void pre_create_task(void *wrapcxt, INOUT void **user_data)
{
	file_t log_file;
	CHAR log_file_name[MAX_PATH];
	CHAR log_msg[1000];
	//CHAR create_dump_file_name[MAX_PATH];

	DWORD i;

	DWORD *cmdid_index;
	UCHAR *cmdid_buffer;

	void *parameters[5];

	void *drcontext = drwrap_get_drcontext(wrapcxt);
	app_pc ret_address = drwrap_get_retaddr(wrapcxt);

	for (i = 0; i < 5; i++)
	{
		parameters[i] = drwrap_get_arg(wrapcxt, i);
	}

	if (parameters[1] == d3d10.start + TASK_RASTERIZE_OFFSET) // only tracking rasterization tasks
	{
		cmdid_index = drmgr_get_tls_field(drcontext, winafl_tls_cmdid_index_field);
		cmdid_buffer = drmgr_get_tls_field(drcontext, winafl_tls_cmdid_buffer_field);

		dr_snprintf(log_msg, 1000,
			"CreateTask: base_address=%lld return_address=%lld\r\nparameters:%lld, %lld, %lld, %lld, %lld\r\ncmdid_index=%ld\r\n",
			(ULONGLONG)d3d10.start,
			(ULONGLONG)ret_address,
			parameters[0],
			parameters[1],
			parameters[2],
			parameters[3],
			parameters[4],
			*cmdid_index);

		WaitForSingleObject(log_mutex_handle, INFINITE);

		//EnterCriticalSection(&print_sync);

		dr_snprintf(log_file_name, 1000, "C:\\RDPFuzz\\Logs\\tasks_%ld.txt", GetCurrentProcessId());
		log_file = dr_open_file(log_file_name, DR_FILE_WRITE_APPEND);
		dr_fprintf(log_file, "%s", log_msg);

		for (i = 0; i < *cmdid_index; i++)
		{
			dr_fprintf(log_file, "%ld,", (long)cmdid_buffer[i]);
		}

		dr_close_file(log_file);

		ReleaseMutex(log_mutex_handle);

		//LeaveCriticalSection(&print_sync);

		//dr_snprintf(create_dump_file_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\create_crash_dump_%ld", GetCurrentProcessId());
		//log_file = dr_open_file(create_dump_file_name, DR_FILE_WRITE_APPEND);
		//dr_fprintf(log_file, "%ld\r\n", GetCurrentProcessId());
		//dr_close_file(log_file);

		//Sleep(60000);
	}

	//file_t log_file;
	//unsigned long long thread_mask;
	//CHAR create_dump_file_name[MAX_PATH];
	//void * ptask = drwrap_get_arg(wrapcxt, 1);
	//void *drcontext = drwrap_get_drcontext(wrapcxt);
	//app_pc ret_address = drwrap_get_retaddr(wrapcxt);

	//DWORD *cmdid_index;
	//UCHAR *cmdid_buffer;

	//if (ptask == d3d10.start + TASK_RASTERIZE_OFFSET)
	//{
	//	if (ret_address != d3d10.start + SETUP_TILES_RETURN_OFFSET)
	//	{
	//		cmdid_index = drmgr_get_tls_field(drcontext, winafl_tls_cmdid_index_field);
	//		cmdid_buffer = drmgr_get_tls_field(drcontext, winafl_tls_cmdid_buffer_field);

	//		if (*cmdid_index)
	//		{

	//			thread_mask = (unsigned long long)drmgr_get_tls_field(drcontext, winafl_tls_thread_mask_field);

	//			EnterCriticalSection(&print_sync);

	//			log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", DR_FILE_WRITE_APPEND);
	//			dr_fprintf(log_file, "CreateTask! msg_count=%ld, thread_mask=%lld, pid=%ld tid=%ld trace follows:\r\n", msg_count, thread_mask, GetCurrentProcessId(), GetCurrentThreadId());
	//			dr_close_file(log_file);

	//			log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", DR_FILE_WRITE_APPEND);
	//			dr_fprintf(log_file, "cmdid: count=%ld\r\n", *cmdid_index);
	//			dr_close_file(log_file);

	//			for (DWORD j = 0; j < CMDID_BUFFER_SIZE; j++)
	//			{
	//				log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", DR_FILE_WRITE_APPEND);
	//				dr_fprintf(log_file, "[%i] cmdid = %ld\r\n", j, (DWORD)cmdid_buffer[j]);
	//				dr_close_file(log_file);
	//			}

	//			for (DWORD i = 0; i < coverage_map_size / sizeof(coverage_map_entry); i++)
	//			{
	//				if (coverage_map[i].thread_mask & thread_mask)
	//				{
	//					log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", DR_FILE_WRITE_APPEND);
	//					dr_fprintf(log_file, "mstscax+%x ; %lld ; %lld \r\n", i, coverage_map[i].last_accessed_iteration, coverage_map[i].thread_mask);
	//					dr_close_file(log_file);
	//				}
	//			}

	//			LeaveCriticalSection(&print_sync);

				//dr_snprintf(create_dump_file_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\create_crash_dump_%ld", GetCurrentProcessId());
				//log_file = dr_open_file(create_dump_file_name, DR_FILE_WRITE_APPEND);
				//dr_fprintf(log_file, "%ld\r\n", GetCurrentProcessId());
				//dr_close_file(log_file);

				//Sleep(60000);
	//		}
	//	}
	//}



}

static void pre_execute_task(void *wrapcxt, INOUT void **user_data)
{
	//file_t log_file;
	CHAR log_file_name[MAX_PATH];
	CHAR log_msg[1000];

	BYTE *ptask;
	void *ptask_function;

	void *drcontext = drwrap_get_drcontext(wrapcxt);

	ptask = (BYTE *)drwrap_get_arg(wrapcxt, 0);
	ptask_function = (void *)(*(ULONGLONG *)(ptask + 0x98));

	if (ptask_function == d3d10.start + TASK_RASTERIZE_OFFSET)
	{
		dr_snprintf(log_msg, 1000,
			"ExecuteTask: ptask=%lld base_address=%lld function_address=%lld draw_queue=%lld index=%lld\r\n",
			(ULONGLONG)ptask,
			(ULONGLONG)d3d10.start,
			(ULONGLONG)ptask_function,
			*(ULONGLONG *)(ptask + 0xA8),
			*(ULONGLONG *)(ptask + 0xC0));

		//EnterCriticalSection(&print_sync);

		WaitForSingleObject(log_mutex_handle, INFINITE);

		dr_snprintf(log_file_name, 1000, "C:\\RDPFuzz\\Logs\\tasks_%ld.txt", GetCurrentProcessId());
		
		FUZZ_LOG_IF_ON(log_file_name, log_msg);
		//log_file = dr_open_file(log_file_name, DR_FILE_WRITE_APPEND);
		//dr_fprintf(log_file, "%s", log_msg);
		//dr_close_file(log_file);

		ReleaseMutex(log_mutex_handle);

		//LeaveCriticalSection(&print_sync);
	}
}

static void post_d3d10warp(void *wrapcxt, INOUT void **user_data)
{
	Sleep(1000);
}

static void pre_4ddf0c(void *wrapcxt, INOUT void **user_data)
{
	pthread_handle = (HANDLE *)drwrap_get_arg(wrapcxt, 0) + 3;
	//file_t log_file;
	ULONGLONG i;
	DWORD thread_id;
	ULONGLONG current_num_closed_handles;

	//FILETIME current_time;

	//FILETIME thread_creation_time;
	//FILETIME thread_exit_time;
	//FILETIME thread_kernel_time;
	//FILETIME thread_user_time;
	//file_t log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-thread-termination-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "%lld\r\n", *pthread_handle);
	//dr_close_file(log_file);

	if (*pthread_handle)
	{
		//if (CloseHandle(*pthread_handle))
		//{
		//	log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-thread-termination-success-log.txt", DR_FILE_WRITE_APPEND);
		//	dr_fprintf(log_file, "successfully closed handle %lld\r\n", *pthread_handle);
		//	dr_close_file(log_file);
		//}
		//else
		//{
		//	log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-thread-termination-success-log.txt", DR_FILE_WRITE_APPEND);
		//	dr_fprintf(log_file, "failed to close handle %lld\r\n", *pthread_handle);
		//	dr_close_file(log_file);
		//}

		current_num_closed_handles = InterlockedIncrement64(&num_closed_handles);

		FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-thread-termination-counter-log.txt", "%lld\r\n", current_num_closed_handles);

		//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-thread-termination-counter-log.txt", DR_FILE_WRITE_OVERWRITE);
		//dr_fprintf(log_file, "%lld\r\n", InterlockedIncrement64(&num_closed_handles));
		//dr_close_file(log_file);

		thread_id = GetThreadId(*pthread_handle);
		//GetSystemTimeAsFileTime(&current_time);

		for (i = 0; i < 0x10000u; i += 4)
		{
			if (GetThreadId((HANDLE)i) == thread_id && (HANDLE)i != *pthread_handle)
			{
				//if (GetThreadTimes((HANDLE)i, &thread_creation_time, &thread_exit_time, &thread_kernel_time, &thread_user_time))
				//{
				//	if (current_time.dwHighDateTime > thread_creation_time.dwHighDateTime || current_time.dwLowDateTime >= thread_creation_time.dwLowDateTime + 100000000)
				//	{
				//		// its been over ten secons since the thread was created, let's hope it is safe to close the handle
				//		CloseHandle((HANDLE)i);
				//	}
				//}

				FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-thread-termination-duplicate-handles-log.txt", 
					"first = %lld second = %lld\r\n", *pthread_handle, i);

				//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-thread-termination-duplicate-handles-log.txt", DR_FILE_WRITE_APPEND);
				//dr_fprintf(log_file, "first = %lld second = %lld\r\n", *pthread_handle, i);
				//dr_close_file(log_file);
			}
		}
	}
}

static void pre_thread_alloc_0x4de274(void *wrapcxt, INOUT void **user_data)
{
	//file_t log_file;

	pthread_handle = (HANDLE *)drwrap_get_arg(wrapcxt, 0) + 3;

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-thread-0x4de274-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "%lld: thread creation started\r\nthis_thread_context=%lld &this_thread_context[3]=%lld this_thread_context[3]=%lld\r\n", GetTickCount64(), this_thread_context, &this_thread_context[3], this_thread_context[3]);
	//dr_close_file(log_file);
}

static void post_thread_alloc_0x4de274(void *wrapcxt, INOUT void **user_data)
{
	//file_t log_file;

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-thread-0x4de274-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "%lld: thread creation finished\r\nthis_thread_context=%lld &this_thread_context[3]=%lld this_thread_context[3]=%lld\r\n", GetTickCount64(), this_thread_context, &this_thread_context[3], this_thread_context[3]);
	//dr_close_file(log_file);

	//if (this_thread_context[3])
	//{
	//	if (CloseHandle(this_thread_context[3]) == 0)
	//	{
	//		log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-thread-0x4de274-log.txt", DR_FILE_WRITE_APPEND);
	//		dr_fprintf(log_file, "%lld: CloseHandle failed last error = %ld\r\n", GetTickCount64(), GetLastError());
	//		dr_close_file(log_file);
	//	}
	//	else
	//	{
	//		log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-thread-0x4de274-log.txt", DR_FILE_WRITE_APPEND);
	//		dr_fprintf(log_file, "%lld: CloseHandle succeeded\r\n", GetTickCount64());
	//		dr_close_file(log_file);
	//	}
	//	this_thread_context[3] = NULL;
	//}

	DWORD current_counter = thread_handle_counter++;
	DWORD flags = 0;
	ULONGLONG current_num_thread_opened;
	//char file_name[1000];

	//if (GetHandleInformation(*pthread_handle, &flags) == 0)
	//{
	//	file_t log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-thread-flags-log.txt", DR_FILE_WRITE_APPEND);
	//	dr_fprintf(log_file, "GetHandleInformation failed for handle %lld\r\n", *pthread_handle);
	//	dr_close_file(log_file);
	//}
	//else
	//{
	//	file_t log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-thread-flags-log.txt", DR_FILE_WRITE_APPEND);
	//	dr_fprintf(log_file, "handle %lld: inherit=%i protect=%i\r\n", *pthread_handle, (flags & 1) == 1, (flags & 2) == 2);
	//	dr_close_file(log_file);
	//}

	//thread_handle_table[current_counter % 1000] = *pthread_handle;

	FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-thread-log.txt", "%lld\r\n", *pthread_handle);

	//file_t log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-thread-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "%lld\r\n", *pthread_handle);
	//dr_close_file(log_file);

	if (*pthread_handle)
	{
		current_num_thread_opened = InterlockedIncrement64(&num_threads_opened);
		FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-thread-creation-counter-log.txt", "%lld\r\n", current_num_thread_opened);

		//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-thread-creation-counter-log.txt", DR_FILE_WRITE_OVERWRITE);
		//dr_fprintf(log_file, "%lld\r\n", InterlockedIncrement64(&num_threads_opened));
		//dr_close_file(log_file);
	}

	//CloseHandle(*pthread_handle);
}

static void pre_thread_start(void *wrapcxt, INOUT void **user_data)
{
	//file_t log_file;

	FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-thread-log.txt", "thread %i start started\r\n", GetCurrentThreadId());

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-thread-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "thread %i start started\r\n", GetCurrentThreadId());
	//dr_close_file(log_file);
}

static void post_thread_start(void *wrapcxt, INOUT void **user_data)
{
	//file_t log_file;

	FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-thread-log.txt", "thread %i start finished\r\n", GetCurrentThreadId());

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-thread-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "thread %i start finished\r\n", GetCurrentThreadId());
	//dr_close_file(log_file);
}

static void pre_thread_stop(void *wrapcxt, INOUT void **user_data)
{
	//file_t log_file;

	HANDLE *this_thread = (HANDLE *)drwrap_get_arg(wrapcxt, 0);
	HANDLE this_handle = this_thread[0x56];

	FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-thread-stop-log.txt", "thread stopped handle = %lld\r\n", this_handle);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-thread-stop-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "thread stopped handle = %lld\r\n", this_handle);
	//dr_close_file(log_file);
}

static void post_thread_stop(void *wrapcxt, INOUT void **user_data)
{
	//file_t log_file;

	FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-thread-stop-log.txt", "thread %ld stop\r\n", GetCurrentThreadId());

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-thread-stop-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "thread %ld stop\r\n", GetCurrentThreadId());
	//dr_close_file(log_file);
}


static void
basic_pre_loop_start_handler(void *wrapcxt, INOUT void **user_data)
{
	void *drcontext = drwrap_get_drcontext(wrapcxt);
	//bool *inside;

	// mark that we are inside!
	//inside = drmgr_get_tls_field(drcontext, inside_winafl_tls_field);
	//inside[0] = true;

	if (!options.debug_mode) {
		if (fuzz_target.iteration == options.fuzz_iterations) {
			dr_exit_process(0);
		}
		fuzz_target.iteration++;

		//let server know we are starting a new cycle
		WriteCommandToPipe('P');

		//wait for server acknowledgement for cycle start
		char command = ReadCommandFromPipe();

		if (command != 'F') {
			if (command == 'Q') {
				dr_exit_process(0);
			}
			else {
				char errorMessage[] = "unrecognized command received over pipe: ";
				errorMessage[sizeof(errorMessage) - 2] = command;
				DR_ASSERT_MSG(false, errorMessage);
			}
		}
	}
	else {
		debug_data.pre_hanlder_called++;
		dr_fprintf(winafl_data.log, "In pre_loop_start_handler: %d\n", debug_data.pre_hanlder_called);
	}

	memset(winafl_data.afl_area, 0, MAP_SIZE);

	if (options.coverage_kind == COVERAGE_EDGE || options.thread_coverage) {
		void **thread_data = (void **)drmgr_get_tls_field(drcontext, winafl_tls_field);
		thread_data[0] = 0;
		thread_data[1] = winafl_data.afl_area;
	}
}

static void
basic_post_loop_start_handler(void *wrapcxt, INOUT void **user_data)
{
	void *drcontext = drwrap_get_drcontext(wrapcxt);
	//bool *inside;

	//inside = drmgr_get_tls_field(drcontext, inside_winafl_tls_field);

	if (inside)
	{
		// mark that we are outside!
		inside = false;
			
		//let server know we finished a cycle, redundunt on first cycle.
		WriteCommandToPipe('K');
	}
}


static void print_msg(PUCHAR msg, DWORD msg_size, bool last)
{
	//file_t log_file;
	char * print_msg = (char *)dr_global_alloc(msg_size * 10);
	char current[3];
	char fuzz_log_name[MAX_PATH];

	dr_snprintf(fuzz_log_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\winafl-received-msgs-%s.txt", &options.pipe_name[strlen("\\\\.\\pipe\\afl_pipe_")]);

	FUZZ_LOG_IF_ON(fuzz_log_name, "Incoming MSG [last=%ld]:\r\n", last);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-received_msgs-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "Incoming MSG:\r\n");
	//dr_close_file(log_file);

	print_msg[0] = 0;

	for (DWORD i = 0; i < msg_size; i++)
	{
		if ((i % 16) == 8)
		{
			strcat(print_msg, " ");
		}
		else if ((i % 16) == 0)
		{
			strcat(print_msg, "\r\n");
		}

		dr_snprintf(current, 3, "%02x", msg[i]);
		strcat(print_msg, current);
	}

	strcat(print_msg, "\r\n");

	FUZZ_LOG_IF_ON(fuzz_log_name, print_msg);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-received_msgs-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, print_msg);
	//dr_close_file(log_file);
	dr_global_free(print_msg, msg_size * 10);
}

static void pre_callback(void *wrapcxt, INOUT void **user_data)
{
	//file_t log_file;

	FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "pre_callback\r\n");

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "pre_callback\r\n");
	//dr_close_file(log_file);
}

static void post_callback(void *wrapcxt, INOUT void **user_data)
{
	//file_t log_file;

	FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "post_callback\r\n");

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "post_callback\r\n");
	//dr_close_file(log_file);
}

//static void
//pre_wrapper_handler(void *wrapcxt, INOUT void **user_data)
//{
//	bool fuzz_msg = false;
//	char fuzz_log_name[MAX_PATH];
//
//	dr_snprintf(fuzz_log_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\winafl-start-log-%i.txt", GetCurrentProcessId());
//
//	FUZZ_LOG_IF_ON(fuzz_log_name, "Wrapper got a message!\r\n");
//
//	void *drcontext = drwrap_get_drcontext(wrapcxt);
//
//	EnterCriticalSection(&wrapper_msg_handling_lock);
//
//	PUCHAR msg = (PUCHAR)drwrap_get_arg(wrapcxt, options.wrapper_data_param_number);
//	DWORD msg_size = (DWORD)(ULONGLONG)drwrap_get_arg(wrapcxt, options.wrapper_data_size_param_number);
//
//	wrap_last = false;
//
//	FUZZ_LOG_IF_ON(fuzz_log_name, "msg=%lld msg_size=%ld\r\n", (LONGLONG)msg, msg_size);
//
//	if (msg && msg_size >= options.wrapper_marker_offset)
//	{
//		if (strlen(options.wrapper_identifier))
//		{
//			if (memcmp(options.wrapper_identifier, msg + options.wrapper_marker_offset, strlen(options.wrapper_identifier)) == 0)
//			{
//				fuzz_msg = true;
//				wrap_last = true;
//			}
//		}
//		else
//		{
//			if (msg[options.marker_offset] >= options.marker)
//			{
//				fuzz_msg = true;
//
//				FUZZ_LOG_IF_ON(fuzz_log_name, "it is ours!\r\n");
//
//				if (msg[options.marker_offset] >= options.marker + options.last_msg_marker)
//				{
//					FUZZ_LOG_IF_ON(fuzz_log_name, "it is last in sequence!\r\n");
//
//					wrap_last = true;
//				}
//			}
//		}
//	}
//	if (!fuzz_msg)
//	{
//		LeaveCriticalSection(&wrapper_msg_handling_lock);
//		return;
//	}
//
//	//print_msg(msg, msg_size, last);
//
//	inside_wrapper = true;
//
//	wrap_msg_count = msg_count;
//}
//
//static void
//post_wrapper_handler(void *wrapcxt, INOUT void **user_data)
//{
//	void *drcontext = drwrap_get_drcontext(wrapcxt);
//	char fuzz_log_name[MAX_PATH];
//
//	dr_snprintf(fuzz_log_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\winafl-start-log-%i.txt", GetCurrentProcessId());
//
//	if (inside_wrapper)
//	{
//		Sleep(options.inside_timeout);	// give the normal flow the chance to deal with it
//		if (wrap_last)
//		{
//			// this is the last in a sequence: a command needs to be written on the pipe
//			
//			if (msg_count == wrap_msg_count)
//			{
//				// we have not handled this message
//				
//				if (!inside)
//				{
//					// we are not in the process of handling this message
//
//					// workaround so server does not wait for unanswered messages
//					WriteCommandToPipe('K');
//				}
//			}
//		}
//		LeaveCriticalSection(&fuzz_msg_handling_lock);
//	}
//}

static void
pre_loop_handler(void *wrapcxt, INOUT void **user_data)
{
	bool fuzz_msg = false;
	char fuzz_log_name[MAX_PATH];

	FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "Got a message!\r\n");

	dr_snprintf(fuzz_log_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\winafl-start-log-%i.txt", GetCurrentProcessId());

	FUZZ_LOG_IF_ON(fuzz_log_name, "Got a message!\r\n");

	void *drcontext = drwrap_get_drcontext(wrapcxt);
	bool *inside_winafl;
	unsigned char *thread_afl_map;
	
	PUCHAR msg;
	DWORD msg_size;

	EnterCriticalSection(&fuzz_msg_handling_lock);

	if (options.no_marker)
	{
		last = true;
		first = true;
	}
	else
	{
		msg = (PUCHAR)drwrap_get_arg(wrapcxt, options.data_param_number);
		if (options.data_size_param_number == 0xFFFFFFFFu)
		{
			msg_size = 0xFFFFFFFFu;
		}
		else
		{
			msg_size = (DWORD)(ULONGLONG)drwrap_get_arg(wrapcxt, options.data_size_param_number);
		}

		last = false;
		first = false;

		FUZZ_LOG_IF_ON(fuzz_log_name, "msg=%lld msg_size=%ld\r\n", (LONGLONG)msg, msg_size);

		FUZZ_LOG_IF_ON(fuzz_log_name, "options.marker_less_than=%ld\r\n", options.marker_less_than);

		//print_msg(msg, msg_size, last);

		if (options.marker_less_than)
		{
			if (msg && msg_size >= options.marker_offset)
			{
				if (msg[options.marker_offset] <= options.marker)
				{
					fuzz_msg = true;

					FUZZ_LOG_IF_ON(fuzz_log_name, "it is ours!\r\n");

					msg[options.marker_offset] += options.marker;

					if (msg[options.marker_offset] <= options.marker + options.last_msg_marker)
					{
						FUZZ_LOG_IF_ON(fuzz_log_name, "it is last in sequence!\r\n");

						last = true;
						msg[options.marker_offset] += options.last_msg_marker;
					}
				}
			}
		}
		else
		{
			if (options.marker_is_bits)
			{
				FUZZ_LOG_IF_ON(fuzz_log_name, "options.marker_is_bits\r\n", (LONGLONG)msg, msg_size);
				if (msg && msg_size >= options.marker_offset)
				{
					FUZZ_LOG_IF_ON(fuzz_log_name, "msg && msg_size >= options.marker_offset\r\n", (LONGLONG)msg, msg_size);
					if (msg[options.marker_offset] & options.marker)
					{
						fuzz_msg = true;

						FUZZ_LOG_IF_ON(fuzz_log_name, "it is ours!\r\n");

						msg[options.marker_offset] -= options.marker;

						if (msg[options.marker_offset] & options.last_msg_marker)
						{
							FUZZ_LOG_IF_ON(fuzz_log_name, "it is last in sequence!\r\n");

							last = true;
							msg[options.marker_offset] -= options.last_msg_marker;
						}
					}
				}
			}
			else
			{
				if (msg && msg_size >= options.marker_offset)
				{
					if (msg[options.marker_offset] >= options.marker)
					{
						fuzz_msg = true;

						FUZZ_LOG_IF_ON(fuzz_log_name, "it is ours!\r\n");

						msg[options.marker_offset] -= options.marker;

						if (msg[options.marker_offset] >= options.last_msg_marker)
						{
							FUZZ_LOG_IF_ON(fuzz_log_name, "it is last in sequence!\r\n");

							last = true;
							msg[options.marker_offset] -= options.last_msg_marker;
						}
					}
				}
			}
		}
		if (!fuzz_msg)
		{
			LeaveCriticalSection(&fuzz_msg_handling_lock);
			return;
		}
		//print_msg(msg, msg_size, last);
	}

	if (last)
	{
		msg_count++;
	}

	// mark that we are inside for this thread!
	inside_winafl = drmgr_get_tls_field(drcontext, inside_winafl_tls_field);
	inside_winafl[0] = true;

	// mark that we are inside (and first?) for all threads!
	if (!inside)
	{
		first = true;
		inside = true;
	}

	//if (!options.no_marker)
	//{
	//	accumulate_fuzz_msgs(msg, msg_size, first);
	//}

	FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-wrap-log.txt", "[pre] inside=%ld first=%ld last=%ld\r\n", (LONG)(inside), (LONG)first, (LONG)last);
	FUZZ_LOG_IF_ON(fuzz_log_name, "[pre] inside=%ld first=%ld last=%ld\r\n", (LONG)(inside), (LONG)first, (LONG)last);
	
	if (first)
	{
		FUZZ_LOG_IF_ON(fuzz_log_name, "[pre] 1\r\n");

		if (!options.debug_mode) {
			FUZZ_LOG_IF_ON(fuzz_log_name, "[pre] 2\r\n");
			if (fuzz_target.iteration == options.fuzz_iterations) {
				FUZZ_LOG_IF_ON(fuzz_log_name, "[pre] 3\r\n");
				dr_exit_process(0);
			}
			fuzz_target.iteration++;

			FUZZ_LOG_IF_ON(fuzz_log_name, "[pre] 4\r\n");

			//let server know we are starting a new cycle
			WriteCommandToPipe('P');

			FUZZ_LOG_IF_ON(fuzz_log_name, "[pre] 5\r\n");

			//wait for server acknowledgement for cycle start
			char command = ReadCommandFromPipe();

			FUZZ_LOG_IF_ON(fuzz_log_name, "[pre] 6\r\n");

			if (command != 'F') {
				FUZZ_LOG_IF_ON(fuzz_log_name, "[pre] 7\r\n");
				if (command == 'Q') {
					FUZZ_LOG_IF_ON(fuzz_log_name, "[pre] 8\r\n");
					dr_exit_process(0);
				}
				else {
					FUZZ_LOG_IF_ON(fuzz_log_name, "[pre] 9\r\n");
					char errorMessage[] = "unrecognized command received over pipe: ";
					errorMessage[sizeof(errorMessage) - 2] = command;
					DR_ASSERT_MSG(false, errorMessage);
				}
			}
		}
		else {
			FUZZ_LOG_IF_ON(fuzz_log_name, "[pre] 10\r\n");
			debug_data.pre_hanlder_called++;
			dr_fprintf(winafl_data.log, "In pre_loop_start_handler: %d\n", debug_data.pre_hanlder_called);
		}

		FUZZ_LOG_IF_ON(fuzz_log_name, "[pre] 11\r\n");
		memset(winafl_data.afl_area, 0, MAP_SIZE);
	}

	if (options.coverage_kind == COVERAGE_EDGE || options.thread_coverage) {
		FUZZ_LOG_IF_ON(fuzz_log_name, "[pre] 12\r\n");
		
		thread_afl_map = (unsigned char *)drmgr_get_tls_field(drcontext, thread_afl_map_tls_field);

		FUZZ_LOG_IF_ON(fuzz_log_name, "[pre] 13\r\n");

		memset(thread_afl_map, 0, MAP_SIZE);

		FUZZ_LOG_IF_ON(fuzz_log_name, "[pre] 14\r\n");

		void **thread_data = (void **)drmgr_get_tls_field(drcontext, winafl_tls_field);
		thread_data[0] = 0;
		thread_data[1] = thread_afl_map;

		FUZZ_LOG_IF_ON(fuzz_log_name, "[pre] 15\r\n");
	}

	FUZZ_LOG_IF_ON(fuzz_log_name, "[pre] 16\r\n");

	//if (!options.no_marker)
	//{
	//	zero_accumulate_coverage_map_for_current_thread(drcontext);
	//}

	FUZZ_LOG_IF_ON(fuzz_log_name, "[pre] finished\r\n");
}

static void
post_loop_handler(void *wrapcxt, INOUT void **user_data)
{
	void *drcontext = drwrap_get_drcontext(wrapcxt);
	bool *inside_winafl;
	unsigned char *thread_afl_map;
	DWORD i;
	char fuzz_log_name[MAX_PATH];

	dr_snprintf(fuzz_log_name, MAX_PATH, "C:\\RDPFuzz\\Logs\\winafl-start-log-%i.txt", GetCurrentProcessId());

	FUZZ_LOG_IF_ON(fuzz_log_name, "[post] starting\r\n");

	if (inside)
	{
		inside_winafl = drmgr_get_tls_field(drcontext, inside_winafl_tls_field);
		if (inside_winafl[0])
		{
			inside_winafl[0] = false;

			//if (!options.no_marker)
			//{
			//	accumulate_coverage_map(drcontext);
			//}

			thread_afl_map = (unsigned char *)drmgr_get_tls_field(drcontext, thread_afl_map_tls_field);

			FUZZ_LOG_IF_ON(fuzz_log_name, "[post] inside=%ld first=%ld last=%ld\r\n", (LONG)(inside), (LONG)first, (LONG)last);

			for (i = 0; i < MAP_SIZE; i++)
			{
				// collect coverage map from this thread
				winafl_data.afl_area[i] += thread_afl_map[i];
			}

			if (last)
			{
				//if (!options.no_marker)
				//{
				//	dump_accumulate_coverage_map(drcontext);
				//}

				// mark that we are outside!
				inside = false;

				//let server know we finished a cycle, redundunt on first cycle.
				WriteCommandToPipe('K');
			}

			FUZZ_LOG_IF_ON(fuzz_log_name, "[post] finished\r\n");

			LeaveCriticalSection(&fuzz_msg_handling_lock);
		}
	}
}


static void
marker_and_multiple_msgs_pre_loop_start_handler_2(void *wrapcxt, INOUT void **user_data)
{
	bool fuzz_msg = false;
	//file_t log_file; 

	FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "Got a message!\r\n");

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "Got a message!\r\n");
	//dr_close_file(log_file);

	void *drcontext = drwrap_get_drcontext(wrapcxt);
	bool *inside_winafl;
	unsigned char *thread_afl_map;

	PUCHAR msg = (PUCHAR)drwrap_get_arg(wrapcxt, 2);
	DWORD msg_size = (DWORD)(ULONGLONG)drwrap_get_arg(wrapcxt, 1);

	last = false;
	first = false;

	FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "msg=%lld msg_size=%ld\r\n", (LONGLONG)msg, msg_size);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "msg=%lld msg_size=%ld\r\n", (LONGLONG)msg, msg_size);
	//dr_close_file(log_file);

	//print_msg(msg, msg_size);

	if (msg && msg_size >= 8)
	{
		if (msg[8] >= 0xC0u)
		{
			fuzz_msg = true;

			FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "it is ours!\r\n");

			//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
			//dr_fprintf(log_file, "it is ours!\r\n");
			//dr_close_file(log_file);

			msg[8] -= 0xC0u;

			if (msg[8] >= 0x20u)
			{
				FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "it is last in sequence!\r\n");

				//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
				//dr_fprintf(log_file, "it is last in sequence!\r\n");
				//dr_close_file(log_file);

				last = true;
				msg[8] -= 0x20u;
			}
		}
	}
	//if (*(DWORD *)(msg + 12) >= 0x80000000)
	//{
	//	*(DWORD *)(msg + 12) -= 0x80000000;

	//	if (*(DWORD *)(msg + 12) >= 0x40000000)
	//	{
	//		last = true;
	//		*(DWORD *)(msg + 12) -= 0x40000000;
	//	}
	//}
	if (!fuzz_msg)
	{
		return;
	}

	EnterCriticalSection(&fuzz_msg_handling_lock);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "it is ours!\r\n");
	//dr_close_file(log_file);

	//dump_afl_map(drcontext, true);

	//print_msg(msg, msg_size);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "printed it!\r\n");
	//dr_close_file(log_file);

	if (last)
	{
		msg_count++;
	}

	// mark that we are inside for this thread!
	inside_winafl = drmgr_get_tls_field(drcontext, inside_winafl_tls_field);
	inside_winafl[0] = true;

	// mark that we are inside (and first?) for all threads!
	if (!inside)
	{
		first = true;
		inside = true;
	}

	FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-wrap-log.txt", "[pre] inside=%ld first=%ld last=%ld\r\n", (LONG)(inside), (LONG)first, (LONG)last);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-wrap-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "[pre] inside=%ld first=%ld last=%ld\r\n", (LONG)(inside), (LONG)first, (LONG)last);
	//dr_close_file(log_file);

	if (first)
	{
		if (!options.debug_mode) {
			if (fuzz_target.iteration == options.fuzz_iterations) {
				dr_exit_process(0);
			}
			fuzz_target.iteration++;

			//let server know we are starting a new cycle
			WriteCommandToPipe('P');

			//wait for server acknowledgement for cycle start
			char command = ReadCommandFromPipe();

			if (command != 'F') {
				if (command == 'Q') {
					dr_exit_process(0);
				}
				else {
					char errorMessage[] = "unrecognized command received over pipe: ";
					errorMessage[sizeof(errorMessage) - 2] = command;
					DR_ASSERT_MSG(false, errorMessage);
				}
			}
		}
		else {
			debug_data.pre_hanlder_called++;
			dr_fprintf(winafl_data.log, "In pre_loop_start_handler: %d\n", debug_data.pre_hanlder_called);
		}

		memset(winafl_data.afl_area, 0, MAP_SIZE);
	}

	if (options.coverage_kind == COVERAGE_EDGE || options.thread_coverage) {
		thread_afl_map = (unsigned char *)drmgr_get_tls_field(drcontext, thread_afl_map_tls_field);

		memset(thread_afl_map, 0, MAP_SIZE);

		void **thread_data = (void **)drmgr_get_tls_field(drcontext, winafl_tls_field);
		thread_data[0] = 0;
		thread_data[1] = thread_afl_map;
	}

	zero_accumulate_coverage_map_for_current_thread(drcontext);
}

static void
marker_and_multiple_msgs_post_loop_start_handler_2(void *wrapcxt, INOUT void **user_data)
{
	void *drcontext = drwrap_get_drcontext(wrapcxt);
	bool *inside_winafl;
	unsigned char *thread_afl_map;
	DWORD i;

	//file_t log_file;

	//inside = drmgr_get_tls_field(drcontext, inside_winafl_tls_field);

	if (inside)
	{
		inside_winafl = drmgr_get_tls_field(drcontext, inside_winafl_tls_field);
		if (inside_winafl[0])
		{
			inside_winafl[0] = false;

			accumulate_coverage_map(drcontext);

			LeaveCriticalSection(&fuzz_msg_handling_lock);

			thread_afl_map = (unsigned char *)drmgr_get_tls_field(drcontext, thread_afl_map_tls_field);

			FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-wrap-log.txt", "[post] inside=%ld first=%ld last=%ld\r\n", (LONG)(inside), (LONG)first, (LONG)last);

			//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-wrap-log.txt", DR_FILE_WRITE_APPEND);
			//dr_fprintf(log_file, "[post] inside=%ld first=%ld last=%ld\r\n", (LONG)(inside), (LONG)first, (LONG)last);
			//dr_close_file(log_file);

			for (i = 0; i < MAP_SIZE; i++)
			{
				// collect coverage map from this thread
				winafl_data.afl_area[i] += thread_afl_map[i];
				// collect coverage map from this message
				//joint_afl_area[i] += winafl_data.afl_area[i];
			}

			if (last)
			{
				dump_accumulate_coverage_map(drcontext);

				//dump_afl_map(drcontext, false);

				// write combined coverage map to the shared memory
				//memcpy(winafl_data.afl_area, joint_afl_area, MAP_SIZE);

				// mark that we are outside!
				inside = false;

				//let server know we finished a cycle, redundunt on first cycle.
				WriteCommandToPipe('K');

				// zero the combined coverage map
				//memset(joint_afl_area, 0, MAP_SIZE);
			}
		}
	}
}


static void
marker_and_multiple_msgs_pre_loop_start_handler(void *wrapcxt, INOUT void **user_data)
{
	bool fuzz_msg = false;
	//file_t log_file; 

	FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "Got a message!\r\n");

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "Got a message!\r\n");
	//dr_close_file(log_file);

	void *drcontext = drwrap_get_drcontext(wrapcxt);
	bool *inside_winafl;
	unsigned char *thread_afl_map;

	PUCHAR msg = (PUCHAR)drwrap_get_arg(wrapcxt, 2);
	DWORD msg_size = (DWORD)(ULONGLONG)drwrap_get_arg(wrapcxt, 1);

	last = false;
	first = false;

	FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "msg=%lld msg_size=%ld\r\n", (LONGLONG)msg, msg_size);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "msg=%lld msg_size=%ld\r\n", (LONGLONG)msg, msg_size);
	//dr_close_file(log_file);

	print_msg(msg, msg_size, last);

	if (msg && msg_size)
	{
		if (msg[0] >= 0xC0u)
		{
			fuzz_msg = true;

			FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "it is ours!\r\n");

			//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
			//dr_fprintf(log_file, "it is ours!\r\n");
			//dr_close_file(log_file);

			msg[0] -= 0xC0u;

			if (msg[0] >= 0x20u)
			{
				FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "it is last in sequence!\r\n");

				//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
				//dr_fprintf(log_file, "it is last in sequence!\r\n");
				//dr_close_file(log_file);

				last = true;
				msg[0] -= 0x20u;
			}
		}
	}
	//if (*(DWORD *)(msg + 12) >= 0x80000000)
	//{
	//	*(DWORD *)(msg + 12) -= 0x80000000;

	//	if (*(DWORD *)(msg + 12) >= 0x40000000)
	//	{
	//		last = true;
	//		*(DWORD *)(msg + 12) -= 0x40000000;
	//	}
	//}
	if (!fuzz_msg)
	{
		return;
	}

	EnterCriticalSection(&fuzz_msg_handling_lock);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "it is ours!\r\n");
	//dr_close_file(log_file);

	//dump_afl_map(drcontext, true);

	//print_msg(msg, msg_size);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "printed it!\r\n");
	//dr_close_file(log_file);

	if (last)
	{
		msg_count++;
	}

	// mark that we are inside for this thread!
	inside_winafl = drmgr_get_tls_field(drcontext, inside_winafl_tls_field);
	inside_winafl[0] = true;
	
	// mark that we are inside (and first?) for all threads!
	if (!inside)
	{
		first = true;
		inside = true;
	}

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-wrap-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "[pre] inside=%ld first=%ld last=%ld\r\n", (LONG)(inside), (LONG)first, (LONG)last);
	//dr_close_file(log_file);

	if (first)
	{
		if (!options.debug_mode) {
			if (fuzz_target.iteration == options.fuzz_iterations) {
				dr_exit_process(0);
			}
			fuzz_target.iteration++;

			//let server know we are starting a new cycle
			WriteCommandToPipe('P');

			//wait for server acknowledgement for cycle start
			char command = ReadCommandFromPipe();

			if (command != 'F') {
				if (command == 'Q') {
					dr_exit_process(0);
				}
				else {
					char errorMessage[] = "unrecognized command received over pipe: ";
					errorMessage[sizeof(errorMessage) - 2] = command;
					DR_ASSERT_MSG(false, errorMessage);
				}
			}
		}
		else {
			debug_data.pre_hanlder_called++;
			dr_fprintf(winafl_data.log, "In pre_loop_start_handler: %d\n", debug_data.pre_hanlder_called);
		}

		memset(winafl_data.afl_area, 0, MAP_SIZE);
	}

	if (options.coverage_kind == COVERAGE_EDGE || options.thread_coverage) {
		thread_afl_map = (unsigned char *)drmgr_get_tls_field(drcontext, thread_afl_map_tls_field);

		memset(thread_afl_map, 0, MAP_SIZE);

		void **thread_data = (void **)drmgr_get_tls_field(drcontext, winafl_tls_field);
		thread_data[0] = 0;
		thread_data[1] = thread_afl_map;
	}

	zero_accumulate_coverage_map_for_current_thread(drcontext);
}

static void
marker_and_multiple_msgs_post_loop_start_handler(void *wrapcxt, INOUT void **user_data)
{
	void *drcontext = drwrap_get_drcontext(wrapcxt);
	bool *inside_winafl;
	unsigned char *thread_afl_map;
	DWORD i;

	//file_t log_file;

	//inside = drmgr_get_tls_field(drcontext, inside_winafl_tls_field);

	if (inside)
	{
		inside_winafl = drmgr_get_tls_field(drcontext, inside_winafl_tls_field);
		if (inside_winafl[0])
		{
			inside_winafl[0] = false;

			//accumulate_coverage_map(drcontext);

			LeaveCriticalSection(&fuzz_msg_handling_lock);

			thread_afl_map = (unsigned char *)drmgr_get_tls_field(drcontext, thread_afl_map_tls_field);
		
			FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-wrap-log.txt", "[post] inside=%ld first=%ld last=%ld\r\n", (LONG)(inside), (LONG)first, (LONG)last);

			//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-wrap-log.txt", DR_FILE_WRITE_APPEND);
			//dr_fprintf(log_file, "[post] inside=%ld first=%ld last=%ld\r\n", (LONG)(inside), (LONG)first, (LONG)last);
			//dr_close_file(log_file);

			for (i = 0; i < MAP_SIZE; i++)
			{
				// collect coverage map from this thread
				winafl_data.afl_area[i] += thread_afl_map[i];
				// collect coverage map from this message
				//joint_afl_area[i] += winafl_data.afl_area[i];
			}

			if (last)
			{
				//dump_accumulate_coverage_map(drcontext);

				//dump_afl_map(drcontext, false);

				// write combined coverage map to the shared memory
				//memcpy(winafl_data.afl_area, joint_afl_area, MAP_SIZE);

				// mark that we are outside!
				inside = false;

				//let server know we finished a cycle, redundunt on first cycle.
				WriteCommandToPipe('K');

				// zero the combined coverage map
				//memset(joint_afl_area, 0, MAP_SIZE);
			}
		}
	}
}


static void
pre_loop_start_handler(void *wrapcxt, INOUT void **user_data)
{
	void *drcontext = drwrap_get_drcontext(wrapcxt);
	//bool *inside;
	DWORD index = 2;
	DWORD size = (DWORD)(ULONGLONG)drwrap_get_arg(wrapcxt, 1);

	//file_t log_file;

	//dump_afl_map(drcontext, true);

	this_for_fuzz_wrapper = (DWORD *)drwrap_get_arg(wrapcxt, 0);

	this_for_fuzz_wrapper[34] = 1;
	//this_for_fuzz_wrapper[35] = 0;
	//this_for_fuzz_wrapper[36] = 1;

	PUCHAR ours = (PUCHAR)drwrap_get_arg(wrapcxt, 2);

	if (ours[0] == 0 || ours[0] == 1)
	{
		ours[0] += 0xE0u;
		print_mem_diff(true);
	}
	else if (ours[0] == 2)
	{
		ours[0] += 0xDEu;
		//print_mem_diff(true);

		return;
	}
	else
	{
		return;
	}

	DWORD *cmdid_index = drmgr_get_tls_field(drcontext, winafl_tls_cmdid_index_field);
	*cmdid_index = 0;

	UCHAR *cmdid_buffer = drmgr_get_tls_field(drcontext, winafl_tls_cmdid_buffer_field);
	memset(cmdid_buffer, 0, CMDID_BUFFER_SIZE);

	//file_t log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "size=%ld\r\n", size);
	//dr_close_file(log_file);

	while (index < size && *cmdid_index < CMDID_BUFFER_SIZE)
	{
		//file_t log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", DR_FILE_WRITE_APPEND);
		//dr_fprintf(log_file, "ours=%lld size=%ld index=%ld *cmdid_index=%ld ours[index]=%ld\r\n", ours, size, index, *cmdid_index, (DWORD)ours[index]);
		//dr_close_file(log_file);


		cmdid_buffer[*cmdid_index] = ours[index];
		(*cmdid_index)++;
		index += *(DWORD *)(ours + index + 4);
	}
	
	//for (DWORD j = 0; j < CMDID_BUFFER_SIZE; j++)
	//{
	//	file_t log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-crash-log.txt", DR_FILE_WRITE_APPEND);
	//	dr_fprintf(log_file, "[%i] cmdid = %ld\r\n", j, (DWORD)cmdid_buffer[j]);
	//	dr_close_file(log_file);
	//}

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "%lld: in\r\n", GetTickCount64(), fuzz_target.iteration);
	//dr_close_file(log_file);

	msg_count++;

	dump_afl_map(drcontext, true);

	treat_cmd_id = true;

	cmd_id = ours[2];

	// mark that we are inside!
	//inside = drmgr_get_tls_field(drcontext, inside_winafl_tls_field);
	inside = true;

	//if (!is_ours(wrapcxt))
	//{
	//	return;
	//}

	if (ours[2] == 0x13)
	{
		treat_cmd_id = false;
		return;
	}

	if (!options.debug_mode) {
		if (fuzz_target.iteration == options.fuzz_iterations) {
			dr_exit_process(0);
		}
		fuzz_target.iteration++;

		//let server know we are starting a new cycle
		WriteCommandToPipe('P');

		//wait for server acknowledgement for cycle start
		char command = ReadCommandFromPipe();

		if (command != 'F') {
			if (command == 'Q') {
				dr_exit_process(0);
			}
			else {
				char errorMessage[] = "unrecognized command received over pipe: ";
				errorMessage[sizeof(errorMessage) - 2] = command;
				DR_ASSERT_MSG(false, errorMessage);
			}
		}
	}
	else {
		debug_data.pre_hanlder_called++;
		dr_fprintf(winafl_data.log, "In pre_loop_start_handler: %d\n", debug_data.pre_hanlder_called);
	}

	memset(winafl_data.afl_area, 0, MAP_SIZE);

	if (options.coverage_kind == COVERAGE_EDGE || options.thread_coverage) {
		void **thread_data = (void **)drmgr_get_tls_field(drcontext, winafl_tls_field);
		thread_data[0] = 0;
		thread_data[1] = winafl_data.afl_area;
	}
}

static void
post_loop_start_handler(void *wrapcxt, INOUT void **user_data)
{
	void *drcontext = drwrap_get_drcontext(wrapcxt);
	//bool *inside;
	//file_t log_file;
	ULONGLONG rc;

	//dump_afl_map(drcontext, false);

	//inside = drmgr_get_tls_field(drcontext, inside_winafl_tls_field);

	if (inside)
	{
		//this_for_fuzz_wrapper[40] = 0;
		print_mem_diff(false);

		dump_afl_map(drcontext, false);

		//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-log.txt", DR_FILE_WRITE_APPEND);
		//dr_fprintf(log_file, "%lld: out\r\n", GetTickCount64());
		//dr_close_file(log_file);

		// mark that we are outside!
		inside = false;

		if (treat_cmd_id)
		{
			treat_cmd_id = false;
			//let server know we finished a cycle, redundunt on first cycle.
			WriteCommandToPipe('K');

			rc = (ULONGLONG)drwrap_get_retval(wrapcxt);
			if (rc)
			{
				//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-log.txt", DR_FILE_WRITE_APPEND);
				//dr_fprintf(log_file, "failed! rc=%lld\r\n", rc);
				//dr_close_file(log_file);
			}

			//for (DWORD i = 0; i < 1000; i++)
			//{
			//	if (thread_handle_table[i])
			//	{
			//		CloseHandle(thread_handle_table[i]);
			//		thread_handle_table[i] = NULL;
			//	}
			//}
		}
	}
}

static void
pre_audio_loop_start_handler(void *wrapcxt, INOUT void **user_data)
{
	void *drcontext = drwrap_get_drcontext(wrapcxt);
	//bool *inside;

	PUCHAR data = (PUCHAR)drwrap_get_arg(wrapcxt, 2);

	last = false;
	first = false;

	if (data[0]  >= 0x10)
	{
		data[0] -= 0x10u;

		if (data[0] >= 0x10)
		{
			last = true;
			data[0] -= 0x10u;
		}
	}
	else
	{
		return;
	}

	if (last)
	{
		msg_count++;
	}

	dump_afl_map(drcontext, true);

	// mark that we are inside!
	//inside = drmgr_get_tls_field(drcontext, inside_winafl_tls_field);
	if (!inside)
	{
		first = true;
		inside = true;
	}


	if (first)
	{
		if (!options.debug_mode) {
			if (fuzz_target.iteration == options.fuzz_iterations) {
				dr_exit_process(0);
			}
			fuzz_target.iteration++;

			//let server know we are starting a new cycle
			WriteCommandToPipe('P');

			//wait for server acknowledgement for cycle start
			char command = ReadCommandFromPipe();

			if (command != 'F') {
				if (command == 'Q') {
					dr_exit_process(0);
				}
				else {
					char errorMessage[] = "unrecognized command received over pipe: ";
					errorMessage[sizeof(errorMessage) - 2] = command;
					DR_ASSERT_MSG(false, errorMessage);
				}
			}
		}
		else {
			debug_data.pre_hanlder_called++;
			dr_fprintf(winafl_data.log, "In pre_loop_start_handler: %d\n", debug_data.pre_hanlder_called);
		}
	}

	if (first)
	{
		memset(winafl_data.afl_area, 0, MAP_SIZE);
	}

	if (options.coverage_kind == COVERAGE_EDGE || options.thread_coverage) {
		void **thread_data = (void **)drmgr_get_tls_field(drcontext, winafl_tls_field);
		thread_data[0] = 0;
		thread_data[1] = winafl_data.afl_area;
	}
}

static void
post_audio_loop_start_handler(void *wrapcxt, INOUT void **user_data)
{
	void *drcontext = drwrap_get_drcontext(wrapcxt);
	//bool *inside;

	//inside = drmgr_get_tls_field(drcontext, inside_winafl_tls_field);

	if (inside)
	{
		dump_afl_map(drcontext, false);

		if (last)
		{
			// mark that we are outside!
			inside = false;

			//let server know we finished a cycle, redundunt on first cycle.
			WriteCommandToPipe('K');
		}
	}
}


static void
pre_geometry_loop_start_handler(void *wrapcxt, INOUT void **user_data)
{
	void *drcontext = drwrap_get_drcontext(wrapcxt);
	//bool *inside;

	DWORD * data = (DWORD *)drwrap_get_arg(wrapcxt, 2);

	if (data[1] >= 0x10)
	{
		last = false;
		first = false;

		data[1] -= 0x10u;

		if (data[1] >= 0x10)
		{
			last = true;
			data[1] -= 0x10u;
		}
	}
	else
	{
		return;
	}

	if (last)
	{
		msg_count++;
	}

	dump_afl_map(drcontext, true);

	// mark that we are inside!
	//inside = drmgr_get_tls_field(drcontext, inside_winafl_tls_field);
	if (!inside)
	{
		first = true;
		inside = true;
	}


	if (first)
	{
		if (!options.debug_mode) {
			if (fuzz_target.iteration == options.fuzz_iterations) {
				dr_exit_process(0);
			}
			fuzz_target.iteration++;

			//let server know we are starting a new cycle
			WriteCommandToPipe('P');

			//wait for server acknowledgement for cycle start
			char command = ReadCommandFromPipe();

			if (command != 'F') {
				if (command == 'Q') {
					dr_exit_process(0);
				}
				else {
					char errorMessage[] = "unrecognized command received over pipe: ";
					errorMessage[sizeof(errorMessage) - 2] = command;
					DR_ASSERT_MSG(false, errorMessage);
				}
			}
		}
		else {
			debug_data.pre_hanlder_called++;
			dr_fprintf(winafl_data.log, "In pre_loop_start_handler: %d\n", debug_data.pre_hanlder_called);
		}
	}

	if (first)
	{
		memset(winafl_data.afl_area, 0, MAP_SIZE);
	}

	if (options.coverage_kind == COVERAGE_EDGE || options.thread_coverage) {
		void **thread_data = (void **)drmgr_get_tls_field(drcontext, winafl_tls_field);
		thread_data[0] = 0;
		thread_data[1] = winafl_data.afl_area;
	}
}

static void
post_geometry_loop_start_handler(void *wrapcxt, INOUT void **user_data)
{
	void *drcontext = drwrap_get_drcontext(wrapcxt);
	//bool *inside;

	//inside = drmgr_get_tls_field(drcontext, inside_winafl_tls_field);

	if (inside)
	{
		dump_afl_map(drcontext, false);

		if (last)
		{
			// mark that we are outside!
			inside = false;

			//let server know we finished a cycle, redundunt on first cycle.
			WriteCommandToPipe('K');
		}
	}
}


static void
pre_video_loop_start_handler(void *wrapcxt, INOUT void **user_data)
{
	void *drcontext = drwrap_get_drcontext(wrapcxt);
	//bool *inside;

	DWORD * data = (DWORD *)drwrap_get_arg(wrapcxt, 2);

	if (data[1] >= 0x10)
	{
		last = false;
		first = false;

		data[1] -= 0x10u;

		if (data[1] >= 0x10)
		{
			last = true;
			data[1] -= 0x10u;
		}
	}
	else
	{
		return;
	}

	if (last)
	{
		msg_count++;
	}

	dump_afl_map(drcontext, true);

	// mark that we are inside!
	//inside = drmgr_get_tls_field(drcontext, inside_winafl_tls_field);
	if (!inside)
	{
		first = true;
		inside = true;
	}


	if (first)
	{
		if (!options.debug_mode) {
			if (fuzz_target.iteration == options.fuzz_iterations) {
				dr_exit_process(0);
			}
			fuzz_target.iteration++;

			//let server know we are starting a new cycle
			WriteCommandToPipe('P');

			//wait for server acknowledgement for cycle start
			char command = ReadCommandFromPipe();

			if (command != 'F') {
				if (command == 'Q') {
					dr_exit_process(0);
				}
				else {
					char errorMessage[] = "unrecognized command received over pipe: ";
					errorMessage[sizeof(errorMessage) - 2] = command;
					DR_ASSERT_MSG(false, errorMessage);
				}
			}
		}
		else {
			debug_data.pre_hanlder_called++;
			dr_fprintf(winafl_data.log, "In pre_loop_start_handler: %d\n", debug_data.pre_hanlder_called);
		}
	}

	if (first)
	{
		memset(winafl_data.afl_area, 0, MAP_SIZE);
	}

	if (options.coverage_kind == COVERAGE_EDGE || options.thread_coverage) {
		void **thread_data = (void **)drmgr_get_tls_field(drcontext, winafl_tls_field);
		thread_data[0] = 0;
		thread_data[1] = winafl_data.afl_area;
	}
}

static void
post_video_loop_start_handler(void *wrapcxt, INOUT void **user_data)
{
	void *drcontext = drwrap_get_drcontext(wrapcxt);
	//bool *inside;

	//inside = drmgr_get_tls_field(drcontext, inside_winafl_tls_field);

	if (inside)
	{
		dump_afl_map(drcontext, false);

		if (last)
		{
			// mark that we are outside!
			inside = false;

			//let server know we finished a cycle, redundunt on first cycle.
			WriteCommandToPipe('K');
		}
	}
}

static void
pre_fuzz_handler(void *wrapcxt, INOUT void **user_data)
{
	char command = 0;
	int i;
	void *drcontext;

	app_pc target_to_fuzz = drwrap_get_func(wrapcxt);
	dr_mcontext_t *mc = drwrap_get_mcontext_ex(wrapcxt, DR_MC_ALL);
	drcontext = drwrap_get_drcontext(wrapcxt);

	fuzz_target.xsp = mc->xsp;
	fuzz_target.func_pc = target_to_fuzz;

	if (!options.debug_mode) {
		WriteCommandToPipe('P');
		command = ReadCommandFromPipe();

		if (command != 'F') {
			if (command == 'Q') {
				dr_exit_process(0);
			}
			else {
				DR_ASSERT_MSG(false, "unrecognized command received over pipe");
			}
		}
	}
	else {
		debug_data.pre_hanlder_called++;
		dr_fprintf(winafl_data.log, "In pre_fuzz_handler\n");
	}

	//save or restore arguments
	if (!options.no_loop) {
		if (fuzz_target.iteration == 0) {
			for (i = 0; i < options.num_fuz_args; i++)
				options.func_args[i] = drwrap_get_arg(wrapcxt, i);
		}
		else {
			for (i = 0; i < options.num_fuz_args; i++)
				drwrap_set_arg(wrapcxt, i, options.func_args[i]);
		}
	}

	memset(winafl_data.afl_area, 0, MAP_SIZE);

	if (options.coverage_kind == COVERAGE_EDGE || options.thread_coverage) {
		void **thread_data = (void **)drmgr_get_tls_field(drcontext, winafl_tls_field);
		thread_data[0] = 0;
		thread_data[1] = winafl_data.afl_area;
	}
}

static void
post_fuzz_handler(void *wrapcxt, void *user_data)
{
	dr_mcontext_t *mc;
	mc = drwrap_get_mcontext(wrapcxt);

	if (!options.debug_mode) {
		WriteCommandToPipe('K');
	}
	else {
		debug_data.post_handler_called++;
		dr_fprintf(winafl_data.log, "In post_fuzz_handler\n");
	}

	/* We don't need to reload context in case of network-based fuzzing. */
	if (options.no_loop)
		return;

	fuzz_target.iteration++;
	if (fuzz_target.iteration == options.fuzz_iterations) {
		dr_exit_process(0);
	}

	mc->xsp = fuzz_target.xsp;
	mc->pc = fuzz_target.func_pc;
	drwrap_redirect_execution(wrapcxt);
}

static void
createfilew_interceptor(void *wrapcxt, INOUT void **user_data)
{
	wchar_t *filenamew = (wchar_t *)drwrap_get_arg(wrapcxt, 0);
	if (options.debug_mode)
		dr_fprintf(winafl_data.log, "In OpenFileW, reading %ls\n", filenamew);
}

static void
createfilea_interceptor(void *wrapcxt, INOUT void **user_data)
{
	char *filename = (char *)drwrap_get_arg(wrapcxt, 0);
	if (options.debug_mode)
		dr_fprintf(winafl_data.log, "In OpenFileA, reading %s\n", filename);
}

static void
verfierstopmessage_interceptor_pre(void *wrapctx, INOUT void **user_data)
{
	EXCEPTION_RECORD exception_record = { 0 };
	dr_exception_t dr_exception = { 0 };
	dr_exception.record = &exception_record;
	exception_record.ExceptionCode = STATUS_HEAP_CORRUPTION;

	onexception(NULL, &dr_exception);
}

static void
recvfrom_interceptor(void *wrapcxt, INOUT void **user_data)
{
	if (options.debug_mode)
		dr_fprintf(winafl_data.log, "In recvfrom\n");
}

static void
recv_interceptor(void *wrapcxt, INOUT void **user_data)
{
	if (options.debug_mode)
		dr_fprintf(winafl_data.log, "In recv\n");
}

static void
isprocessorfeaturepresent_interceptor_pre(void *wrapcxt, INOUT void **user_data)
{
	DWORD feature = (DWORD)(ULONGLONG)drwrap_get_arg(wrapcxt, 0);
	*user_data = (void*)(ULONGLONG)feature;
}

static void
isprocessorfeaturepresent_interceptor_post(void *wrapcxt, void *user_data)
{
	DWORD feature = (DWORD)(ULONGLONG)user_data;
	if (feature == PF_FASTFAIL_AVAILABLE) {
		if (options.debug_mode) {
			dr_fprintf(winafl_data.log, "About to make IsProcessorFeaturePresent(%d) returns 0\n", feature);
		}

		// Make the software thinks that _fastfail() is not supported.
		drwrap_set_retval(wrapcxt, (void*)0);
	}
}

static void
unhandledexceptionfilter_interceptor_pre(void *wrapcxt, INOUT void **user_data)
{
	PEXCEPTION_POINTERS exception = (PEXCEPTION_POINTERS)drwrap_get_arg(wrapcxt, 0);
	dr_exception_t dr_exception = { 0 };

	// Fake an exception
	dr_exception.record = exception->ExceptionRecord;
	onexception(NULL, &dr_exception);
}

static void
event_module_unload(void *drcontext, const module_data_t *info)
{
	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[event_module_unload] starting\r\n");
	module_table_unload(module_table, info);
}

static void post_flush_callback(int flush_id)
{
	//file_t log_file;

	FUZZ_LOG_IF_ON("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "post_flush_callback called\r\n");

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "post_flush_callback called\r\n");
	//dr_close_file(log_file);
}

static void
event_module_load(void *drcontext, const module_data_t *info, bool loaded)
{
	const char *module_name = info->names.exe_name;
	app_pc to_wrap = 0;
	app_pc to_replace = 0;
	app_pc replace_with = 0;
	//file_t log_file;

	if (module_name == NULL) {
		// In case exe_name is not defined, we will fall back on the preferred name.
		module_name = dr_module_preferred_name(info);
	}

	if (options.debug_mode)
		dr_fprintf(winafl_data.log, "Module loaded, %s\n", module_name);

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[event_module_load] loading %s\r\n", module_name);

	if (options.fuzz_module[0]) {
		if (_stricmp(module_name, options.fuzz_module) == 0) {

			FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "found module=%s\r\n", module_name);

			//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
			//dr_fprintf(log_file, "found module=%s\r\n", module_name);
			//dr_close_file(log_file);

			if (options.offset_to_replace && options.offset_to_replace_with)
			{
				to_replace = info->start + options.offset_to_replace;
				replace_with = info->start + options.offset_to_replace_with;

				if (drwrap_replace(to_replace, replace_with, true))
				{
					FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "replaced %lld+%lld=%lld with %lld+%lld=%lld\r\n", (ULONGLONG)info->start, (ULONGLONG)options.offset_to_replace, (ULONGLONG)to_replace, (ULONGLONG)info->start, (ULONGLONG)options.offset_to_replace_with, (ULONGLONG)replace_with);
				}
				else
				{
					FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "failed to replace %lld with %lld\r\n", (ULONGLONG)to_replace, (ULONGLONG)replace_with);
				}
			}

			if (options.fuzz_offset) {
				to_wrap = info->start + options.fuzz_offset;

				FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "to_wrap=%lld 1st_ll=%016llx\r\n", (ULONGLONG)to_wrap, *(ULONGLONG *)to_wrap);

				//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
				//dr_fprintf(log_file, "to_wrap=%lld\r\n", (ULONGLONG)to_wrap);
				//dr_close_file(log_file);
			}
			else {
				//first try exported symbols
				to_wrap = (app_pc)dr_get_proc_address(info->handle, options.fuzz_method);
				if (!to_wrap) {
					//if that fails, try with the symbol access library
#ifdef USE_DRSYMS
					drsym_init(0);
					drsym_lookup_symbol(info->full_path, options.fuzz_method, (size_t *)(&to_wrap), 0);
					drsym_exit();
#endif
					DR_ASSERT_MSG(to_wrap, "Can't find specified method in fuzz_module");
					to_wrap += (size_t)info->start;
				}
			}
			if (options.persistence_mode == native_mode)
			{
				drwrap_wrap_ex(to_wrap, pre_fuzz_handler, post_fuzz_handler, NULL, options.callconv);
			}
			if (options.persistence_mode == in_app)
			{
				FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "wrapping persistence_mode==in_app!\r\n");

				//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
				//dr_fprintf(log_file, "wrapping persistence_mode==in_app!\r\n");
				//dr_close_file(log_file);

				//drwrap_wrap_ex(to_wrap, pre_loop_start_handler, post_loop_start_handler, NULL, options.callconv);
				//drwrap_wrap_ex(to_wrap, pre_audio_loop_start_handler, post_audio_loop_start_handler, NULL, options.callconv);
				//drwrap_wrap_ex(to_wrap, pre_geometry_loop_start_handler, post_geometry_loop_start_handler, NULL, options.callconv);
				//drwrap_wrap_ex(to_wrap, pre_video_loop_start_handler, post_video_loop_start_handler, NULL, options.callconv);
				//if (drwrap_wrap_ex(to_wrap, marker_and_multiple_msgs_pre_loop_start_handler,
				//	marker_and_multiple_msgs_post_loop_start_handler,
				//	NULL, options.callconv))
				//if (drwrap_wrap_ex(to_wrap, marker_and_multiple_msgs_pre_loop_start_handler, marker_and_multiple_msgs_post_loop_start_handler, NULL, options.callconv))
				if (drwrap_wrap_ex(to_wrap, pre_loop_handler, post_loop_handler, NULL, options.callconv))
				{
					FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "succesfully wrapped target function\r\n");

					//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
					//dr_fprintf(log_file, "succesfully wrapped target function\r\n");
					//dr_close_file(log_file);

					if (dr_unlink_flush_region(to_wrap, 1))
					//if (dr_delay_flush_region(to_wrap, 1, 0x12345, post_flush_callback))
					{
						FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "succesfully flsuhed function region\r\n");
					
						//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
						//dr_fprintf(log_file, "succesfully flsuhed function region\r\n");
						//dr_close_file(log_file);
					
						//if (drwrap_is_wrapped(to_wrap,
						//	marker_and_multiple_msgs_pre_loop_start_handler,
						//	marker_and_multiple_msgs_post_loop_start_handler))
						if (drwrap_is_wrapped(to_wrap, pre_loop_handler, post_loop_handler))
						{
							FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "target function is wrapped\r\n");
					
							//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
							//dr_fprintf(log_file, "target function is wrapped\r\n");
							//dr_close_file(log_file);
						}
						else
						{
							FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "target function is NOT wrapped\r\n");
					
							//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
							//dr_fprintf(log_file, "target function is NOT wrapped\r\n");
							//dr_close_file(log_file);
						}
					}
					else
					{
						FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "failed to flush function region\r\n");
					
						//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
						//dr_fprintf(log_file, "failed to flush function region\r\n");
						//dr_close_file(log_file);
					}
				}
				else
				{
					FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "failed to wrap target function\r\n");

					//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
					//dr_fprintf(log_file, "failed to wrap target function\r\n");
					//dr_close_file(log_file);
				}

				//if (options.fuzz_wrapper)
				//{
				//	to_wrap = info->start + options.fuzz_wrapper_offset;
				//	drwrap_wrap_ex(to_wrap, pre_wrapper_handler, post_wrapper_handler, NULL, options.callconv);
				//}
			}

			//to_wrap = info->start + 0x381944;
			//drwrap_wrap_ex(to_wrap, pre_error_log_handler, NULL, NULL, options.callconv);

			//to_wrap = info->start + 0x2a53a0;
			//drwrap_wrap_ex(to_wrap, pre_thread_alloc_0x2a53a0, NULL, NULL, options.callconv);

			//to_wrap = info->start + 0x2c1260;
			//drwrap_wrap_ex(to_wrap, pre_thread_alloc_0x2c1260, NULL, NULL, options.callconv);

			//to_wrap = info->start + 0x4de274;
			//drwrap_wrap_ex(to_wrap, pre_thread_alloc_0x4de274, post_thread_alloc_0x4de274, NULL, options.callconv);

			//to_wrap = info->start + 0x2c1600;
			//drwrap_wrap_ex(to_wrap, pre_thread_start, post_thread_start, NULL, options.callconv);

			//to_wrap = info->start + 0x2b98f8;
			//drwrap_wrap_ex(to_wrap, pre_thread_stop, NULL, NULL, options.callconv);

			//to_wrap = info->start + 0x1BE5A0;
			//drwrap_wrap_ex(to_wrap, pre_ReconnectSoftwareMode, post_ReconnectSoftwareMode, NULL, options.callconv);

			//// this is a very bad hack - we replace the function start with its return
			//to_wrap = info->start + 0xD64C0;
			//drwrap_replace(to_wrap, info->start + 0xD64D4, true);

			//to_wrap = info->start + 0x4ddf0c;
			//drwrap_wrap_ex(to_wrap, pre_4ddf0c, NULL, NULL, options.callconv);
		}

		//if (_stricmp(module_name, "d3d10warp.dll") == 0)
		//{
		//	memcpy(&d3d10, info, sizeof(d3d10));

		//	to_wrap = info->start + CREATE_TASK_OFFSET;
		//	drwrap_wrap_ex(to_wrap, pre_create_task, NULL, NULL, options.callconv);

		//	to_wrap = info->start + EXECUTE_TASK_OFFSET;
		//	drwrap_wrap_ex(to_wrap, pre_execute_task, NULL, NULL, options.callconv);
		//}

		if (options.debug_mode && (_stricmp(module_name, "WS2_32.dll") == 0)) {
			to_wrap = (app_pc)dr_get_proc_address(info->handle, "recvfrom");
			bool result = drwrap_wrap(to_wrap, recvfrom_interceptor, NULL);
			to_wrap = (app_pc)dr_get_proc_address(info->handle, "recv");
			result = drwrap_wrap(to_wrap, recv_interceptor, NULL);
		}

		if (options.debug_mode && (_stricmp(module_name, "KERNEL32.dll") == 0)) {
			to_wrap = (app_pc)dr_get_proc_address(info->handle, "CreateFileW");
			drwrap_wrap(to_wrap, createfilew_interceptor, NULL);
			to_wrap = (app_pc)dr_get_proc_address(info->handle, "CreateFileA");
			drwrap_wrap(to_wrap, createfilea_interceptor, NULL);
		}

		if (_stricmp(module_name, "kernelbase.dll") == 0) {
			// Since Win8, software can use _fastfail() to trigger an exception that cannot be caught.
			// This is a problem for winafl as it also means DR won't be able to see it. Good thing is that
			// usually those routines (__report_gsfailure for example) accounts for platforms that don't
			// have support for fastfail. In those cases, they craft an exception record and pass it
			// to UnhandledExceptionFilter.
			//
			// To work around this we set up two hooks:
			//   (1) IsProcessorFeaturePresent(PF_FASTFAIL_AVAILABLE): to lie and pretend that the
			//       platform doesn't support fastfail.
			//   (2) UnhandledExceptionFilter: to intercept the exception record and forward it
			//       to winafl's exception handler.
			to_wrap = (app_pc)dr_get_proc_address(info->handle, "IsProcessorFeaturePresent");
			drwrap_wrap(to_wrap, isprocessorfeaturepresent_interceptor_pre, isprocessorfeaturepresent_interceptor_post);
			to_wrap = (app_pc)dr_get_proc_address(info->handle, "UnhandledExceptionFilter");
			drwrap_wrap(to_wrap, unhandledexceptionfilter_interceptor_pre, NULL);
		}
	}

	if (_stricmp(module_name, "verifier.dll") == 0) {
		to_wrap = (app_pc)dr_get_proc_address(info->handle, "VerifierStopMessage");
		drwrap_wrap(to_wrap, verfierstopmessage_interceptor_pre, NULL);
	}

	module_table_load(module_table, info);
}

static void
event_exit(void)
{
	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[event_exit] starting\r\n");

	if (options.debug_mode) {
		if (debug_data.pre_hanlder_called == 0) {
			dr_fprintf(winafl_data.log, "WARNING: Target function was never called. Incorrect target_offset?\n");
		}
		else if (debug_data.post_handler_called == 0 && options.persistence_mode != in_app) {
			dr_fprintf(winafl_data.log, "WARNING: Post-fuzz handler was never reached. Did the target function return normally?\n");
		}
		else if (debug_data.pre_hanlder_called == 1 && options.persistence_mode == in_app) {
			dr_fprintf(winafl_data.log, "WARNING: Only hit pre_loop_start_handler once, Is your target function in a loop?\n");
		}
		else {
			dr_fprintf(winafl_data.log, "Everything appears to be running normally.\n");
		}

		dr_fprintf(winafl_data.log, "Coverage map follows:\n");
		dump_winafl_data();
		dr_close_file(winafl_data.log);
	}

	/* destroy module table */
	module_table_destroy(module_table);

	drx_exit();
	drmgr_exit();
}

static void
event_init(void)
{
	char buf[MAXIMUM_PATH];

	module_table = module_table_create();

	memset(winafl_data.cache, 0, sizeof(winafl_data.cache));
	memset(winafl_data.afl_area, 0, MAP_SIZE);

	if (options.debug_mode) {
		debug_data.pre_hanlder_called = 0;
		debug_data.post_handler_called = 0;

		winafl_data.log =
			drx_open_unique_appid_file(options.logdir, dr_get_process_id(),
				"afl", "proc.log",
				DR_FILE_ALLOW_LARGE,
				buf, BUFFER_SIZE_ELEMENTS(buf));
		if (winafl_data.log != INVALID_FILE) {
			dr_log(NULL, LOG_ALL, 1, "winafl: log file is %s\n", buf);
			NOTIFY(1, "<created log file %s>\n", buf);
		}
	}

	fuzz_target.iteration = 0;
}


static void
setup_pipe() {
	pipe = CreateFile(
		options.pipe_name,   // pipe name
		GENERIC_READ |  // read and write access
		GENERIC_WRITE,
		0,              // no sharing
		NULL,           // default security attributes
		OPEN_EXISTING,  // opens existing pipe
		0,              // default attributes
		NULL);          // no template file

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[setup_pipe] pipe_name=%s pipe=%lld last_error=%ld\r\n", options.pipe_name, (ULONGLONG)pipe, GetLastError());

	//file_t log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "[setup_pipe] pipe_name=%s pipe=%lld last_error=%ld\r\n", options.pipe_name, (ULONGLONG)pipe, GetLastError());
	//dr_close_file(log_file);

	if (pipe == INVALID_HANDLE_VALUE) DR_ASSERT_MSG(false, "error connecting to pipe");
}

static void
setup_shmem() {
	HANDLE map_file;

	map_file = OpenFileMapping(
		FILE_MAP_ALL_ACCESS,   // read/write access
		FALSE,                 // do not inherit the name
		options.shm_name);            // name of mapping object

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[setup_shmem] tickcount=%ld shm_name=%s map_file=%lld last_error=%ld\r\n", GetTickCount(), options.shm_name, (ULONGLONG)map_file, GetLastError());

	//file_t log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "[setup_shmem] tickcount=%ld shm_name=%s map_file=%lld last_error=%ld\r\n", GetTickCount(), options.shm_name, (ULONGLONG)map_file, GetLastError());
	//dr_close_file(log_file);

	if (map_file == NULL) DR_ASSERT_MSG(false, "error accesing shared memory");

	winafl_data.afl_area = (unsigned char *)MapViewOfFile(map_file, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		MAP_SIZE);

	if (winafl_data.afl_area == NULL) DR_ASSERT_MSG(false, "error accesing shared memory");

	//joint_afl_area = (unsigned char *)dr_global_alloc(MAP_SIZE);
	//if (joint_afl_area == NULL) DR_ASSERT_MSG(false, "error allocating joint afl area");
}

static void
options_init(client_id_t id, int argc, const char *argv[])
{
	int i;
	const char *token;
	target_module_t *target_modules;
	/* default values */
	options.persistence_mode = native_mode;
	options.nudge_kills = true;
	options.debug_mode = false;
	options.thread_coverage = false;
	options.coverage_kind = COVERAGE_BB;
	options.target_modules = NULL;
	options.target_all_modules = false;
	options.fuzz_module[0] = 0;
	options.fuzz_method[0] = 0;
	options.fuzz_offset = 0;
	options.fuzz_iterations = 1000;
	options.no_loop = false;
	options.func_args = NULL;
	options.num_fuz_args = 0;
	options.callconv = DRWRAP_CALLCONV_DEFAULT;
	options.dr_persist_cache = false;
	options.fuzz_log = false;
	//options.fuzz_wrapper = false;
	//options.fuzz_wrapper_offset = 0;
	//options.wrapper_data_param_number = 1;
	//options.wrapper_data_size_param_number = 2;
	//options.wrapper_marker_offset = 0;
	options.data_param_number = 1;
	options.data_size_param_number = 2;
	options.marker_offset = 0;
	options.marker = 0xC0u;
	options.last_msg_marker = 0x20u;
	options.marker_less_than = false;
	options.marker_is_bits = false;
	options.offset_to_replace = 0;
	options.offset_to_replace_with = 0;
	options.no_marker = false;
	dr_snprintf(options.logdir, BUFFER_SIZE_ELEMENTS(options.logdir), ".");

	strcpy(options.pipe_name, "\\\\.\\pipe\\afl_pipe_default");
	strcpy(options.shm_name, "afl_shm_default");

	for (i = 1/*skip client*/; i < argc; i++) {
		token = argv[i];
		if (strcmp(token, "-no_nudge_kills") == 0)
			options.nudge_kills = false;
		else if (strcmp(token, "-nudge_kills") == 0)
			options.nudge_kills = true;
		else if (strcmp(token, "-thread_coverage") == 0)
			options.thread_coverage = true;
		else if (strcmp(token, "-debug") == 0)
			options.debug_mode = true;
		else if (strcmp(token, "-logdir") == 0) {
			USAGE_CHECK((i + 1) < argc, "missing logdir path");
			strncpy(options.logdir, argv[++i], BUFFER_SIZE_ELEMENTS(options.logdir));
		}
		else if (strcmp(token, "-fuzzer_id") == 0) {
			USAGE_CHECK((i + 1) < argc, "missing fuzzer id");
			strcpy(options.pipe_name, "\\\\.\\pipe\\afl_pipe_");
			strcpy(options.shm_name, "Global\\afl_shm_");
			strcat(options.pipe_name, argv[i + 1]);
			strcat(options.shm_name, argv[i + 1]);
			i++;
		}
		else if (strcmp(token, "-covtype") == 0) {
			USAGE_CHECK((i + 1) < argc, "missing coverage type");
			token = argv[++i];
			if (strcmp(token, "bb") == 0) options.coverage_kind = COVERAGE_BB;
			else if (strcmp(token, "edge") == 0) options.coverage_kind = COVERAGE_EDGE;
			else USAGE_CHECK(false, "invalid coverage type");
		}
		else if (strcmp(token, "-coverage_module") == 0) {
			USAGE_CHECK((i + 1) < argc, "missing module");
			target_modules = options.target_modules;
			options.target_modules = (target_module_t *)dr_global_alloc(sizeof(target_module_t));
			options.target_modules->next = target_modules;
			strncpy(options.target_modules->module_name, argv[++i], BUFFER_SIZE_ELEMENTS(options.target_modules->module_name));
		}
		else if (strcmp(token, "-target_module") == 0) {
			USAGE_CHECK((i + 1) < argc, "missing module");
			strncpy(options.fuzz_module, argv[++i], BUFFER_SIZE_ELEMENTS(options.fuzz_module));
		}
		else if (strcmp(token, "-target_method") == 0) {
			USAGE_CHECK((i + 1) < argc, "missing method");
			strncpy(options.fuzz_method, argv[++i], BUFFER_SIZE_ELEMENTS(options.fuzz_method));
		}
		else if (strcmp(token, "-fuzz_iterations") == 0) {
			USAGE_CHECK((i + 1) < argc, "missing number of iterations");
			options.fuzz_iterations = atoi(argv[++i]);
		}
		else if (strcmp(token, "-nargs") == 0) {
			USAGE_CHECK((i + 1) < argc, "missing number of arguments");
			options.num_fuz_args = atoi(argv[++i]);
		}
		else if (strcmp(token, "-target_offset") == 0) {
			USAGE_CHECK((i + 1) < argc, "missing offset");
			options.fuzz_offset = strtoul(argv[++i], NULL, 0);
		}
		else if (strcmp(token, "-verbose") == 0) {
			USAGE_CHECK((i + 1) < argc, "missing -verbose number");
			token = argv[++i];
			if (dr_sscanf(token, "%u", &verbose) != 1) {
				USAGE_CHECK(false, "invalid -verbose number");
			}
		}
		else if (strcmp(token, "-call_convention") == 0) {
			USAGE_CHECK((i + 1) < argc, "missing calling convention");
			++i;
			if (strcmp(argv[i], "stdcall") == 0)
				options.callconv = DRWRAP_CALLCONV_CDECL;
			else if (strcmp(argv[i], "fastcall") == 0)
				options.callconv = DRWRAP_CALLCONV_FASTCALL;
			else if (strcmp(argv[i], "thiscall") == 0)
				options.callconv = DRWRAP_CALLCONV_THISCALL;
			else if (strcmp(argv[i], "ms64") == 0)
				options.callconv = DRWRAP_CALLCONV_MICROSOFT_X64;
			else
				NOTIFY(0, "Unknown calling convention, using default value instead.\n");
		}
		else if (strcmp(token, "-no_loop") == 0) {
			options.no_loop = true;
		}
		else if (strcmp(token, "-drpersist") == 0) {
			options.dr_persist_cache = true;
		}
		else if (strcmp(token, "-persistence_mode") == 0) {
			USAGE_CHECK((i + 1) < argc, "missing mode arg: '-fuzz_mode' arg");
			const char* mode = argv[++i];
			if (strcmp(mode, "in_app") == 0)
			{
				options.persistence_mode = in_app;
			}
			else
			{
				options.persistence_mode = native_mode;
			}
		}
		else if (strcmp(token, "-fuzz_log") == 0) {
			options.fuzz_log = true;
		}
		//else if (strcmp(token, "-fuzz_wrapper") == 0) {
		//	options.fuzz_wrapper = true;
		//}
		//else if (strcmp(token, "-inside_timeout") == 0) {
		//	USAGE_CHECK((i + 1) < argc, "missing inside_timeout");
		//	options.inside_timeout = strtoul(argv[++i], NULL, 0);
		//}
		//else if (strcmp(token, "-fuzz_wrapper_offset") == 0) {
		//	USAGE_CHECK((i + 1) < argc, "missing fuzz_wrapper_offset");
		//	options.fuzz_wrapper_offset = strtoul(argv[++i], NULL, 0);
		//}
		//else if (strcmp(token, "-wrapper_data_param_number") == 0) {
		//USAGE_CHECK((i + 1) < argc, "missing wrapper_data_param_number");
		//options.wrapper_data_param_number = strtoul(argv[++i], NULL, 0);
		//}
		//else if (strcmp(token, "-wrapper_data_size_param_number") == 0) {
		//USAGE_CHECK((i + 1) < argc, "missing wrapper_data_size_param_number");
		//options.wrapper_data_size_param_number = strtoul(argv[++i], NULL, 0);
		//}
		//else if (strcmp(token, "-wrapper_marker_offset") == 0) {
		//USAGE_CHECK((i + 1) < argc, "missing wrapper_marker_offset");
		//options.wrapper_marker_offset = strtoul(argv[++i], NULL, 0);
		//}
		//else if (strcmp(token, "-wrapper_identifier") == 0) {
		//USAGE_CHECK((i + 1) < argc, "missing wrapper_identifier");
		//strncpy(options.wrapper_identifier, argv[++i], BUFFER_SIZE_ELEMENTS(options.wrapper_identifier));
		//}
		//else if (strcmp(token, "-inside_timeout") == 0) {
		//	USAGE_CHECK((i + 1) < argc, "missing inside_timeout");
		//	options.inside_timeout = strtoul(argv[++i], NULL, 0);
		//}
		else if (strcmp(token, "-data_param_number") == 0) {
			USAGE_CHECK((i + 1) < argc, "missing data_param_number");
			options.data_param_number = strtoul(argv[++i], NULL, 0);
		}
		else if (strcmp(token, "-data_size_param_number") == 0) {
			USAGE_CHECK((i + 1) < argc, "missing data_size_param_number");
			options.data_size_param_number = strtoul(argv[++i], NULL, 0);
		}
		else if (strcmp(token, "-marker_offset") == 0) {
		USAGE_CHECK((i + 1) < argc, "missing marker_offset");
		options.marker_offset = strtoul(argv[++i], NULL, 0);
		}
		else if (strcmp(token, "-marker") == 0) {
		USAGE_CHECK((i + 1) < argc, "missing marker");
		options.marker = (unsigned char)strtoul(argv[++i], NULL, 0);
		}
		else if (strcmp(token, "-last_msg_marker") == 0) {
		USAGE_CHECK((i + 1) < argc, "missing last_msg_marker");
		options.last_msg_marker = (unsigned char)strtoul(argv[++i], NULL, 0);
		}
		else if (strcmp(token, "-marker_less_than") == 0) {
		options.marker_less_than = true;
		}
		else if (strcmp(token, "-marker_is_bits") == 0) {
		options.marker_is_bits = true;
		}
		else if (strcmp(token, "-offset_to_replace") == 0) {
		USAGE_CHECK((i + 1) < argc, "missing offset_to_replace");
		options.offset_to_replace = strtoul(argv[++i], NULL, 0);
		}
		else if (strcmp(token, "-offset_to_replace_with") == 0) {
		USAGE_CHECK((i + 1) < argc, "missing offset_to_replace_with");
		options.offset_to_replace_with = strtoul(argv[++i], NULL, 0);
		}
		else if (strcmp(token, "-no_marker") == 0) {
		options.no_marker = true;
		}
		else if (strcmp(token, "-target_all_modules") == 0) {
		options.target_all_modules = true;
		}
		else {
			NOTIFY(0, "UNRECOGNIZED OPTION: \"%s\"\n", token);
			USAGE_CHECK(false, "invalid option");
		}
	}

	if (options.fuzz_module[0] && (options.fuzz_offset == 0) && (options.fuzz_method[0] == 0)) {
		USAGE_CHECK(false, "If fuzz_module is specified, then either fuzz_method or fuzz_offset must be as well");
	}

	if (options.num_fuz_args) {
		options.func_args = (void **)dr_global_alloc(options.num_fuz_args * sizeof(void *));
	}
}

//static unsigned int __stdcall timeout_thread(void * param)
//{
//	// this is a hack 
//
//	dr_mark_safe_to_suspend(dr_get_current_drcontext(), true);
//	dr_sleep(60000);
//	dr_mark_safe_to_suspend(dr_get_current_drcontext(), false);
//
//	last_msg_tick_count = GetTickCount();
//
//	while (true)
//	{
//		dr_mark_safe_to_suspend(dr_get_current_drcontext(), true);
//		dr_sleep(1000);
//		dr_mark_safe_to_suspend(dr_get_current_drcontext(), false);
//
//		if (msg_count)
//		{
//			if (GetTickCount() - last_msg_tick_count > options.inside_timeout)
//			{
//				//WriteCommandToPipe('P');
//				//ReadCommandFromPipe();
//				WriteCommandToPipe('K');
//			}
//		}
//	}
//}

//void start_timeout_thread()
//{
//	dr_create_client_thread(timeout_thread, NULL);
//}

DR_EXPORT void
dr_client_main(client_id_t id, int argc, const char *argv[])
{
	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] starting\r\n");

	//file_t log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "[dr_client_main] starting\r\n");
	//dr_close_file(log_file);

	drreg_options_t ops = { sizeof(ops), 2 /*max slots needed: aflags*/, false };

	dr_set_client_name("WinAFL", "");

	drmgr_init();
	drx_init();
	drreg_init(&ops);
	drwrap_init();

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] starting\r\n");

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "[dr_client_main] starting\r\n");
	//dr_close_file(log_file);

	options_init(id, argc, argv);

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] after options_init\r\n");

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "[dr_client_main] after options_init\r\n");
	//dr_close_file(log_file);

	dr_register_exit_event(event_exit);

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] after dr_register_exit_event\r\n");

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "[dr_client_main] after dr_register_exit_event\r\n");
	//dr_close_file(log_file);

	drmgr_register_exception_event(onexception);

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] after drmgr_register_exception_event\r\n");

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "[dr_client_main] after drmgr_register_exception_event\r\n");
	//dr_close_file(log_file);

	drmgr_register_bb_app2app_event(app_instrument_bb_coverage, NULL);

	if (options.coverage_kind == COVERAGE_BB) {
		drmgr_register_bb_instrumentation_event(NULL, instrument_bb_coverage, NULL);

		FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] after drmgr_register_bb_instrumentation_event(NULL, instrument_bb_coverage, NULL)\r\n");

		//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
		//dr_fprintf(log_file, "[dr_client_main] after drmgr_register_bb_instrumentation_event(NULL, instrument_bb_coverage, NULL)\r\n");
		//dr_close_file(log_file);
	}
	else if (options.coverage_kind == COVERAGE_EDGE) {
		drmgr_register_bb_instrumentation_event(NULL, instrument_edge_coverage, NULL);

		FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] after drmgr_register_bb_instrumentation_event(NULL, instrument_edge_coverage, NULL)\r\n");

		//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
		//dr_fprintf(log_file, "[dr_client_main] after drmgr_register_bb_instrumentation_event(NULL, instrument_edge_coverage, NULL)\r\n");
		//dr_close_file(log_file);
	}

	drmgr_register_module_load_event(event_module_load);
	drmgr_register_module_unload_event(event_module_unload);
	dr_register_nudge_event(event_nudge, id);

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] after dr_register_nudge_event\r\n");

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "[dr_client_main] after dr_register_nudge_event\r\n");
	//dr_close_file(log_file);

	client_id = id;

	if (options.nudge_kills)
		drx_register_soft_kills(event_soft_kill);

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] after drx_register_soft_kills\r\n");

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "[dr_client_main] after drx_register_soft_kills\r\n");
	//dr_close_file(log_file);

	if (options.thread_coverage) {
		winafl_data.fake_afl_area = (unsigned char *)dr_global_alloc(MAP_SIZE);

		FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] after winafl_data.fake_afl_area = (unsigned char *)dr_global_alloc(MAP_SIZE)\r\n");

		//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
		//dr_fprintf(log_file, "[dr_client_main] after winafl_data.fake_afl_area = (unsigned char *)dr_global_alloc(MAP_SIZE)\r\n");
		//dr_close_file(log_file);
	}

	if (!options.debug_mode) {
		FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] before setup_pipe\r\n");

		//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
		//dr_fprintf(log_file, "[dr_client_main] before setup_pipe\r\n");
		//dr_close_file(log_file);

		setup_pipe();

		FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] after setup_pipe\r\n");

		//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
		//dr_fprintf(log_file, "[dr_client_main] after setup_pipe\r\n");
		//dr_close_file(log_file);

		setup_shmem();

		FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] after setup_shmem\r\n");

		//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
		//dr_fprintf(log_file, "[dr_client_main] after setup_shmem\r\n");
		//dr_close_file(log_file);
	}
	else {
		winafl_data.afl_area = (unsigned char *)dr_global_alloc(MAP_SIZE);
	}

	winafl_tls_field_registered = false;

	if (options.coverage_kind == COVERAGE_EDGE || options.thread_coverage || options.dr_persist_cache) {
		winafl_tls_field = drmgr_register_tls_field();

		FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] winafl_tls_field=%ld\r\n", winafl_tls_field);

		//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
		//dr_fprintf(log_file, "[dr_client_main] winafl_tls_field=%ld\r\n", winafl_tls_field);
		//dr_close_file(log_file);

		if (winafl_tls_field == -1) {
			DR_ASSERT_MSG(false, "error reserving TLS field");
		}

		winafl_tls_field_registered = true;
	}

	inside_winafl_tls_field = drmgr_register_tls_field();

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] inside_winafl_tls_field=%ld\r\n", inside_winafl_tls_field);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "[dr_client_main] inside_winafl_tls_field=%ld\r\n", inside_winafl_tls_field);
	//dr_close_file(log_file);

	if (inside_winafl_tls_field == -1) {
		DR_ASSERT_MSG(false, "error reserving TLS field");
	}

	inside_winafl_tls_field_registered = true;

	thread_afl_map_tls_field = drmgr_register_tls_field();

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] thread_afl_map_tls_field=%ld\r\n", thread_afl_map_tls_field);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "[dr_client_main] thread_afl_map_tls_field=%ld\r\n", thread_afl_map_tls_field);
	//dr_close_file(log_file);

	if (thread_afl_map_tls_field == -1) {
		DR_ASSERT_MSG(false, "error reserving TLS field");
	}

	thread_afl_map_tls_field_reistered = true;

	winafl_tls_coverage_map_field = drmgr_register_tls_field();

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] winafl_tls_coverage_map_field=%ld\r\n", winafl_tls_coverage_map_field);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "[dr_client_main] winafl_tls_coverage_map_field=%ld\r\n", winafl_tls_coverage_map_field);
	//dr_close_file(log_file);

	if (winafl_tls_coverage_map_field == -1) {
		DR_ASSERT_MSG(false, "error reserving TLS field");
	}
	winafl_tls_coverage_map_registered = true;

	winafl_tls_thread_mask_field = drmgr_register_tls_field();

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] winafl_tls_thread_mask_field=%ld\r\n", winafl_tls_thread_mask_field);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "[dr_client_main] winafl_tls_thread_mask_field=%ld\r\n", winafl_tls_thread_mask_field);
	//dr_close_file(log_file);

	if (winafl_tls_thread_mask_field == -1) {
		DR_ASSERT_MSG(false, "error reserving TLS field");
	}
	winafl_tls_thread_mask_registered = true;

	winafl_tls_cmdid_buffer_field = drmgr_register_tls_field();

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] winafl_tls_cmdid_buffer_field=%ld\r\n", winafl_tls_cmdid_buffer_field);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "[dr_client_main] winafl_tls_cmdid_buffer_field=%ld\r\n", winafl_tls_cmdid_buffer_field);
	//dr_close_file(log_file);

	if (winafl_tls_cmdid_buffer_field == -1) {
		DR_ASSERT_MSG(false, "error reserving TLS field");
	}
	winafl_tls_cmdid_buffer_registered = true;

	winafl_tls_cmdid_index_field = drmgr_register_tls_field();

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] winafl_tls_cmdid_index_field=%ld\r\n", winafl_tls_cmdid_index_field);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "[dr_client_main] winafl_tls_cmdid_index_field=%ld\r\n", winafl_tls_cmdid_index_field);
	//dr_close_file(log_file);

	if (winafl_tls_cmdid_index_field == -1) {
		DR_ASSERT_MSG(false, "error reserving TLS field");
	}
	winafl_tls_cmdid_index_registered = true;

	coverage_map_size = sizeof(coverage_map_entry) * MAX_COVERAGE_MAP_SIZE;

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] coverage_map_size=%ld\r\n", coverage_map_size);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "[dr_client_main] coverage_map_size=%ld\r\n", coverage_map_size);
	//dr_close_file(log_file);

	coverage_map = (coverage_map_entry *)dr_global_alloc(coverage_map_size);
	accumulative_coverage_map = (unsigned char *)dr_global_alloc(MAX_COVERAGE_MAP_SIZE);

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] coverage_map=%lld\r\n", (ULONGLONG)coverage_map);

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "[dr_client_main] coverage_map=%lld\r\n", (ULONGLONG)coverage_map);
	//dr_close_file(log_file);

	if (coverage_map == NULL || accumulative_coverage_map == NULL)
	{
		DR_ASSERT_MSG(false, "out of space jam");
	}

	msg_count = 0;
	bb_counter = 0;

	num_closed_handles = 0;
	num_threads_opened = 0;

	InitializeCriticalSection(&coverage_sync);
	//InitializeCriticalSection(&print_sync);

	InitializeCriticalSection(&fuzz_msg_handling_lock);
	InitializeCriticalSection(&wrapper_msg_handling_lock);

	log_mutex_handle = CreateMutexA(NULL, false, "Global\\log_mutex");

	ProcessIdToSessionId(GetCurrentProcessId(), &session_id);

	thread_handle_counter = 0;
	memset(thread_handle_table, 0, sizeof(thread_handle_table));

	drmgr_register_thread_init_event(event_thread_init);
	drmgr_register_thread_exit_event(event_thread_exit);

	event_init();

	FUZZ_LOG("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", "[dr_client_main] after event_init\r\n");

	//log_file = dr_open_file("C:\\RDPFuzz\\Logs\\winafl-start-log.txt", DR_FILE_WRITE_APPEND);
	//dr_fprintf(log_file, "[dr_client_main] after event_init\r\n");
	//dr_close_file(log_file);

	//if (options.inside_timeout)
	//{
	//	start_timeout_thread();
	//}
}
