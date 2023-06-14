#pragma once

#include "IOCP.h"
#include "DBConnector.h"
#include "SessionsManager.h"


#define DB_ADDRESS      "mygame.cvwu4vep08cv.us-east-2.rds.amazonaws.com"
#define DB_PORT         3306
#define DB_ID           "admin"
#define DB_PW           "ohsoo0015$"

enum EPacketType
{
    SIGNUP,
    LOGIN,
    SETUSERDATA,
    SEARCHSESSION,
    CREATESESSION,
    DESTROYSESSION
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
    static float            HitPoint;
    static DBConnector      Conn;
    static SessionsManager  Sessions;

    FuncProcess             fnProcess[100];

    // 회원가입
    static void SignUp(stringstream& RecvStream, SOCKETINFO* pSocket);
    // DB에 로그인
    static void Login(stringstream& RecvStream, SOCKETINFO* pSocket);
    // DB에 유저 데이터 저장
    static void SetUserData(stringstream& RecvStream, SOCKETINFO* pSocket);

    static void CreateSession(stringstream& RecvStream, SOCKETINFO* pSocket);
    
    static void SearchSession(stringstream& RecvStream, SOCKETINFO* pSocket);
    
    static void DestroySession(stringstream& RecvStream, SOCKETINFO* pSocket);
};
