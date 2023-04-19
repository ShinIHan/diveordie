// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "DiveOrDie/Core/ClientSocket.h"
#include "LoginGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API ALoginGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALoginGameMode();
	
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = true))
	TSubclassOf<class UUserWidget> HUDWidgetClass = nullptr;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = true))
	class UUserWidget* CurrentWidget = nullptr;

	
};
