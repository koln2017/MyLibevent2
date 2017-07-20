#include "TcpLibServer.h"
#include <process.h> 

void do_accept(evutil_socket_t listenfd, short event, void *arg);
void read_cb(struct bufferevent *pBev, void *arg);
void write_cb(struct bufferevent *pBev, void *arg);
void error_cb(struct bufferevent *pBev, short event, void *arg);

static CBFUN_PARAM_T g_param;
vector<bufferevent*> g_vecBev;

CTcpLibServer::CTcpLibServer(CBFUN_PARAM_T param)
	: m_pBase(NULL),
	m_pEvent(NULL)
{
	g_param = param;
}

CTcpLibServer::~CTcpLibServer(void)
{
	event_base_loopexit(m_pBase, NULL);
	if (NULL != m_pEvent)
	{
		event_free(m_pEvent);
		m_pEvent = NULL;
	}
	if (NULL != m_pBase)
	{
		event_base_free(m_pBase);
		m_pBase = NULL;
	}
	closesocket(m_listenfd);
	WSACleanup();
}

//…Ë÷√∑«◊Ë»˚£¨Ω˚÷πNagleÀ„∑®
void CTcpLibServer::SetTcpNoDelay(evutil_socket_t fd)
{
	int nOne = 1;
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&nOne, sizeof nOne);
}

int CTcpLibServer::Init()
{
	WSADATA wsaData;
	DWORD dwRet;
	if ((dwRet =WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		return WSAStartup_fail;
	}

	if ((m_listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		return socket_create_fail;
	}

	struct sockaddr_in servaddr;
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(g_param.nPort);

	if (bind(m_listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
	{
		return bind_fail;
	}

	if (listen(m_listenfd, LISTEN_BACKLOG) < 0)
	{
		return listen_fail;
	}

	evutil_make_listen_socket_reuseable(m_listenfd);
	evutil_make_socket_nonblocking(m_listenfd);

	m_pBase = event_base_new();
	if (m_pBase == NULL)
	{ 
		return event_base_new_fail;
	}

	const char *pEventMechanism = event_base_get_method(m_pBase);
	m_pEvent = event_new(m_pBase, m_listenfd, EV_READ | EV_WRITE | EV_PERSIST, do_accept, (void*)m_pBase);
	event_add(m_pEvent, NULL);
	return success;
}

unsigned __stdcall EventLoopThread(void *pParam)
{
	//CTcpLibevent *pThis = (CTcpLibevent *)pParam;
	event_base *pBase = (event_base *)pParam;
	if (NULL != pBase)
	{
		int nRet = event_base_dispatch(pBase);
	}
	return 0;
}

void CTcpLibServer::Start()
{
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &EventLoopThread, m_pBase, 0, &m_threadID);
	CloseHandle(hThread);
}

void CTcpLibServer::Stop()
{
	event_base_loopexit(m_pBase, NULL);
}

int CTcpLibServer::Send(void *pSendID, const unsigned char*pBuf, unsigned int nLen)
{
	//bufferevent_write(m_pBev, pBuf, nLen);
	return -1;
}

void do_accept(evutil_socket_t listenfd, short event, void *arg)
{
	struct event_base *pBase = (struct event_base*)arg;
	evutil_socket_t fd;
	struct sockaddr_in cliaddr;
	socklen_t clilen = sizeof(sockaddr_in);
	fd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
	if(fd < 0)
	{
		return;
	}

	struct bufferevent *pBev = bufferevent_socket_new(pBase, fd, BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(pBev, read_cb, write_cb, error_cb, arg);
	bufferevent_enable(pBev, EV_READ | EV_WRITE | EV_PERSIST);
	g_vecBev.push_back(pBev);
	
}

void read_cb(struct bufferevent *pBev, void *arg)
{
	char szBuf[MAX_LINE +1] = {0};
	int nLen = 0;
	evutil_socket_t fd = bufferevent_getfd(pBev);
	while(nLen = bufferevent_read(pBev, szBuf, sizeof(szBuf)-1), nLen > 0)
	{
		g_param.cbFun(TCP_READ_DATA, g_param.pThis, szBuf);
		bufferevent_write(pBev, szBuf, nLen);
	}
}

void write_cb(struct bufferevent *pBev, void *arg)
{
	//char szSendMsg[1024] = "[I am server]\r\n";
	//int nLen = strlen(szSendMsg) + 1;
	//int nRet = bufferevent_write(pBev, szSendMsg, nLen);
	//if (-1 == nRet)
	//{
	//}
}

void error_cb(struct bufferevent *pBev, short event, void *arg)
{
	evutil_socket_t fd = bufferevent_getfd(pBev);
	if (event & BEV_EVENT_TIMEOUT)
	{

	}
	else if (event & BEV_EVENT_EOF)
	{

	}
	else if (event & BEV_EVENT_ERROR)
	{

	}
	bufferevent_free(pBev);
}
