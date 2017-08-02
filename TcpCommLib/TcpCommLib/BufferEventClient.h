#ifndef _BUFFEREVENTCLIENT_H_
#define _BUFFEREVENTCLIENT_H_
#include "Header.h"

class CBufferEventClient : public CTcpCommLibBase
{
public:
	CBufferEventClient(CBFUN_PARAM_T param);
	~CBufferEventClient(void);
public:
	virtual int Init();
	virtual void Start();
	virtual void Stop();
	virtual int Send(const DATA_PACKAGE_T *dataPackage);
	void SetTcpNoDelay(evutil_socket_t fd);
	virtual int GetSocketID();

	void ReleaseRes();
	static void Delete(void *pParam);

private:
	event_base *m_pBase;
	bufferevent *m_pBufEvent;
};

#endif