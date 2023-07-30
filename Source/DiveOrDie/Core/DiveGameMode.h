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
extern int SumRX;
extern int SumRY;
extern float AvRx;
extern float AvRy;
extern int AVcount;
extern int Ba;
extern int Bb;
extern int Bc;
extern int Bd;
extern int Bx;
extern int By;
extern std::atomic<bool> bIsReadingSerialData;

/**
 *
 */
UCLASS()
class GAME_API ADiveGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ADiveGameMode();

	int countdie = 0;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	void ReadSerialData();

	const FString GameOverMapURL = "/Game/Maps/GameOver?listen";
	const FString GameOverMapName = "GameOver";
	

protected:
	UFUNCTION()
	void GameClear();

	UFUNCTION()
	void GameOver();

	UFUNCTION()
	void DeleteSerial();

	

	UPROPERTY(BlueprintReadWrite)
	AStageManagerActor* StageManagerActor = nullptr;

	bool bIsOnline;
};
