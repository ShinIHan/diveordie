// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	virtual void Init() override;
	
	void SetUserInfo(int difficulty, int stage, int key);

	UFUNCTION(BlueprintCallable)
	int GetUserKey();

	UFUNCTION(BlueprintCallable)
	int GetDifficulty();

	UFUNCTION(BlueprintCallable)
	int GetStage();
	
private:
	FUserInfo _stUserInfo;
};
