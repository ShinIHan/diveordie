// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TutorialWidget.generated.h"


DECLARE_MULTICAST_DELEGATE(FTutorialDelegate)
/**
 * 
 */
UCLASS()
class GAME_API UTutorialWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void StartTutorialTimer();

	void TutorialEnd();
	
	FTutorialDelegate OnTutorialEnd;

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	FTimerHandle TutorialTimer;
	
};
