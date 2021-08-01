




#ifndef __APPMANAGER_H
#define __APPMANAGER_H
#pragma warning(disable:4996)
#include "IDefine.h"
#include <iostream>
#include <string>
#include <windows.h>
#include <vector>
#include <unordered_map>
using namespace std;
#define DATASIZE  (8*5+12)
class APPManager
{
public:
	
	APPManager();
	char everyframedata[100000][DATASIZE];
	char gamesyncdata[DATASIZE];
	int gamesyncdatasize = 12;
	unordered_map<int, playerdata> players;
	char tempchar[8];
	unsigned int frame = 0;  

	void gamesync(SOCKET serversock);
	void response(int size, char* data, SOCKADDR_IN& playeraddr, SOCKET& serversock);
	void heartcheck();
	void head10002(int& size, char* data, SOCKADDR_IN& playeraddr, SOCKET& serversock);

	void playerdelete();

};


#endif // !__APPMANAGER_H

