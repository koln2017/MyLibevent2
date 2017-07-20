#ifndef _TCPLIBCLIENT_H_
#define _TCPLIBCLIENT_H_

#include "Header.h"

class CTcpLibClient : public CTcpCommLibBase
{
public:
	CTcpLibClient(CBFUN_PARAM_T param);
	~CTcpLibClient(void);

	virtual int Init();
	virtual void Start();
	virtual void Stop();
	virtual int Send(void *pSendID, const unsigned char*pBuf, unsigned int nLen);
	virtual int GetSocketID();

public:
	evutil_socket_t m_sockfd;
	event_base *m_pBase;
	event *m_pEvent;
	bufferevent *m_pBev;

};

#endif
