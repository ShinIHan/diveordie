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
    catch (sql::SQLException &e)
    {
        cout << "[DB] 오류 : ";
        cout << e.what() << endl;
        return false;
    }
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

    if(Res->next())
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

}

bool DBConnector::SignUpAccount(const string& Id, const string& Password)
{
    string sql = "SELECT * FROM `UserAccount`.`UserInfo` WHERE `Id` = '";
    sql += Id + "'";
    
    Res = Stmt->executeQuery(sql);

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

        cout << "[DB] 회원가입 성공" << endl;
        return true;
    }
}

bool DBConnector::SetUserData(const int& difficulty, const int& stage, const int& key)
{
    string sql = "SELECT * FROM `UserAccount`.`UserInfo` WHERE `Key` = '";
    sql += to_string(key) + "'";

    Res = Stmt->executeQuery(sql);

    if (Res->next())
    {
        sql = "UPDATE `UserAccount`.`UserInfo` SET `Difficulty`=";
        sql += to_string(difficulty) + ", `Stage`=" + to_string(stage);
        sql += " WHERE `key`=" + to_string(key);

        Stmt->executeUpdate(sql);
        cout << "[DB] 데이터 입력 성공" << endl;
        
        return true;
    }
    
    else cout << "[DB] 해당 유저 없음 {KEY = " << key << "}"<< endl;
    return false;
}

int DBConnector::GetDifficulty(const string& Id)
{
    string sql = "SELECT * FROM `UserAccount`.`UserInfo` WHERE `Id` = '";
    sql += Id + "'";

    Res = Stmt->executeQuery(sql);

    if (Res->next())
    {
        return Res->getInt("Difficulty");
    }

    return 0;
}

int DBConnector::GetStage(const string& Id)
{
    string sql = "SELECT * FROM `UserAccount`.`UserInfo` WHERE `Id` = '";
    sql += Id + "'";

    Res = Stmt->executeQuery(sql);

    if (Res->next())
    {
        return Res->getInt("Stage");
    }

    return 0;
}

int DBConnector::GetKey(const string& Id)
{
    string sql = "SELECT * FROM `UserAccount`.`UserInfo` WHERE `Id` = '";
    sql += Id + "'";

    Res = Stmt->executeQuery(sql);

    if (Res->next())
    {
        return Res->getInt("Key");
    }

    return 0;
}
