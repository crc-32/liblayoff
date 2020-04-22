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
	LayoffCmdId_AcquireUiEvent = 7,
	LayoffCmdId_GetLastUiEvent = 8,
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
	LayoffUIKind_ComboBox = 3,
	LayoffUIKind_CheckBoxList = 4,
	LayoffUIKind_RadioButtonList = 5,
} LayoffUIKind;

//TODO: Not sure about keeping this in the API, do we really need non-ascci support ?
typedef enum {
	LayoffEncoding_ASCII = 0,
} LayoffEncoding;

typedef struct 
{
	LayoffIdentifier panelID; //Must be > 0
	LayoffUIKind kind;
	LayoffEncoding encoding;
} PACKED LayoffUIHeader;

typedef struct
{
	u8 count;
	u64 inlineFlags; //assuming bits 00001010 second and fourth buttons are on the same line as the previous element (You rarely want the first control to be in the same line)
	LayoffName data[];
} PACKED LayoffUIButtonList;
typedef LayoffUIButtonList LayoffUICheckBoxList;
typedef LayoffUIButtonList LayoffUIRadioButtonList;

typedef struct
{
	LayoffName label;
	LayoffUIButtonList entries;
} PACKED LayoffUIComboBox;

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

Result layoffPushUIPanel(LayoffUIHeader UIData, const void* payload, u32 blockLen);
Result layoffAcquireUIEvent(Event* event);
Result layoffGetLastUIEvent(LayoffUIEvent* event);

void layoffExit();

#ifdef __cplusplus
}
#endif
