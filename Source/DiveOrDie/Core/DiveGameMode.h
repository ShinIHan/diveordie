// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Paths.h"
#include "DiveOrDie/InGame/StageManager/StageManagerActor.h"
#include "GameFramework/GameModeBase.h"
#include "DiveGameMode.generated.h"

extern int buttonA;
extern int buttonB;
extern int buttonC;
extern int buttonD;
extern int ax;
extern int ay;
extern int az;
extern int gx;
extern int gy;
extern int gz;
extern float AvRx;
extern float AvRy;
extern int count;
extern float tx;
extern float ty;

/**
 *
 */
UCLASS()
class GAME_API ADiveGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADiveGameMode();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	void ReadSerialData();

protected:
	UFUNCTION()
	void GameClear();

	UFUNCTION()
	void GameOver();


	UPROPERTY(BlueprintReadWrite)
	AStageManagerActor* StageManagerActor = nullptr;

	bool bIsOnline;
};
