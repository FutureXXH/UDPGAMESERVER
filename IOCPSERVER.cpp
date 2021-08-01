#include "IOCPSERVER.h"
using namespace std;
IOCPSERVER::~IOCPSERVER()
{
    close();
}
bool IOCPSERVER::initIOCP(int port)
{
    
    if (!InitWinsock())return false;
    if (!InitWinSERVER(port))return false;
   // if (!StartUDPServer())return false;
    AppManager = new APPManager();
    SERVERPRINT_INFO << "IOCP初始化成功" << endl;
    return true;
}

void IOCPSERVER::close()
{
   
 

}

void IOCPSERVER::ProcessIO(LPVOID lpParam)
{
    HANDLE CompletionPort = (HANDLE)lpParam;
    DWORD BytesTransferred;
    SOCKET serversocket;
    LPPER_IO_OPERATION_DATA PerIoData;
    while (true)
    {
        //INFINITE
        if (0 == GetQueuedCompletionStatus(CompletionPort, &BytesTransferred, (LPDWORD)&serversocket, (LPOVERLAPPED*)&PerIoData, 10000))
        {
         
            if ((GetLastError() == WAIT_TIMEOUT) || (GetLastError() == ERROR_NETNAME_DELETED))
            {
                //SERVERPRINT_INFO << "等待超时" << endl;
                //IOCPSERVER::deleteclient(*(ClientMap->at(csocket)));

                //delete PerIoData;


            }
            else
            {
                SERVERPRINT_INFO << "完成端口失败 可能是因为相关连接已经断开" << endl;
                DWORD Flags = 0;
                DWORD dwRecv = 0;
                ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
                PerIoData->DataBuf.buf = (char*)&(PerIoData->data);
                PerIoData->DataBuf.len = DATA_BUFSIZE;
                int iLen = sizeof(PerIoData->remoteAddr);
                WSARecvFrom(serversocket, &(PerIoData->DataBuf), 1, &BytesTransferred, &Flags, (sockaddr*)&(PerIoData->remoteAddr), &iLen, &PerIoData->Overlapped, NULL);
            }
            continue;

        }

        // 客户端退出
        if (BytesTransferred == 0)
        {
            SERVERPRINT_INFO << "服务器socket关闭" << serversocket << endl;
         
            delete PerIoData;

            return;
        }
        if (BytesTransferred >0)
        {
            SERVERPRINT_INFO << "接收到数据" << endl;
            AppManager->response(BytesTransferred, PerIoData->data, PerIoData->remoteAddr,serversocket);
            
            //处理事件
            //ClientMap->at(csocket)->response(BytesTransferred, PerIoData->data);
        }



        // 继续向 socket 投递WSARecv操作
        DWORD Flags = 0;
        DWORD dwRecv = 0;
        ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
        PerIoData->DataBuf.buf = (char*)&(PerIoData->data);
        PerIoData->DataBuf.len = DATA_BUFSIZE;
        int iLen = sizeof(PerIoData->remoteAddr);
        WSARecvFrom(serversocket, &(PerIoData->DataBuf), 1, &BytesTransferred, &Flags, (sockaddr*)&(PerIoData->remoteAddr), &iLen, &PerIoData->Overlapped, NULL);
    }

    return;

}

bool IOCPSERVER::startIOCP()
{
    CompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);

    if (SystemInfo.dwNumberOfProcessors > 3)
    {
        for (int i = 0; i < SystemInfo.dwNumberOfProcessors * 2 - 2; i++)
        {
            thread* threadp = new thread(&IOCPSERVER::ProcessIO, this, CompletionPort);

        }
        SERVERPRINT_INFO << "创建" << SystemInfo.dwNumberOfProcessors * 2 - 2 << "个IO线程" << endl;
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            thread* threadp = new thread(&IOCPSERVER::ProcessIO, this, CompletionPort);

        }
        SERVERPRINT_INFO << "创建" << 4 << "个IO线程" << endl;
    }
   

    thread* heartthread = new thread(&APPManager::heartcheck, AppManager);
    thread* gamesyncthread = new thread(&APPManager::gamesync, AppManager,serversocket);


    SOCKET sClient;

    LPPER_IO_OPERATION_DATA PerIoData;

    sockaddr_in tempcaddr;
    CreateIoCompletionPort((HANDLE)serversocket, CompletionPort, (DWORD)serversocket, 0);
    PerIoData = new PER_IO_OPERATION_DATA();

    ZeroMemory(PerIoData, sizeof(PER_IO_OPERATION_DATA));
    PerIoData->DataBuf.buf = (char*)&(PerIoData->data);
    PerIoData->DataBuf.len = DATA_BUFSIZE;


    DWORD Flags = 0;
    DWORD RecvBytes;

    int iLen = sizeof(PerIoData->remoteAddr);
    

    if (WSARecvFrom(serversocket, &(PerIoData->DataBuf), 1, &RecvBytes, &Flags,(sockaddr*)&(PerIoData->remoteAddr), &iLen, &PerIoData->Overlapped, NULL) == SOCKET_ERROR)
    {
        if (WSAGetLastError() != ERROR_IO_PENDING)
        {
            printf("WSARecv() failed with error %d\n", WSAGetLastError());
        }
      

    }
    SERVERPRINT_INFO << "IOCP模式启动成功，正在等待数据" << endl;
    return true;
}
