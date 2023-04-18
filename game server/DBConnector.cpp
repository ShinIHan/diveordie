#include "DBConnector.h"

DBConnector::DBConnector()
{
    Driver = get_driver_instance();
}

DBConnector::~DBConnector()
{

}

bool DBConnector::Connect(const string& Server, const string& User, const string& Password, const int& Port)

{
    try
    {
        Con = Driver->connect(Server + ":" + std::to_string(Port), User, Password);
        Stmt = Con->createStatement();

        cout << "[DB] 접속 성공" << endl;
        return true;
    }
    catch (sql::SQLException& e)
    {
        cout << "[DB] 오류 : ";
        cout << e.what() << endl;
        return false;
    }

    return true;
}

void DBConnector::Close()
{
    Con->close();
}

bool DBConnector::SearchAccount(const string& Id, const string& Password)
{
    string sql = "SELECT * FROM `UserAccount`.`UserInfo` WHERE `Id` = '";
    sql += Id + "' and `Pw` = '" + Password + "'";

    Res = Stmt->executeQuery(sql);
  
    if (Res->next())
    {
        cout << "Id = " << Res->getString("Id") << endl;
        cout << "Pw = " << Res->getString("Pw") << endl;

        return true;
    }
    else
    {
        cout << "[DB] 해당 계정 존재하지 않음" << endl;
        return false;
    }
    mysql_free_result(Res);

    return bResult;
}

bool DBConnector::SignUpAccount(const string& Id, const string& Password)
{
    string sql = "SELECT * FROM `UserAccount`.`UserInfo` WHERE `Id` = '";
    sql += Id + "'";
    if (Res->next())
    {
        cout << "[DB] 이미 존재하는 아이디 : " << Res->getString("Id") << endl;

        return false;
    }

    else
    {
        sql = "INSERT INTO `UserAccount`.`UserInfo` (`Id`, `Pw`) VALUES ('";
        sql += Id + "', '" + Password + "')";

        Stmt->executeUpdate(sql);
        return true;
    }
}