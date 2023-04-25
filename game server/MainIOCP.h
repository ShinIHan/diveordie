#pragma once
#pragma once

#include "IOCP.h"
#include "DBConnector.h"


#define DB_ADDRESS      "localhost"
#define DB_PORT         7777
#define DB_ID           "admin"
#define DB_PW           "qwer123$"

enum EPacketType
{
    SIGNUP,
    LOGIN,
    SETUSERDATA
};

class MainIOCP : public IOCP
{
public:
    MainIOCP();
    ~MainIOCP();

    virtual bool CreateWorkerThread() override;
    virtual void WorkerThread() override;

    virtual void ServerStart() override;

    static void Send(SOCKETINFO* pSocket);

private:
    static map<int, SOCKET> SessionSocket;
    static float            HitPoint;
    static DBConnector      Conn;

    FuncProcess             fnProcess[100];

    // 회원가입
    static void SignUp(stringstream& RecvStream, SOCKETINFO* pSocket);
    // DB에 로그인
    static void Login(stringstream& RecvStream, SOCKETINFO* pSocket);

    // DB에 유저 데이터 저장
    static void SetUserData(stringstream& RecvStream, SOCKETINFO* pSocket);
};
