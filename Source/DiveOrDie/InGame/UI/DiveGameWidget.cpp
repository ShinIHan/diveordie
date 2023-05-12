// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveGameWidget.h"

void UDiveGameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Socket = ClientSocket::GetSingleton();
	Socket->InitSocket();
	_bIsConnected = Socket->Connect(SERVER_IP, SERVER_PORT);
	if (_bIsConnected)
	{
		LOG_SCREEN("IOCP Server Connect Success");
	}
	else
	{
		LOG_SCREEN("IOCP Server Connect Fail");
	}
}

void UDiveGameWidget::NativeDestruct()
{
	Super::NativeDestruct();

	Socket->CloseSocket();
}

bool UDiveGameWidget::GetIsConnect()
{
	return _bIsConnected;
}

ClientSocket* UDiveGameWidget::GetSocket()
{
	return Socket;
}
