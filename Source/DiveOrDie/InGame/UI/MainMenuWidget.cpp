// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuWidget.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMainMenuWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

int UMainMenuWidget::GetPlayType()
{
	return _ePlayType;
}

void UMainMenuWidget::SetPlayType(int playType)
{
	_ePlayType = playType;
}
