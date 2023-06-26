// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/Object/Whale.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
AWhale::AWhale()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	for (int i = 0; i < 5; i++)
	{
		Whalerr.Emplace(CreateDefaultSubobject<UStaticMeshComponent>(FName(FString::FromInt(i))));
	}

	RootComponent = Whalerr[0];

	static ConstructorHelpers::FObjectFinder<UStaticMesh> WHALE_MESH(TEXT("/Game/Meshes/Whale.Whale"));
	if (WHALE_MESH.Succeeded())
	{
		for (auto& Whale : Whalerr)
		{
			Whale->SetCollisionProfileName("OverlapAllDynamic");
			Whale->SetStaticMesh(WHALE_MESH.Object);
			Whale->OnComponentBeginOverlap.AddDynamic(this, &AWhale::OnOverlapBegin);

			if (Whale == RootComponent)
			{
				Whale->SetRelativeScale3D(FVector(0.3f, 0.3f, 0.3f));
				continue;
			}

			Whale->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
}

// Called when the game starts or when spawned
void AWhale::BeginPlay()
{
	Super::BeginPlay();	
}

void AWhale::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);
	if (character)
	{
		character->ReceiveOxygenDamage(25.f);

		FVector CharacterLocation = character->GetActorLocation();
		FVector WhaleLocation = GetActorLocation();
		FVector Direction = CharacterLocation - WhaleLocation;
		Direction.Normalize();

		float DistanceFromWhale = FVector::Distance(CharacterLocation, WhaleLocation);
		float BlockingDistance = 20.0f; 

		if (DistanceFromWhale < BlockingDistance)
		{
			FVector BlockedLocation = WhaleLocation + Direction * BlockingDistance;
			character->SetActorLocation(BlockedLocation);
		}
	}
}

// Called every frame
void AWhale::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLocation = GetActorLocation();
	FVector NewLocation = CurrentLocation;

	NewLocation.Y += 25.f;

	SetActorLocation(NewLocation);
}