// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/Core/DiveGameInstance.h"
#include <string>
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"


void UDiveGameInstance::Init()
{
	Super::Init();

	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		SessionInterface = Subsystem->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UDiveGameInstance::OnCreateSessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UDiveGameInstance::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UDiveGameInstance::OnJoinSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UDiveGameInstance::OnDestroySessionComplete);
		}
		LOG_SCREEN("Subsystem : %s", *Subsystem->GetSubsystemName().ToString());
	}
}

void UDiveGameInstance::CreateServer()
{
	LOG_SCREEN("Create Server");
	SessionStatus = SessionStatus::HOST;
	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bIsLANMatch = bIsLocal;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.NumPublicConnections = 5;

	FString str = FString::FromInt(_stUserInfo.eDifficulty + iStageNum);
	SessionSettings.Set(SEARCH_KEYWORDS, str, EOnlineDataAdvertisementType::ViaOnlineService);

	SessionInterface->DestroySession(CurrentSessionName);
	SessionInterface->CreateSession(0, FName("TestSession"), SessionSettings);
}

void UDiveGameInstance::JoinServer()
{
	SessionStatus = SessionStatus::SEARCH;
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	
	SessionSearch->bIsLanQuery = bIsLocal;
	SessionSearch->MaxSearchResults = 100;
	FString str = FString::FromInt(_stUserInfo.eDifficulty + iStageNum);
	SessionSearch->QuerySettings.Set(SEARCH_KEYWORDS, str, EOnlineComparisonOp::Equals);

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UDiveGameInstance::DestroySession()
{
	SessionInterface->DestroySession(CurrentSessionName);
}

void UDiveGameInstance::OnCreateSessionComplete(FName SessionName, bool bResult)
{
	if (bResult)
	{
		CurrentSessionName = SessionName;

		LOG_SCREEN("%s", *(IOnlineSubsystem::Get()->GetSubsystemName().ToString()));
	}
}

void UDiveGameInstance::OnFindSessionComplete(bool bResult)
{
	if (bResult)
	{
		TArray<FOnlineSessionSearchResult> SearchResults = SessionSearch->SearchResults;

		LOG_SCREEN("Search Session Count : %d", SearchResults.Num());
		
		if (SearchResults.Num() != 0)
		{
			SessionStatus = SessionStatus::JOIN;
			SessionInterface->JoinSession(0, FName("TestSession"), SearchResults[0]);
		}
		else
		{
			LOG_SCREEN("Find Fail");
			CreateServer();
		}
	}
}

void UDiveGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FString JoinAddress = "";
		SessionInterface->GetResolvedConnectString(SessionName, JoinAddress);

		if (JoinAddress != "")
		{
			LOG_SCREEN("Join Success");
			CurrentSessionName = SessionName;
			PlayerController->ClientTravel(JoinAddress, ETravelType::TRAVEL_Absolute);
		}
	}
}

void UDiveGameInstance::OnDestroySessionComplete(FName SessionName, bool bResult)
{
	if (bResult)
	{
		LOG_SCREEN("Destroy Session");
	}
}

void UDiveGameInstance::SetUserInfo(int difficulty, int stage, int key)
{
	_stUserInfo.iKey = key;
	_stUserInfo.eDifficulty = difficulty;
	_stUserInfo.iStage = stage;
}

int UDiveGameInstance::GetUserKey()
{
	return _stUserInfo.iKey;
}

int UDiveGameInstance::GetDifficulty()
{
	return _stUserInfo.eDifficulty;
}

void UDiveGameInstance::SetDifficulty(int difficulty)
{
	_stUserInfo.eDifficulty = difficulty;
}

int UDiveGameInstance::GetStage()
{
	return _stUserInfo.iStage;
}

void UDiveGameInstance::SetStage(int stage)
{
	_stUserInfo.iStage = stage;
}

void UDiveGameInstance::GameClear(int stage)
{
	if (!(stage == 2))
	{
		if (GetStage() == (stage + 1))
		{
			SetStage(GetStage() + 1);
		}
	}

	UGameplayStatics::OpenLevel(GetWorld(), "MainMenu");
}