/**
 * FreeRDP: A Remote Desktop Protocol Implementation
 * Generic Virtual Channel Extension
 *
 * Copyright 2013 Christian Hofstaedtler
 * Copyright 2015 Thincast Technologies GmbH
 * Copyright 2015 DI (FH) Martin Haimberger <martin.haimberger@thincast.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
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

#include <winpr/crt.h>
#include <winpr/stream.h>

#include "generic_main.h"
#include <freerdp/channels/log.h>
#include <freerdp/channels/generic.h>

#define TAG CHANNELS_TAG("generic.client")

typedef struct _GENERIC_LISTENER_CALLBACK GENERIC_LISTENER_CALLBACK;
struct _GENERIC_LISTENER_CALLBACK
{
	IWTSListenerCallback iface;

	IWTSPlugin* plugin;
	IWTSVirtualChannelManager* channel_mgr;
};

typedef struct _GENERIC_CHANNEL_CALLBACK GENERIC_CHANNEL_CALLBACK;
struct _GENERIC_CHANNEL_CALLBACK
{
	IWTSVirtualChannelCallback iface;

	IWTSPlugin* plugin;
	IWTSVirtualChannelManager* channel_mgr;
	IWTSVirtualChannel* channel;
};

typedef struct _GENERIC_PLUGIN GENERIC_PLUGIN;
struct _GENERIC_PLUGIN
{
	IWTSPlugin iface;

	GENERIC_LISTENER_CALLBACK* listener_callback;
	IWTSListener* listener;
	BOOL initialized;
};

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT generic_on_data_received(IWTSVirtualChannelCallback* pChannelCallback, wStream* data)
{
	WLog_INFO("FUZZ", "[generic_on_data_received]");
	GENERIC_CHANNEL_CALLBACK* callback = (GENERIC_CHANNEL_CALLBACK*)pChannelCallback;
	BYTE* pBuffer = Stream_Pointer(data);
	UINT32 cbSize = Stream_GetRemainingLength(data);

	/* generic back what we have received. GENERIC does not have any message IDs. */
	return callback->channel->Write(callback->channel, cbSize, pBuffer, NULL);
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT generic_on_close(IWTSVirtualChannelCallback* pChannelCallback)
{
	WLog_INFO("FUZZ", "[generic_on_close]");

	GENERIC_CHANNEL_CALLBACK* callback = (GENERIC_CHANNEL_CALLBACK*)pChannelCallback;

	free(callback);

	return CHANNEL_RC_OK;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT generic_on_new_channel_connection(IWTSListenerCallback* pListenerCallback,
                                           IWTSVirtualChannel* pChannel, BYTE* Data, BOOL* pbAccept,
                                           IWTSVirtualChannelCallback** ppCallback)
{
	WLog_INFO("FUZZ", "[generic_on_new_channel_connection]");

	GENERIC_CHANNEL_CALLBACK* callback;
	GENERIC_LISTENER_CALLBACK* listener_callback = (GENERIC_LISTENER_CALLBACK*)pListenerCallback;

	callback = (GENERIC_CHANNEL_CALLBACK*)calloc(1, sizeof(GENERIC_CHANNEL_CALLBACK));

	if (!callback)
	{
		WLog_ERR(TAG, "calloc failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	callback->iface.OnDataReceived = generic_on_data_received;
	callback->iface.OnClose = generic_on_close;
	callback->plugin = listener_callback->plugin;
	callback->channel_mgr = listener_callback->channel_mgr;
	callback->channel = pChannel;

	*ppCallback = (IWTSVirtualChannelCallback*)callback;

	return CHANNEL_RC_OK;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT generic_plugin_initialize(IWTSPlugin* pPlugin, IWTSVirtualChannelManager* pChannelMgr)
{
	WLog_INFO("FUZZ", "[generic_plugin_initialize]");

	UINT status;
	GENERIC_PLUGIN* generic = (GENERIC_PLUGIN*)pPlugin;
	if (generic->initialized)
	{
		WLog_ERR(TAG, "[%s] channel initialized twice, aborting", GENERIC_DVC_CHANNEL_NAME);
		return ERROR_INVALID_DATA;
	}
	generic->listener_callback = (GENERIC_LISTENER_CALLBACK*)calloc(1, sizeof(GENERIC_LISTENER_CALLBACK));

	if (!generic->listener_callback)
	{
		WLog_ERR(TAG, "calloc failed!");
		return CHANNEL_RC_NO_MEMORY;
	}

	generic->listener_callback->iface.OnNewChannelConnection = generic_on_new_channel_connection;
	generic->listener_callback->plugin = pPlugin;
	generic->listener_callback->channel_mgr = pChannelMgr;

	status = pChannelMgr->CreateListener(pChannelMgr, GENERIC_DVC_CHANNEL_NAME, 0,
	                                     &generic->listener_callback->iface, &generic->listener);

	generic->initialized = status == CHANNEL_RC_OK;

	pChannelMgr->CreateChannel(pChannelMgr, GENERIC_DVC_CHANNEL_NAME);
	return status;
}

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
static UINT generic_plugin_terminated(IWTSPlugin* pPlugin)
{
	WLog_INFO("FUZZ", "[generic_plugin_terminated]");

	GENERIC_PLUGIN* generic = (GENERIC_PLUGIN*)pPlugin;
	if (generic && generic->listener_callback)
	{
		IWTSVirtualChannelManager* mgr = generic->listener_callback->channel_mgr;
		if (mgr)
			IFCALL(mgr->DestroyListener, mgr, generic->listener);
	}
	free(generic);

	return CHANNEL_RC_OK;
}

#ifdef BUILTIN_CHANNELS
#define DVCPluginEntry generic_DVCPluginEntry
#else
#define DVCPluginEntry FREERDP_API DVCPluginEntry
#endif

/**
 * Function description
 *
 * @return 0 on success, otherwise a Win32 error code
 */
UINT DVCPluginEntry(IDRDYNVC_ENTRY_POINTS* pEntryPoints)
{
	WLog_INFO("FUZZ", "[DVCPluginEntry]");

	UINT status = CHANNEL_RC_OK;
	GENERIC_PLUGIN* generic;

	generic = (GENERIC_PLUGIN*)pEntryPoints->GetPlugin(pEntryPoints, "generic");

	if (!generic)
	{
		generic = (GENERIC_PLUGIN*)calloc(1, sizeof(GENERIC_PLUGIN));

		if (!generic)
		{
			WLog_ERR(TAG, "calloc failed!");
			return CHANNEL_RC_NO_MEMORY;
		}

		generic->iface.Initialize = generic_plugin_initialize;
		generic->iface.Connected = NULL;
		generic->iface.Disconnected = NULL;
		generic->iface.Terminated = generic_plugin_terminated;

		status = pEntryPoints->RegisterPlugin(pEntryPoints, "generic", (IWTSPlugin*)generic);
	}

	return status;
}
