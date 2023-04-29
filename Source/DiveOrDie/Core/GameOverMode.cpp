// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/Core/GameOverMode.h"
#include "DiveOrDie/Core/DiveGameInstance.h"
#include "DiveOrDie/InGame/UI/GameOverWidget.h"

AGameOverMode::AGameOverMode()
{

}

void AGameOverMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	_iPlayerCount++;
}

void AGameOverMode::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
}

void AGameOverMode::RetryGame()
{
	_iRetrying++;
	LOG_SCREEN("%d", _iRetrying);

	if (_iRetrying >= _iPlayerCount)
	{
		LOG_SCREEN("ReStart Game");
		UDiveGameInstance* DiveGameInstance = Cast<UDiveGameInstance>(GetWorld()->GetGameInstance());
		
		if (DiveGameInstance)
		{
			int iStage = DiveGameInstance->iStageNum;

			if (DiveGameInstance->bIsOnline)
			{
				if (iStage == 1)
				{
					GetWorld()->ServerTravel("/Game/Maps/Stage_1?listen");
				}
				else if (iStage == 2)
				{
					GetWorld()->ServerTravel("/Game/Maps/Stage_2?listen");
				}			
				else if (iStage == 3)
				{
					GetWorld()->ServerTravel("/Game/Maps/Stage_3?listen");
				}

				return;
			}

			if (iStage == 1)
			{
				GetWorld()->ServerTravel("Stage_1");
			}
			else if (iStage == 2)
			{
				GetWorld()->ServerTravel("Stage_2");
			}			
			else if (iStage == 3)
			{
				UGameplayStatics::OpenLevel(GetWorld(), "Stage_3");
			}
		}
	}
}