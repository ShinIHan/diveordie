// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"
#include "DiveOrDie/Core/DiveGameInstance.h"

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	DiveGameInstance = Cast<UDiveGameInstance>(GetGameInstance());

	if (DiveGameInstance)
	{
		SessionStatus = ESessionStatus::SESSIONSEARCH;
		if (SearchSession())
		{
			SessionStatus = ESessionStatus::SESSIONJOIN;
			JoinSession();
		}
		else
		{
			SessionStatus = ESessionStatus::SESSIONCREATE;
			CreateSession();
		}
	}
}

void ULobbyWidget::NativeDestruct()
{
	Super::NativeDestruct();
	
	GetSocket()->DestroySession();
}

bool ULobbyWidget::SearchSession()
{
	int difficulty = DiveGameInstance->GetDifficulty();
	int stage = DiveGameInstance->GetStage();
	
	return GetSocket()->SearchSession(difficulty, stage);
}

bool ULobbyWidget::CreateSession()
{
	int difficulty = DiveGameInstance->GetDifficulty();
	int stage = DiveGameInstance->GetStage();

	return GetSocket()->CreateSession(difficulty, stage);
}

void ULobbyWidget::JoinSession()
{
	FString strIP = GetSocket()->GetSearchResult().IP.c_str();
	LOG_SCREEN("Session IP : %s", *strIP);
	FName IP = FName(*strIP);
	UGameplayStatics::OpenLevel(GetWorld(), IP);
}
