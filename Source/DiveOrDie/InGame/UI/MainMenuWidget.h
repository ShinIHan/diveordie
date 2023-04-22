// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DiveGameWidget.h"
#include "MainMenuWidget.generated.h"

/**
 * 
 */
UENUM()
enum EPlayType
{
	SINGLEPLAY,
	MULTIPLAY
};

UCLASS()
class GAME_API UMainMenuWidget : public UDiveGameWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UFUNCTION(BlueprintCallable)
	int GetPlayType();

	UFUNCTION(BlueprintCallable)
	void SetPlayType(int playType);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int _ePlayType = EPlayType::SINGLEPLAY;
};
