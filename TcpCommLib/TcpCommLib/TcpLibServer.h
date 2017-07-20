#ifndef _TCPLIBSERVER_H_
#define _TCPLIBSERVER_H_

#include "Header.h"

class CTcpLibServer : public CTcpCommLibBase
{
public:
	CTcpLibServer(CBFUN_PARAM_T param);
	~CTcpLibServer(void);

public:
	virtual int Init();
	virtual void Start();
	virtual void Stop();
	virtual int Send(void *pSendID, const unsigned char*pBuf, unsigned int nLen);

	void SetTcpNoDelay(evutil_socket_t fd);


public:
	event_base *m_pBase;
	event *m_pEvent;
	unsigned m_threadID;
	evutil_socket_t m_listenfd;
};

#endif
