// Fill out your copyright notice in the Description page of Project Settings.


#include "ClientSocket.h"

#include "DiveGameInstance.h"
#include "DiveOrDie/InGame/Character/DivePlayerController.h"

ClientSocket::ClientSocket()
{
	
}

ClientSocket::~ClientSocket()
{
	closesocket(ServerSocket);
	WSACleanup();
}

bool ClientSocket::InitSocket()
{
	WSADATA wsaData;

	int iRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iRet != 0)
	{
		return false;
	}

	ServerSocket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (ServerSocket == INVALID_SOCKET)
	{
		return false;
	}

	return true;
}

void ClientSocket::CloseSocket()
{
	closesocket(ServerSocket);
	WSACleanup();
}

bool ClientSocket::Connect(const char* pszIP, int iPort)
{
	SOCKADDR_IN stServerAddr;

	stServerAddr.sin_family = AF_INET;
	stServerAddr.sin_port = htons(iPort);
	stServerAddr.sin_addr.s_addr = inet_addr(pszIP);

	int iRet = connect(ServerSocket, (sockaddr*)&stServerAddr, sizeof(sockaddr));
	if (iRet == SOCKET_ERROR)
	{
		return false;
	}

	return true;
}

bool ClientSocket::SignUp(const FText& Id, const FText& Pw)
{
	stringstream SendStream;

	SendStream << EPacketType::SIGNUP << endl;
	SendStream << TCHAR_TO_UTF8(*Id.ToString()) << endl;
	SendStream << TCHAR_TO_UTF8(*Pw.ToString()) << endl;

	int iSendLen = send(
		ServerSocket,
		(CHAR*)SendStream.str().c_str(),
		SendStream.str().length(),
		0);

	if (iSendLen == -1) return false;

	int iRecvLen = recv(
		ServerSocket,
		(CHAR*)&recvBuffer,
		MAX_BUFFER,
		0);

	if (iRecvLen <= 0) return false;

	stringstream RecvStream;
	int ePacketType;
	bool bSignUpResult;

	RecvStream << recvBuffer;
	RecvStream >> ePacketType;
	RecvStream >> bSignUpResult;

	if (ePacketType != EPacketType::SIGNUP) return false;

	return bSignUpResult;
}

bool ClientSocket::Login(const FText& Id, const FText& Pw, UDiveGameInstance* GameInstance)
{
	stringstream SendStream;

	SendStream << EPacketType::LOGIN << endl;
	SendStream << TCHAR_TO_UTF8(*Id.ToString()) << endl;
	SendStream << TCHAR_TO_UTF8(*Pw.ToString()) << endl;

	int iSendLen = send(
		ServerSocket,
		(CHAR*)SendStream.str().c_str(),
		SendStream.str().length(),
		0);

	if (iSendLen == -1) return false;

	int iRecvLen = recv(
		ServerSocket,
		(CHAR*)&recvBuffer,
		MAX_BUFFER,
		0);

	if (iRecvLen <= 0) return false;

	stringstream RecvStream;
	int ePacketType;
	bool bLoginResult;

	RecvStream << recvBuffer;
	RecvStream >> ePacketType;
	RecvStream >> bLoginResult;
	
	if (ePacketType != EPacketType::LOGIN) return false;

	if (bLoginResult)
	{
		int Key;
		int Difficulty;
		int Stage;

		RecvStream >> Difficulty;
		RecvStream >> Stage;
		RecvStream >> Key;
		
		GameInstance->SetUserInfo(Difficulty, Stage, Key);
	}

	return bLoginResult;
}

bool ClientSocket::SetUserData(int difficulty, int stage, int key)
{
	stringstream SendStream;

	SendStream << EPacketType::SETUSERDATA << endl;
	SendStream << difficulty << endl;
	SendStream << stage << endl;
	SendStream << key << endl;

	int iSendLen = send(
		ServerSocket,
		(CHAR*)SendStream.str().c_str(),
		SendStream.str().length(),
		0);

	if (iSendLen == -1) return false;

	int iRecvLen = recv(
		ServerSocket,
		(CHAR*)&recvBuffer,
		MAX_BUFFER,
		0);

	if (iRecvLen <= 0) return false;

	stringstream RecvStream;
	int ePacketType;
	bool bResult;

	RecvStream << recvBuffer;
	RecvStream >> ePacketType;
	RecvStream >> bResult;

	if (ePacketType != EPacketType::SETUSERDATA) return false;

	return bResult ? true : false;
}

void ClientSocket::SetPlayerController(ADivePlayerController* playerController)
{
	if (playerController)
	{
		_playerController = playerController;
	}
}


