#include "../include/layoff.h"
#include <string.h>

static Service layoff_srv;
static LayoffName clientName;
static LayoffIdentifier clientID = 0;

static Result layoffEnsureRegistered() 
{
	if (clientID != 0) return 0;

	LayoffName tmp;
	memcpy(&tmp, &clientName, sizeof(clientName));

	Result rc = serviceDispatchInOut(&layoff_srv, LayoffCmdId_RegisterClient, tmp, clientID);
	return rc;
}

static Result layoffUnregister() 
{
	if (clientID == 0) return 0;
	return serviceDispatchIn(&layoff_srv, LayoffCmdId_UnregisterClient, clientID);
}

Result layoffInitialize(const char name[16])
{
    strncpy(clientName.name, name, sizeof(clientName.name) - 1);
	clientName.name[sizeof(clientName.name) - 1] = '\0';
	Result rc = smGetService(&layoff_srv, "layoff");;
	if (R_FAILED(rc)) return rc;
	return layoffEnsureRegistered();
}

Result layoffNotifySimple(const char contents[64])
{
	SimpleNotification notif = {};
	memcpy(&notif.name, &clientName, sizeof(clientName));
    strncpy(notif.message, contents, sizeof(notif.message) - 1);
    return serviceDispatchIn(&layoff_srv, LayoffCmdId_NotifySimple, notif);
}

Result layoffPushUIPanel(LayoffUIHeader UIHeader, const void* data, u32 blockLen)
{
	UIHeader.client = clientID;
	return serviceDispatchIn(&layoff_srv, LayoffCmdId_PushUIPanel, UIHeader,
		.buffer_attrs = { SfBufferAttr_HipcMapAlias | SfBufferAttr_In },
		.buffers = { { data, blockLen } },
	);
}

Result layoffGetLastUIEvent(LayoffUIEvent* out_evt)
{
	LayoffUIEvent event;
	Result rc = serviceDispatchInOut(&layoff_srv, LayoffCmdId_AcquireUiEvent, clientID, event);
	if (!R_FAILED(rc))
		*out_evt = event;
	return rc;
}

void layoffExit()
{
	layoffUnregister();
    serviceClose(&layoff_srv);
}