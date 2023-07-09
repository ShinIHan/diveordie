// Fill out your copyright notice in the Description page of Project Settings.


#include "StageManagerActor.h"

#include "DiveOrDie/Core/DiveGameInstance.h"
#include "DiveOrDie/Core/DiveGameState.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
AStageManagerActor::AStageManagerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = box;

	box->SetBoxExtent(FVector(500.0f, 500.0f, 500.0f));
	box->SetCollisionProfileName("OverlapAllDynamic");

	box->OnComponentBeginOverlap.AddDynamic(this, &AStageManagerActor::OnOverlapBegin);

	
}

// Called when the game starts or when spawned
void AStageManagerActor::BeginPlay()
{
	Super::BeginPlay();

	
	UDiveGameInstance* GameInstance = Cast<UDiveGameInstance>(GetWorld()->GetGameInstance());
	if (GameInstance)
	{
		bIsOnline = GameInstance->bIsOnline;
		
	}
	
}

// Called every frame
void AStageManagerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AStageManagerActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character)
	{
		StageClear();
		
		if (bIsOnline)
		{
			LOG_SCREEN("Server Travel");
			OnEndCheck.Broadcast();
		}
		else
		{
			LOG_SCREEN("Client Travel");
			UGameplayStatics::OpenLevel(GetWorld(), "GameClear");
		}
	}
}

void AStageManagerActor::StageClear_Implementation()
{
	UDiveGameInstance* DiveGameInstance = Cast<UDiveGameInstance>(GetWorld()->GetGameInstance());
	if (DiveGameInstance)
	{
		if (DiveGameInstance->GetStage() == iStage && DiveGameInstance->GetStage() < 3)
		{
			DiveGameInstance->SetStage(iStage + 1);
		}
	}
}


