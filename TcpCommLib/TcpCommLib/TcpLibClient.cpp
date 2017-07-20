#include "TcpLibClient.h"
#include <process.h> 

void cmd_msg_cb(int fd, short event, void *arg);
void read_cli_cb(struct bufferevent *pBev, void *arg);
void error_cli_cb(struct bufferevent *pBev, short event, void *arg);
static CBFUN_PARAM_T g_param;

CTcpLibClient::CTcpLibClient(CBFUN_PARAM_T param)
	: m_pBase(NULL),
	m_pEvent(NULL),
	m_pBev(NULL)
{
	g_param = param;
}

CTcpLibClient::~CTcpLibClient(void)
{
}

int CTcpLibClient::Init()
{
	WSADATA wsaData;
	DWORD dwRet;
	if ((dwRet = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
	{
		return WSAStartup_fail;
	}
	if ((m_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		return socket_create_fail;
	}
	struct sockaddr_in seraddr;
	bzero(&seraddr, sizeof(seraddr));
	seraddr.sin_family = AF_INET;
	seraddr.sin_port = htons(g_param.nPort);
	if (inet_pton(AF_INET, g_param.pIP, &seraddr.sin_addr) < 1)
	{
		return inet_ntop_fail;
	}
	
	if (connect(m_sockfd, (struct sockaddr*)&seraddr, sizeof(seraddr)) < 0)
	{
		return connect_fail;
	}

	evutil_make_socket_nonblocking(m_sockfd);
	m_pBase = event_base_new();
	if (m_pBase == NULL)
	{
		return event_base_new_fail;
	}
	const char *eventMechanism = event_base_get_method(m_pBase);
	
	m_pBev= bufferevent_socket_new(m_pBase, m_sockfd, BEV_OPT_CLOSE_ON_FREE);
	m_pEvent = event_new(m_pBase, m_sockfd, EV_READ | EV_WRITE | EV_PERSIST, cmd_msg_cb, (void *)m_pBev);
	event_add(m_pEvent, NULL);

	bufferevent_setcb(m_pBev, read_cli_cb, NULL, error_cli_cb, (void*)m_pEvent);
	bufferevent_enable(m_pBev, EV_READ | EV_PERSIST);
	
	return success;
}

unsigned __stdcall EventLoopCliThread(void *pParam)
{
	event_base *pBase = (event_base *)pParam;
	if (NULL != pBase)
	{
		int nRet = event_base_dispatch(pBase);
	}
	return 0;
}

void CTcpLibClient::Start()
{
	unsigned threadID;
	HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, &EventLoopCliThread, m_pBase, 0, &threadID);
	CloseHandle(hThread);
}

void CTcpLibClient::Stop()
{
	event_base_loopexit(m_pBase, NULL);	
}

int CTcpLibClient::Send(void *pSendID, const unsigned char*pBuf, unsigned int nLen)
{
	return bufferevent_write(m_pBev, pBuf, nLen);
}

int CTcpLibClient::GetSocketID()
{
	evutil_socket_t nFd = bufferevent_getfd(m_pBev);
	return nFd;
}

void cmd_msg_cb(int fd, short event, void *arg)
{
}

void read_cli_cb(struct bufferevent *pBev, void *arg)
{
	char szLine[MAX_LINE+1] = {0};
	int n;
	evutil_socket_t fd = bufferevent_getfd(pBev);

	while((n = bufferevent_read(pBev, szLine, MAX_LINE)) > 0)
	{
		g_param.cbFun(TCP_READ_DATA, g_param.pThis, szLine);
	}
}

void error_cli_cb(struct bufferevent *pBev, short event, void *arg)
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

	struct event *ev = (struct event*)arg;
	event_free(ev);
}
