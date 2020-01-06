#include <layoff.h>
#include <string.h>
Service layoff_srv;
char id[16];

Result layoffInitialize(char identifier[16])
{
    strcpy(id, identifier);
    return smGetService(&layoff_srv, "layoff");
}

Result pushNotification(char contents[64])
{
    SimpleNotification notif;
    strcpy(notif.identifier, id);
    strcpy(notif.message, contents);
    return serviceDispatchIn(&layoff_srv, LayoffCmdId_NotifySimple, notif);
}

void layoffExit()
{
    serviceClose(&layoff_srv);
}