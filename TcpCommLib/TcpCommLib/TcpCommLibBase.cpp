#include "TcpCommLibBase.h"
#include "TcpLibServer.h"
#include "TcpLibClient.h"
#include "BufferEventSer.h"
#include "BufferEventClient.h"

static int g_nType = -1;
static CTcpCommLibBase *g_pTcpServerBuffer;
static CTcpCommLibBase *g_pTcpClientBuffer;

CTcpCommLibBase *CreateTcpCommLib(int nType, CBFUN_PARAM_T param)
{
	g_nType = nType;
	switch (nType)
	{
	case 0:
		{
			//服务端_bufferevent
			g_pTcpServerBuffer = new CBufferEventSer(param);
			return g_pTcpServerBuffer;
		}
		break;
	case 1:
		{
			//客户端_bufferevent
			g_pTcpClientBuffer = new CBufferEventClient(param);
			return g_pTcpClientBuffer;
		}
		break;
	//case 2:
	//	{
	//		//服务端_event
	//		return new CTcpLibServer(param);
	//	}
	//	break;
	//case 3:
	//	{
	//		//客户端_event
	//		return new CTcpLibClient(param);
	//	}
	//break;
	default:
		break;
	}
	return NULL;
}

void DeleteTcpCommLib()
{
	if (0 == g_nType)
	{
		if (NULL != g_pTcpServerBuffer)
		{
			CBufferEventSer::Delete(g_pTcpServerBuffer);
			delete g_pTcpServerBuffer;
		}
	}
	else if (1 == g_nType)
	{
		if (NULL != g_pTcpClientBuffer)
		{
			CBufferEventClient::Delete(g_pTcpClientBuffer);
			delete g_pTcpClientBuffer;
		}
	}
}

int CTcpCommLibBase::GetSocketID()
{
	return -1;
}
