#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>
#include <map>
#include "Core.h"

#define MAX_BUFFER 4096
#define SERVER_PORT 25565
#define MAX_CLIENTS 100

struct SOCKETINFO
{
    WSAOVERLAPPED   overlapped;
    WSABUF          dataBuf;
    SOCKET          socket;
    char            messageBuffer[MAX_BUFFER];
    int             recvBytes;
    int             sendBytes;
};

struct FuncProcess
{
    void(*funcProcessPacket)(stringstream& RecvStream, SOCKETINFO* pSocket);
    FuncProcess()
    {
        funcProcessPacket = nullptr;
    }
};

class IOCP
{
public:
    IOCP();
    virtual ~IOCP();

    // 소켓 등록 및 서버 설정
    bool Initialize();

    // 서버 시작
    virtual void ServerStart();

    // 클라이언트 송신
    virtual void Send(SOCKETINFO* pSocket);

    // 클라이언트 수신 대기
    virtual void Recv(SOCKETINFO* pSocket);

protected:
    SOCKETINFO* SocketInfo;
    SOCKET      ListenSocket;
    HANDLE      hIOCP;
    bool        bWorkerThread;
    HANDLE* hWorkerHandle;
    int         iThreadCnt;
};

