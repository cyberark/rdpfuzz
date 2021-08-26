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

#ifndef FREERDP_CHANNEL_GENERIC_SERVER_H
#define FREERDP_CHANNEL_GENERIC_SERVER_H

#include <freerdp/channels/wtsvc.h>

typedef enum GENERIC_SERVER_OPEN_RESULT
{
	GENERIC_SERVER_OPEN_RESULT_OK = 0,
	GENERIC_SERVER_OPEN_RESULT_CLOSED = 1,
	GENERIC_SERVER_OPEN_RESULT_NOTSUPPORTED = 2,
	GENERIC_SERVER_OPEN_RESULT_ERROR = 3
} GENERIC_SERVER_OPEN_RESULT;

typedef struct _generic_server_context generic_server_context;

typedef UINT (*psGenericServerOpen)(generic_server_context* context);
typedef UINT (*psGenericServerClose)(generic_server_context* context);
typedef BOOL (*psGenericServerRequest)(generic_server_context* context, const BYTE* buffer,
                                    UINT32 length);

typedef UINT (*psGenericServerOpenResult)(generic_server_context* context,
                                       GENERIC_SERVER_OPEN_RESULT result);
typedef UINT (*psGenericServerResponse)(generic_server_context* context, const BYTE* buffer,
                                     UINT32 length);

struct _generic_server_context
{
	HANDLE vcm;

	/* Server self-defined pointer. */
	void* data;

	/*** APIs called by the server. ***/
	/**
	 * Open the generic channel.
	 */
	psGenericServerOpen Open;
	/**
	 * Close the generic channel.
	 */
	psGenericServerClose Close;
	/**
	 * Send generic request PDU.
	 */
	psGenericServerRequest Request;

	/*** Callbacks registered by the server. ***/
	/**
	 * Indicate whether the channel is opened successfully.
	 */
	psGenericServerOpenResult OpenResult;
	/**
	 * Receive generic response PDU.
	 */
	psGenericServerResponse Response;

	rdpContext* rdpcontext;
};

#ifdef __cplusplus
extern "C"
{
#endif

	FREERDP_API generic_server_context* generic_server_context_new(HANDLE vcm);
	FREERDP_API void generic_server_context_free(generic_server_context* context);

#ifdef __cplusplus
}
#endif

#endif /* FREERDP_CHANNEL_GENERIC_SERVER_H */
