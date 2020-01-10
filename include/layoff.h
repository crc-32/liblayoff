#pragma once

#include <switch.h>

#ifdef __cplusplus
extern "C" {
#endif

enum LayoffCmdId{
    LayoffCmdId_NotifySimple = 1,
    LayoffCmdId_NotifyEx = 2
};

typedef struct {
    char identifier[16];
    char message[64];
}SimpleNotification;

Result layoffInitialize(char identifier[16]);
Result pushNotification(char message[64]);
void layoffExit();

#ifdef __cplusplus
}
#endif
