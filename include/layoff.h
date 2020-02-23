#pragma once

#include <switch.h>

#ifdef __cplusplus
extern "C" {
#endif

enum LayoffCmdId{
    LayoffCmdId_NotifySimple = 1,
	LayoffCmdId_NotifyEx = 2,

	LayoffCmdId_SetClientName = 3,
	LayoffCmdId_PushUIPanel = 5,
	LayoffCmdId_GetUIState = 6,
	LayoffCmdId_AcquireUiEvent = 7
};

typedef u32 LayoffIdentifier;

typedef struct 
{
	char str[16];
} PACKED LayoffName;

typedef struct {
	LayoffName name;
    char message[64];
} PACKED SimpleNotification;

typedef enum {
	LayoffUIKind_None = 0,
	LayoffUIKind_TextBlock = 1,
	LayoffUIKind_ButtonList = 2,
	//TODO:
	// combo box
	// check box
	// more ?
} LayoffUIKind;

typedef enum {
	LayoffEncoding_ASCII = 0,
} LayoffEncoding;

typedef struct 
{
	LayoffIdentifier panelID;
	LayoffUIKind kind;
	LayoffEncoding encoding;
} PACKED LayoffUIHeader;

typedef struct 
{
	u8 ButtonCount;
	LayoffName data[];
} PACKED LayoffUIButtonList;

typedef struct
{
	LayoffIdentifier panel;
	u64 data1;
	u64 data2;
	//TODO: find a nice and generic enough api, maybe use char[16] ?
	//don't expect to support overly complex ui components so just an index + space for metadata should be enough
} PACKED LayoffUIEvent;

Result layoffInitialize(const char identifier[16]);
Result layoffNotifySimple(const char message[64]);

Result layoffPushUIPanel(LayoffUIHeader UIData, const void* data, u32 blockLen);
Result layoffGetLastUIEvent(LayoffUIEvent* event);

void layoffExit();

#ifdef __cplusplus
}
#endif
