#ifndef _HEADER_H_
#define _HEADER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <errno.h>
#include <iostream>
#include <vector>
#include <process.h> 
using namespace std;

#include "event2/event.h"
#include "event2/listener.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "event2/thread.h"
#include "event2/util.h"

#include <WinSock2.h>
#include <Windows.h>
#include <WS2tcpip.h>
//#include <signal.h>
#include <io.h>

#include "TcpCommLibBase.h"

#define bzero(argc, len)		memset(argc, 0, len)

struct BUFFER_INFO_T
{
	evutil_socket_t fdClient;
	bufferevent *pBufvClient;
	BUFFER_INFO_T()
	{
		fdClient = -1;
		pBufvClient =NULL;
	}
};

#endif