#include "BufferEventSer.h"

void OnAccept(evconnlistener *listener, evutil_socket_t fdClient, sockaddr *pAddr, int nSocklen, void *pParam);
void OnRead(bufferevent *pBufEvent, void *pParam);
void OnWrite(bufferevent *pBufEvent, void *pParam);
void OnEvent(bufferevent *pBufEvent, short nEventType, void *pParam);

static CBFUN_PARAM_T g_param;

CBufferEventSer::CBufferEventSer(CBFUN_PARAM_T param)
	: m_pEvlistener(NULL)
	, m_pBase(NULL)
	, m_bIOCP(false)
{
	g_param = param;
}

CBufferEventSer::~CBufferEventSer(void)
{
}

void CBufferEventSer::SetWindowsIOCP()
{
	//使用IOPC复用异步IO
	struct event_config *cfg = event_config_new();
	if (cfg)
	{
		//event_config_set_num_cpus_hint(cfg, 8);
		//event_config_require_features(cfg, /*EV_FEATURE_ET*/  /*EV_FEATURE_O1*/  EV_FEATURE_FDS);
		event_config_set_flag(cfg, EVENT_BASE_FLAG_STARTUP_IOCP);
		m_pBase = event_base_new_with_config(cfg);
		event_config_free(cfg);
	}
}

int CBufferEventSer::Init()
{
#ifdef _WIN32
	WSADATA wsaData;
	DWORD dwRet;
	if ((dwRet =WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		return WSAStartup_fail;
	}
#endif

	//设置多线程
#ifdef WIN32
	evthread_use_windows_threads();//win上设置
#else
	evthread_use_pthreads();    //unix上设置
#endif

	sockaddr_in addr;
	addr.sin_addr.s_addr = 0;
	addr.sin_port = htons(g_param.nPort);
	addr.sin_family = PF_INET; //=AF_INET
	
	if (!m_bIOCP)
	{
		m_pBase = event_base_new();
	}
	else
	{
		SetWindowsIOCP();
	}
	
	if (NULL == m_pBase)
	{
		return event_base_new_fail;
	}
	evthread_make_base_notifiable((event_base *)m_pBase);

	m_pEvlistener = evconnlistener_new_bind(m_pBase, OnAccept, this, LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, MAX_LISTEN_SOCKET_NUM, (sockaddr *)&addr, sizeof(addr));
	if (NULL == m_pEvlistener)
	{
		event_base_free(m_pBase);
		return evconnlistener_new_bind_fail;
	}

	return success;
}

static unsigned __stdcall EventLoopThread(void *pParam)
{
	event_base *pBase = (event_base *)pParam;
	if (NULL != pBase)
	{
		int nRet = event_base_dispatch(pBase);
	}
	return 0;
}

void CBufferEventSer::Start()
{
	unsigned threadID;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &EventLoopThread, m_pBase, 0, &threadID);
	CloseHandle(hThread);
}

void CBufferEventSer::Stop()
{
	event_base_loopexit(m_pBase, NULL);
}

int CBufferEventSer::Send(const DATA_PACKAGE_T *dataPackage)
{
	if (NULL == dataPackage || NULL == dataPackage->pSendID)
	{
		return -1;
	}
	return bufferevent_write((bufferevent*)dataPackage->pSendID, dataPackage->pData, dataPackage->nLen);

	////服务器使用bufferevent_write时，需要使用到bufferevent，所以发送时需要遍历fd，大并发量时需要考虑效率
	// if (NULL == dataPackage->pSendID)
	// {
	//	 int nSize = m_vecBufferInfo.size();
	//	 if (nSize > 0)
	//	 {
	//		 //没有指定SocketID则给最后连接的Socket客户端发送数据
	//		return bufferevent_write(m_vecBufferInfo[nSize-1].pBufvClient, dataPackage->pData, dataPackage->nLen);
	//		//int nRet = send(m_vecBufferInfo.at(nSize-1).fdClient, (const char*)pBuf, nLen, 0);
	//		//if (SOCKET_ERROR == nRet)
	//		//{
	//		//	return -1;
	//		//}
	//	 }
	// }
	// else
	// {
	//	for (int i = 0; i < (int)m_vecBufferInfo.size(); i++)
	//	{
	//		if (m_vecBufferInfo[i].pBufvClient == dataPackage->pSendID)
	//		{
	//			return bufferevent_write(m_vecBufferInfo[i].pBufvClient, dataPackage->pData, dataPackage->nLen);
	//		}
	//	}
	// }
	// return -1;
}

