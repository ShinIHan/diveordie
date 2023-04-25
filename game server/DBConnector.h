#pragma once
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include "Core.h"

class DBConnector
{
public:
    DBConnector();
    ~DBConnector();

    bool Connect(
        const string& Server,
        const string& User,
        const string& Password,
        const int& Port
    );

    void Close();

    bool SearchAccount(const string& Id, const string& Password);
    bool SignUpAccount(const string& Id, const string& Password);
    bool SetUserData(const int& difficulty, const int& stage, const int& key);


    int GetDifficulty(const string& Id);
    int GetStage(const string& Id);
    int GetKey(const string& Id);

private:
    sql::Driver* Driver = nullptr;
    sql::Connection* Con = nullptr;
    sql::Statement* Stmt = nullptr;
    sql::ResultSet* Res = nullptr;
};