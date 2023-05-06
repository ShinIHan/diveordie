// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/Core/DiveGameInstance.h"
#include <string>
#include "SocketSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"







void UDiveGameInstance::SetUserInfo(int difficulty, int stage, int key)
{
	_stUserInfo.iKey = key;
	_stUserInfo.eDifficulty = difficulty;
	_stUserInfo.iStage = stage;
}

int UDiveGameInstance::GetUserKey()
{
	return _stUserInfo.iKey;
}

int UDiveGameInstance::GetDifficulty()
{
	return _stUserInfo.eDifficulty;
}

void UDiveGameInstance::SetDifficulty(int difficulty)
{
	_stUserInfo.eDifficulty = difficulty;
}

int UDiveGameInstance::GetStage()
{
	return _stUserInfo.iStage;
}

void UDiveGameInstance::SetStage(int stage)
{
	_stUserInfo.iStage = stage;
}

void UDiveGameInstance::GameClear(int stage)
{
	if (!(stage == 2))
	{
		if (GetStage() == (stage + 1))
		{
			SetStage(GetStage() + 1);
		}
	}

	UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");
}

