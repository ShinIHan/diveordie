// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DiveOrDie/InGame/game.h"
#include "Engine/GameInstance.h"
#include "DiveGameInstance.generated.h"

/**
 * 
 */
UENUM()
enum EDIFFICULTY
{
	EASY,
	NORMAL,
	HARD
};



USTRUCT()
struct FUserInfo
{
	GENERATED_BODY()

	int eDifficulty = EDIFFICULTY::EASY;

	int iStage = 0;

	int iKey = 0;
};

UCLASS()
class GAME_API UDiveGameInstance : public UGameInstance
{
	GENERATED_BODY()


public:
	void SetUserInfo(int difficulty, int stage, int key);

	UFUNCTION(BlueprintCallable)
	int GetUserKey();

	UFUNCTION(BlueprintCallable)
	int GetDifficulty();

	UFUNCTION(BlueprintCallable)
	void SetDifficulty(int difficulty);

	UFUNCTION(BlueprintCallable)
	int GetStage();

	UFUNCTION(BlueprintCallable)
	void SetStage(int stage);

	void GameClear(int stage);

	
	UPROPERTY(BlueprintReadWrite)
	int iStageNum = 1;

	UPROPERTY(BlueprintReadWrite)
	bool bIsOnline = false;

	


	UPROPERTY(BlueprintReadWrite)
	bool bIsLogin = false;

	

private:
	FUserInfo _stUserInfo;
};