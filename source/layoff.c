#include <layoff.h>
#include <string.h>
static Service layoff_srv;
static char identifier[16];

Result layoffInitialize(const char id[16])
{
    strncpy(identifier, id, sizeof(identifier));
    return smGetService(&layoff_srv, "layoff");
}

Result pushNotification(const char contents[64])
{
    SimpleNotification notif;
    strncpy(notif.identifier, identifier, sizeof(notif.identifier));
    strncpy(notif.message, contents, sizeof(notif.message));
    return serviceDispatchIn(&layoff_srv, LayoffCmdId_NotifySimple, notif);
}

void layoffExit()
{
    serviceClose(&layoff_srv);
}