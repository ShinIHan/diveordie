// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DiveGameWidget.h"
#include "LobbyWidget.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class ESessionStatus : uint8
{
	SESSIONSEARCH UMETA(DisplayName = "SessionSearch"),
	SESSIONCREATE UMETA(DisplayName = "SessionCreate"),
	SESSIONJOIN UMETA(DisplayName = "SessionJoin")
};

UCLASS()
class GAME_API ULobbyWidget : public UDiveGameWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;

protected:
	UFUNCTION(BlueprintCallable)
	bool SearchSession();
	
	UFUNCTION(BlueprintCallable)
	bool CreateSession();

	UFUNCTION(BlueprintCallable)
	void JoinSession();
	
	UDiveGameInstance* DiveGameInstance;

	UPROPERTY(BlueprintReadOnly)
	ESessionStatus SessionStatus;
};
