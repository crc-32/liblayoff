#include "../include/layoff.h"
#include <string.h>

static Service layoff_srv;
static LayoffName clientName;

Result layoffInitialize(const char name[16])
{
    strncpy(clientName.str, name, sizeof(clientName.str) - 1);
	clientName.str[sizeof(clientName.str) - 1] = '\0';
	Result rc = smGetService(&layoff_srv, "layoff");;
	if (R_FAILED(rc)) return rc;
	return serviceDispatchIn(&layoff_srv, LayoffCmdId_SetClientName, clientName);
}

Result layoffNotifySimple(const char contents[64])
{
	SimpleNotification notif = {0};
	memcpy(&notif.name, &clientName, sizeof(clientName));
    strncpy(notif.message, contents, sizeof(notif.message) - 1);
    return serviceDispatchIn(&layoff_srv, LayoffCmdId_NotifySimple, notif);
}

Result layoffPushUIPanel(LayoffUIHeader UIHeader, const void* data, u32 blockLen)
{
	return serviceDispatchIn(&layoff_srv, LayoffCmdId_PushUIPanel, UIHeader,
		.buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
		.buffers = { { data, blockLen } },
	);
}

Result layoffAcquireUIEvent(Event* evt)
{
	Handle event = INVALID_HANDLE;
	Result rc = serviceDispatch(&layoff_srv, LayoffCmdId_AcquireUiEvent,
		.out_handle_attrs = { SfOutHandleAttr_HipcCopy },
		.out_handles = &event
	);

	if (R_SUCCEEDED(rc))
		eventLoadRemote(evt, event, true);

	return rc;
}

Result layoffGetLastUIEvent(LayoffUIEvent* event)
{
	return serviceDispatchOut(&layoff_srv, LayoffCmdId_GetLastUiEvent, *event);
}

void layoffExit()
{
    serviceClose(&layoff_srv);
}