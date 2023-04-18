#pragma once
#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <map>
#include <WinSock2.h>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

#define MAX_BUFFER  4096
#define SERVER_PORT 25565
#define SERVER_IP   "127.0.0.1"
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

enum EPacketType
{
    SIGNUP,
    LOGIN
};

int main()
{
    WSADATA wsaData;

    int iRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iRet != 0)
    {
        return 0;
    }
    
    SOCKET  ServerSocket;
    char    recvBuffer[MAX_BUFFER];

    ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (ServerSocket == INVALID_SOCKET)
    {
        return 0;
    }

    SOCKADDR_IN ServerAddr;
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(SERVER_PORT);
    ServerAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    iRet = connect(ServerSocket, (sockaddr*)&ServerAddr, sizeof(sockaddr));
    if (iRet == SOCKET_ERROR)
    {
        cout << "소켓 에러" << endl;
        return 0;
    }

    stringstream SendStream;

    SendStream << EPacketType::SIGNUP << endl;
    SendStream << "aaaadmisss123n" << endl;
    SendStream << "admin" << endl;

    int iSendLen = send(ServerSocket, (CHAR*)SendStream.str().c_str(), SendStream.str().length(), 0);

    if (iSendLen == -1)
    {
        cout << "전송 실패" << endl;
        return 0;
    }

    int iRecvLen = recv(ServerSocket, (CHAR*)&recvBuffer, MAX_BUFFER, 0);

    if (iRecvLen <= 0)
    {
        cout << "실패" << endl;
        return 0;
    }

    stringstream RecvStream;
    int PacketType;
    bool SignUpResult;

    RecvStream << recvBuffer;
    RecvStream >> PacketType;
    RecvStream >> SignUpResult;

    if (PacketType != EPacketType::SIGNUP)
    {
        return 0;
    }

    if (SignUpResult)
    {
        cout << "Success" << endl;
    }
    else
    {
        cout << "Fail" << endl;
        
    }

    closesocket(ServerSocket);
    WSACleanup();

    system("pause");
    return 0;
}
