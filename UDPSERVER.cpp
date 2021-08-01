#include "UDPSERVER.h"

using namespace std;
UDPserver::UDPserver()
{
}

UDPserver::~UDPserver()
{
    shutdown(serversocket,2);
    SERVERPRINT_INFO << "已关闭服务器socket" << endl;
}

bool UDPserver::InitWinsock()
{
    WSADATA wsd;
    if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0)
    {
        SERVERPRINT_INFO << "Winsock初始化失败" << endl;
        return false;
    }

    return true;
}

bool UDPserver::InitWinSERVER(int port)
{
    serversocket = WSASocket(AF_INET, SOCK_DGRAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (serversocket == INVALID_SOCKET)
    {
        SERVERPRINT_INFO << "服务器socket初始化失败" << endl;
    }
    memset(&server_adr, 0, sizeof(server_adr));
    server_adr.sin_family = AF_INET;
    server_adr.sin_addr.S_un.S_addr = INADDR_ANY;
    server_adr.sin_port = htons(port);
    if (bind(serversocket, (struct sockaddr*)&server_adr, sizeof(server_adr)) == SOCKET_ERROR)
    {
        SERVERPRINT_INFO << "服务器bind初始化失败" << endl;
        return false;
    }
 
    SERVERPRINT_INFO << "服务器初始化成功" << endl;
	return true;
}

bool UDPserver::StartUDPServer()
{

    SERVERPRINT_INFO << "UDP服务器初始化成功" << endl;
    return true;
}
