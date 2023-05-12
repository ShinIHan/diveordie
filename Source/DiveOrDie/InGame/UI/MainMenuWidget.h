// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DiveGameWidget.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */


UCLASS()
class GAME_API UMainMenuWidget : public UDiveGameWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable)
	void DataSave(int difficulty, int stage, int key);
	
	
};
