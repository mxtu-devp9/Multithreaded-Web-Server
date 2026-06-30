#ifndef DASHBOARD_METRICS_H
#define DASHBOARD_METRICS_H

#include <string>

void dashboardSetMode(const std::string& mode);
void dashboardSetMaxThreads(int count);
void dashboardSetMaxQueue(int count);
void dashboardSetRequestLimit(int count);
void dashboardSetQueueSize(int count);
void dashboardSetServerStatus(const std::string& status);
void dashboardAddRejected(const std::string& reason);
void dashboardAddEventPublic(const std::string& message);
void dashboardSetPriorityVisual(bool active);

#endif
