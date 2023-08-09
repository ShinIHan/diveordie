// Fill out your copyright notice in the Description page of Project Settings.


#include "DiveOrDie/InGame/Object/Megalodon.h"
#include "DiveOrDie/InGame/Character/DiveCharacter.h"

AMegalodon::AMegalodon()
{
	PrimaryActorTick.bCanEverTick = true;

	box = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	RootComponent = box;

	box->SetBoxExtent(FVector(450.0f, 1100.0f, 300.0f));

	mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	mesh->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	mesh->SetCollisionProfileName("Object");
	box->SetCollisionProfileName("OverlapAllDynamic");

	mesh->SetRelativeScale3D(FVector(10.0f, 10.0f, 10.0f));
	mesh->SetRelativeLocation(FVector(0.0f, 300.0f, -300.0f));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MEGALODON_MESH(TEXT("/Game/Meshes/Megalodon.Megalodon"));
	if (MEGALODON_MESH.Succeeded())
	{
		mesh->SetStaticMesh(MEGALODON_MESH.Object);
	}

	box->OnComponentBeginOverlap.AddDynamic(this, &AMegalodon::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AMegalodon::BeginPlay()
{
	Super::BeginPlay();
}

void AMegalodon::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ADiveCharacter* Character = Cast<ADiveCharacter>(OtherActor);
	if (Character)
	{
		Character->ReceiveAnyDamage(50.0f);

		FVector CharacterLocation = Character->GetActorLocation();
		FVector WhaleLocation = GetActorLocation();
		FVector Direction = CharacterLocation - WhaleLocation;
		Direction.Normalize();

		float DistanceFromWhale = FVector::Distance(CharacterLocation, WhaleLocation);
		float BlockingDistance = 5.0f;

		if (DistanceFromWhale < BlockingDistance)
		{
			FVector BlockedLocation = WhaleLocation + Direction * BlockingDistance;
			Character->SetActorLocation(BlockedLocation);
		}
	}
}

// Called every frame
void AMegalodon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector CurrentLocation = GetActorLocation();

	float ArcHeight = 20.f;
	float ArcDuration = 2.f;

	float TimeElapsed = GetWorld()->GetTimeSeconds() / 2;
	float ZOffset = (FMath::Sin(TimeElapsed / ArcDuration * PI) * ArcHeight);

	FVector NewLocation = CurrentLocation;

	FRotator CurrentRotation = GetActorRotation();
	FRotator RotationToAdd = FRotator(0.f, 90.f, 0.f);
	FRotator NewRotation = CurrentRotation + RotationToAdd;
	FVector ForwardDirection = NewRotation.Vector();

	NewLocation += ForwardDirection * 25.f;
	NewLocation.Z += ZOffset;

	SetActorLocation(NewLocation);
}
