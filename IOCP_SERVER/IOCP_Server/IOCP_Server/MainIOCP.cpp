﻿#include "MainIOCP.h"
#include <sstream>
#include <algorithm>
#include <process.h>

DBConnector         MainIOCP::Conn;
SessionsManager     MainIOCP::Sessions;

unsigned int WINAPI CallWorkerThread(LPVOID p)
{
    MainIOCP* pOverlappedEvent = (MainIOCP*)p;
    pOverlappedEvent->WorkerThread();
    return 0;
}

MainIOCP::MainIOCP()
{
    Conn.Connect(DB_ADDRESS, DB_ID, DB_PW, DB_PORT);
    
    fnProcess[EPacketType::SIGNUP].funcProcessPacket = SignUp;
    fnProcess[EPacketType::LOGIN].funcProcessPacket = Login;
    fnProcess[EPacketType::SETUSERDATA].funcProcessPacket = SetUserData;
    fnProcess[EPacketType::SEARCHSESSION].funcProcessPacket = SearchSession;
    fnProcess[EPacketType::CREATESESSION].funcProcessPacket = CreateSession;
    fnProcess[EPacketType::DESTROYSESSION].funcProcessPacket = DestroySession;
}

MainIOCP::~MainIOCP()
{
    WSACleanup();

    if (SocketInfo)
    {
        delete[] SocketInfo;
        SocketInfo = NULL;
    }

    if (hWorkerHandle)
    {
        delete[] hWorkerHandle;
        hWorkerHandle = NULL;
    }

    Conn.Close();
}

void MainIOCP::ServerStart()
{
    
    IOCP::ServerStart();
}

void MainIOCP::Send(SOCKETINFO* pSocket)
{
    int iResult;
    DWORD sendBytes;
    DWORD dwFlags = 0;

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
        cout << "[ERROR] WSASend 실패 : " << WSAGetLastError() << endl;
    }
}

