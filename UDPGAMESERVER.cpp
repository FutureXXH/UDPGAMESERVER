

#include "IOCPSERVER.h"
class IOCPSERVER;
bool isrun;
int main()
{
	isrun = true;
	IOCPSERVER* server = new IOCPSERVER();
	server->initIOCP(6666);
	server->startIOCP();
	while (isrun)
	{
		
	}
	delete server;

}