#include "IOCP.h"

IOCP::IOCP()
{

}

IOCP::~IOCP()
{
    WSACleanup();

    if (SocketInfo)
    {
        delete[] SocketInfo;
        SocketInfo = NULL;
    }
}

bool IOCP::Initialize()
{
    WSADATA wsaData;
    int iResult;

    // 버전 초기화
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (iResult != 0)
    {
        cout << "[ERROR] WinSock 초기화 실패" << endl;
        return false;
    }

    // 소켓 생성
    ListenSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

    if (ListenSocket == INVALID_SOCKET)
    {
        cout << "[ERROR] 소켓 생성 실패" << endl;
        return false;
    }

    // 서버 정보 설정
    SOCKADDR_IN serverAddr;
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

    // 소켓 설정
    iResult = bind(ListenSocket, (struct sockaddr*)&serverAddr, sizeof(SOCKADDR_IN));

    if (iResult == SOCKET_ERROR)
    {
        cout << "[ERROR] Bind 실패" << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return false;
    }

    // 수신 대기열 생성
    iResult = listen(ListenSocket, 5);
    if (iResult == SOCKET_ERROR)
    {
        cout << "[ERROR] Listen 실패" << endl;
        closesocket(ListenSocket);
        WSACleanup();
        return false;
    }

    return true;
}

void IOCP::ServerStart()
{
    int iResult;
    SOCKADDR_IN clientAddr;
    int addrLen = sizeof(SOCKADDR_IN);
    SOCKET clientSocket;
    DWORD recvBytes;
    DWORD flags;

    hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

    cout << "[INFO] 서버 시작 중...." << endl;

    while (true)
    {
        clientSocket = WSAAccept(ListenSocket, (struct sockaddr*)&clientAddr, &addrLen, NULL, NULL);

        if (clientSocket == INVALID_SOCKET)
        {
            cout << "[ERROR] Accept 실패" << endl;
            return;
        }

        SocketInfo = new SOCKETINFO();
        SocketInfo->socket = clientSocket;
        SocketInfo->recvBytes = 0;
        SocketInfo->sendBytes = 0;
        SocketInfo->dataBuf.len = MAX_BUFFER;
        SocketInfo->dataBuf.buf = SocketInfo->messageBuffer;
        flags = 0;

        hIOCP = CreateIoCompletionPort((HANDLE)clientSocket, hIOCP, (DWORD)SocketInfo, 0);

        iResult = WSARecv(
            SocketInfo->socket,
            &SocketInfo->dataBuf,
            1,
            &recvBytes,
            &flags,
            &(SocketInfo->overlapped),
            NULL
        );

        if (iResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
        {
            cout << "[ERROR] IO Pending 실패 : " << WSAGetLastError();
            return;
        }
    }
}

void IOCP::Send(SOCKETINFO* pSocket)
{
    int iResult;
    DWORD   sendBytes;
    DWORD   dwFlags = 0;

    iResult = WSASend(
        pSocket->socket,
        &(pSocket->dataBuf),
        1,
        &sendBytes,
        dwFlags,
        NULL,
        NULL
    );

    if (iResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
    {
        cout << "[ERROR] IO Pending 실패 : " << WSAGetLastError();
        return;
    }
}

void IOCP::Recv(SOCKETINFO* pSocket)
{
    int iResult;
    // DWORD	sendBytes;
    DWORD	dwFlags = 0;

    // SOCKETINFO 데이터 초기화
    ZeroMemory(&(pSocket->overlapped), sizeof(OVERLAPPED));
    ZeroMemory(pSocket->messageBuffer, MAX_BUFFER);
    pSocket->dataBuf.len = MAX_BUFFER;
    pSocket->dataBuf.buf = pSocket->messageBuffer;
    pSocket->recvBytes = 0;
    pSocket->sendBytes = 0;

    dwFlags = 0;

    // 클라이언트로부터 다시 응답을 받기 위해 WSARecv 를 호출해줌
    iResult = WSARecv(
        pSocket->socket,
        &(pSocket->dataBuf),
        1,
        (LPDWORD)&pSocket,
        &dwFlags,
        (LPWSAOVERLAPPED) & (pSocket->overlapped),
        NULL
    );

    if (iResult == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
    {
        printf_s("[ERROR] WSARecv 실패 : ", WSAGetLastError());
    }
}