void CBufferEventSer::ReleaseRes()
{
	event_base_loopbreak(m_pBase);
	if (NULL != m_pEvlistener)
	{
		evconnlistener_free(m_pEvlistener);
	}
	if (NULL != m_pBase)
	{
		event_base_free(m_pBase);
	}
#ifdef _WIN32
	WSACleanup();
#endif
}

void CBufferEventSer::Delete(void *pParam)
{
	CBufferEventSer *pThis = (CBufferEventSer *)pParam;
	if (NULL != pThis)
	{
		pThis->ReleaseRes();
	}
}

void OnAccept(evconnlistener *listener, evutil_socket_t fdClient, sockaddr *pAddr, int nSocklen, void *pParam)
{
	if (NULL == pParam || NULL == pAddr)
	{
		return;
	}
	
	CBufferEventSer *pDlg = (CBufferEventSer *)pParam;
	event_base *pBase = evconnlistener_get_base(listener);
	bufferevent *pBufEvent = bufferevent_socket_new(pBase, fdClient, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
	if (NULL == pBufEvent)
	{
		return;
	}
	//BUFFER_INFO_T bufvInfo;
	//bufvInfo.fdClient = fdClient;
	//bufvInfo.pBufvClient = pBufEvent;
	//pDlg->m_vecBufferInfo.push_back(bufvInfo);
	bufferevent_setcb(pBufEvent, OnRead, NULL, OnEvent, pParam);
	bufferevent_enable(pBufEvent, EV_READ | /*EV_WRITE |*/ EV_PERSIST);
	evutil_socket_t nFd = bufferevent_getfd(pBufEvent);
	g_param.cbFun(TCP_SERVER_CONNECT, g_param.pThis, (void*)&nFd);
}

void OnRead(bufferevent *pBufEvent, void *pParam)
{
	unsigned char szMsg[MAX_READ_MSG_LEN] = {0};

	CBufferEventSer *pDlg = (CBufferEventSer *)pParam;
	int nLen = bufferevent_read(pBufEvent, szMsg, MAX_READ_MSG_LEN);
	DATA_PACKAGE_T data;
	if (nLen > 0)
	{
		data.pSendID = pBufEvent;
		data.nLen = nLen;
		data.pData = szMsg;
		data.nFd = bufferevent_getfd(pBufEvent);
		g_param.cbFun(TCP_READ_DATA, g_param.pThis, (void*)&data);
		//const char *pBuf = (const char*)szMsg;
		//bufferevent_write(pBufEvent, pBuf, strlen(pBuf)+1);
	}
}

void OnWrite(bufferevent *pBufEvent, void *pParam)
{

}

void OnEvent(bufferevent *pBufEvent, short nEventType, void *pParam)
{
	if (nEventType & BEV_EVENT_EOF)
	{
		//OnEvent connect closed
		bufferevent_free(pBufEvent);
		evutil_socket_t nFd = bufferevent_getfd(pBufEvent);
		g_param.cbFun(TCP_SERVER_DISCONNECT, g_param.pThis, (void*)&nFd);

		//CBufferEventSer *pDlg = (CBufferEventSer *)pParam;
		//vector<BUFFER_INFO_T>::iterator it = pDlg->m_vecBufferInfo.begin();
		//for ( ; it != pDlg->m_vecBufferInfo.end() ; )
		//{
		//	if (it->pBufvClient == pBufEvent)
		//	{
		//		g_param.cbFun(TCP_SERVER_DISCONNECT, g_param.pThis, (void*)&it->fdClient);
		//		it = pDlg->m_vecBufferInfo.erase(it);	//崩溃
		//		break;
		//	}
		//	else
		//	{
		//		it++;
		//	}
		//}
	}
	else if (nEventType & BEV_EVENT_CONNECTED)
	{
		//OnEvent connect to server success
	}
	else if (nEventType & BEV_EVENT_ERROR)
	{
		//OnEvent meet some other error
		bufferevent_free(pBufEvent);
	}
	else
	{

	}
}
