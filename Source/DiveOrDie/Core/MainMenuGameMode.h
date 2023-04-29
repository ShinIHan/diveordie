// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MainMenuGameMode.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API AMainMenuGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	AMainMenuGameMode();

	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;


private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = true))
	TSubclassOf<class UUserWidget> HUDWidgetClass = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta = (AllowPrivateAccess = true))
	class UUserWidget* CurrentWidget = nullptr;
};
