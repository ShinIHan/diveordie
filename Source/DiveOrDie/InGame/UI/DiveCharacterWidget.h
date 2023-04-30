// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DiveOrDie/InGame/Character/DiveCharacter.h"
#include "Blueprint/UserWidget.h"
#include "DiveCharacterWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API UDiveCharacterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ADiveCharacter* Character;
};
