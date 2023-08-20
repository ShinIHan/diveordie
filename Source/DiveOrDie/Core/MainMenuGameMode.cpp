// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"

#include "DiveGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "DiveOrDie/Core/DiveGameMode.h"


AMainMenuGameMode::AMainMenuGameMode()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> LOGIN_WG(TEXT("/Game/Blueprints/MainMenu_WGBP.MainMenu_WGBP_C"));
	if (LOGIN_WG.Succeeded())
	{
		HUDWidgetClass = LOGIN_WG.Class;
	}
}
void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	bIsReadingSerialData = false;

	AVcount = 0, SumRX = 0, SumRY = 0, AvRx = 0, AvRy = 0, recount = 0, Bx = NULL, By = NULL;

	UDiveGameInstance* DiveGameInstance = Cast<UDiveGameInstance>(GetWorld()->GetGameInstance());
	if (DiveGameInstance)
	{
		if(!DiveGameInstance->bIsLogin) UGameplayStatics::OpenLevel(GetWorld(), "LoginMenu");
	}
	
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

void AMainMenuGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}