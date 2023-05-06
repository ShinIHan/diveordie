#include <vector>
#include "Core.h"

#pragma once

struct SessionInfo
{
    string  IP = "None";
    int     Difficulty = 0;
    int     Stage = 0;
};

class SessionsManager
{
public:
    SessionsManager();

    bool CreateSession(const SessionInfo& session);
    SessionInfo SearchSession(const int& difficulty, const int& stage);
    bool DestroySession(const string& ip);

private:
    vector<SessionInfo> Sessions;
};