#include "SessionsManager.h"

SessionsManager::SessionsManager()
{

}

void SessionsManager::CreateSession(const SessionInfo& session)
{
    Sessions.push_back(session);
}

SessionInfo SessionsManager::SearchSession(const int& difficulty, const int& stage)
{
    for (auto Session : Sessions)
    {
        if ((Session.Difficulty == difficulty) && (Session.Stage == stage))
        {
            return Session;
        }
    }

    SessionInfo Session;
    return Session;
}

bool SessionsManager::DestroySession(const string& ip, const int& difficulty, const int& stage)
{
    int i = 0;
    for (auto Session : Sessions)
    {
        if ((Session.IP == ip) && (Session.Difficulty == difficulty) && (Session.Stage == stage))
        {
            Sessions.erase(Sessions.begin() + i);
            return true;
        }
        i++;
    }
    return false;
}