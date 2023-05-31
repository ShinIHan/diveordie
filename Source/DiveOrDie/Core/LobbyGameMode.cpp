// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "DiveGameInstance.h"
#include "DiveOrDie/Core/DiveGameMode.h"

ALobbyGameMode::ALobbyGameMode()
{
	
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	bIsReadingSerialData = false;

	count = 0, SumRX = 0, SumRY = 0, AvRx = 0, AvRy = 0, Bx = NULL, By = NULL;

	UDiveGameInstance* DiveGameInstance = Cast<UDiveGameInstance>(GetWorld()->GetGameInstance());

	if (DiveGameInstance)
	{
		iStage = DiveGameInstance->iStageNum;
	}
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	iPlayerCount++;

	LOG_SCREEN("%d", iPlayerCount);

	if (iPlayerCount == 2)
	{
		if (iStage == 1) GetWorld()->ServerTravel("/Game/Maps/Stage_1?listen");
		else if (iStage == 2) GetWorld()->ServerTravel("/Game/Maps/Stage_2?listen");
		else if (iStage == 3) GetWorld()->ServerTravel("/Game/Maps/Stage_3?listen");
	}
}