void MainIOCP::SignUp(stringstream& RecvStream, SOCKETINFO* pSocket)
{
    string Id;
    string Pw;

    RecvStream >> Id;
    RecvStream >> Pw;

    cout << "[INFO] 회원가입 시도 {" << Id << "} {" << Pw << "}" << endl;

    stringstream SendStream;
    SendStream << EPacketType::SIGNUP << endl;
    SendStream << Conn.SignUpAccount(Id, Pw) << endl;

    CopyMemory(pSocket->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
    pSocket->dataBuf.buf = pSocket->messageBuffer;
    pSocket->dataBuf.len = SendStream.str().length();
    
    Send(pSocket);
}

void MainIOCP::Login(stringstream& RecvStream, SOCKETINFO* pSocket)
{
    string Id;
    string Pw;

    RecvStream >> Id;
    RecvStream >> Pw;

    cout << "[INFO] 로그인 시도 {" << Id << "} {" << Pw << "}" << endl;

    stringstream SendStream;
    SendStream << EPacketType::LOGIN << endl;
    if (Conn.SearchAccount(Id, Pw))
    {
        SendStream << true << endl;
        SendStream << Conn.GetDifficulty(Id) << endl;
        SendStream << Conn.GetStage(Id) << endl;
        SendStream << Conn.GetKey(Id) << endl;
    }
    else
    {
        SendStream << false << endl;
    }

    CopyMemory(pSocket->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());
    pSocket->dataBuf.buf = pSocket->messageBuffer;
    pSocket->dataBuf.len = SendStream.str().length();

    Send(pSocket);
}

void MainIOCP::SetUserData(stringstream& RecvStream, SOCKETINFO* pSocket)
{
    int difficulty;
    int stage;
    int key;

    RecvStream >> difficulty;
    RecvStream >> stage;
    RecvStream >> key;

    cout << "[INFO] 유저 데이터 저장 시도 {KEY = " << key << "}" << endl;
    
    stringstream SendStream;
    SendStream << EPacketType::SETUSERDATA << endl;
    
    if (Conn.SetUserData(difficulty, stage, key))
    {
        SendStream << true << endl;
    }
    else
    {
        SendStream << false << endl;
    }

    CopyMemory(pSocket->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());

    pSocket->dataBuf.buf = pSocket->messageBuffer;
    pSocket->dataBuf.len = SendStream.str().length();

    Send(pSocket);
}

void MainIOCP::CreateSession(stringstream& RecvStream, SOCKETINFO* pSocket)
{
    int difficulty;
    int stage;
    
    RecvStream >> difficulty;
    RecvStream >> stage;

    cout << "[INFO] 세션을 생성 중..." << endl;
    cout << "[INFO] IP : " << pSocket->clientAddr << endl;
    
    stringstream SendStream;
    SendStream << EPacketType::CREATESESSION << endl;

    SessionInfo Session;
    Session.IP = pSocket->clientAddr;
    Session.Difficulty = difficulty;
    Session.Stage = stage;

    bool bResult = Sessions.CreateSession(Session);
    cout << "[INFO] 세션 생성 " << (bResult ? "성공" : "실패") << endl;
    
    SendStream << bResult << endl;

    CopyMemory(pSocket->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());

    pSocket->dataBuf.buf = pSocket->messageBuffer;
    pSocket->dataBuf.len = SendStream.str().length();

    Send(pSocket);
}

void MainIOCP::SearchSession(stringstream& RecvStream, SOCKETINFO* pSocket)
{
    int difficulty;
    int stage;

    RecvStream >> difficulty;
    RecvStream >> stage;

    stringstream SendStream;
    SendStream << EPacketType::SEARCHSESSION << endl;

    cout << "[INFO] 세션을 찾는 중..." << endl;

    SessionInfo Session = Sessions.SearchSession(difficulty, stage);

    if (Session.IP == "None")
    {
        cout << "[INFO] 해당하는 세션 없음" << endl;
        SendStream << false << endl;
    }
    else
    {
        cout << "[INFO] 세션 찾음 : " << Session.IP << endl;
        SendStream << true << endl;
        SendStream << Session.IP << endl;
    }
    
    CopyMemory(pSocket->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());

    pSocket->dataBuf.buf = pSocket->messageBuffer;
    pSocket->dataBuf.len = SendStream.str().length();

    Send(pSocket);
}

void MainIOCP::DestroySession(stringstream& RecvStream, SOCKETINFO* pSocket)
{
    cout << "[INFO] 세션을 삭제 중..." << endl;
    
    stringstream SendStream;
    SendStream << EPacketType::DESTROYSESSION << endl;

    bool bResult = Sessions.DestroySession(pSocket->clientAddr);
    
    SendStream << bResult << endl;
    
    CopyMemory(pSocket->messageBuffer, (CHAR*)SendStream.str().c_str(), SendStream.str().length());

    pSocket->dataBuf.buf = pSocket->messageBuffer;
    pSocket->dataBuf.len = SendStream.str().length();

    Send(pSocket);
}

bool MainIOCP::CreateWorkerThread()
{
    unsigned int threadId;
    // 시스템 정보 가져옴
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    cout << "[INFO] CPU 갯수 : " << sysInfo.dwNumberOfProcessors << endl;
    // 적절한 작업 스레드의 갯수는 (CPU * 2) + 1
    iThreadCnt = sysInfo.dwNumberOfProcessors * 2;

    // thread handler 선언
    hWorkerHandle = new HANDLE[iThreadCnt];
    // thread 생성
    for (int i = 0; i < iThreadCnt; i++)
    {
        hWorkerHandle[i] = (HANDLE *)_beginthreadex(
            NULL, 0, &CallWorkerThread, this, CREATE_SUSPENDED, &threadId
        );
        if (hWorkerHandle[i] == NULL)
        {
            cout << "[ERROR] Worker Thread 생성 실패" << endl;
            return false;
        }
        ResumeThread(hWorkerHandle[i]);
    }
    cout << "[INFO] Worker Thread 시작...." << endl;
    return true;
}

void MainIOCP::WorkerThread()
{
    // 함수 호출 성공 여부
    BOOL	bResult;
    int		nResult;
    // Overlapped I/O 작업에서 전송된 데이터 크기
    DWORD	recvBytes;
    DWORD	sendBytes;
    // Completion Key를 받을 포인터 변수
    SOCKETINFO *	pCompletionKey;
    // I/O 작업을 위해 요청한 Overlapped 구조체를 받을 포인터	
    SOCKETINFO *	pSocketInfo;
    DWORD	dwFlags = 0;


    while (bWorkerThread)
    {
        /**
         * 이 함수로 인해 쓰레드들은 WaitingThread Queue 에 대기상태로 들어가게 됨
         * 완료된 Overlapped I/O 작업이 발생하면 IOCP Queue 에서 완료된 작업을 가져와
         * 뒷처리를 함
         */
        bResult = GetQueuedCompletionStatus(hIOCP,
            &recvBytes,				// 실제로 전송된 바이트
            (PULONG_PTR)&pCompletionKey,	// completion key
            (LPOVERLAPPED *)&pSocketInfo,			// overlapped I/O 객체
            INFINITE				// 대기할 시간
        );

        if (!bResult && recvBytes == 0)
        {
            cout << "[INFO] socket(" << pSocketInfo->socket << ") 접속 끊김" << endl;
            closesocket(pSocketInfo->socket);
            free(pSocketInfo);
            continue;
        }

        pSocketInfo->dataBuf.len = recvBytes;

        if (recvBytes == 0)
        {
            closesocket(pSocketInfo->socket);
            free(pSocketInfo);
            continue;
        }

        try
        {
            // 패킷 종류
            int PacketType;
            // 클라이언트 정보 역직렬화
            stringstream RecvStream;

            RecvStream << pSocketInfo->dataBuf.buf;
            RecvStream >> PacketType;

            // 패킷 처리
            if (fnProcess[PacketType].funcProcessPacket != nullptr)
            {
                fnProcess[PacketType].funcProcessPacket(RecvStream, pSocketInfo);
            }
            else
            {
                cout << "[ERROR] 정의 되지 않은 패킷 : " << PacketType << endl;
            }
        }
        catch (const std::exception& e)
        {
            cout << "[ERROR] 알 수 없는 예외 발생 : " << e.what() << endl;
        }

        // 클라이언트 대기
        Recv(pSocketInfo);
    }
}
