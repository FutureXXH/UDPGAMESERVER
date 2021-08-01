
#ifndef __DEFINE_H
#define __DEFINE_H



#define NOWTIME(){ SYSTEMTIME sys;GetLocalTime(&sys);std::cout << sys.wHour << ":" << sys.wMinute << ":" << sys.wSecond;}
#define SERVERPRINT_INFO NOWTIME();std::cout<<"[INFO]:"
#define DATA_BUFSIZE 1024

#define CONNECT 10001
#define HEART 10002
#define OPERATIONSYNC 20001
#define OPERATIONSYNCPACK 20002

#include <winsock2.h>

#include <windows.h>
#include<iostream>



struct playerdata
{
	int playerid;
	SOCKADDR_IN Addr;
	unsigned int frame ;
	double starttime, endtime;

};

/*struct DATA
{
	int head;
	int sendsize;
	int ID;
	char buf[1024];
};*/

typedef struct
{
	OVERLAPPED Overlapped;
	WSABUF DataBuf;
	char data[1024];

	SOCKADDR_IN remoteAddr; //存储数据来源IP地址
	int remoteAddrLen;              //存储数据来源IP地址长度

}PER_IO_OPERATION_DATA, * LPPER_IO_OPERATION_DATA;







#endif