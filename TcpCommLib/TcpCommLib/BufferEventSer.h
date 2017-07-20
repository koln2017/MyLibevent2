#ifndef _BUFFEREVENTSER_H_
#define _BUFFEREVENTSER_H_
#include "Header.h"
#include <set>

class CBufferEventSer : public CTcpCommLibBase
{
public:
	CBufferEventSer(CBFUN_PARAM_T param);
	~CBufferEventSer(void);

public:
	virtual int Init();
	virtual void Start();
	virtual void Stop();
	virtual int Send(void *pSendID, const unsigned char*pBuf, unsigned int nLen);

	void SetTcpNoDelay(evutil_socket_t fd);
	void SetWindowsIOCP();

	void ReleaseRes();
	static void Delete(void *pParam);


private:
	evconnlistener *m_pEvlistener;
	event_base *m_pBase;
	bool m_bIOCP;		//是否使用iocp异步模式（windows）
	
	vector<BUFFER_INFO_T> m_vecBufferInfo;
};

#endif
