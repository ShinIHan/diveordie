// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveGameInstance.h"

void UDiveGameInstance::Init()
{
	Super::Init();
}

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

int UDiveGameInstance::GetStage()
{
	return _stUserInfo.iStage;
}
