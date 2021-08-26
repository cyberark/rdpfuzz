/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * Generic Virtual Channel Extension
 *
 * Copyright 2014 Vic Lee
 * Copyright 2015 Thincast Technologies GmbH
 * Copyright 2015 DI (FH) Martin Haimberger <martin.haimberger@thincast.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	 http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <winpr/crt.h>
#include <winpr/synch.h>
#include <winpr/thread.h>
#include <winpr/stream.h>
#include <winpr/sysinfo.h>

#include <freerdp/server/generic.h>
#include <freerdp/channels/log.h>

#define TAG CHANNELS_TAG("generic.server")

typedef struct _generic_server
{
	generic_server_context context;

	BOOL opened;

	HANDLE stopEvent;

	HANDLE thread;
	void* generic_channel;

	DWORD SessionId;

} generic_server;

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT generic_server_open_channel(generic_server* generic)
{
	DWORD Error;
	HANDLE hEvent;
	DWORD StartTick;
	DWORD BytesReturned = 0;
	PULONG pSessionId = NULL;

	if (WTSQuerySessionInformationA(generic->context.vcm, WTS_CURRENT_SESSION, WTSSessionId,
	                                (LPSTR*)&pSessionId, &BytesReturned) == FALSE)
	{
		WLog_ERR(TAG, "WTSQuerySessionInformationA failed!");
		return ERROR_INTERNAL_ERROR;
	}

	generic->SessionId = (DWORD)*pSessionId;
	WTSFreeMemory(pSessionId);
	hEvent = WTSVirtualChannelManagerGetEventHandle(generic->context.vcm);
	StartTick = GetTickCount();

	while (generic->generic_channel == NULL)
	{
		if (WaitForSingleObject(hEvent, 1000) == WAIT_FAILED)
		{
			Error = GetLastError();
			WLog_ERR(TAG, "WaitForSingleObject failed with error %" PRIu32 "!", Error);
			return Error;
		}

		generic->generic_channel =
		    WTSVirtualChannelOpenEx(generic->SessionId, "GENERIC", WTS_CHANNEL_OPTION_DYNAMIC);

		if (generic->generic_channel)
			break;

		Error = GetLastError();

		if (Error == ERROR_NOT_FOUND)
			break;

		if (GetTickCount() - StartTick > 5000)
			break;
	}

	return generic->generic_channel ? CHANNEL_RC_OK : ERROR_INTERNAL_ERROR;
}

