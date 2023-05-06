#include "SessionsManager.h"

SessionsManager::SessionsManager()
{

}

bool SessionsManager::CreateSession(const SessionInfo& session)
{
    for (auto S : Sessions)
    {
        if (S.IP == session.IP) return false;
    }

    Sessions.push_back(session);

    return true;
}

SessionInfo SessionsManager::SearchSession(const int& difficulty, const int& stage)
{
    for (auto Session : Sessions)
    {
        if ((Session.Difficulty == difficulty) && (Session.Stage == stage))
        {
            DestroySession(Session.IP);
            return Session;
        }
    }

    SessionInfo Session;
    return Session;
}

bool SessionsManager::DestroySession(const string& ip)
{
    int i = 0;
    for (auto Session : Sessions)
    {
        if (Session.IP == ip)
        {
            Sessions.erase(Sessions.begin() + i);
            return true;
        }
        i++;
    }
    return false;
}