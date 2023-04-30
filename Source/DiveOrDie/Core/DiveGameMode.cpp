// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/Core/DiveGameMode.h"
#include "DiveOrDie/Core/DiveGameInstance.h"
#include "DiveOrDie/Core/DiveGameState.h"
#include "HeadMountedDisplayTypes.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

ADiveGameMode::ADiveGameMode()
{
	/*static ConstructorHelpers::FClassFinder<ADiveCharacter> CHARCTER_BP(TEXT("/Game/Blueprints/BP_DiveCharacter.BP_DiveCharacter_C"));
	if (CHARCTER_BP.Succeeded())
	{
		DefaultPawnClass = CHARCTER_BP.Class;
	}

	static ConstructorHelpers::FClassFinder<ADiveGameState> GAMESTATE_BP(TEXT("/Game/Blueprints/BP_DiveGameState.BP_DiveGameState_C"));
	if (GAMESTATE_BP.Succeeded())
	{
		GameStateClass = GAMESTATE_BP.Class;
	}*/

	bUseSeamlessTravel = true;
}

void ADiveGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AStageManagerActor::StaticClass(), Actors);

	for (auto Actor : Actors)
	{
		StageManagerActor = Cast<AStageManagerActor>(Actor);
	}
	if (StageManagerActor)
	{
		StageManagerActor->OnEndCheck.AddUObject(this, &ADiveGameMode::GameClear);
	}
}

void ADiveGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (NewPlayer)
	{
		ADiveCharacter* DiveCharacter = Cast<ADiveCharacter>(NewPlayer->GetPawn());
		
		if (DiveCharacter)
		{
			DiveCharacter->OnPlayerDieCheck.AddUObject(this, &ADiveGameMode::GameOver);
		}
	}
}

void ADiveGameMode::GameClear()
{
	LOG_SCREEN("GameClear");
	GetWorld()->ServerTravel("/Game/Maps/GameClear");
}

void ADiveGameMode::GameOver()
{
	LOG_SCREEN("GameOver");
	
	if (StageManagerActor->bIsOnline)
	{
		GetWorld()->ServerTravel("/Game/Maps/GameOver");
		return;
	}
	
	UGameplayStatics::OpenLevel(GetWorld(), "GameOver");
}

