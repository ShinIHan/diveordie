// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DiveOrDie/InGame/game.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "DiveGameInstance.generated.h"

/**
 * 
 */
UENUM()
enum EDIFFICULTY
{
	EASY,
	NORMAL,
	HARD
};

UENUM(BlueprintType)
enum class SessionStatus : uint8
{
	NONE UMETA(DisplayName = "None"),
	SEARCH UMETA(DisplayName = "Search"),
	JOIN UMETA(DisplayName = "Join"),
	HOST UMETA(DisplayName = "Host")
};

USTRUCT()
struct FUserInfo
{
	GENERATED_BODY()

	int eDifficulty = EDIFFICULTY::EASY;

	int iStage = 0;

	int iKey = 0;
};

UCLASS()
class GAME_API UDiveGameInstance : public UGameInstance
{
	GENERATED_BODY()


public:
	void SetUserInfo(int difficulty, int stage, int key);

	UFUNCTION(BlueprintCallable)
	int GetUserKey();

	UFUNCTION(BlueprintCallable)
	int GetDifficulty();

	UFUNCTION(BlueprintCallable)
	void SetDifficulty(int difficulty);

	UFUNCTION(BlueprintCallable)
	int GetStage();

	UFUNCTION(BlueprintCallable)
	void SetStage(int stage);

	void GameClear(int stage);

	UPROPERTY(BlueprintReadWrite)
	int iStageNum = 1;

	UPROPERTY(BlueprintReadWrite)
	bool bIsOnline = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsLocal = false;

	UFUNCTION(BlueprintCallable)
	void DestroySession();

	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	void CreateServer();

	UFUNCTION(BlueprintCallable)
	void JoinServer();

	virtual void OnCreateSessionComplete(FName SessionName, bool bResult);
	virtual	void OnFindSessionComplete(bool bResult);
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	virtual void OnDestroySessionComplete(FName SessionName, bool bResult);

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	IOnlineSessionPtr SessionInterface;

	UPROPERTY(BlueprintReadOnly)
	FName CurrentSessionName = FName("");

	UPROPERTY(BlueprintReadWrite)
	bool bIsLogin = false;

	UPROPERTY(BlueprintReadOnly)
	SessionStatus SessionStatus = SessionStatus::NONE;

private:
	FUserInfo _stUserInfo;
};