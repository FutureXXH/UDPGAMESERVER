



#ifndef __UDPSERVER_H
#define __UDPSERVER_H
#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32")
#include "IDefine.h"







class UDPserver
{
public:
	SOCKET serversocket;
	SOCKADDR_IN server_adr;
	
	UDPserver();
	virtual ~UDPserver();
	bool InitWinsock();
	bool InitWinSERVER(int port);
	bool StartUDPServer();
  
};








#endif // !__UDPSERVER_H





