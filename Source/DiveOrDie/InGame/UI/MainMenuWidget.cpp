// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"

#include "DiveOrDie/Core/DiveGameInstance.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GetSocket()->DestroySession();
}

void UMainMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UMainMenuWidget::DataSave(int difficulty, int stage, int key)
{
	GetSocket()->SetUserData(difficulty, stage, key);
}