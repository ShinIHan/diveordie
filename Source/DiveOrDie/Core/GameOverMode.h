// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GameOverMode.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API AGameOverMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AGameOverMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void RetryGame();

private:

	int _iPlayerCount = 0;

	int _iRetrying = 0;
};
