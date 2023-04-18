#pragma once
#pragma once

#include "IOCP.h"
#include "DBConnector.h"


#define DB_ADDRESS      "localhost"
#define DB_PORT         3306
#define DB_ID           "admin"
#define DB_PW           "qwer123$"

enum EPacketType
{
    SIGNUP,
    LOGIN
};

class MainIOCP : public IOCP
{
public:
    MainIOCP();
    ~MainIOCP();

    virtual bool CreateWorkerThread();
    virtual void WorkerThread();

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


};
