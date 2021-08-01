


#ifndef __IOCPSERVER_H
#define __IOCPSERVER_H
//#include "IDefine.h"
#include "UDPSERVER.h"
#include "AppManager.h"
#include <thread>
#include <mutex>




class IOCPSERVER:UDPserver
{
public:

	~IOCPSERVER();
	HANDLE CompletionPort;
	APPManager* AppManager;
	bool initIOCP(int port);
	void close();
	bool startIOCP();
	void ProcessIO(LPVOID lpParam);
};





#endif // !__IOCPSERVER_H


