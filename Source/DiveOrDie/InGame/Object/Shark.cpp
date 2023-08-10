// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/Object/Shark.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

// Sets default values
AShark::AShark()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	for (int i = 0; i < 3; i++)
	{
		Sharkrr.Emplace(CreateDefaultSubobject<UStaticMeshComponent>(FName(FString::FromInt(i))));
	}

	RootComponent = Sharkrr[0];

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SHARK_MESH(TEXT("/Game/Meshes/Shark.Shark"));
	if (SHARK_MESH.Succeeded())
	{
		for (auto& Shark : Sharkrr)
		{
			Shark->SetCollisionProfileName("OverlapAllDynamic");
			Shark->SetStaticMesh(SHARK_MESH.Object);
			Shark->OnComponentBeginOverlap.AddDynamic(this, &AShark::OnOverlapBegin);

			if (Shark == RootComponent)
			{
				Shark->SetRelativeScale3D(FVector(1.2f, 1.2f, 1.2f));
				continue;
			}

			Shark->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		}
	}
}

// Called when the game starts or when spawned
void AShark::BeginPlay()
{
	Super::BeginPlay();	
}

void AShark::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* character = Cast<ADiveCharacter>(OtherActor);

	if (character)
	{
		character->ReceiveAnyDamage(30.0f);

		FVector CharacterLocation = character->GetActorLocation();
		FVector WhaleLocation = GetActorLocation();
		FVector Direction = CharacterLocation - WhaleLocation;
		Direction.Normalize();

		float DistanceFromWhale = FVector::Distance(CharacterLocation, WhaleLocation);
		float BlockingDistance = 5.0f;

		if (DistanceFromWhale < BlockingDistance)
		{
			FVector BlockedLocation = WhaleLocation + Direction * BlockingDistance;
			character->SetActorLocation(BlockedLocation);
		}
	}
}

// Called every frame
void AShark::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLocation = GetActorLocation();

	float ArcHeight = 5.f;
	float ArcDuration = 2.f;

	float TimeElapsed = GetWorld()->GetTimeSeconds();
	float ZOffset = (FMath::Sin(TimeElapsed / ArcDuration * PI) * ArcHeight);

	FVector NewLocation = CurrentLocation;

	FRotator CurrentRotation = GetActorRotation();
	FRotator RotationToAdd = FRotator(0.f, 90.f, 0.f);
	FRotator NewRotation = CurrentRotation + RotationToAdd;
	FVector ForwardDirection = NewRotation.Vector();

	NewLocation += ForwardDirection * 15.f;
	NewLocation.Z += ZOffset;

	SetActorLocation(NewLocation);
}