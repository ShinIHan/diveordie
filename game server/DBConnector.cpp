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

        cout << "[DB] ���� ����" << endl;
        return true;
    }
    catch (sql::SQLException& e)
    {
        cout << "[DB] ���� : ";
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

    try
    {
        Res->next();
        cout << "Id = " << Res->getString("Id") << endl;
        cout << "Pw = " << Res->getString("Pw") << endl;

        return true;
    }
    catch (sql::SQLException& e)
    {
        cout << "[DB] ���� : ";
        cout << e.what() << endl;
        return false;
    }
    mysql_free_result(Res);

    return bResult;
}

bool DBConnector::SignUpAccount(const string& Id, const string& Password)
{
    string sql = "SELECT * FROM `UserAccount`.`UserInfo` WHERE `Id` = '";
    sql += Id + "'";
    try
    {
        Res = Stmt->executeQuery(sql);

        Res->next();
        cout << "[DB] �̹� �����ϴ� ���̵� : " << Res->getString("Id") << endl;

        return false;
    }

    catch (sql::SQLException& e)
    {
        sql = "INSERT INTO `UserAccount`.`UserInfo` (`Id`, `Pw`) VALUES ('";
        sql += Id + "', '" + Password + "')";

        Stmt->executeQuery(sql);
        return true;
    }
}