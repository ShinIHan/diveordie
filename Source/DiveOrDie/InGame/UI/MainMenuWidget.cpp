// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"
#include "DiveOrDie/Core/DiveGameInstance.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UMainMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();

	GetSocket()->DestroySession();
}

void UMainMenuWidget::DataSave(int difficulty, int stage, int key)
{
	GetSocket()->SetUserData(difficulty, stage, key);
}
