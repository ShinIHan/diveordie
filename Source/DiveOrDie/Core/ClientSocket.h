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
#define SERVER_PORT		25565
#define SERVER_IP		"127.0.0.1"
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
	CREATESESSION,
	SEARCHSESSION
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

	void SetPlayerController(ADivePlayerController* playerController);

	bool SetUserData(int difficulty, int stage, int key);

	static ClientSocket* GetSingleton()
	{
		static ClientSocket ins;
		return &ins;
	}

private:
	ADivePlayerController* _playerController = nullptr;
	SOCKET ServerSocket;
	char recvBuffer[MAX_BUFFER];
	
};
