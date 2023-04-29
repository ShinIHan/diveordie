// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/Core/MainMenuGameMode.h"
#include "DiveOrDie/Core/DiveGameInstance.h"
#include "Blueprint/UserWidget.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> LOGIN_WG(TEXT("/Game/Blueprints/MainMenu_WGBP.MainMenu_WGBP'"));

	if (LOGIN_WG.Succeeded())
	{
		HUDWidgetClass = LOGIN_WG.Class;
	}
}

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass != nullptr)
	{
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		if(HUDWidgetClass != nullptr)
		{
			CurrentWidget->AddToViewport();
		}
	}

	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
}

void AMainMenuGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}
