//Utils lib for layoff UI
#pragma once

#include "layoff.h"
#include <stdarg.h>
#include <malloc.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void layoffInitializeUIHeader(LayoffUIHeader* header, LayoffIdentifier panelID, LayoffUIKind kind)
{
	header->panelID = panelID;
	header->kind = kind;
	header->encoding = LayoffEncoding_ASCII;
}

static inline void layoffFillButtonList(LayoffUIButtonList* payload, u8 count, va_list args)
{
	payload->count = count;

	for (int i = 0; i < count; i++)
	{
		strncpy(payload->data[i].str, va_arg(args, const char*), sizeof(LayoffName));
		payload->data[i].str[sizeof(LayoffName) - 1] = '\0';
	}
}

static inline void layoffFreePayload(void* payload)
{
	if (payload)
		free(payload);
}

#define layoffFreeButtonList(x) layoffFreePayload(x)

static inline u32 layoffNewButtonListv(LayoffUIButtonList** payload, u8 count, va_list args)
{
	if (count <= 0) return 0;

	const u32 PayloadSize = sizeof(LayoffUIButtonList) + sizeof(LayoffName) * count;
	void* data = malloc(PayloadSize); //TODO: consider doing realloc(*payload, PayloadSize) ?
	if (!data) return 0;
	*payload = (LayoffUIButtonList*)data;

	layoffFillButtonList(*payload, count, args);

	return PayloadSize;
}

static inline u32 layoffNewButtonList(LayoffUIHeader* header, LayoffIdentifier panelID, LayoffUIButtonList** payload, u8 count, ...)
{
	if (count <= 0 || !panelID) return 0;

	if (header)
		layoffInitializeUIHeader(header, panelID, LayoffUIKind_ButtonList);

	va_list args;
	va_start(args, count);
	Result rc = layoffNewButtonListv(payload, count, args);
	va_end(args);
	return rc;
}

#define layoffFreeCheckBoxList(x) layoffFreePayload(x)

static inline u32 layoffNewCheckboxList(LayoffUIHeader* header, LayoffIdentifier panelID, LayoffUICheckBoxList** payload, u8 count, ...)
{
	if (count <= 0 || count > 62 || !panelID) return 0; //The checkbox event returns all the checked boxes, one for each bit of the payload, last bit has to be 0

	if (header)
		layoffInitializeUIHeader(header, panelID, LayoffUIKind_CheckBoxList);

	va_list args;
	va_start(args, count);
	Result rc = layoffNewButtonListv(payload, count, args);
	va_end(args);
	return rc;
}

#define layoffFreeRadioButtonList(x) layoffFreePayload(x)

static inline u32 layoffNewRadioButtonList(LayoffUIHeader* header, LayoffIdentifier panelID, LayoffUIRadioButtonList** payload, u8 count, ...)
{
	if (count <= 0 || !panelID) return 0;

	if (header)
		layoffInitializeUIHeader(header, panelID, LayoffUIKind_RadioButtonList);

	va_list args;
	va_start(args, count);
	Result rc = layoffNewButtonListv(payload, count, args);
	va_end(args);
	return rc;
}

#define layoffFreeComboBox(x) layoffFreePayload(x)

static inline u32 layoffNewComboBox(LayoffUIHeader* header, LayoffIdentifier panelID, LayoffUIComboBox** payload, const char *name, u8 count, ...)
{
	if (count <= 0 || !panelID) return 0;

	if (header)
		layoffInitializeUIHeader(header, panelID, LayoffUIKind_ComboBox);

	const u32 PayloadSize = sizeof(LayoffName) + sizeof(LayoffUIButtonList) + sizeof(LayoffName) * count;
	void* data = malloc(PayloadSize);
	if (!data) return 0;
	*payload = (LayoffUIComboBox*)data;

	strncpy((*payload)->label.str, name, sizeof(LayoffName));
	(*payload)->label.str[sizeof(LayoffName) - 1] = '\0';

	va_list args;
	va_start(args, count);
	layoffFillButtonList(&(*payload)->entries, count, args);
	va_end(args);

	return PayloadSize;
}

static inline Result layoffRemoveUIPanel(LayoffIdentifier panelID)
{
	LayoffUIHeader header;
	header.kind = LayoffUIKind_None;
	header.panelID = panelID;
	return layoffPushUIPanel(header, NULL, 0);
}

#ifdef __cplusplus
}
#endif
