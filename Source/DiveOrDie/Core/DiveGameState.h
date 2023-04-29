// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "DiveGameState.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API ADiveGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	ADiveGameState();

	void StageInit();

	int iScore = 0;
};
