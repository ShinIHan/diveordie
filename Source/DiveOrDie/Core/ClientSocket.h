// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DiveOrDie/InGame/game.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include <iostream>
#include <map>
#include <sstream>
#include <string>



using namespace std;

class ADivePlayerController;
class UDiveGameInstance;

#define	MAX_BUFFER		4096
#define SERVER_PORT		80
#define SERVER_IP		"14.63.15.105"
#define MAX_CLIENTS		100

struct SOCKETINFO
{
	WSAOVERLAPPED   overlapped;
	WSABUF          dataBuf;
	SOCKET          socket;
	char            messageBuffer[MAX_BUFFER];
	int             recvBytes;
	int             sendBytes;
};

enum EPacketType
{
	SIGNUP,
	LOGIN,
	SETUSERDATA,
	SEARCHSESSION,
	CREATESESSION,
	DESTROYSESSION
};

struct SessionInfo
{
	string  IP = "";
	int     Difficulty;
	int     Stage;
};
/**
 * 
 */

class GAME_API ClientSocket
{
public:
	ClientSocket();
	~ClientSocket();

	bool InitSocket();
	void CloseSocket();
	bool Connect(const char* pszIP, int iPort);
	bool SignUp(const FText& Id, const FText& Pw);
	bool Login(const FText& Id, const FText& Pw, UDiveGameInstance* GameInstance);

	bool SearchSession(int difficulty, int stage);

	bool CreateSession(int difficulty, int stage);

	bool DestroySession();

	

	void SetPlayerController(ADivePlayerController* playerController);

	bool SetUserData(int difficulty, int stage, int key);

	SessionInfo GetSearchResult() { return SearchResult; }

	static ClientSocket* GetSingleton()
	{
		static ClientSocket ins;
		return &ins;
	}

private:
	ADivePlayerController* _playerController = nullptr;
	SOCKET ServerSocket;
	SessionInfo SearchResult;
	char recvBuffer[MAX_BUFFER];
	
};
