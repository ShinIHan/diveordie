// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameOverWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDelegate);
/**
 * 
 */
UCLASS()
class GAME_API UGameOverWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintCallable, BlueprintReadWrite, BlueprintAssignable)
	FPlayerDelegate OnPlayerGoToMainMenuCheck;

	UPROPERTY(BlueprintCallable, BlueprintReadWrite, BlueprintAssignable)
	FPlayerDelegate OnPlayerRetryCheck;
};