static DWORD WINAPI generic_server_thread_func(LPVOID arg)
{
	wStream* s;
	void* buffer;
	DWORD nCount;
	HANDLE events[8];
	BOOL ready = FALSE;
	HANDLE ChannelEvent;
	DWORD BytesReturned = 0;
	generic_server* generic = (generic_server*)arg;
	UINT error;
	DWORD status;

	if ((error = generic_server_open_channel(generic)))
	{
		UINT error2 = 0;
		WLog_ERR(TAG, "generic_server_open_channel failed with error %" PRIu32 "!", error);
		IFCALLRET(generic->context.OpenResult, error2, &generic->context,
		          GENERIC_SERVER_OPEN_RESULT_NOTSUPPORTED);

		if (error2)
			WLog_ERR(TAG, "generic server's OpenResult callback failed with error %" PRIu32 "",
			         error2);

		goto out;
	}

	buffer = NULL;
	BytesReturned = 0;
	ChannelEvent = NULL;

	if (WTSVirtualChannelQuery(generic->generic_channel, WTSVirtualEventHandle, &buffer,
	                           &BytesReturned) == TRUE)
	{
		if (BytesReturned == sizeof(HANDLE))
			CopyMemory(&ChannelEvent, buffer, sizeof(HANDLE));

		WTSFreeMemory(buffer);
	}

	nCount = 0;
	events[nCount++] = generic->stopEvent;
	events[nCount++] = ChannelEvent;

	/* Wait for the client to confirm that the Graphics Pipeline dynamic channel is ready */

	while (1)
	{
		status = WaitForMultipleObjects(nCount, events, FALSE, 100);

		if (status == WAIT_FAILED)
		{
			error = GetLastError();
			WLog_ERR(TAG, "WaitForMultipleObjects failed with error %" PRIu32 "", error);
			break;
		}

		if (status == WAIT_OBJECT_0)
		{
			IFCALLRET(generic->context.OpenResult, error, &generic->context,
			          GENERIC_SERVER_OPEN_RESULT_CLOSED);

			if (error)
				WLog_ERR(TAG, "OpenResult failed with error %" PRIu32 "!", error);

			break;
		}

		if (WTSVirtualChannelQuery(generic->generic_channel, WTSVirtualChannelReady, &buffer,
		                           &BytesReturned) == FALSE)
		{
			IFCALLRET(generic->context.OpenResult, error, &generic->context,
			          GENERIC_SERVER_OPEN_RESULT_ERROR);

			if (error)
				WLog_ERR(TAG, "OpenResult failed with error %" PRIu32 "!", error);

			break;
		}

		ready = *((BOOL*)buffer);
		WTSFreeMemory(buffer);

		if (ready)
		{
			IFCALLRET(generic->context.OpenResult, error, &generic->context, GENERIC_SERVER_OPEN_RESULT_OK);

			if (error)
				WLog_ERR(TAG, "OpenResult failed with error %" PRIu32 "!", error);

			break;
		}
	}

	s = Stream_New(NULL, 4096);

	if (!s)
	{
		WLog_ERR(TAG, "Stream_New failed!");
		WTSVirtualChannelClose(generic->generic_channel);
		ExitThread(ERROR_NOT_ENOUGH_MEMORY);
		return ERROR_NOT_ENOUGH_MEMORY;
	}

	while (ready)
	{
		status = WaitForMultipleObjects(nCount, events, FALSE, INFINITE);

		if (status == WAIT_FAILED)
		{
			error = GetLastError();
			WLog_ERR(TAG, "WaitForMultipleObjects failed with error %" PRIu32 "", error);
			break;
		}

		if (status == WAIT_OBJECT_0)
			break;

		Stream_SetPosition(s, 0);
		WTSVirtualChannelRead(generic->generic_channel, 0, NULL, 0, &BytesReturned);

		if (BytesReturned < 1)
			continue;

		if (!Stream_EnsureRemainingCapacity(s, BytesReturned))
		{
			WLog_ERR(TAG, "Stream_EnsureRemainingCapacity failed!");
			error = CHANNEL_RC_NO_MEMORY;
			break;
		}

		if (WTSVirtualChannelRead(generic->generic_channel, 0, (PCHAR)Stream_Buffer(s),
		                          (ULONG)Stream_Capacity(s), &BytesReturned) == FALSE)
		{
			WLog_ERR(TAG, "WTSVirtualChannelRead failed!");
			error = ERROR_INTERNAL_ERROR;
			break;
		}

		IFCALLRET(generic->context.Response, error, &generic->context, (BYTE*)Stream_Buffer(s),
		          BytesReturned);

		if (error)
		{
			WLog_ERR(TAG, "Response failed with error %" PRIu32 "!", error);
			break;
		}
	}

	Stream_Free(s, TRUE);
	WTSVirtualChannelClose(generic->generic_channel);
	generic->generic_channel = NULL;
out:

	if (error && generic->context.rdpcontext)
		setChannelError(generic->context.rdpcontext, error,
		                "generic_server_thread_func reported an error");

	ExitThread(error);
	return error;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT generic_server_open(generic_server_context* context)
{
	generic_server* generic = (generic_server*)context;

	if (generic->thread == NULL)
	{
		if (!(generic->stopEvent = CreateEvent(NULL, TRUE, FALSE, NULL)))
		{
			WLog_ERR(TAG, "CreateEvent failed!");
			return ERROR_INTERNAL_ERROR;
		}

		if (!(generic->thread = CreateThread(NULL, 0, generic_server_thread_func, (void*)generic, 0, NULL)))
		{
			WLog_ERR(TAG, "CreateEvent failed!");
			CloseHandle(generic->stopEvent);
			generic->stopEvent = NULL;
			return ERROR_INTERNAL_ERROR;
		}
	}

	return CHANNEL_RC_OK;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT generic_server_close(generic_server_context* context)
{
	UINT error = CHANNEL_RC_OK;
	generic_server* generic = (generic_server*)context;

	if (generic->thread)
	{
		SetEvent(generic->stopEvent);

		if (WaitForSingleObject(generic->thread, INFINITE) == WAIT_FAILED)
		{
			error = GetLastError();
			WLog_ERR(TAG, "WaitForSingleObject failed with error %" PRIu32 "", error);
			return error;
		}

		CloseHandle(generic->thread);
		CloseHandle(generic->stopEvent);
		generic->thread = NULL;
		generic->stopEvent = NULL;
	}

	return error;
}

static BOOL generic_server_request(generic_server_context* context, const BYTE* buffer, UINT32 length)
{
	generic_server* generic = (generic_server*)context;
	return WTSVirtualChannelWrite(generic->generic_channel, (PCHAR)buffer, length, NULL);
}

generic_server_context* generic_server_context_new(HANDLE vcm)
{
	generic_server* generic;
	generic = (generic_server*)calloc(1, sizeof(generic_server));

	if (generic)
	{
		generic->context.vcm = vcm;
		generic->context.Open = generic_server_open;
		generic->context.Close = generic_server_close;
		generic->context.Request = generic_server_request;
	}
	else
		WLog_ERR(TAG, "calloc failed!");

	return (generic_server_context*)generic;
}

void generic_server_context_free(generic_server_context* context)
{
	generic_server* generic = (generic_server*)context;
	generic_server_close(context);
	free(generic);
}
