// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "DiveGameState.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API ADiveGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	ADiveGameState();

	void StageInit();
	
	UPROPERTY(BlueprintReadWrite)
	int iScore = 0;

	UPROPERTY(BlueprintReadWrite)
	int iMinutes = 0;
	
	UPROPERTY(BlueprintReadWrite)
	int iSeconds = 0;
	
};
