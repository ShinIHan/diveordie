// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginGameMode.h"
#include "Blueprint/UserWidget.h"
#include "DiveOrDie/InGame/game.h"

ALoginGameMode::ALoginGameMode()
{
	//static ConstructorHelpers::FClassFinder<UUserWidget> LOGIN_WG(TEXT("/Game/Blueprints/Login_WGBP.Login_WGBP_C"));
	static ConstructorHelpers::FClassFinder<UUserWidget> LOGIN_WG(TEXT(""));
	if (LOGIN_WG.Succeeded())
	{
		HUDWidgetClass = LOGIN_WG.Class;
	}
}

void ALoginGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		if (CurrentWidget != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}

	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
}
