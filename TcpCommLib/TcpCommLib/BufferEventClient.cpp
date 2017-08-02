#include "BufferEventClient.h"


void OnReadClient(bufferevent *pBufEvent, void *pParam);
void OnWriteClient(bufferevent *pBufEvent, void *pParam);
void OnEventClient(bufferevent *pBufEvent, short nEventType, void *pParam);

static CBFUN_PARAM_T g_param;

CBufferEventClient::CBufferEventClient(CBFUN_PARAM_T param)
	: m_pBufEvent(NULL)
	, m_pBase(NULL)
{
	g_param = param;
}

CBufferEventClient::~CBufferEventClient(void)
{
}

int CBufferEventClient::Init()
{
#ifdef _WIN32
	WSADATA wsaData;
	DWORD dwRet;
	if ((dwRet =WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		return WSAStartup_fail;
	}
#endif

	sockaddr_in addr;
	if (inet_pton(AF_INET, g_param.pIP, &addr.sin_addr) < 1)
	{
		return inet_ntop_fail;
	}
	addr.sin_port = htons(g_param.nPort);
	addr.sin_family = PF_INET; //=AF_INET

	//设置多线程
#ifdef WIN32
	evthread_use_windows_threads();//win上设置
#else
	evthread_use_pthreads();    //unix上设置
#endif

	m_pBase = event_base_new();
	if (NULL == m_pBase)
	{
		return event_base_new_fail;
	}
	evthread_make_base_notifiable((event_base *)m_pBase);

	m_pBufEvent = bufferevent_socket_new(m_pBase, -1, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE);
	if (NULL == m_pBufEvent)
	{
		event_base_free(m_pBase);
		return bufferevent_socket_new_fail;
	}

	bufferevent_setcb(m_pBufEvent, OnReadClient, NULL, OnEventClient, this);
	bufferevent_enable(m_pBufEvent, EV_READ | /*EV_WRITE |*/ EV_PERSIST);

	if (bufferevent_socket_connect(m_pBufEvent, (sockaddr*)&addr, sizeof(addr)) < 0)
	{
		bufferevent_free(m_pBufEvent);
		return bufferevent_socket_connect_fail;
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

void CBufferEventClient::Start()
{
	unsigned threadID;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &EventLoopThread, m_pBase, 0, &threadID);
	CloseHandle(hThread);
}

void CBufferEventClient::Stop()
{
	event_base_loopexit(m_pBase, NULL);
}

int CBufferEventClient::Send(const DATA_PACKAGE_T *dataPackage)
{
	if (NULL == dataPackage)
	{
		return -1;
	}
	return bufferevent_write(m_pBufEvent, dataPackage->pData, dataPackage->nLen);
}

int CBufferEventClient::GetSocketID()
{
	evutil_socket_t nFd = bufferevent_getfd(m_pBufEvent);
	return nFd;
}

void CBufferEventClient::ReleaseRes()
{
	if (NULL != m_pBufEvent)
	{
		bufferevent_free(m_pBufEvent);
	}
	if (NULL != m_pBase)
	{
		event_base_free(m_pBase);
	}
#ifdef _WIN32
	WSACleanup();
#endif
}

void CBufferEventClient::Delete(void *pParam)
{
	CBufferEventClient *pThis = (CBufferEventClient *)pParam;
	if (NULL != pThis)
	{
		pThis->ReleaseRes();
	}
}

void OnReadClient(bufferevent *pBufEvent, void *pParam)
{
	unsigned char szMsg[MAX_READ_MSG_LEN] = {0};

	int nLen = bufferevent_read(pBufEvent, szMsg, MAX_READ_MSG_LEN);
	if (nLen > 0)
	{
		DATA_PACKAGE_T data;
		data.pSendID = pBufEvent;
		data.nLen = nLen;
		data.pData = szMsg;
		data.nFd =bufferevent_getfd(pBufEvent);
		g_param.cbFun(TCP_READ_DATA, g_param.pThis, (void*)&data);
	}
}

void OnWriteClient(bufferevent *pBufEvent, void *pParam)
{
	//char szSendMsg[1024] = "[I am server]\r\n";
	//int nLen = strlen(szSendMsg) + 1;
	//int nRet = bufferevent_write(pBufEvent, szSendMsg, nLen);
}

void OnEventClient(bufferevent *pBufEvent, short nEventType, void *pParam)
{
	if (nEventType & BEV_EVENT_EOF)
	{
		//OnEvent connect closed
		bufferevent_free(pBufEvent);
		CBufferEventClient *pDlg = (CBufferEventClient *)pParam;
		int nFd = pDlg->GetSocketID();
		g_param.cbFun(TCP_CLIENT_DISCONNECT, g_param.pThis, (void*)&nFd);
	}
	else if (nEventType & BEV_EVENT_CONNECTED)
	{
		//OnEvent connect to server success
		CBufferEventClient *pDlg = (CBufferEventClient *)pParam;
		int nFd = pDlg->GetSocketID();
		g_param.cbFun(TCP_CLIENT_CONNECT, g_param.pThis, (void*)&nFd);
	}
	else if (nEventType & BEV_EVENT_ERROR)
	{
		//OnEvent meet some other error
	}
	else
	{

	}
}