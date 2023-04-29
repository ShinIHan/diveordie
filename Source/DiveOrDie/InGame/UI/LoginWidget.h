// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "DiveGameWidget.h"
#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class GAME_API ULoginWidget : public UDiveGameWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

protected:
	UFUNCTION(BlueprintCallable, Category = "Login")
	bool Login(const FText& Id, const FText& Pw);

	UFUNCTION(BlueprintCallable, Category = "Login")
	bool SignUp(const FText& Id, const FText& Pw);
};
