#ifndef SERVER_RTSP_H
#define SERVER_RTSP_H

#include "Config.h"

#if ENABLE_RTSP_SERVER

void initServerRTSP();
void loopServerRTSP();

#endif

#endif // SERVER_RTSP_H
