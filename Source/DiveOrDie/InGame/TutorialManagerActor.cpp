// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/TutorialManagerActor.h"
#include "DiveOrDie/Core/DiveGameInstance.h"
#include "DiveOrDie/Core/DiveGameState.h"

// Sets default values
ATutorialManagerActor::ATutorialManagerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	/*static ConstructorHelpers::FClassFinder<UUserWidget> TUTORIAL_WG(TEXT("/Game/Blueprints/Tutorial_WGBP.Tutorial_WGBP_C"));
	if (TUTORIAL_WG.Succeeded())
	{
		Tutorial_WGBP = TUTORIAL_WG.Class;
	}*/
}

// Called when the game starts or when spawned
void ATutorialManagerActor::BeginPlay()
{
	Super::BeginPlay();
}

void ATutorialManagerActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!Tutorial_WG)
	{
		Tutorial_WG = CreateWidget<UTutorialWidget>(GetWorld(), Tutorial_WGBP);

		if (!Tutorial_WG->IsInViewport())
		{
			Tutorial_WG->AddToViewport();
		}		

		Tutorial_WG->OnTutorialEnd.AddUObject(this, &ATutorialManagerActor::TutorialEnd);
	}
}

void ATutorialManagerActor::TutorialEnd()
{
	ADiveGameState* DiveGameState = Cast<ADiveGameState>(GetWorld()->GetGameState());
	UDiveGameInstance* DiveGameInstance = Cast<UDiveGameInstance>(GetWorld()->GetGameInstance());

	if (DiveGameState && DiveGameInstance)
	{
		int iRingCount = (DiveGameState->iScore) / 10;

		if (iRingCount <= 20)
		{
			DiveGameInstance->SetUserInfo(EDIFFICULTY::EASY, 1, DiveGameInstance->GetUserKey());
		}
		else if (iRingCount > 20 && iRingCount < 36)
		{
			DiveGameInstance->SetUserInfo(EDIFFICULTY::NORMAL, 1, DiveGameInstance->GetUserKey());
		}
		else
		{
			DiveGameInstance->SetUserInfo(EDIFFICULTY::HARD, 1, DiveGameInstance->GetUserKey());
		}
	}
}