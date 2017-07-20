#ifndef _TCPLIBBASE_H_
#define _TCPLIBBASE_H_

#ifdef DLL_IMPLEMENT
#define DLL_API	__declspec(dllexport)
#else
#define DLL_API	__declspec(dllimport)
#endif

#include <stdio.h>

#define MAX_LINE		1024
#define LISTEN_BACKLOG	32
#define MAX_LISTEN_SOCKET_NUM	5000
#define MAX_READ_MSG_LEN	4096

enum STATUS_EN
{
	success = 0,
	WSAStartup_fail,
	socket_create_fail,
	bind_fail,
	listen_fail,
	event_base_new_fail,
	inet_ntop_fail,
	connect_fail,
	evconnlistener_new_bind_fail,
	bufferevent_socket_new_fail,
	bufferevent_socket_connect_fail,
};

enum CBTYPE_EN
{
	TCP_CLIENT_CONNECT = 0,		//客户端响应连接上的消息
	TCP_CLIENT_DISCONNECT,		//客户端响应断开时的消息
	TCP_SERVER_CONNECT,			//服务端响应客户端连接的消息
	TCP_SERVER_DISCONNECT,		//服务端响应客户端断开的消息
	TCP_READ_DATA,						//数据读取消息
};

//enType：回调类型，pThis：调用对象指针，pBuf：数据缓存
typedef void (*ReadCbFun)(CBTYPE_EN enType, void *pThis, void *pBuf);

struct CBFUN_PARAM_T
{
	char *pIP;
	int nPort;
	void *pThis;
	ReadCbFun cbFun;
};

struct DATA_PACKAGE_T
{
	int nLen;
	unsigned char *pData;
	void *pSendID;			//发送数据的ID
	DATA_PACKAGE_T()
	{
		nLen = 0;
		pData = NULL;
		pSendID = NULL;
	}
};


class CTcpCommLibBase
{
public:
	//返回错误码 STATUS_EN：0-success
	virtual int Init() = 0;
	virtual void Start() = 0;
	virtual void Stop() = 0;
	/**********************************
	//pSendID：数据发送ID，服务端必须传入。服务端传入NULL时，将向最后一次连接的客户端发送数据。		
	//										客户端可默认传入NULL。					
	//pBuf：需发送的字符串
	//nLen：字符串大小，要求小于4096
	//返回值：0-success，小于0-fail
	**********************************/
	virtual int Send(void *pSendID, const unsigned char*pBuf, unsigned int nLen) = 0;

	//客户端使用
	virtual int GetSocketID();
};

//nType：0-服务端 1-客户端
DLL_API CTcpCommLibBase *CreateTcpCommLib(int nType, CBFUN_PARAM_T param);
DLL_API void DeleteTcpCommLib();


#endif